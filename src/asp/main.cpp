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
	std::string p_fn_color;
	std::string p_fn_depth;
	std::string p_method;


	namespace po = boost::program_options;
	po::options_description desc;
	desc.add_options()
		("help", "produce help message")
		("color", po::value(&p_fn_color), "path to input color image")
		("depth", po::value(&p_fn_depth), "path to input depth image")
		("method", po::value(&p_method)->default_value("SLIC"), "superpixel method: SLIC, DASP")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	if(vm.count("help")) {
		std::cerr << desc << std::endl;
		return 1;
	}

	if(p_method == "SLIC") {
		// load data
		slimage::Image3ub img_color = slimage::Load3ub(p_fn_color);
		slimage::GuiShow("color", img_color);
		// compute superpixels
		auto sp = asp::SLIC(img_color);
		// visualize superpixels
		slimage::GuiShow("slic", PlotColor(sp));
		slimage::GuiWait();
	}
	else if(p_method == "DASP") {
		// load data
		slimage::Image3ub img_color = slimage::Load3ub(p_fn_color);
		slimage::GuiShow("color", img_color);
		slimage::Image1ui16 img_depth = slimage::Load1ui16(p_fn_depth);
		{
			slimage::Image1ui16 img_depth_vis = img_depth;
			for(auto& p : img_depth_vis) {
				p *= 16;
			}
			slimage::GuiShow("depth", img_depth_vis);
		}
		// compute superpixels
		auto sp = asp::DASP(img_color, img_depth);
		// visualize superpixels
		slimage::GuiShow("dasp", PlotColor(sp));
		slimage::GuiWait();
	}
	else {
		std::cerr << "Unknown method. Use --h for help." << std::endl;
		return 1;
	}

	return 0;
}
