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
	std::string p_method;
	std::string p_fn_color;
	std::string p_fn_density;
	std::string p_fn_depth;
	std::string p_output;


	namespace po = boost::program_options;
	po::options_description desc;
	desc.add_options()
		("help", "produce help message")
		("method", po::value(&p_method)->default_value("SLIC"), "superpixel method: SLIC, ASP, DASP")
		("color", po::value(&p_fn_color), "path to input color image")
		("density", po::value(&p_fn_density), "path to input density image (required for ASP)")
		("depth", po::value(&p_fn_depth), "path to input depth image (required for DASP)")
		("output", po::value(&p_output)->default_value("/tmp/asp_"), "path/prefix for created images (optional)")
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
		auto vis_sp_color = VisualizeSuperpixelColor(sp);
		slimage::GuiShow("SLIC superpixel", vis_sp_color);
		slimage::GuiWait();
		// output of displayed images
		if(!p_output.empty()) {
			slimage::Save(p_output + "color.png", img_color);
			slimage::Save(p_output + "slic.png", vis_sp_color);
		}
	}

	else if(p_method == "ASP") {
		// load data
		slimage::Image3ub img_color = slimage::Load3ub(p_fn_color);
		slimage::GuiShow("pixel color", img_color);
		slimage::Image1f img_density =
			p_fn_density.empty()
			? slimage::Image1f{img_color.dimensions(),
				1000.0f / static_cast<float>(img_color.width()*img_color.height())}
			: slimage::Convert(slimage::Load1ui16(p_fn_density),
				[](uint16_t v) { return 1.0f / static_cast<float>(v); });
		// compute superpixels
		auto sp = asp::ASP(img_color, img_density);
		// visualize superpixels
		auto vis_px_density = VisualizePixelDensity(sp);
		auto vis_sp_color = VisualizeSuperpixelColor(sp);
		slimage::GuiShow("pixel density", vis_px_density);
		slimage::GuiShow("ASP superpixel", vis_sp_color);
		slimage::GuiWait();
		// output of displayed images
		if(!p_output.empty()) {
			slimage::Save(p_output + "color.png", img_color);
			slimage::Save(p_output + "density.png", vis_px_density);
			slimage::Save(p_output + "asp.png", vis_sp_color);
		}
	}

	else if(p_method == "DASP") {
		// load data
		slimage::Image3ub img_color = slimage::Load3ub(p_fn_color);
		slimage::GuiShow("pixel color", img_color);
		slimage::Image1ui16 img_depth = slimage::Load1ui16(p_fn_depth);
		auto vis_px_depth = slimage::Convert(slimage::Rescale(img_depth, 500, 3000),
				[](float v) { return asp::uf32_to_ui08(v); });
		slimage::GuiShow("pixel depth", vis_px_depth);
		// compute superpixels
		auto sp = asp::DASP(img_color, img_depth);
		// visualize superpixels
		auto vis_px_density = VisualizePixelDensity(sp);
		auto vis_px_normals = slimage::Convert(sp.input,
				[](const asp::Pixel<asp::PixelRgbd>& px) { return asp::sf32_to_ui08(px.data.normal); });
		auto vis_sp_color = VisualizeSuperpixelColor(sp);
		auto vis_sp_normals = VisualizeSuperpixelNormal(sp);
		slimage::GuiShow("pixel density", vis_px_density);
		slimage::GuiShow("pixel normals", vis_px_normals);
		slimage::GuiShow("DASP superpixel (color)", vis_sp_color);
		slimage::GuiShow("DASP superpixel (normal)", vis_sp_normals);
		slimage::GuiWait();
		// output of displayed images
		if(!p_output.empty()) {
			slimage::Save(p_output + "depth.png", vis_px_depth);
			slimage::Save(p_output + "density.png", vis_px_density);
			slimage::Save(p_output + "color.png", img_color);
			slimage::Save(p_output + "normals.png", vis_px_normals);
			slimage::Save(p_output + "dasp.png", vis_sp_color);
			slimage::Save(p_output + "dasp_normals.png", vis_sp_normals);
		}
	}
	
	else {
		std::cerr << "Unknown method. Use --h for help." << std::endl;
		return 1;
	}

	return 0;
}
