#pragma once

#include <asp/Segmentation.hpp>
#include <Slimage/Slimage.hpp>
#include <Eigen/Dense>

namespace asp
{

	/** RGB pixel data */
	struct PixelRgb
	{
		Eigen::Vector3f color;

		static PixelRgb Zero()
		{ return {
			Eigen::Vector3f::Zero()
		}; }

		PixelRgb& operator+=(const PixelRgb& x)
		{
			color += x.color;
			return *this;
		}

		friend PixelRgb operator*(float s, const PixelRgb& x)
		{ return {
			s * x.color
		}; }
		
	};

	Segmentation<PixelRgb> SLIC(const slimage::Image3ub& img_input);

}
