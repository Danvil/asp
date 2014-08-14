#include <asp/algos.hpp>
#include <asp/plot.hpp>
#include <slimage/opencv.hpp>
#include <slimage/io.hpp>
#include <slimage/gui.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/progress.hpp>
#include <iostream>

int main(int argc, char** argv)
{
	std::string p_img_rgb = "";
	std::string p_img_d = "";

	namespace po = boost::program_options;
	po::options_description desc;
	desc.add_options()
		("help", "produce help message")
		("rgb", po::value(&p_img_rgb), "path to input color image")
		("depth", po::value(&p_img_d), "path to input depth image")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	if(vm.count("help")) {
		std::cerr << desc << std::endl;
		return 1;
	}

	slimage::Image3ub img_input = slimage::Load3ub(p_img_rgb);
	slimage::GuiShow("input", img_input);

	// {
	// 	auto sp = asp::SLIC(img_input);

	// 	slimage::Image3ub img_asp = asp::Plot(sp)
	// 		<< asp::PlotDense(
	// 			[](const asp::Pixel<asp::PixelRgb>& u) {
	// 				return slimage::Pixel3ub{
	// 					static_cast<unsigned char>(255.0f*u.data.color[0]),
	// 					static_cast<unsigned char>(255.0f*u.data.color[1]),
	// 					static_cast<unsigned char>(255.0f*u.data.color[2])
	// 				};
	// 			})
	// 		<< asp::PlotBorder();
	// 	slimage::gui::Show("slic", img_asp, 3);
	// }

	{
		slimage::Image1ui16 img_depth = slimage::Load1ui16(p_img_d);

		auto sp = asp::DASP(img_input, img_depth);

		slimage::Image3ub img_dasp = asp::Plot(sp)
			<< asp::PlotDense(
				[](const asp::Pixel<asp::PixelRgbd>& u) {
					return slimage::Pixel3ub{
						static_cast<unsigned char>(255.0f*u.data.color[0]),
						static_cast<unsigned char>(255.0f*u.data.color[1]),
						static_cast<unsigned char>(255.0f*u.data.color[2])
					};
				})
			<< asp::PlotBorder();
		slimage::GuiShow("dasp", img_dasp);
	}

	slimage::GuiWait();

	return 0;
}
