#include "slot_id.h"
#include <mutex>
#include <random>


bool csg::SlotId::operator==(const SlotId& other) const
{
	return _node_id == other._node_id && _index == other._index;
}

bool csg::SlotId::operator<(const SlotId& other) const
{
	if (_node_id < other._node_id) return true;
	if (other._node_id < _node_id) return false;

	if (_index < other._index) return true;
	if (other._index < _index) return false;

	return false;
}

