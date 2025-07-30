#pragma once

#include "core/ecs/ecs.hpp"
#include <functional>
#include <memory>

namespace quark {

class Quark {
private:
  bool m_running = false;

  // Core
  std::unique_ptr<ECS> m_ecs;

  friend class ECS;

public:
  Quark() {
    // Core constructors are private, so we need this roundabout
    // way to instanciate them.
    // Do not pass these raw pointers out of this constructor.
    ECS *ecs = new ECS();
    m_ecs = std::unique_ptr<ECS>(ecs);
  }

  void run();

  template<Component ... Ts>
  void add_system(std::function<void(Ts&...)> callback, ECS::SystemType system_type) {
    m_ecs->_add_system_callback(std::move(callback), system_type);
  }

};
  
}
            
 
