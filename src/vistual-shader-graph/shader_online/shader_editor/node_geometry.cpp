#include "node_geometry.h"

#include <cmath>
#include <vector>

#include "../shader_graph/node.h"
#include "../shader_graph/node_type.h"

#include "graph_display.h"

cse::NodeGeometry::NodeGeometry(const csg::Node& node) :
	_pos{ csc::Float2{ node.position } },
	_size{ NODE_DEFAULT_WIDTH, NODE_HEADER_HEIGHT + NODE_ROW_HEIGHT * node.slots().size() }
{
	// Add extra width for some specific nodes
	switch (node.type()) {
	case csg::NodeType::PRINCIPLED_BSDF:
		_size = _size + csc::Float2{ 40.0f, 0.0f };
		break;
	case csg::NodeType::PRINCIPLED_HAIR:
		_size = _size + csc::Float2{ 50.0f, 0.0f };
		break;
	case csg::NodeType::PRINCIPLED_VOLUME:
		_size = _size + csc::Float2{ 30.0f, 0.0f };
		break;
    case csg::NodeType::IMAGE_TEX:
        _size = _size + csc::Float2{0.0f , 64.0f };
        break;
	default:
		// Do nothing
		{}
	}
}

csc::Float2 cse::NodeGeometry::pin_pos(const size_t index, const csg::SlotDirection direction) const
{
	const bool use_input{ direction == csg::SlotDirection::INPUT };
	const float x = use_input ? 0.0f : size().x;
	const float y = NODE_HEADER_HEIGHT + NODE_ROW_HEIGHT * index + NODE_ROW_HEIGHT / 2.0f;
	const csc::Float2 offset{ x, y };
	return pos() + offset;
}

boost::optional<size_t> cse::NodeGeometry::pin_at_pos(const csc::Float2 pos, const csg::SlotDirection direction) const
{
	constexpr float PIN_SELECT_SIZE{ 6.0f };
	if (this->rect().grow(PIN_SELECT_SIZE).contains(pos) == false) {
		// pos is not near this node, no pin
		return boost::none;
	}

	const csc::Float2 local_pos{ pos - _pos };

	// Check that the horizontal position lines up correctly based on direction
	const bool use_input{ direction == csg::SlotDirection::INPUT };
	const float target_x = use_input ? 0.0f : rect().size().x;
	if (fabs(local_pos.x - target_x) > PIN_SELECT_SIZE) {
		return boost::none;
	}

	// Check that the position isn't over the header
	if (local_pos.y < NODE_HEADER_HEIGHT) {
		return boost::none;
	}

	// Match the vertical position to a specific slot
	const size_t slot_index{ static_cast<size_t>( (local_pos.y - NODE_HEADER_HEIGHT) / NODE_ROW_HEIGHT ) };
	const csc::Float2 slot_pos{ 0.0f, NODE_HEADER_HEIGHT + NODE_ROW_HEIGHT * slot_index };

	// Find the offset from that slot
	const csc::Float2 slot_offset{ local_pos - slot_pos };

	// Check if the vertical position matches
	if (fabs(slot_offset.y - NODE_ROW_HEIGHT / 2.0f) > PIN_SELECT_SIZE) {
		return boost::none;
	}

	return slot_index;
}

csc::FloatRect cse::NodeGeometry::slot_rect(const size_t index) const
{
	const csc::Float2 begin{ pos() + csc::Float2{ 0.0f, NODE_HEADER_HEIGHT + NODE_ROW_HEIGHT * index } };
	const csc::Float2 end{ begin + csc::Float2{ size().x, NODE_ROW_HEIGHT } };
	return csc::FloatRect{ begin, end };
}

boost::optional<size_t> cse::NodeGeometry::slot_at_pos(const csc::Float2 pos) const
{
	if (rect().contains(pos) == false) {
		return boost::none;
	}
	const csc::Float2 local_pos{ pos - _pos };
	if (local_pos.y < NODE_HEADER_HEIGHT) {
		return boost::none;
	}
	return static_cast<size_t>((local_pos.y - NODE_HEADER_HEIGHT) / NODE_ROW_HEIGHT);
}
