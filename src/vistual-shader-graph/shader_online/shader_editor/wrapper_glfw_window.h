//#pragma once
//
//struct GLFWwindow;
//
//namespace cse {
//
//	// Class to safely wrap glfwCreateWindow and glfwDestroyWindow
//	class GlfwWindow {
//	public:
//		GlfwWindow(int width, int height, const char* title);
//		GlfwWindow(const GlfwWindow&) = delete;
//		~GlfwWindow();
//
//		bool valid() const;
//
//		GLFWwindow* const window_ptr;
//	};
//}
