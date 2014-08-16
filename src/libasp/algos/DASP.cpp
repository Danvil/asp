#include "gradient.hpp"
#include <asp/algos.hpp>
#include <asp/aslic.hpp>
#include <iostream>

namespace asp
{

	constexpr PoissonDiskSamplingMethod PDS_METHOD = PoissonDiskSamplingMethod::FloydSteinbergExpo;

	Eigen::Vector3f Backproject(const Eigen::Vector2f& pos, const Eigen::Vector2f& center, float depth, const DaspParameters& opt)
	{
		return (depth / opt.focal_px) * Eigen::Vector3f{ pos.x() - center.x(), pos.y() - center.y(), opt.focal_px };
	}

	float Density(float depth, const Eigen::Vector2f& gradient, const DaspParameters& opt)
	{
		float q = depth / (opt.radius * opt.focal_px);
		return q * q / 3.1415f * std::sqrt(gradient.squaredNorm() + 1.0f);
	}

	float NormalDistance(const Eigen::Vector3f& a, const Eigen::Vector3f& b)
	{
		// approximation for the angle between the two normals
		return 1.0f - a.dot(b);
	}

	Segmentation<PixelRgbd> DASP(const slimage::Image3ub& img_rgb, const slimage::Image1ui16& img_d, const DaspParameters& opt_in)
	{
		const DaspParameters opt = opt_in; // use local copy for higher performance
		const unsigned width = img_rgb.width();
		const unsigned height = img_d.height();
		const Eigen::Vector2f cam_center = 0.5f * Eigen::Vector2f{ static_cast<float>(width), static_cast<float>(height) };

		Image<Pixel<PixelRgbd>> img_data{width, height};
		for(unsigned y=0, i=0; y<height; y++) {
			for(unsigned x=0; x<width; x++, i++) {
				const auto& rgb = img_rgb(x,y);
				auto idepth = img_d(x,y);
				Pixel<PixelRgbd>& q = img_data(x,y);
				q.position = { static_cast<float>(x), static_cast<float>(y) };
				q.data.color = Eigen::Vector3f{ static_cast<float>(rgb[0]), static_cast<float>(rgb[1]), static_cast<float>(rgb[2]) }/255.0f;
				if(idepth == 0) {
					// invalid pixel
					q.num = 0.0f;
					q.data.depth = 0.0f;
					q.data.world = Eigen::Vector3f::Zero();
					Eigen::Vector2f gradient = Eigen::Vector2f::Zero();
					q.density = 0.0f;
					q.data.normal = Eigen::Vector3f(0.0f, 0.0f, -1.0f);
				}
				else {
					// normal pixel
					q.num = 1.0f;
					q.data.depth = static_cast<float>(idepth) * opt.depth_to_z;
					q.data.world = Backproject(q.position, cam_center, q.data.depth, opt);
					Eigen::Vector2f gradient = LocalDepthGradient(img_d, x, y, 0.1f*opt.radius, opt.focal_px, opt.depth_to_z);
					q.density = Density(q.data.depth, gradient, opt);
					q.data.normal = NormalFromGradient(gradient, q.data.world);
				}
			}
		}

		auto sp = ASLIC(img_data,
			ComputeSeeds(PDS_METHOD, img_data),
			[COMPACTNESS=opt.compactness, NORMAL_WEIGHT=opt.normal_weight, RADIUS_SCL=1.0f/(opt.radius*opt.radius)]
			(const asp::Superpixel<PixelRgbd>& a, const Pixel<PixelRgbd>& b) {
				return
					COMPACTNESS * (a.data.world - b.data.world).squaredNorm() * RADIUS_SCL
					+ (1.0f - COMPACTNESS) * (
						(1.0f - NORMAL_WEIGHT) * (a.data.color - b.data.color).squaredNorm()
						+ NORMAL_WEIGHT * NormalDistance(a.data.normal, b.data.normal)
					);
			});

		std::cout << sp.superpixels.size() << " superpixels" << std::endl;

		return sp;
	}


}