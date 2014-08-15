#include "gradient.hpp"
#include <asp/algos.hpp>
#include <asp/aslic.hpp>
#include <iostream>

namespace asp
{

	constexpr PoissonDiskSamplingMethod PDS_METHOD = PoissonDiskSamplingMethod::FloydSteinbergExpo;
	constexpr float FOCAL_PX = 540.0f;
	constexpr float DEPTH_TO_Z = 0.001f;
	constexpr float RADIUS = 0.025f;
	constexpr float COMPACTNESS = 0.5f;
	constexpr float NORMAL_WEIGHT = 0.2f;

	Eigen::Vector3f Backproject(const Eigen::Vector2f& pos, const Eigen::Vector2f& center, float depth)
	{
		return (depth / FOCAL_PX) * Eigen::Vector3f{ pos.x() - center.x(), pos.y() - center.y(), FOCAL_PX };
	}

	float Density(float depth, const Eigen::Vector2f& gradient)
	{
		float q = depth / (RADIUS * FOCAL_PX);
		return q * q / 3.1415f * std::sqrt(gradient.squaredNorm() + 1.0f);
	}

	float NormalDistance(const Eigen::Vector3f& a, const Eigen::Vector3f& b)
	{
		// approximation for the angle between the two normals
		return 1.0f - a.dot(b);
	}

	Segmentation<PixelRgbd> DASP(const slimage::Image3ub& img_rgb, const slimage::Image1ui16& img_d)
	{
		const unsigned width = img_rgb.width();
		const unsigned height = img_d.height();
		const Eigen::Vector2f cam_center = 0.5f * Eigen::Vector2f{ static_cast<float>(width), static_cast<float>(height) };

		Image<Pixel<PixelRgbd>> img_data{width, height};
		for(unsigned y=0, i=0; y<height; y++) {
			for(unsigned x=0; x<width; x++, i++) {
				const auto& rgb = img_rgb(x,y);
				auto idepth = img_d(x,y);
				Pixel<PixelRgbd>& q = img_data(x,y);
				q.num = (idepth == 0) ? 0.0f : 1.0f;
				q.position = { static_cast<float>(x), static_cast<float>(y) };
				q.data.color = Eigen::Vector3f{ static_cast<float>(rgb[0]), static_cast<float>(rgb[1]), static_cast<float>(rgb[2]) }/255.0f;
				q.data.depth = static_cast<float>(idepth) * DEPTH_TO_Z;
				q.data.world = (idepth == 0) ? Eigen::Vector3f::Zero() : Backproject(q.position, cam_center, q.data.depth);
				Eigen::Vector2f gradient = LocalDepthGradient(img_d, x, y, 0.1f*RADIUS, FOCAL_PX, DEPTH_TO_Z);
				q.density = Density(q.data.depth, gradient);
				q.data.normal = NormalFromGradient(gradient, q.data.world);
			}
		}

		auto sp = ASLIC(img_data,
			ComputeSeeds(PDS_METHOD, img_data),
			[](const asp::Superpixel<PixelRgbd>& a, const Pixel<PixelRgbd>& b) {
				return
					COMPACTNESS * (a.data.world - b.data.world).squaredNorm() / (RADIUS * RADIUS)
					+ (1.0f - COMPACTNESS) * (
						(1.0f - NORMAL_WEIGHT) * (a.data.color - b.data.color).squaredNorm()
						+ NORMAL_WEIGHT * NormalDistance(a.data.normal, b.data.normal)
					);
			});

		std::cout << sp.superpixels.size() << " superpixels" << std::endl;

		return sp;
	}


}