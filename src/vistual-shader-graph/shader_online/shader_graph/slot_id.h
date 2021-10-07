#pragma once

/**
 * @file
 * @brief Defines SlotId.
 */

#include <cstddef>

#include "node_id.h"

namespace csg {

	/**
	 * @brief Class used to address a single slot, either input or output.
	 */
	class SlotId {
	public:
		SlotId(NodeId node_id, size_t index) : _node_id{ node_id }, _index{ index } {

        }

		NodeId node_id() const { return _node_id; }
        uint64_t slot_id() const { return _index + (0XFFFFFFFF00000000 &_node_id); }
		size_t index() const { return _index; }

		bool operator==(const SlotId& other) const;
		bool operator!=(const SlotId& other) const { return operator==(other) == false; }

		bool operator<(const SlotId& other) const;

	private:

        
        
		NodeId _node_id;
		size_t _index;

    };
}
