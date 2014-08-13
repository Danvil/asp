#pragma once

#include <Eigen/Dense>
#include <vector>

namespace asp {

enum class PoissonDiskSamplingMethod
{
	FloydSteinberg,
	FloydSteinbergExpo
};

std::vector<Eigen::Vector2f> PoissonDiskSampling(PoissonDiskSamplingMethod method, const Eigen::MatrixXf& density);

}
