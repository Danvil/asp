#pragma once

#include <asp/Segmentation.hpp>
#include <slimage/image.hpp>
#include <slimage/types.hpp>
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

	/** RGB pixel data */
	struct PixelRgbd
	{
		Eigen::Vector3f color;
		float depth;
		Eigen::Vector3f world;
		Eigen::Vector3f normal;

		static PixelRgbd Zero()
		{ return {
			Eigen::Vector3f::Zero(),
			0.0f,
			Eigen::Vector3f::Zero(),
			Eigen::Vector3f::Zero()
		}; }

		PixelRgbd& operator+=(const PixelRgbd& x)
		{
			color += x.color;
			depth += x.depth;
			world += x.world;
			normal += x.normal;
			return *this;
		}

		friend PixelRgbd operator*(float s, const PixelRgbd& x)
		{ return {
			s * x.color,
			s * x.depth,
			s * x.world,
			s * x.normal
		}; }
		
	};

	Segmentation<PixelRgb> SLIC(const slimage::Image3ub& img_rgb);

	Segmentation<PixelRgbd> DASP(const slimage::Image3ub& img_rgb, const slimage::Image1ui16& img_d);

}
