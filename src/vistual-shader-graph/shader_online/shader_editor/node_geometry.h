#pragma once

/**
 * @file
 * @brief Defines NodeGeometry.
 */

#include <cstddef>

#include <boost/optional.hpp>

#include "../shader_core/rect.h"
#include "../shader_core/vector.h"
#include "../shader_graph/slot.h"

namespace csg {
	class Node;
}

namespace cse {

	/**
	 * @brief Class used to measure the viewport geometry of csg::Nodes.
	 */
	class NodeGeometry {
	public:
		NodeGeometry(const csg::Node& node);

		csc::Float2 pos() const { return _pos; }
		csc::Float2 size() const { return _size; }
		csc::Float2 end() const { return _pos + _size; }
		csc::FloatRect rect() const { return csc::FloatRect{ _pos, end() }; }

		csc::Float2 pin_pos(size_t index, csg::SlotDirection direction) const;
		boost::optional<size_t> pin_at_pos(csc::Float2 pos, csg::SlotDirection direction) const;

		csc::FloatRect slot_rect(size_t index) const;
		boost::optional<size_t> slot_at_pos(csc::Float2 pos) const;

		NodeGeometry with_pos(csc::Float2 pos) const { return NodeGeometry{ pos, _size }; }

	private:
		NodeGeometry(csc::Float2 pos, csc::Float2 size) : _pos{ pos }, _size{ size } {}

		csc::Float2 _pos;
		csc::Float2 _size;
	};
}
