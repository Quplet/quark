#pragma once

#include <functional>
#include <memory>

#include "config/config.hpp"
#include "core/ecs/ecs-core.hpp"
#include "core/ecs/resources/resource.hpp"

namespace quark {

class Quark {
private:
  bool m_running = false;

  // Core
  std::unique_ptr<ECSCore> m_ecs_core;
  std::unique_ptr<Config> m_config;

  friend class ECSCore;

  void _add_core_modules();
  void _game_loop();

public:
  Quark();

  void run();

  template<Component ... Ts>
  void add_system(std::function<void(Ts&...)> callback, SystemType system_type) {
    this->m_ecs_core->_add_system_callback(std::move(callback), system_type);
  }

  template<isResource R>
  void add_resource(R resource) {
    this->m_ecs_core->_add_resource(std::unique_ptr<R>(std::move(resource)));
  }

};
  
}
            
 
