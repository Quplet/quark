#include "ecs.hpp"

#include <algorithm>
#include <memory>
#include <ranges>
#include <typeindex>

namespace quark {

Entity ECS::_create_entity() {
    return Entity {
      .active = true,
      .id = this->_next_entity_id++
    };
}

void ECS::_update() {
  this->_run_ecs_update(this->m_update_callbacks);
}

void ECS::_fixed_update() {
  this->_run_ecs_update(this->m_update_fixed_callbacks);
}

void ECS::_run_ecs_update(std::vector<std::unique_ptr<ISystem>>& callbacks) {
  for (auto& callback_ptr : callbacks) {

    auto types = callback_ptr->m_types;

    // TODO: At some point, for performance, we might want to keep
    // a record of the smallest map and update it when new entities
    // are added. That way we wouldn't have to recompute it every
    // update cycle.
    auto& smallest_map = this->m_component_map[
      *std::min_element(
        types.begin(),
        types.end(),
        [this](const auto& a, const auto& b) {
          return this->m_component_map[a].size() < this->m_component_map[b].size();
        }
      )
    ];

    auto filtered_view = std::views::filter(
  smallest_map,
   [this, types](const auto& pair){
        Entity entity = pair.first;

        // Filter out entities that don't have all the correct components.
        for (const std::type_index type: types) {
          if (!this->m_component_map[type].contains(entity)) {
            return false;
          }  
        }
        
        return entity.active;                         
      }
    );
     
    for (const auto& [entity, _] : filtered_view) {
      callback_ptr->_call(*this, entity);
    }
  }

}
  
}
