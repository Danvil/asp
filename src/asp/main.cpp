#include <asp/slic.hpp>
#include <asp/plot.hpp>
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

	auto sp = asp::SLIC(img_input);

	slimage::Image3ub img_asp = asp::Plot(sp)
		<< asp::PlotDense(
			[](const asp::Pixel<asp::PixelRgb>& u) {
				return slimage::Pixel3ub{
					static_cast<unsigned char>(255.0f*u.data.color[0]),
					static_cast<unsigned char>(255.0f*u.data.color[1]),
					static_cast<unsigned char>(255.0f*u.data.color[2])
				};
			})
		<< asp::PlotBorder();
	slimage::gui::Show("asp", img_asp, 3);

	slimage::gui::WaitForKeypress();

	return 0;
}
