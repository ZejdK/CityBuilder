


#pragma once



#include <boost/graph/adjacency_list.hpp>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <optional>
#include "RoadGraphTypes.hpp"
#include "boost/graph/detail/adjacency_list.hpp"
#include <functional>



inline std::optional<std::vector<RoadEdgeDescriptor>> findRandomEdgesCycle(const RoadGraph& graph) {

    const auto numVertices = boost::num_vertices(graph);
    if (numVertices < 3)
        return std::nullopt;

    std::random_device rd;
    std::mt19937 rng(rd());

    std::vector<RoadVertexDescriptor> vertices;
    vertices.reserve(numVertices);

    for (auto [ it, end ] = boost::vertices(graph); it != end; ++it)
        vertices.push_back(*it);

    std::shuffle(vertices.begin(), vertices.end(), rng);

    for (RoadVertexDescriptor start : vertices)
    {
        std::unordered_map<RoadVertexDescriptor, size_t> pathIndex;
        std::vector<RoadVertexDescriptor> vertexPath;
        std::vector<RoadEdgeDescriptor> edgePath;

        std::function<std::optional<std::vector<RoadEdgeDescriptor>>(RoadVertexDescriptor)> dfs;

        dfs = [&](RoadVertexDescriptor v) -> std::optional<std::vector<RoadEdgeDescriptor>> {

            pathIndex[v] = vertexPath.size();
            vertexPath.push_back(v);

            std::vector<RoadEdgeDescriptor> edges;

            for (auto [it, end] = boost::out_edges(v, graph); it != end; ++it)
                edges.push_back(*it);

            std::shuffle(edges.begin(), edges.end(), rng);

            for (RoadEdgeDescriptor &e : edges)
            {
                RoadVertexDescriptor next = boost::target(e, graph);
                auto found = pathIndex.find(next);

                if (found != pathIndex.end())
                {
                    size_t cycleStart = found->second;
                    size_t cycleLength = vertexPath.size() - cycleStart;

                    if (cycleLength >= 3)
                    {
                        std::vector<RoadEdgeDescriptor> cycle;
                        for (size_t i = cycleStart; i < edgePath.size(); ++i)
                            cycle.push_back(edgePath[i]);
                        cycle.push_back(e);

                        return cycle;
                    }
                }
                else
                {
                    edgePath.push_back(e);
                    auto result = dfs(next);
                    if (result)
                        return result;

                    edgePath.pop_back();
                }
            }

            pathIndex.erase(v);
            vertexPath.pop_back();

            return std::nullopt;
        };

        auto result = dfs(start);
        if (result)
            return result;
    }

    return std::nullopt;
}



