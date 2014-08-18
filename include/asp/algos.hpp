#pragma once

#include <asp/segmentation.hpp>
#include <slimage/image.hpp>
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

	/** RGB-D pixel data (including 3D position and 3D normal) */
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

	/** Parameters for the SLIC algorithm */
	struct SlicParameters
	{
		// number of superpixels
		unsigned num_superpixels = 1000;

		// tradeoff between compact superpixels (compactness=1) and boundary recall (compactness=0)
		float compactness = 0.15f;
	};

	/** Simple Iterative Clustering superpixel algorithm for color images */
	Segmentation<PixelRgb> SuperpixelsSlic(const slimage::Image3ub& color, const SlicParameters& opt=SlicParameters());

	/** Parameters for the ASP algorithm */
	struct AspParameters
	{
		// tradeoff between compact superpixels (compactness=1) and boundary recall (compactness=0)
		float compactness = 0.15f;
	};

	/** Adaptive Superpixels algorithm for color images with a user defined density function */
	Segmentation<PixelRgb> SuperpixelsAsp(const slimage::Image3ub& color, const slimage::Image1f& density, const AspParameters& opt=AspParameters());

	/** Parameters for the DASP algorithm */
	struct DaspParameters
	{
		// focal length in pixel of the camera (used to compute correct 3D points)
		float focal_px = 540.0f;

		// factor for converting Primesense depth values to meters 
		float depth_to_z = 0.001f;

		// 3D radius of superpixels in meters
		float radius = 0.025f;

		// tradeoff between 3D compact superpixels (compactness=1) and boundary recall (compactness=0)
		float compactness = 0.4f;

		// tradeoff between using color (normal_weight=0) and normals (normal_weight=1) as data term in the distance function
		float normal_weight = 0.2f;
	};

	/** Depth-Adaptive Superpixels for RGB-D images */
	Segmentation<PixelRgbd> SuperpixelsDasp(const slimage::Image3ub& color, const slimage::Image1ui16& depth, const DaspParameters& opt=DaspParameters());
}
