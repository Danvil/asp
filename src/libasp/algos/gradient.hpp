#pragma once

#include <slimage/image.hpp>
#include <Eigen/Dense>
#include <cmath>

namespace asp
{

	template<typename K>
	float LocalFiniteDifferencesKinect(K v0, K v1, K v2, K v3, K v4)
	{
		if(v0 == 0 && v4 == 0 && v1 != 0 && v3 != 0) {
			return float(v3 - v1);
		}

		bool left_invalid = (v0 == 0 || v1 == 0);
		bool right_invalid = (v3 == 0 || v4 == 0);
		if(left_invalid && right_invalid) {
			return 0.0f;
		}
		else if(left_invalid) {
			return float(v4 - v2);
		}
		else if(right_invalid) {
			return float(v2 - v0);
		}
		else {
			float a = static_cast<float>(std::abs(v2 + v0 - static_cast<K>(2)*v1));
			float b = static_cast<float>(std::abs(v4 + v2 - static_cast<K>(2)*v3));
			float p, q;
			if(a + b == 0.0f) {
				p = q = 0.5f;
			}
			else {
				p = a / (a + b);
				q = b / (a + b);
			}
			return q * static_cast<float>(v2 - v0) + p * static_cast<float>(v4 - v2);
		}
	}

	inline
	Eigen::Vector2f LocalDepthGradient(const slimage::Image1ui16& depth, unsigned int j, unsigned int i, float base_radius_m, float focal_px, float depth_to_z)
	{
		uint16_t d00 = depth(j,i);

		float z_over_f = static_cast<float>(d00) * depth_to_z / focal_px;
		float window = base_radius_m/z_over_f;

		// compute w = base_scale*f/d
		unsigned int w = std::max(static_cast<unsigned int>(window + 0.5f), 4u);
		if(w % 2 == 1) w++;

		// can not compute the gradient at the border, so return 0
		if(i < w || depth.height() - w <= i || j < w || depth.width() - w <= j) {
			return Eigen::Vector2f::Zero();
		}

		float dx = LocalFiniteDifferencesKinect<int>(
			depth(j-w,i),
			depth(j-w/2,i),
			d00,
			depth(j+w/2,i),
			depth(j+w,i)
		);

		float dy = LocalFiniteDifferencesKinect<int>(
			depth(j,i-w),
			depth(j,i-w/2),
			d00,
			depth(j,i+w/2),
			depth(j,i+w)
		);

		// Theoretically scale == base_scale, but w must be an integer, so we
		// compute scale from the actually used w.

		// compute 1 / scale = 1 / (w*d/f)
		float scl = 1.0f / (float(w) * z_over_f);

		return (scl*depth_to_z) * Eigen::Vector2f(static_cast<float>(dx), static_cast<float>(dy));
	}

	/** Computes normal from gradient and assures that it points towards the camera (which is in 0) */
	inline
	Eigen::Vector3f NormalFromGradient(const Eigen::Vector2f& g, const Eigen::Vector3f& position)
	{
		const float gx = g.x();
		const float gy = g.y();
		const float scl = 1.0f / std::sqrt(1.0f + gx*gx + gy*gy); // Danvil::MoreMath::FastInverseSqrt
		Eigen::Vector3f normal(scl*gx, scl*gy, -scl);
		// force normal to look towards the camera
		// check if point to camera direction and normal are within 90 deg
		// enforce: normal * (cam_pos - pos) > 0
		// do not need to normalize (cam_pos - pos) as only sign is considered
		const float q = normal.dot(-position);
		if(q < 0) {
			normal *= -1.0f;
		}
		return normal;
	}

}
