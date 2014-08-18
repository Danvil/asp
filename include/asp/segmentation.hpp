#pragma once

#include <slimage/image.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <Eigen/Dense>
#include <vector>

namespace asp {

/** Base type for segment in the hierarchy (pixel, superpixel, segment) */
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

/** Type of pixels */
template<typename T>
using Pixel = SegmentBase<T>;

/** Type of superpixels */
template<typename T>
struct Superpixel
: public SegmentBase<T>
{
	float radius;
};

/** Superpixel graph node */
template<typename T>
struct SegmentGraphNode
{
	Superpixel<T> data;
};

/** Superpixel graph edge */
struct SegmentGraphEdge
{
	float weight;
	std::vector<size_t> indices;
};

/** Superpixel graph */
template<typename T>
using SegmentGraph =
	boost::adjacency_list<
		boost::vecS, boost::vecS, boost::undirectedS,
		SegmentGraphNode<T>, // vertex type
		SegmentGraphEdge // edge type
	>;

/** Superpixel segmentation */
template<typename T>
struct Segmentation
{
	// original pixel data used for superpixel computation
	slimage::Image<Pixel<T>,1> input;
	
	// list of superpixels
	std::vector<Superpixel<T>> superpixels;

	// superpixel graph
	SegmentGraph<T> graph;
	
	// superpixel index for each pixel (can be used as an index into 'superpixels', -1 for no assignment)
	slimage::Image<int,1> indices;

	// pixel-superpixel distance for each pixel
	slimage::Image<float,1> weights;
};

}
