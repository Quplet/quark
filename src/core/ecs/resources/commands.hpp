#pragma once

#include <vector>
#include <functional>

#include "core/ecs/resources/resource.hpp"
#include "core/ecs/ecs.hpp"
#include "core/ecs/components.hpp"

namespace quark {

class Commands : public Resource {
private:
  std::vector<std::function<void(ECS&)>> m_commands;

  template<Component ... Ts>
  void create_entity(Ts ... components) {
    auto create_func = [components...](ECS& ecs) {
      ecs.create_entity(components...);
    };

    m_commands.push_back(std::move(create_func));
  }
};
  
}
