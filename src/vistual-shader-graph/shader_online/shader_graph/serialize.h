#pragma once

#include <string>

#include <boost/optional.hpp>

namespace csg {
	class Graph;

	std::string serialize_graph(const Graph& graph);
	
	boost::optional<Graph> deserialize_graph(const std::string& graph_string);
}
