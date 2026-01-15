#pragma once

namespace quark {

class Config {
private:
  friend class Quark;
  
  struct core_configs {
    int target_fps = -1; // -1 means unlimited
    int target_fixed_cycle = 60;
  };

  core_configs m_core_configs;

  Config() = default;

public:
  int get_target_fps() const { return this->m_core_configs.target_fps; }
  void set_target_fps(int target) { this->m_core_configs.target_fps = target; }

  int get_target_fixed_update_cycle() const {
    return this->m_core_configs.target_fixed_cycle;
  }
  void set_target_fixed_update_cycle(int target) {
    this->m_core_configs.target_fixed_cycle = target;
  }
};
  
}
