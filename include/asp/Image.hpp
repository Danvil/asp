#pragma once

#include <slimage/image.hpp>
#include <type_traits>

namespace asp {

template<typename T>
using Image = slimage::Image<T,1>;

}
