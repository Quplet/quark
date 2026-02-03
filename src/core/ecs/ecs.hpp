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
            .id = this->_next_entity_id++,
            .active = true,
        };
    }

    void _update() override {
        for (auto command : this->m_commands) {
            command(*this);
        }

        this->m_commands.clear();
    }

    template<Component T>
    void _add_component(Entity entity, T component) {
        this->m_component_map[typeid(T)][entity] = std::move(component);
    }

    public:
    template<Component ... Ts>
    Entity create_entity(Ts ... components) {
        Entity new_entity = this->_create_entity();

        auto command = [new_entity, components...](ECS& ecs) {
            (ecs._add_component(new_entity, std::move(components)), ...);
        };

        this->m_commands.push_back(std::move(command));

        return new_entity;
    }

    template<Component T>
    void add_component(Entity entity, T component) {
        auto command = [entity, component](ECS& ecs) {
            ecs._add_component(entity, std::move(component));
        };

        this->m_commands.push_back(std::move(command));
    }

    template<Component ... Ts>
    void add_components(Entity entity, Ts ... components) {
        auto command = [entity, components...](ECS& ecs) {
            (ecs._add_component(entity, std::move(components)), ...);
        };

        this->m_commands.push_back(std::move(command));
    }

    template<Component T>
    T& get_component(const Entity entity) {
        assert(this->m_component_map[typeid(T)].contains(entity) && "Entity doesn't have that component!");

        return std::any_cast<T&>(this->m_component_map[typeid(T)].at(entity));
    }

    template<Component T>
    bool has_component(const Entity entity) {
        return this->m_component_map[typeid(T)].contains(entity);
    }

    template<Component T>
    void remove_component(const Entity entity) {
        // return this->m_component_map[typeid(T)].erase(entity) > 0;

        auto command = [entity](ECS& ecs) {
            ecs.m_component_map[typeid(T)].erase(entity);
        };

        this->m_commands.push_back(std::move(command));
    }

    void remove_entity(Entity entity) {
        auto command = [entity](ECS& ecs) {
            for (auto& component_map : ecs.m_component_map) {
                component_map.second.erase(entity);
            }
        };

        this->m_commands.push_back(std::move(command));
    }
};

}
