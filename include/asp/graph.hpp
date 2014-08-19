#pragma once

#include <asp/segmentation.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>
#include <iostream>

namespace asp
{

	/** Superpixel connectivity graph */
	template<typename T>
	using SegmentBorderGraph =
		boost::adjacency_list<
			boost::vecS, boost::vecS, boost::undirectedS,
			Superpixel<T>, // superpixel
			std::vector<size_t> // border pixel indices
		>;

	template<typename T>
	using SegmentGraph =
		boost::adjacency_list<
			boost::vecS, boost::vecS, boost::undirectedS,
			Superpixel<T>, // superpixel
			float // edge weight
		>;

	namespace detail
	{
		template<class Iter>
		struct iter_pair_range
		: std::pair<Iter,Iter>
		{
			iter_pair_range(const std::pair<Iter,Iter>& x)
			: std::pair<Iter,Iter>(x)
			{}

			Iter begin() const
			{ return this->first; }

			Iter end() const
			{ return this->second; }
		};

		template<class Iter>
		inline iter_pair_range<Iter> as_range(const std::pair<Iter,Iter>& x)
		{ return iter_pair_range<Iter>(x); }

		struct edge_t
		{ int a, b; };

		inline
		bool operator==(const edge_t& u, const edge_t& v)
		{ return u.a == v.a && u.b == v.b; }

		inline
		bool operator<(const edge_t& u, const edge_t& v)
		{ return u.a < v.a || (u.a == v.a && u.b < v.b); }

		inline
		std::map<edge_t,std::vector<size_t>> FindBorders(const slimage::Image<int,1>& indices)
		{
			std::map<edge_t,std::vector<size_t>> result;
			const int width = indices.width();
			const int height = indices.height();
			size_t k = 0;
			for(int y=0; y<height-1; y++) {
				for(int x=0; x<width-1; x++, k++) {
					int i0 = indices(x,y);
					if(i0 == -1) {
						continue;
					}
					int i1 = indices(x+1,y);
					int i2 = indices(x,y+1);
					if(i0 != i1 && i1 != -1) {
						auto& r = result[{i0,i1}];
						r.push_back(k);
						r.push_back(k+1);
					}
					if(i0 != i2 && i2 != -1) {
						auto& r = result[{i0,i2}];
						r.push_back(k);
						r.push_back(k+width);
					}
				}
			}
			return result;
		}
	}

	/** Creates a segment neighbourhood graph from a segmentation */
	template<typename T>
	SegmentBorderGraph<T> CreateSegmentBorderGraph(const Segmentation<T>& seg)
	{
		using graph_t = SegmentBorderGraph<T>;
		// create superpixel graph (one node per superpixel)
		graph_t ng(seg.superpixels.size());
		for(const auto& vid : detail::as_range(boost::vertices(ng))) {
			ng[vid] = seg.superpixels[vid]; // TODO correctly convert vertex descriptor to superpixel id
		}
		// find borders
		auto borders = detail::FindBorders(seg.indices);
		// create edges
		for(const auto& q : borders) {
			auto r = boost::add_edge(q.first.a, q.first.b, ng); // TODO correctly convert superpixel id to vertex descriptor
			assert(r.second);
			ng[r.first] = q.second;
		}
		return ng;
	}

	/** Creates a weighted segment neighbourhood graph */
	template<typename T, typename F>
	SegmentGraph<T> CreateSegmentGraph(const SegmentBorderGraph<T>& border_graph, F dist)
	{
		using input_graph_t = SegmentBorderGraph<T>;
		using result_graph_t = SegmentGraph<T>;
		result_graph_t result;
		boost::copy_graph(border_graph, result,
			boost::vertex_copy(
				[&border_graph,&result,&dist](typename input_graph_t::vertex_descriptor src, typename result_graph_t::vertex_descriptor dst) {
					result[dst] = border_graph[src];
				}
			)
			.edge_copy(
				[&border_graph,&result,&dist](typename input_graph_t::edge_descriptor src, typename result_graph_t::edge_descriptor dst) {
					result[dst] = dist(border_graph[boost::source(src,border_graph)], border_graph[boost::target(src,border_graph)], border_graph[src]);
				}
			)
		);
		return result;
	}

}