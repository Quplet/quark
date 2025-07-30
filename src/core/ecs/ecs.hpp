#pragma once

#include <any>
#include <cassert>
#include <cstddef>
#include <memory>
#include <typeindex>
#include <vector>
#include <functional>
#include <unordered_map>
#include <optional>
#include <spdlog/spdlog.h>

#include "components.hpp"

namespace quark {

// Because c++ doesn't allow optional references for some
// stupid reason and I don't want to include a massive library
// like boost just for the good optional implementation.
template<typename R>
using OptionalRef = std::optional<std::reference_wrapper<R>>;

class ECS {
public:
  enum SystemType {
    UPDATE,
    FIXED_UPDATE
  };
  
private:
  friend class Quark;
  
  // Private constructor as we don't want users making their
  // own ECS instances.
  ECS() = default;

  class ISystem {
  public:
    std::vector<std::type_index> m_types;

    virtual void _call(ECS& ecs, const Entity entity) = 0;
  };

  
  template<Component... Ts>
  class System : public ISystem {
  private:
    std::function<void(Ts&...)> m_callback;

  public:
    explicit System(std::function<void(Ts&...)> callback)
      : m_callback(std::move(callback)) {
        this->m_types = { typeid(Ts) ... };
    }

    void _call(ECS& ecs, const Entity entity) override {
      m_callback(*ecs.get_component<Ts>(entity) ...);
    }
  };

  
  std::size_t _next_entity_id = 0;

  std::unordered_map<std::type_index, std::unordered_map<Entity, std::any>> m_component_map;
  std::vector<std::unique_ptr<ISystem>> m_update_callbacks;
  std::vector<std::unique_ptr<ISystem>> m_update_fixed_callbacks;

  Entity _create_entity();
  
  void _update();
  void _fixed_update();

  void _run_ecs_update(std::vector<std::unique_ptr<ISystem>>& callbacks);

  template<Component ... Ts>
  void _add_system_callback(std::function<void(Ts&...)> callback, SystemType system_type) {
    switch (system_type) {
    case SystemType::UPDATE:
      this->m_update_callbacks.push_back(std::make_unique<System<Ts...>>(std::move(callback)));
      break;
    case SystemType::FIXED_UPDATE:
      this->m_update_fixed_callbacks.push_back(std::make_unique<System<Ts...>>(std::move(callback)));
      break;
    default:
      assert(false && "Unhandled system_type in ECS::_add_system_callback(...)");
      break;
    }
  }

public:
  template<Component ... Ts>
  Entity create_entity(Ts ... components) {
    Entity new_entity = this->_create_entity();

    this->add_components(new_entity, components...);

    return new_entity;
  }

  template<Component T>
  void add_component(Entity entity, T component) {
    this->m_component_map[typeid(T)][entity] = std::move(component);
  }

  template<Component ... Ts>
  void add_components(Entity entity, Ts ... components) {
    (this->add_component(entity, components), ... );
  }

  template<Component T>
  OptionalRef<T> get_component(const Entity entity) {
    if (!this->has_component<T>(entity)) {
      return std::nullopt;
    }

    return std::any_cast<T&>(this->m_component_map[typeid(T)].at(entity));
  }

  template<Component T>
  bool has_component(const Entity entity) {
    return this->m_component_map[typeid(T)].contains(entity);
  }

  template<Component T>
  bool remove_component(const Entity entity) {
    return this->m_component_map[typeid(T)].erase(entity) > 0;
  }
};
  
}
