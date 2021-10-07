#pragma once

/**
 * @file
 * @brief Defines NodeListSubwindow.
 */

#include <boost/optional.hpp>

#include "../shader_graph/node_type.h"

#include "event.h"

namespace cse {
	class NodeListSubwindow {
	public:
		NodeListSubwindow();
		
		InterfaceEventArray run() const;

		void do_event(const InterfaceEvent& event);

		boost::optional<csg::NodeType> selected_type() const;

	private:
		//csg::NodeCategory selected_category{ csg::NodeCategory::SHADER };
		boost::optional<csg::NodeType> _selected_type;
	};
}
