#pragma once

/**
 * @file
 * @brief Defines an abstraction over all platform-specific functionality.
 */

#include <string>

#include <boost/optional.hpp>

namespace cse {
	namespace Platform {
		bool save_graph_dialog();
		void load_graph_dialog();
        void open_image_dialog();
	}
}
