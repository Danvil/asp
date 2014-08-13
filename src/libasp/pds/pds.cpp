#include <asp/pds.hpp>

namespace asp
{

std::vector<Eigen::Vector2f> FloydSteinberg(const Eigen::MatrixXf& density_inp);
std::vector<Eigen::Vector2f> FloydSteinbergExpo(const Eigen::MatrixXf& density_inp);

std::vector<Eigen::Vector2f> PoissonDiskSampling(PoissonDiskSamplingMethod method, const Eigen::MatrixXf& density)
{
	#define OPT(Q) case PoissonDiskSamplingMethod::Q: return Q(density);
	switch(method) {
		OPT(FloydSteinberg)
		OPT(FloydSteinbergExpo)
		default: return {};
	}
}

}
