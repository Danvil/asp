#pragma once

#include <asp/FloydSteinberg.hpp>
#include <Eigen/Dense>
#include <Slimage/Slimage.hpp>
#include <vector>
#include <tuple>
#include <type_traits>
#include <cmath>
#include <limits>

namespace asp {

template<typename T>
using Image = slimage::Image<slimage::Traits<T,1>>;

/** A base segment in the hierarchy (pixel, superpixel, segment) */
template<typename T>
struct SegmentBase
{
	float num;
	Eigen::Vector2f position;
	float density;
	T data;

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

/** Compute superpixel radius from density */
inline
float DensityToRadius(float density)
{ return std::sqrt(1.0f / (density*3.1415f)); } // rho = 1 / (r*r*pi) => r = sqrt(rho/pi)

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
	std::vector<sp_t> superpixels;
	Image<int> indices;
	Image<float> weights;
};

/** RGB pixel data */
struct PixelRgb
{
	Eigen::Vector3f color;

	static PixelRgb Zero()
	{ return {
		Eigen::Vector3f::Zero()
	}; }

	PixelRgb& operator+=(const PixelRgb& x)
	{
		color += x.color;
		return *this;
	}

	friend PixelRgb operator*(float s, const PixelRgb& x)
	{ return {
		s * x.color
	}; }
	
};

/** Pixel-wise image conversion */
template<typename SRC, typename F>
auto Convert(const slimage::Image<SRC>& src, F conv)
-> slimage::Image<slimage::Traits<typename std::decay<decltype(conv(0,0,src[0]))>::type,1>>
{
	const unsigned width = src.width();
	const unsigned height = src.height();
	slimage::Image<slimage::Traits<typename std::decay<decltype(conv(0,0,src[0]))>::type,1>> dst{width, height};
	for(unsigned y=0, i=0; y<height; y++) {
		for(unsigned x=0; x<width; x++, i++) {
			dst[i] = conv(x,y,src[i]);
		}
	}
	return dst;
}

/** Superpixel seed */
struct Seed
{
	Eigen::Vector2f position;
	float density;
};

/** Compute seeds accordingly to pixel density values */
template<typename T>
std::vector<Seed> ComputeSeeds(const Image<Pixel<T>>& input)
{
	const unsigned width = input.width();
	const unsigned height = input.height();
	Eigen::MatrixXf density{width, height};
	for(unsigned y=0; y<height; y++) {
		for(unsigned x=0; x<width; x++) {
			density(x,y) = ((Pixel<T>&)input(x,y)).density;
		}
	}
	std::vector<Eigen::Vector2f> pntseeds = FloydSteinbergExpo(density);
	std::vector<Seed> seeds(pntseeds.size());
	for(unsigned i=0; i<pntseeds.size(); i++) {
		auto& sp = seeds[i];
		sp.position = pntseeds[i];
		const Pixel<T>& inp_px = input(std::floor(sp.position.x()), std::floor(sp.position.y()));
		sp.density = inp_px.density; // FIXME use bb?
	}
	return seeds;
}

namespace impl
{
	std::tuple<int,int> GetRange(int a, int b, float x, float w)
	{
		return std::make_tuple(
			std::max<int>(a, std::floor(x - w)),
			std::min<int>(b, std::ceil(x + w))
		);
	}
}

/** Adaptive SLIC superpixel algorithm */
template<typename T, typename F>
Segmentation<T> ASLIC(const Image<Pixel<T>>& input, const std::vector<Seed>& seeds, F dist)
{
	constexpr unsigned ITERATIONS = 10;
	constexpr float LAMBDA = 3.0f;
	const unsigned width = input.width();
	const unsigned height = input.height();
	// initialize
	using sp_t = typename Segmentation<T>::sp_t;
	Segmentation<T> s;
	s.superpixels.resize(seeds.size());
	for(size_t i=0; i<seeds.size(); i++) {
		const Seed& seed = seeds[i];
		auto& sp = s.superpixels[i];
		reinterpret_cast<SegmentBase<T>&>(sp) = reinterpret_cast<const SegmentBase<T>&>(
			(Pixel<T>&)input(std::floor(seed.position.x()), std::floor(seed.position.y())));
		sp.num = 1.0f;
		sp.position = seed.position;
		sp.density = seed.density;
		sp.radius = DensityToRadius(sp.density);
	}
	s.indices = Image<int>{width, height};
	s.weights = slimage::Image1f{width, height};
	// iterate
	for(unsigned k=0; k<ITERATIONS; k++) {
		// reset weights
		s.indices.fill(-1);
		s.weights.fill(std::numeric_limits<float>::max());
		// iterate over all superpixels
		for(size_t sid=0; sid<s.superpixels.size(); sid++) {
			const sp_t& sp = s.superpixels[sid];
			// compute superpixel bounding box
			int x1, x2, y1, y2;
			std::tie(x1,x2) = impl::GetRange(0,  width, sp.position.x(), LAMBDA*sp.radius);
			std::tie(y1,y2) = impl::GetRange(0, height, sp.position.y(), LAMBDA*sp.radius);
			// iterate over superpixel bounding box
			for(int y=y1; y<y2; y++) {
				for(int x=x1; x<x2; x++) {
					float d = dist(sp, input(x,y));
					if(d < s.weights(x,y)) {
						s.weights(x,y) = d;
						s.indices(x,y) = sid;
					}
				}
			}
		}
		// update superpixels
		std::vector<SegmentAccumulator<T>> acc(s.superpixels.size(), SegmentAccumulator<T>{});
		for(unsigned y=0; y<s.indices.height(); y++) {
			for(unsigned x=0; x<s.indices.width(); x++) {
				int sid = s.indices(x,y);
				if(sid >= 0) {
					acc[sid].add(input(x,y));
				}
			}
		}
		for(size_t i=0; i<s.superpixels.size(); i++) {
			auto& sp = s.superpixels[i];
			reinterpret_cast<SegmentBase<T>&>(sp) = acc[i].mean();
			sp.radius = DensityToRadius(sp.density);
		}
	}
	return s;
}

namespace plot
{
	template<typename T>
	struct SPlot
	{
		slimage::Image3ub vis;
		Segmentation<T> seg;

		SPlot(const Segmentation<T>& seg)
		:	vis{seg.indices.width(), seg.indices.height(), slimage::Pixel3ub{0,0,0}},
			seg(seg)
		{}

		operator const slimage::Image3ub&() const
		{ return vis; }
	};

	template<typename T>
	SPlot<T> Plot(const Segmentation<T>& seg)
	{ return SPlot<T>(seg); }

	template<typename F>
	struct SDense
	{
		F colfnc;
		slimage::Pixel3ub invalid;
	};

	template<typename F>
	SDense<F> Dense(F colfnc)
	{ return {
		colfnc,
		slimage::Pixel3ub{255,0,255}
	}; }

	struct SBorder
	{
		slimage::Pixel3ub color;
	};

	inline
	SBorder Border(const slimage::Pixel3ub& color = slimage::Pixel3ub{0,0,0})
	{ return SBorder{ color }; }

	template<typename T, typename F>
	SPlot<T> operator<<(SPlot<T>&& p, const SDense<F>& u)
	{
		for(size_t i=0; i<p.seg.indices.size(); i++) {
			int sid = p.seg.indices[i];
			p.vis[i] = (sid >= 0) ? u.colfnc(p.seg.superpixels[sid]) : u.invalid;
		}
		return p;
	}

	template<typename T>
	SPlot<T> operator<<(SPlot<T>&& p, const SBorder& u)
	{
		const int width = p.vis.width();
		const int height = p.vis.height();
		for(int y=0; y<height; y++) {
			int ym = std::max(y-1, 0);
			int yp = std::min(y+1, height);
			for(int x=0; x<width; x++) {
				int xm = std::max(x-1, 0);
				int xp = std::min(x+1, width);
				int i = p.seg.indices(x,y);
				if(    i != p.seg.indices(xm,y)
					|| i != p.seg.indices(xp,y)
					|| i != p.seg.indices(x,ym)
					|| i != p.seg.indices(x,yp)) {
					p.vis(x,y) = u.color;
				}
			}
		}
		return p;
	}
}

}