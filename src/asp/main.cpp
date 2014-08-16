#include <asp/algos.hpp>
#include <asp/plot.hpp>
#include <slimage/opencv.hpp>
#include <slimage/io.hpp>
#include <slimage/gui.hpp>
#include <slimage/algorithm.hpp>
#include <boost/program_options.hpp>
#include <iostream>

int main(int argc, char** argv)
{
	std::string p_fn_color;
	std::string p_fn_depth;
	std::string p_fn_density;
	std::string p_method;


	namespace po = boost::program_options;
	po::options_description desc;
	desc.add_options()
		("help", "produce help message")
		("color", po::value(&p_fn_color), "path to input color image")
		("depth", po::value(&p_fn_depth), "path to input depth image")
		("density", po::value(&p_fn_density), "path to input density image")
		("method", po::value(&p_method)->default_value("SLIC"), "superpixel method: SLIC, ASP, DASP")
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
		slimage::GuiShow("pixel color", img_color);
		// compute superpixels
		auto sp = asp::SLIC(img_color);
		// visualize superpixels
		slimage::GuiShow("SLIC superpixel", VisualizeSuperpixelColor(sp));
		slimage::GuiWait();
	}

	else if(p_method == "ASP") {
		// load data
		slimage::Image3ub img_color = slimage::Load3ub(p_fn_color);
		slimage::GuiShow("pixel color", img_color);
		slimage::Image1ui16 img_density_ui16 = slimage::Load1ui16(p_fn_density);
		slimage::Image1f img_density = slimage::Convert(img_density_ui16,
			[](uint16_t v) { return 1.0f / static_cast<float>(v); });
		slimage::GuiShow("pixel density", slimage::Rescale(img_density));
		// compute superpixels
		auto sp = asp::ASP(img_color, img_density);
		// visualize superpixels
		slimage::GuiShow("ASP superpixel", VisualizeSuperpixelColor(sp));
		slimage::GuiWait();
	}

	else if(p_method == "DASP") {
		// load data
		slimage::Image3ub img_color = slimage::Load3ub(p_fn_color);
		slimage::GuiShow("pixel color", img_color);
		slimage::Image1ui16 img_depth = slimage::Load1ui16(p_fn_depth);
		slimage::GuiShow("pixel depth", slimage::Rescale(img_depth, 500, 3000));
		// compute superpixels
		auto sp = asp::DASP(img_color, img_depth);
		// visualize superpixels
		slimage::GuiShow("pixel normals",
			slimage::Convert(sp.input,
				[](const asp::Pixel<asp::PixelRgbd>& px) { return asp::sf32_to_ui08(px.data.normal); }));
		slimage::GuiShow("DASP superpixel (color)", VisualizeSuperpixelColor(sp));
		slimage::GuiShow("DASP superpixel (normal)", VisualizeSuperpixelNormal(sp));
		slimage::GuiWait();
	}
	
	else {
		std::cerr << "Unknown method. Use --h for help." << std::endl;
		return 1;
	}

	return 0;
}
