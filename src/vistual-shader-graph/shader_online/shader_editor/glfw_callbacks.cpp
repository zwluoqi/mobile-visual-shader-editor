#include "glfw_callbacks.h"

//#include <GLFW/glfw3.h>

#include "main_window.h"
#include "imgui/imgui.h"
#include "entry/entry.h"
#include "entry/input.h"

thread_local cse::MainWindow* threadlocal_main_window{ nullptr };

//static void callback_character(const unsigned int codepoint)
//{
//	if (threadlocal_main_window != nullptr) {
//		threadlocal_main_window->callback_character(codepoint);
//	}
//}

static void callback_cursor_pos(const double x, const double y)
{
	if (threadlocal_main_window != nullptr) {
		threadlocal_main_window->callback_cursor_pos(x, y);
	}
}

static void callback_key(const int key, const int scancode, const int action, const int mods)
{
	if (threadlocal_main_window != nullptr) {
		threadlocal_main_window->callback_key(key, scancode, action, mods);
	}
}

static void callback_mouse_button(const int button, const int action, const int mods)
{
	if (threadlocal_main_window != nullptr) {
		threadlocal_main_window->callback_mouse_button(button, action, mods);
	}
}

static void callback_scroll(const double xoffset, const double yoffset)
{
	if (threadlocal_main_window != nullptr) {
		threadlocal_main_window->callback_scroll(xoffset, yoffset);
	}
}

static void callback_drop_file(const bx::FilePath & filePath,const int result)
{
    if (threadlocal_main_window != nullptr) {
        threadlocal_main_window->callback_drop_file(filePath,result);
    }
}

static void callback_save_file(const bx::FilePath & filePath,const int result)
{
    if (threadlocal_main_window != nullptr) {
        threadlocal_main_window->callback_save_file(filePath,result);
    }
}


void cse::register_window_pair_for_callbacks(MainWindow* const main_window)
{
	threadlocal_main_window = main_window;
	//glfwSetCharCallback(glfw_window, callback_character);
	//glfwSetCursorPosCallback(glfw_window, callback_cursor_pos);
	//glfwSetKeyCallback(glfw_window, callback_key);
	//glfwSetMouseButtonCallback(glfw_window, callback_mouse_button);
	//glfwSetScrollCallback(glfw_window, callback_scroll);
}

void cse::iteration_window_pair_for_callbacks() {

	ImGuiIO& io = ImGui::GetIO();
	callback_cursor_pos(io.MousePos.x, io.MousePos.y);

	for (int32_t ii = 0; ii < 3; ++ii) {
		if (io.MouseClicked[ii]) {
			callback_mouse_button(ImGuiMouseButton_Left + ii, entry::ClickAction::Pressing, io.KeyMods);
		}
		if (io.MouseReleased[ii]) {
			callback_mouse_button(ImGuiMouseButton_Left + ii, entry::ClickAction::Release, io.KeyMods);
			callback_mouse_button(ImGuiMouseButton_Left+ii, entry::ClickAction::Click, io.KeyMods);
		}
		if (io.MouseDoubleClicked[ii]) {
			callback_mouse_button(entry::MouseButton::Left + ii, entry::ClickAction::DoubleClick, io.KeyMods);
		}
	}


	for (int32_t ii = 0; ii < (int32_t)entry::Key::Count; ++ii)
	{
		bool keyDown = io.KeysDown[ii];
		if (keyDown) {
			if (io.KeysDownDuration[ii] <= 0) {
				callback_key(ii, ii, entry::ClickAction::Pressing, io.KeyMods);
			}
		}
		else {
			if (io.KeysDownDuration[ii] > 0) {
				callback_key(ii, ii, entry::ClickAction::Release, io.KeyMods);
				callback_key(ii, ii, entry::ClickAction::Click, io.KeyMods);
			}
		}
	}
	if (io.MouseWheel > 0) {
		callback_scroll(io.MouseWheelH, io.MouseWheel);
	}

    auto filePath = popDropFile();
    while(filePath != nullptr){
        callback_drop_file(filePath->filePath,filePath->result);
        filePath = popDropFile();
    }
    
    auto saveFilePath = popSaveFile();
    if(saveFilePath != nullptr){
        callback_save_file(saveFilePath->filePath,saveFilePath->result);
    }
}
