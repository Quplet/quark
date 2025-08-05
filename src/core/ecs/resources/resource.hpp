#pragma once

#include <type_traits>

namespace quark {

class Resource {
private:
  friend class ECSCore;
  
  virtual void _update() = 0;
};

template<typename R>
concept isResource = std::is_base_of_v<Resource, R>;

}
