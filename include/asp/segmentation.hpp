#pragma once

#include <slimage/image.hpp>
#include <Eigen/Dense>
#include <vector>

namespace asp {

/** A base segment in the hierarchy (pixel, superpixel, segment) */
template<typename T>
struct SegmentBase
{
	float num;
	Eigen::Vector2f position;
	float density;
	T data;

	bool valid() const
	{ return num > 0.0f; }

	static SegmentBase Zero()
	{ return {
		0.0f,
		Eigen::Vector2f::Zero(),
		0.0f,
		T::Zero()
	}; }

	SegmentBase& operator+=(const SegmentBase& x)
	{
		num += x.num;
		position += x.num * x.position;
		density += x.num * x.density;
		data += x.num * x.data;
		return *this;
	}

	friend SegmentBase operator*(float s, const SegmentBase& x)
	{ return {
		s * x.num,
		s * x.position,
		s * x.density,
		s * x.data
	}; }
	
};

template<typename T>
using Pixel = SegmentBase<T>;

/** Individual superpixel */
template<typename T>
struct Superpixel
: public SegmentBase<T>
{
	float radius;
};

/** Accumulate pixels */
template<typename T>
struct SegmentAccumulator
{
	SegmentAccumulator()
	:	sum_(SegmentBase<T>::Zero())
	{}

	void add(const SegmentBase<T>& v)
	{ sum_ += v; }

	bool empty() const
	{ return sum_.num == 0.0f; }

	SegmentBase<T> mean() const
	{
		if(empty()) {
			return sum_;
		}
		auto seg = (1.0f / static_cast<float>(sum_.num)) * sum_;
		seg.num = sum_.num; // preserve accumulated number
		return seg;
	}

private:
	SegmentBase<T> sum_;
};

/** Superpixel segmentation */
template<typename T>
struct Segmentation
{
	using sp_t = Superpixel<T>;
	slimage::Image<Pixel<T>,1> input;
	std::vector<sp_t> superpixels;
	slimage::Image<int,1> indices;
	slimage::Image<float,1> weights;
};

}
