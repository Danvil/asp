#include <slimage/algorithm.hpp>
#include <asp/algos.hpp>
#include <asp/aslic.hpp>

namespace asp
{

	Segmentation<PixelRgb> SLIC(const slimage::Image3ub& img_rgb)
	{
		constexpr unsigned NUM_SUPERPIXELS = 1000;
		constexpr PoissonDiskSamplingMethod PDS_METHOD = PoissonDiskSamplingMethod::FloydSteinbergExpo;
		constexpr float COMPACTNESS = 0.1f;

		float density = static_cast<float>(NUM_SUPERPIXELS) / (img_rgb.width() * img_rgb.height());
		auto img_data = slimage::ConvertUV(img_rgb,
			[density](unsigned x, unsigned y, const slimage::Pixel3ub& px) {
				return Pixel<PixelRgb>{
					1.0f,
					{
						static_cast<float>(x),
						static_cast<float>(y)
					},
					density,
					{
						Eigen::Vector3f{
							static_cast<float>(px[0]),
							static_cast<float>(px[1]),
							static_cast<float>(px[2])
						}/255.0f
					}
				};
			});

		auto sp = ASLIC(img_data,
			ComputeSeeds(PDS_METHOD, img_data),
			[](const Superpixel<PixelRgb>& a, const Pixel<PixelRgb>& b) {
				return COMPACTNESS * (a.position - b.position).squaredNorm() / (a.radius * a.radius)
					+ (1.0f - COMPACTNESS) * (a.data.color - b.data.color).squaredNorm();
			});

		return sp;
	}


}