#pragma once

#include "core/ecs/ecs-core.hpp"
#include "core/ecs/resources/resource.hpp"
#include <functional>
#include <memory>

namespace quark {

class Quark {
private:
  bool m_running = false;

  // Core
  std::unique_ptr<ECSCore> m_ecs_core;

  friend class ECSCore;

  void _add_core_modules();
  void _game_loop();

public:
  Quark() {
    // Core constructors are private, so we need this roundabout
    // way to instanciate them.
    // Do not pass these raw pointers out of this constructor.
    ECSCore *ecs = new ECSCore();
    m_ecs_core = std::unique_ptr<ECSCore>(ecs);
  }

  void run();

  template<Component ... Ts>
  void add_system(std::function<void(Ts&...)> callback, SystemType system_type) {
    m_ecs_core->_add_system_callback(std::move(callback), system_type);
  }

  template<isResource R>
  void add_resource(R resource) {
    m_ecs_core->_add_resource(std::move(std::unique_ptr<R>(resource)));
  }

};
  
}
            
 
