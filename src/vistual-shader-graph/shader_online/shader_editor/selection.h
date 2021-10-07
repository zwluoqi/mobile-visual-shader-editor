#pragma once

/**
 * @file
 * @brief Defines NodeSelection.
 */

#include <cstddef>
#include <set>

#include "../shader_graph/node_id.h"

#include "enum.h"

namespace cse {

	/**
	 * @brief Class used to track and manage the set of nodes a user has selected.
	 */
	class NodeSelection {
	public:
		NodeSelection() {}

		void select(SelectMode mode, csg::NodeId id);
		void clear() { _selected.clear(); }

		size_t count() const { return _selected.size(); }
		bool is_selected(csg::NodeId id) const { return static_cast<bool>(_selected.count(id)); }

		const std::set<csg::NodeId>& selected() const { return _selected; }

	private:
		std::set<csg::NodeId> _selected;
	};
}
