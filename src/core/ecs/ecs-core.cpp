#include "ecs-core.hpp"

#include <ranges>
#include <typeindex>

namespace quark {

void ECSCore::_ecs_update(SystemType system_type) { 
  for (auto& [_, res] : this->m_resource_map) {
    res->_update();
  }

  auto& callbacks = this->_get_type_callbacks(system_type);
  ECS& ecs = *this->_get_resource<ECS>();
  
  for (auto& callback : callbacks) {

    std::vector<std::type_index>& types = callback->m_types;

    std::unordered_map<Entity, std::any>& smallest_map = ecs.m_component_map[
      *std::min_element(
        types.begin(),
        types.end(),
        [&ecs](const auto& a, const auto& b) {
          return ecs.m_component_map[a].size() < ecs.m_component_map[b].size();
        }
      )
    ];

    auto filtered_view = std::views::filter(
  smallest_map,
   [&ecs, &types](const auto& pair){
        Entity entity = pair.first;

        // Filter out entities that don't have all the correct components.
        for (const std::type_index type: types) {
          if (!ecs.m_component_map[type].contains(entity)) {
            return false;
          }  
        }
        
        return entity.active;                         
      }
    );
     
    for (const auto& [entity, _] : filtered_view) {
        callback->_call(ecs, entity);
    }
  }

}
 
}
