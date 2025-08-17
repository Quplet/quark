#pragma once

#include <any>
#include <cassert>
#include <cstddef>
#include <typeindex>
#include <vector>
#include <functional>
#include <unordered_map>
#include <optional>
#include <spdlog/spdlog.h>

#include "components.hpp"
#include "core/ecs/resources/resource.hpp"

namespace quark {

// Because c++ doesn't allow optional references for some
// stupid reason and I don't want to include a massive library
// like boost just for the good optional implementation.
template<typename R>
using OptionalRef = std::optional<std::reference_wrapper<R>>;


class ECS: public Resource {
private:
  friend class ECSCore;
  
  std::size_t _next_entity_id = 0;

  std::unordered_map<std::type_index, std::unordered_map<Entity, std::any>> m_component_map;
  std::vector<std::function<void(ECS&)>> m_commands;

  Entity _create_entity() {
    return Entity {
      .active = true,
      .id = this->_next_entity_id++
    };
    
  }

  void _update() override {
    for (auto command : this->m_commands) {
      command(*this);
    }
  }
  
public:
  template<Component ... Ts>
  Entity create_entity(Ts ... components) {
    Entity new_entity = this->_create_entity();

    auto command = [new_entity, components...](ECS& ecs) {
      ecs.add_components(new_entity, components...);
    };

    this->m_commands.push_back(command);
   
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
