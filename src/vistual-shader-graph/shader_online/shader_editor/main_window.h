#pragma once

/**
 * @file
 * @brief Defines MainWindow.
 */

#include <memory>
#include <vector>

#include <boost/optional.hpp>

#include "../shader_core/vector.h"

#include "enum.h"
#include "event.h"
#include "modal_curve_editor.h"
#include "modal_ramp_color_pick.h"
#include "subwindow_alert.h"
#include "subwindow_debug.h"
#include "subwindow_graph.h"
#include "subwindow_node_list.h"
#include "subwindow_param_editor.h"
#include "subwindow_shader_text.h"

#include "undo.h"

struct ImGuiContext;

namespace csg {
	class Graph;
	class SlotId;
}

namespace cse {

	//class GlfwWindow;
	class SharedState;

	/**
	 * @brief Responsible for managing the application's main window.
	 */
	class MainWindow {
	public:
		MainWindow(const std::shared_ptr<SharedState>& shared_state);
		~MainWindow();

		bool valid() const;
		bool should_close() const;

		void event_loop_iteration();
		
		void callback_character(unsigned int codepoint);
		void callback_cursor_pos(double x, double y);
		void callback_key(int key, int scancode, int action, int mods);
		void callback_mouse_button(int button, int action, int mods);
		void callback_scroll(double xoffset, double yoffset);
        void callback_drop_file(const bx::FilePath& filePath,const uint8_t result);
        void callback_save_file(const bx::FilePath& filePath,const uint8_t result);
		
        void load_graph(std::string serialized_graph);

	private:
		void new_frame();
        void setGraph(const boost::optional<csg::Graph> opt_graph);
		InterfaceEventArray run_gui() const;
		InterfaceEventArray run_gui_menu_bar() const;
		InterfaceEventArray run_gui_windows() const;
		
		InterfaceEventArray process_event(const InputEvent& event) const;
		InterfaceEventArray process_event_select(const InputEvent& event) const;
		InterfaceEventArray process_event_create(const InputEvent& event) const;

		void do_event(const InterfaceEvent& event);

		InteractionMode get_mode() const;

		std::shared_ptr<csg::Graph> the_graph;
		bool graph_unsaved{ false };

		std::shared_ptr<SharedState> shared_state;

		//std::unique_ptr<GlfwWindow> glfw_window;
//		ImGuiContext* imgui_context{ nullptr };

		std::vector<InputEvent> pending_input_events;
		
		AlertSubwindow window_alert;
		DebugSubwindow window_debug;
        ShaderTextSubwindow window_shader_text;
		GraphSubwindow window_graph;
		NodeListSubwindow window_node_list;
		ParamEditorSubwindow window_param_editor;

		ModalCurveEditor modal_curve_editor;
		ModalRampColorPicker modal_ramp_color_pick;

		boost::optional<csg::SlotId> selected_slot;

		bool quit_requested{ false };

		bool show_window_about{ false };
		bool show_window_demo{ false };
		bool show_window_debug{ false };
        bool show_window_shader_text{ false };
        bool show_window_examples{false};

		csc::Float2 mouse_position;
		csc::Float2 mouse_position_prev;

		csc::Int2 fb_dimensions;

		UndoStack undo_stack;

		enum class ModalWindow {
			ALERT,
			CURVE_EDITOR,
			RAMP_COLOR_PICKER,
		};

		// If this were a stack, there could be multiple layers of modal windows
		// Not worth adding right now
		boost::optional<ModalWindow> modal_window;

		bool enable_debug_menu{ true };

		// Frame-specific data below
		bool should_do_undo_push;
		bool process_input_events;
		boost::optional<SubwindowId> hovered_subwindow;
	};
}
