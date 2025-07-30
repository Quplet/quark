#pragma once

#include <array>
#include <typeindex>

#include "components.hpp"

namespace Quark {

template <Component ... Types>
struct With {
    static std::array<std::type_index, sizeof...(Types)> getFilter() {
        return std::array<std::type_index, sizeof...(Types)> { typeid(Types) ... };
    }
};

template <Component ... Types>
struct Without {
    static std::array<std::type_index, sizeof...(Types)> getFilter() {
        return std::array<std::type_index, sizeof...(Types)> { typeid(Types) ... };
    }
};

} /* namespace QuEngine */
