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
	// 	slimage::gui::Show("slic", PlotColor(sp));
	// }

	{
		slimage::Image1ui16 img_depth = slimage::Load1ui16(p_img_d);
		auto sp = asp::DASP(img_input, img_depth);
		slimage::GuiShow("dasp", PlotColor(sp));
	}

	slimage::GuiWait();

	return 0;
}
