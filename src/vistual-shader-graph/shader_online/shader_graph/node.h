#pragma once

/**
 * @file
 * @brief Defines Node.
 */

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <boost/optional.hpp>
#include <boost/utility/string_view.hpp>

#include "../shader_core/vector.h"

#include "node_id.h"
#include "node_type.h"
#include "slot.h"

namespace csg {

	/**
	 * @brief A single node in a shader graph.
	 */
	class Node {
	public:
		Node(NodeType type, csc::Int2 position);
		Node(NodeType type, csc::Int2 position, NodeId id);

		NodeId id() const { return _id; }
		NodeType type() const { return _type; }
		const std::vector<Slot>& slots() const { return _slots; }
		
		boost::optional<size_t> slot_index(SlotDirection dir, const boost::string_view& slot_name) const;
		boost::optional<Slot> slot(size_t index) const;
		boost::optional<Slot> slot(SlotDirection dir, const boost::string_view& slot_name) const;
		boost::optional<SlotValue> slot_value(size_t index) const;
		boost::optional<SlotValue> slot_value(const boost::string_view& slot_name) const;
		Slot& slot_ref(size_t index) { return _slots[index]; }

		template <typename T> boost::optional<T> slot_value_as(size_t index) const
		{
			const boost::optional<SlotValue> opt_value{ slot_value(index) };
			if (opt_value) {
				return opt_value->as<T>();
			}
			else {
				return boost::none;
			}
		}

		template <typename T> boost::optional<T> slot_value_as(const std::string& slot_name) const
		{
			const boost::optional<SlotValue> opt_value{ slot_value(slot_name) };
			if (opt_value) {
				return opt_value->as<T>();
			}
			else {
				return boost::none;
			}
		}

		void copy_from(const Node& other);

		bool has_pin(size_t index, SlotDirection direction) const { return index < _slots.size() && _slots[index].dir() == direction; }

		bool operator==(const Node& other) const;
		bool operator!=(const Node& other) const { return operator==(other) == false; }

		csc::Int2 position;
        SlotDirection slot_dir(size_t index) const {
            const auto& slot_r = _slots[index];
            return slot_r.dir();
        }

	private:
		NodeId roll_id();

		NodeId _id;
		NodeType _type;
		std::vector<Slot> _slots;
		std::vector<std::pair<const char*, const char*>> _slot_aliases;
	};
}
