#pragma once

#include <cassert>
#include <memory>
#include <optional>
#include <stdexcept>
#include <typeindex>

#include "core/ecs/resources/resource.hpp"
#include "core/ecs/resources/time.hpp"
#include "ecs.hpp"

namespace quark {

enum SystemType {
  UPDATE,
  FIXED_UPDATE
};

class ECSCore {
private:
  friend class Quark;
  
  class ISystem {
  public:
    std::vector<std::type_index> m_components;
    std::vector<std::type_index> m_resources;

    virtual void _call(ECSCore& ecs, const Entity entity) = 0;
  };

  template<Component... Ts>
  class System : public ISystem {
  private:
    std::function<void(Ts&...)> m_callback;

  public:
    explicit System(std::function<void(Ts&...)> callback)
      : m_callback(std::move(callback)) {
      ([&] {
        if constexpr (std::is_same_v<Ts, Resource>) {
          this->m_resources.push_back(typeid(Ts));
        } else {
          this->m_components.push_back(typeid(Ts));
        }
      }(), ...);
    }

    void _call(ECSCore& ecs, const Entity entity) override {
      m_callback(*ecs._get_comp_or_res<Ts>(entity) ...);
    }

    ~System() = default;
  };

  std::unordered_map<std::type_index, std::unique_ptr<Resource>> m_resource_map;
  std::vector<std::unique_ptr<ISystem>> m_update_callbacks;
  std::vector<std::unique_ptr<ISystem>> m_update_fixed_callbacks;

  ECSCore() {
    // Insert default resources needed by the engine
    m_resource_map[typeid(ECS)] = std::make_unique<ECS>();
    m_resource_map[typeid(Time)] = std::make_unique<Time>();
  }
  
  void _ecs_update(SystemType system_type);

  std::vector<std::unique_ptr<ISystem>>& _get_type_callbacks(SystemType system_type) {
    switch (system_type) {
    case SystemType::UPDATE:
      return this->m_update_callbacks;
    case SystemType::FIXED_UPDATE:
      return this->m_update_fixed_callbacks;
    default:
      assert(false && "Unhandled system_type in ECS::_add_system_callback(...)");
      throw std::invalid_argument("Unhandled system_type in ECS::_add_system_callback(...)");
    }
  }
  
  template<Component ... Ts>
  void _add_system_callback(std::function<void(Ts&...)> callback, SystemType system_type) {
    this->_get_type_callbacks(system_type)
      .push_back(std::make_unique<System<Ts...>>(std::move(callback)));
  }

  template<isResource R>
  void _add_resource(std::unique_ptr<R> res_ptr) {
    this->m_resource_map[typeid(R)] = std::move(res_ptr);
  }

  template<isResource R>
  OptionalRef<R> _get_resource() {
    if (!this->m_resource_map.contains(typeid(R))) {
      return std::nullopt;
    }

    return static_cast<R&>(*this->m_resource_map[typeid(R)]);
  }

  template<isResource R>
  bool _has_resource() {
    return this->m_resource_map.contains(typeid(R));
  } 
  
  template<Component T>
  OptionalRef<T> _get_comp_or_res(const Entity entity) {
    if constexpr (std::is_same_v<T, Resource>) {
      return this->m_resource_map[typeid(T)];
    }
      
    return (*this->_get_resource<ECS>()).get().get_component<T>(entity);
  }
};

}
