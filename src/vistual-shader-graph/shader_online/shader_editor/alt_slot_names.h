#pragma once

#include <boost/utility/string_view.hpp>

#include "../shader_graph/node.h"

namespace cse {
	boost::string_view get_alt_slot_name(const csg::Node& node, const boost::string_view& disp_name);
}
