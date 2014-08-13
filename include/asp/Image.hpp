#pragma once

#include <Slimage/Slimage.hpp>
#include <type_traits>

namespace asp {

template<typename T>
using Image = slimage::Image<slimage::Traits<T,1>>;

/** Pixel-wise image conversion */
template<typename SRC, typename F>
auto Convert(const slimage::Image<SRC>& src, F conv)
-> slimage::Image<slimage::Traits<typename std::decay<decltype(conv(0,0,src[0]))>::type,1>>
{
	const unsigned width = src.width();
	const unsigned height = src.height();
	slimage::Image<slimage::Traits<typename std::decay<decltype(conv(0,0,src[0]))>::type,1>> dst{width, height};
	for(unsigned y=0, i=0; y<height; y++) {
		for(unsigned x=0; x<width; x++, i++) {
			dst[i] = conv(x,y,src[i]);
		}
	}
	return dst;
}

}
