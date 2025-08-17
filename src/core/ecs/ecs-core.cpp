#include "ecs-core.hpp"

#include <ranges>
#include <typeindex>
#include <vector>

namespace quark {

void ECSCore::_ecs_update(SystemType system_type) { 
  for (auto& [_, res] : this->m_resource_map) {
    res->_update();
  }

  auto& callbacks = this->_get_type_callbacks(system_type);

  // Filter out callbacks that contain resources
  // that don't exist (perhaps yet)
  auto valid_callbacks_view = std::views::filter(
    callbacks,
    [this] (const auto& callback) {
      std::vector<std::type_index>& res_types = callback->m_resources;

      for (std::type_index res_type : res_types) {
        if (!this->m_resource_map.contains(res_type)) {
          return false;
        }
      }

      return true;
    }
  );
  
  ECS& ecs = *this->_get_resource<ECS>();
  
  for (auto& callback : valid_callbacks_view) {

    std::vector<std::type_index>& comp_types = callback->m_components;

    std::unordered_map<Entity, std::any>& smallest_map = ecs.m_component_map[
      *std::min_element(
        comp_types.begin(),
        comp_types.end(),
        [&ecs](const auto& a, const auto& b) {
          return ecs.m_component_map[a].size() < ecs.m_component_map[b].size();
        }
      )
    ];

    auto filtered_view = std::views::filter(
  smallest_map,
   [&ecs, &comp_types](const auto& pair){
        Entity entity = pair.first;

        // Filter out entities that don't have all the correct components.
        for (const std::type_index type: comp_types) {
          if (!ecs.m_component_map[type].contains(entity)) {
            return false;
          }  
        }
        
        return entity.active;                         
      }
    );
     
    for (const auto& [entity, _] : filtered_view) {
        callback->_call(*this, entity);
    }
  }

}
 
}
