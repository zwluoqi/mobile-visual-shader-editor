//#pragma once
//
//#include <memory>
//#include <mutex>
//
////#include <GLFW/glfw3.h>
//
//#include "wrapper_glfw_window.h"
//
//extern std::mutex glfw_global_mutex;
//
//inline void glfwPollEventsSafe(const std::unique_ptr<cse::GlfwWindow>& window)
//{
//	std::lock_guard<std::mutex> lock{ glfw_global_mutex };
//	glfwMakeContextCurrent(window->window_ptr);
//	glfwPollEvents();
//}
//
//inline void glfwGetFramebufferSize(const std::unique_ptr<cse::GlfwWindow>& window, int& width, int& height)
//{
//	glfwGetFramebufferSize(window->window_ptr, &width, &height);
//}
//
//inline void glfwMakeContextCurrent(const std::unique_ptr<cse::GlfwWindow>& window)
//{
//	glfwMakeContextCurrent(window->window_ptr);
//}
