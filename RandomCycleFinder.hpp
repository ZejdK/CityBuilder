


#pragma once
#include <vector>
#include "RoadGraphTypes.hpp"
#include <random>
#include <algorithm>
#include <cstdint>
#include <functional>
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/depth_first_search.hpp"
#include "boost/graph/detail/adjacency_list.hpp"



struct RandomCycleFinder {

	struct cycle_found {};

	struct CycleVisitor : boost::default_dfs_visitor {

		std::vector<RoadVertexDescriptor>& parent;
		std::vector<RoadVertexDescriptor>& cycle;

		CycleVisitor(std::vector<RoadVertexDescriptor>& parent, std::vector<RoadVertexDescriptor>& cycle)
			: parent(parent), cycle(cycle) {}

		void tree_edge(RoadEdgeDescriptor e, const RoadGraph& graph) const {

			RoadVertexDescriptor from = boost::source(e, graph);
			RoadVertexDescriptor to = boost::target(e, graph);

			parent[to] = from;
		}

		void back_edge(RoadEdgeDescriptor e, const RoadGraph& graph) const {

			RoadVertexDescriptor from = boost::source(e, graph);
			RoadVertexDescriptor to = boost::target(e, graph);

			cycle.push_back(to);
			for (RoadVertexDescriptor v = from; v != to; v = parent[v])
				cycle.push_back(v);
			cycle.push_back(to);

			throw cycle_found{};
		}
	};

	static std::vector<RoadVertexDescriptor> getRandomCycle(const RoadGraph& graph) {

		std::vector<RoadVertexDescriptor> parent(boost::num_vertices(graph));
		std::vector<uint8_t> color(boost::num_vertices(graph), 0);
		std::vector<RoadVertexDescriptor> cycle;

		static std::mt19937 rng { std::random_device{}() };

		std::function<bool(RoadVertexDescriptor)> dfs = [&](RoadVertexDescriptor from) -> bool {
			
			color[from] = 1; // gray - currently on DFS stack
			
			std::vector<RoadVertexDescriptor> neighbors;
			for (auto [e, end] = boost::out_edges(from, graph); e != end; ++e) {

				auto to = boost::target(*e, graph);
				if (parent[from] == to)
					continue;
				neighbors.push_back(to);
			}

			std::shuffle(neighbors.begin(), neighbors.end(), rng);

			for (auto to : neighbors) {

				if (color[to] == 0) {

					parent[to] = from;
					if (dfs(to))
						return true;
				}
				else if (color[to] == 1) { // Found an edge back to a vertex currently on the DFS stack => cycle; Reconstruct: to -> ... -> from -> to

					cycle.push_back(to);
					for (auto v = from; v != to; v = parent[v])
						cycle.push_back(v);
					cycle.push_back(to);

					return true;
				}
			}

			color[from] = 2; // black - completely explored
			return false;
		};

		std::vector<RoadVertexDescriptor> vertices; // randomize the starting point.
		for (auto [v, end] = boost::vertices(graph); v != end; ++v)
			vertices.push_back(*v);

		std::shuffle(vertices.begin(), vertices.end(), rng);

		for (auto start : vertices)
			if (color[start] == 0 && dfs(start))
				break;

		return cycle;
	}
};


