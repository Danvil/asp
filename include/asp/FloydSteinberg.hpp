#pragma once

#include <Eigen/Dense>
#include <vector>

namespace asp {

std::vector<Eigen::Vector2f> FloydSteinberg(const Eigen::MatrixXf& density);

std::vector<Eigen::Vector2f> FloydSteinbergExpo(const Eigen::MatrixXf& density);

}
