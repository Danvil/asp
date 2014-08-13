#include <asp/asp.hpp>
#include <Slimage/IO.hpp>
#include <Slimage/Slimage.hpp>
#include <Slimage/Gui.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/progress.hpp>
#include <iostream>

int main(int argc, char** argv)
{
	std::string p_img = "";

	namespace po = boost::program_options;
	po::options_description desc;
	desc.add_options()
		("help", "produce help message")
		("img", po::value(&p_img), "path to input image")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	if(vm.count("help")) {
		std::cerr << desc << std::endl;
		return 1;
	}

	slimage::Image3ub img_input = slimage::Load3ub(p_img);
	slimage::gui::Show("input", img_input, 3);

	float density = 1000.0f / (img_input.width() * img_input.height());
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
		asp::ComputeSeeds(img_data),
		[](const asp::Superpixel<asp::PixelRgb>& a, const asp::Pixel<asp::PixelRgb>& b) {
			constexpr float COMPACTNESS = 0.1f;
			return COMPACTNESS * (a.position - b.position).squaredNorm() / (a.radius * a.radius)
				+ (1.0f - COMPACTNESS) * (a.data.color - b.data.color).squaredNorm();
		});

	slimage::Image3ub img_asp = asp::plot::Plot(sp)
		<< asp::plot::Dense(
			[](const asp::Pixel<asp::PixelRgb>& u) {
				return slimage::Pixel3ub{
					static_cast<unsigned char>(255.0f*u.data.color[0]),
					static_cast<unsigned char>(255.0f*u.data.color[1]),
					static_cast<unsigned char>(255.0f*u.data.color[2])
				};
			})
		<< asp::plot::Border();
	slimage::gui::Show("asp", img_asp, 3);

	slimage::gui::WaitForKeypress();

	return 0;
}
