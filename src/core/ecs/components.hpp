#pragma once

#include <cstddef>
#include <type_traits>
#include <functional>

namespace quark {

template <typename T>
concept Component = std::is_copy_constructible<T>::value;

struct Entity {
  bool active = true;
  std::size_t id;

  bool operator==(const Entity& other) const {
    return this->id == other.id;
  }
};

}

namespace std {
  template <>
  struct hash<quark::Entity> {
    std::size_t operator()(const quark::Entity& entity) const {
      return std::hash<std::size_t>{}(entity.id);
    }
  };
}
