#pragma once

/**
 * @file
 * @brief Defines functions that allow a MainWindow to be hooked up with glfw callbacks.
 */

//struct GLFWwindow;
//#include "entry/entry.h"

namespace cse {

	class MainWindow;

	void register_window_pair_for_callbacks(MainWindow* main_window);
	void iteration_window_pair_for_callbacks();
}
