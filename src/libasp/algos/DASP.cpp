#include <asp/algos.hpp>
#include <asp/aslic.hpp>
#include <iostream>

namespace asp
{

	constexpr PoissonDiskSamplingMethod PDS_METHOD = PoissonDiskSamplingMethod::FloydSteinbergExpo;
	constexpr float FOCAL = 520.0f;
	constexpr float DEPTH_SCL = 0.001f;
	constexpr float RADIUS = 0.18f;
	constexpr float COMPACTNESS = 1.0f;

	Eigen::Vector3f Backproject(const Eigen::Vector2f& pos, float depth)
	{
		return (depth / FOCAL) * Eigen::Vector3f{ pos.x(), pos.y(), FOCAL };
	}

	Segmentation<PixelRgbd> DASP(const slimage::Image3ub& img_rgb, const slimage::Image1ui16& img_d)
	{
		const unsigned width = img_rgb.width();
		const unsigned height = img_d.height();

		Image<Pixel<PixelRgbd>> img_data{width, height};
		for(unsigned y=0, i=0; y<height; y++) {
			for(unsigned x=0; x<width; x++, i++) {
				const slimage::Pixel3ub& rgb = img_rgb(x,y);
				uint16_t idepth = img_d(x,y);
				float depth = static_cast<float>(idepth) * DEPTH_SCL;
				Pixel<PixelRgbd>& q = img_data(x,y);
				q.num = (idepth == 0) ? 0.0f : 1.0f;
				q.position = { static_cast<float>(x), static_cast<float>(y) };
				q.density = depth / (RADIUS * FOCAL * 3.1415f);
				q.data.color = Eigen::Vector3f{ static_cast<float>(rgb[0]), static_cast<float>(rgb[1]), static_cast<float>(rgb[2]) }/255.0f;
				q.data.depth = depth;
				q.data.world = (idepth == 0) ? Eigen::Vector3f::Zero() : Backproject(q.position, depth);
				q.data.normal = Eigen::Vector3f::Zero();
			}
		}

		auto sp = ASLIC(img_data,
			ComputeSeeds(PDS_METHOD, img_data),
			[](const asp::Superpixel<PixelRgbd>& a, const Pixel<PixelRgbd>& b) {
				return COMPACTNESS * (a.data.world - b.data.world).squaredNorm() / (RADIUS * RADIUS)
					+ (1.0f - COMPACTNESS) * (a.data.color - b.data.color).squaredNorm();
			});

		std::cout << sp.superpixels.size() << " superpixels" << std::endl;

		return sp;
	}


}