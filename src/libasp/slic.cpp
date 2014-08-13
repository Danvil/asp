#include <asp/slic.hpp>
#include <asp/aslic.hpp>

namespace asp
{

	Segmentation<PixelRgb> SLIC(const slimage::Image3ub& img_input)
	{
		constexpr unsigned NUM_SUPERPIXELS = 1000;
		constexpr PoissonDiskSamplingMethod PDS_METHOD = PoissonDiskSamplingMethod::FloydSteinbergExpo;
		constexpr float COMPACTNESS = 0.1f;

		float density = static_cast<float>(NUM_SUPERPIXELS) / (img_input.width() * img_input.height());
		auto img_data = asp::Convert(img_input,
			[density](unsigned x, unsigned y, const slimage::Pixel3ub& px) {
				return asp::Pixel<asp::PixelRgb>{
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

		auto sp = asp::ASLIC(img_data,
			asp::ComputeSeeds(PDS_METHOD, img_data),
			[](const asp::Superpixel<asp::PixelRgb>& a, const asp::Pixel<asp::PixelRgb>& b) {
				return COMPACTNESS * (a.position - b.position).squaredNorm() / (a.radius * a.radius)
					+ (1.0f - COMPACTNESS) * (a.data.color - b.data.color).squaredNorm();
			});

		return sp;
	}


}