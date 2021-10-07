#pragma once

/**
 * @file
 * @brief Defines Connection and Graph.
 */

#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>

#include <boost/optional.hpp>

#include "node_id.h"
#include "node_type.h"
#include "slot.h"
#include "slot_id.h"
#include "code_generate.h"

namespace csc {
	struct Float2;
	struct Float3;
	struct Int2;
}

namespace csg {
	class Node;

	enum class GraphType {
		EMPTY,
		MATERIAL,
	};

	/**
	 * @brief Used to represent a connection between two node slots.
	 */
	class Connection {
	public:
		Connection(SlotId source, SlotId dest) : _source{ source }, _dest{ dest } {}

		SlotId source() const { return _source; }
		SlotId dest() const { return _dest; }

		bool operator<(const Connection& other) const;

	private:
		SlotId _source;
		SlotId _dest;
	};

	/**
	 * @brief Class to manage and operate on a shader graph.
	 */
	class Graph {
	public:
		// Deserialize a graph
		static boost::optional<Graph> from(const std::string& graph_string);

		Graph(GraphType type);

		// Copy constructor and copy assignment operator, constructor defers to assignment
		Graph(const Graph& other) { this->operator=(other); }
		Graph& operator=(const Graph& other);

		std::shared_ptr<const Node> get(NodeId id) const;
		boost::optional<SlotValue> get_slot_value(SlotId slot_id) const;
		template <typename T> boost::optional<T> get_slot_value_as(SlotId slot_id) const
		{
			const boost::optional<SlotValue> opt_value{ get_slot_value(slot_id) };
			if (opt_value) {
				return opt_value->as<T>();
			}
			else {
				return boost::none;
			}
		}

		NodeId add(NodeType type, csc::Int2 pos);
		bool add(NodeType type, csc::Int2 pos, NodeId id);
		void remove(const std::set<NodeId>& ids);
		boost::optional<NodeId> duplicate(NodeId node_id);

		bool add_connection(SlotId source, SlotId dest);
		boost::optional<Connection> remove_connection(SlotId dest);
		boost::optional<Connection> get_connection(SlotId dest) const;

		bool set_bool(SlotId slot_id, bool new_value);
		bool set_color(SlotId slot_id, csc::Float3 new_value);
		bool set_enum(SlotId slot_id, size_t new_value);
		bool set_float(SlotId slot_id, float new_value);
		bool set_int(SlotId slot_id, int new_value);
		bool set_vector(SlotId slot_id, csc::Float3 new_value);
		bool set_color_ramp(SlotId slot_id, const ColorRampSlotValue& new_value);
		bool set_curve_rgb(SlotId slot_id, const RGBCurveSlotValue& new_value);
		bool set_curve_vec(SlotId slot_id, const VectorCurveSlotValue& new_value);
        bool set_image_value(SlotId slot_id, const ImageSlotValue& new_value);

		void move(const std::set<NodeId>& ids, csc::Float2 delta);
		void raise(NodeId id);

		bool contains(NodeId id) const;

		const std::list<std::shared_ptr<Node>>& nodes() const { return _nodes; }
		const std::list<Connection> connections() const { return _connections; }

		std::string serialize() const;
		std::shared_ptr<csg::CodeGenerateData> generate_code();
		

		bool operator==(const Graph& other) const;
		bool operator!=(const Graph& other) const { return (operator==(other) == false); }

		int getOrderByNodeId(const NodeId node_id);
        bool hasOrderByNodeId(const NodeId node_id);
		//void csg::Graph::setNodeOrders();
		std::string GetSlotName(const SlotId& slotId);

	private:
		std::list<std::shared_ptr<Node>> _nodes;
		std::list<Connection> _connections;

		std::map<NodeId, std::shared_ptr<Node>> nodes_by_id;
        
        //add info to process code
		std::map<NodeId, int> nodes_by_order;
	};
}
