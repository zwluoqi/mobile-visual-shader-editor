#include "main_window.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <boost/optional.hpp>
////#include <GLFW/glfw3.h>
//#include "imgui/imgui.h"
#include "imgui/imgui.h"

//#include <imgui_impl_glfw.h>
//#include <imgui_impl_opengl2.h>

#include "../shader_core/vector.h"
#include "../shader_graph/graph.h"
#include "../shader_graph/ramp.h"
#include "../shader_graph/slot.h"
#include "../shader_graph/slot_id.h"

#include "enum.h"
#include "glfw_callbacks.h"
#include "graph_display.h"
#include "platform.h"
#include "shared_state.h"
#include "wrapper_glfw_func.h"
#include "wrapper_glfw_window.h"

#include "entry/entry.h"
#include "bgfx_utils.h"
#include "code_rampcolor_byte.h"


cse::MainWindow::MainWindow(const std::shared_ptr<SharedState>& shared_state) :
	the_graph{ std::make_shared<csg::Graph>(csg::GraphType::MATERIAL) },
	shared_state{ shared_state },
	window_graph{ the_graph ,shared_state},
	window_param_editor{ the_graph,shared_state },
	undo_stack{ *the_graph }
{
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	//glfw_window = std::make_unique<GlfwWindow>(1280, 720, "Cycles Shader Editor");

	//glfwMakeContextCurrent(glfw_window);
	//glfwSwapInterval(1);

	register_window_pair_for_callbacks(this);

	//imgui_context = ImGui::CreateContext();
	//ImGui::GetIO().IniFilename = nullptr;

	//ImGui_ImplGlfw_InitForOpenGL(glfw_window->window_ptr, true);
	//ImGui_ImplOpenGL2_Init();
    
//    NFD_Init();
}

cse::MainWindow::~MainWindow()
{
//    NFD_Quit();
	//ImGui_ImplOpenGL2_Shutdown();
	//ImGui_ImplGlfw_Shutdown();
	//if (imgui_context) {
	//	ImGui::DestroyContext(imgui_context);
	//}
}

bool cse::MainWindow::valid() const
{
	//if (glfw_window) {
	//	return glfw_window->valid();
	//}
	//else {
	//	return false;
	//}
	return true;
}

bool cse::MainWindow::should_close() const
{
	//glfwMakeContextCurrent(glfw_window);
	//return quit_requested || (glfwWindowShouldClose(glfw_window->window_ptr) != 0);
	return false;
}

void cse::MainWindow::event_loop_iteration()
{
	new_frame();
	ImGuiIO& io = ImGui::GetIO();
	fb_dimensions = { (int)io.DisplaySize.x,(int)io.DisplaySize.y };

	iteration_window_pair_for_callbacks();

	// Small sleep here to limit the framerate in case vsync is unavailable
	//std::this_thread::sleep_for(std::chrono::milliseconds{ 4 });

	//glfwPollEventsSafe(glfw_window);

	window_graph.set_window_size(fb_dimensions);

	//ImGui_ImplOpenGL2_NewFrame();
	//ImGui_ImplGlfw_NewFrame();
	//ImGui::NewFrame();
    
//    InterfaceEventArray ret_events;
    
	const InterfaceEventArray gui_events{ run_gui() };
//    ret_events.push(gui_events);

	//ImGui::Render();
	//{
	//	glfwGetFramebufferSize(glfw_window, fb_dimensions.x, fb_dimensions.y);
	//	glViewport(0, 0, fb_dimensions.x, fb_dimensions.y);
	//}
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glClear(GL_COLOR_BUFFER_BIT);

	//ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	
	// Handle all events after rendering is complete but before buffer swap

	// Send mouse movement to all windows that support dragging
	const csc::Float2 mouse_delta{ mouse_position - mouse_position_prev };
	window_graph.update_mouse(mouse_position, mouse_delta);
	mouse_position_prev = mouse_position;

	// Do events that were generated in run_gui
	for (const InterfaceEvent event : gui_events) {
		do_event(event);
	}

	// Process InputEvents into InterfaceEvents and do those too
	InterfaceEventArray secondary_events;
	if (process_input_events) {
		for (const auto& input_event : pending_input_events) {
			if (modal_window) {
				// Add special input event handling for modal windows here
			}
			else {
				const InterfaceEventArray events = process_event(input_event);
				secondary_events.push(events);
			}
		}
	}
	pending_input_events.clear();
    
//    ret_events.push(secondary_events);
	for (const InterfaceEvent event : secondary_events) {
		do_event(event);
	}

	// Push undo state if something has changed
	if (should_do_undo_push) {
		if (undo_stack.push_undo(*the_graph)) {
			graph_unsaved = true;
		}
	}

//    return ret_events;

	//glfwSwapBuffers(glfw_window->window_ptr);
}

void cse::MainWindow::callback_character(const unsigned int codepoint)
{
	pending_input_events.push_back(InputCharacterDetails{ codepoint });
}

void cse::MainWindow::callback_cursor_pos(const double x, const double y)
{
	mouse_position = csc::Float2{ static_cast<float>(x), static_cast<float>(y) };
}

void cse::MainWindow::callback_key(const int key, const int scancode, const int action, const int mods)
{
	pending_input_events.push_back(InputKeyDetails{ key, scancode, action, mods });
}

void cse::MainWindow::callback_mouse_button(const int button, const int action, const int mods)
{
	pending_input_events.push_back(InputMouseButtonDetails{ button, action, mods, mouse_position });
}

void cse::MainWindow::callback_scroll(const double xoffset, const double yoffset)
{
	pending_input_events.push_back(InputScrollDetails{ xoffset, yoffset });
}

void cse::MainWindow::callback_drop_file(const bx::FilePath& filePath,const uint8_t result)
{
    pending_input_events.push_back(InputFilePathDetails{ filePath ,result});
}

void cse::MainWindow::callback_save_file(const bx::FilePath& filePath,const uint8_t result)
{
    pending_input_events.push_back(InputSaveFilePathDetails{ filePath,result });
}

void cse::MainWindow::load_graph(const std::string serialized_graph)
{
	const boost::optional<csg::Graph> opt_graph{ csg::Graph::from(serialized_graph) };
	if (opt_graph.has_value()) {
//		*the_graph = *opt_graph;
        setGraph(opt_graph);
		undo_stack.clear(*the_graph);
	}
	else {
		const InterfaceEvent alert_event{
			InterfaceEventType::MODAL_ALERT_SHOW, boost::none, boost::optional<std::string>{ "Selected file is not a valid shader graph." }
		};
		do_event(alert_event);
	}
}

void cse::MainWindow::setGraph(const boost::optional<csg::Graph> opt_graph){
    *the_graph = *opt_graph;
    csg::complie_graph(the_graph,shared_state,false);
}

void cse::MainWindow::new_frame()
{
	should_do_undo_push = false;
	process_input_events = true;
	hovered_subwindow = boost::none;
}

cse::InterfaceEventArray cse::MainWindow::run_gui() const
{
	InterfaceEventArray events;

	// Before drawing any GUI, first check if a modal window should be shown
	// In that case, only draw the modal window instead of the normal gui

	if (modal_window) {
		if (modal_window == ModalWindow::ALERT) {
			const auto alert_events{ window_alert.run() };
			events.push(alert_events);
		}
		else if (modal_window == ModalWindow::CURVE_EDITOR) {
			const auto curve_events{ modal_curve_editor.run() };
			events.push(curve_events);
		}
		else if (modal_window == ModalWindow::RAMP_COLOR_PICKER) {
			const auto ramp_events{ modal_ramp_color_pick.run() };
			events.push(ramp_events);
		}
		else {
			assert(false);
		}
	}
	else {
		const auto menu_bar_events{ run_gui_menu_bar() };
		events.push(menu_bar_events);
		const auto window_events{ run_gui_windows() };
		events.push(window_events);
	}

	return events;
}

cse::InterfaceEventArray cse::MainWindow::run_gui_menu_bar() const
{
	InterfaceEventArray events;

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			events.push(InterfaceEventType::IGNORE_INPUT_EVENTS_THIS_FRAME);
            if (ImGui::MenuItem("New", "Ctrl+N", false)) {
                events.push(InterfaceEventType::NEW_FILE);
            }
			if (ImGui::MenuItem("Save to Max", "Ctrl+S", false)) {
				events.push(InterfaceEventType::SAVE_TO_MAX);
			}
			if (ImGui::MenuItem("Save to file...", nullptr, false)) {
				events.push(InterfaceEventType::SAVE_TO_FILE);
			}
			if (ImGui::MenuItem("Load from file...", nullptr, false)) {
				events.push(InterfaceEventType::LOAD_FROM_FILE);
			}
//            if (ImGui::MenuItem("New...", nullptr, false)) {
//                events.push(InterfaceEventType::NEW_FILE);
//            }
			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) {
				events.push(InterfaceEventType::QUIT);
			}
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Edit")) {
			events.push(InterfaceEventType::IGNORE_INPUT_EVENTS_THIS_FRAME);

			if (ImGui::MenuItem("Undo", "Ctrl+Z", false, undo_stack.undo_available())) {
				events.push(InterfaceEventType::UNDO);
			}
			if (ImGui::MenuItem("Redo", "Ctrl+Y", false, undo_stack.redo_available())) {
				events.push(InterfaceEventType::REDO);
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, window_graph.has_selection())) {
				events.push(InterfaceEvent{ InterfaceEventType::DUPLICATE_SELECTION, SubwindowId::GRAPH });
			}
			/*
			ImGui::Separator();
			if (ImGui::MenuItem("Copy", "Ctrl+C", false, false)) {
				// TODO
			}
			if (ImGui::MenuItem("Paste", "Ctrl+V", false, false)) {
				// TODO
			}
			*/
			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu("Select")) {
			events.push(InterfaceEventType::IGNORE_INPUT_EVENTS_THIS_FRAME);

			if (ImGui::MenuItem("All", "Ctrl+A")) {
				events.push(InterfaceEvent{ InterfaceEventType::SELECT_ALL, SubwindowId::GRAPH });
			}
			if (ImGui::MenuItem("None")) {
				events.push(InterfaceEvent{ InterfaceEventType::SELECT_NONE, SubwindowId::GRAPH });
			}
			if (ImGui::MenuItem("Inverse")) {
				events.push(InterfaceEvent{ InterfaceEventType::SELECT_INVERSE, SubwindowId::GRAPH });
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			events.push(InterfaceEventType::IGNORE_INPUT_EVENTS_THIS_FRAME);

			if (ImGui::MenuItem("Focus Selection", "F", false, window_graph.has_selection())) {
				events.push(InterfaceEvent(InterfaceEventType::FOCUS_SELECTION, SubwindowId::GRAPH));
			}
			if (ImGui::MenuItem("Focus Output", "Shift+F", false)) {
				events.push(InterfaceEvent(InterfaceEventType::FOCUS_OUTPUT, SubwindowId::GRAPH));
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Theme")) {
			events.push(InterfaceEventType::IGNORE_INPUT_EVENTS_THIS_FRAME);

			if (ImGui::MenuItem("Dark (default)")) {
				events.push(InterfaceEventType::SET_THEME_DARK);
			}
			if (ImGui::MenuItem("ImGui Classic")) {
				events.push(InterfaceEventType::SET_THEME_CLASSIC);
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help")) {
			events.push(InterfaceEventType::IGNORE_INPUT_EVENTS_THIS_FRAME);

			if (ImGui::MenuItem("About", nullptr, nullptr)) {
				events.push(InterfaceEventType::WINDOW_SHOW_ABOUT);
			}
			ImGui::EndMenu();
		}

		if (enable_debug_menu) {
			if (ImGui::BeginMenu("Debug")) {
				events.push(InterfaceEventType::IGNORE_INPUT_EVENTS_THIS_FRAME);

				if (ImGui::MenuItem("Reload Graph (with assert)", nullptr, nullptr)) {
					events.push(InterfaceEventType::RELOAD_GRAPH);
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Debug Window...", nullptr, nullptr)) {
					events.push(InterfaceEventType::WINDOW_SHOW_DEBUG);
				}
				ImGui::Separator();
				if (ImGui::MenuItem("ImGui Demo...", nullptr, nullptr)) {
					events.push(InterfaceEventType::WINDOW_SHOW_DEMO);
				}
                if (ImGui::MenuItem("ImGui Profile...", nullptr, nullptr)) {
                    events.push(InterfaceEventType::WINDOW_SHOW_EXAMPLES);
                }
				ImGui::EndMenu();
			}
		}

		ImGui::EndMainMenuBar();
	}

	return events;
}

cse::InterfaceEventArray cse::MainWindow::run_gui_windows() const
{
	InterfaceEventArray events;

	if (show_window_about) {
		if (ImGui::Begin("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Visual Shader Graph");
			ImGui::Text("Created by Zhou Wei");
			ImGui::Separator();
			ImGui::Text("https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause");
			ImGui::Separator();
			ImGui::Text("depend on BGFX,IMGUI,Cycle Graph Editor,assimp");
			if (ImGui::Button("Close")) {
				events.push(InterfaceEventType::WINDOW_CLOSE_ABOUT);
			}
		}
		ImGui::End();
	}

	if (show_window_demo) {
		bool window_open{ true };
		ImGui::ShowDemoWindow(&window_open);
		if (window_open == false) {
			events.push(InterfaceEventType::WINDOW_CLOSE_DEMO);
		}
	}
    
    if(show_window_examples){
        bool window_open{ true };
        showExampleDialog(&window_open);
        if (window_open == false) {
            events.push(InterfaceEventType::WINDOW_CLOSE_EXAMPLES);
        }
    }

	if (show_window_debug) {
		const auto debug_events{ window_debug.run() };
		events.push(debug_events);
	}
    if(show_window_shader_text){
        const auto shader_text_events{ window_shader_text.run() };
        events.push(shader_text_events);
    }

	const auto graph_events{ window_graph.run(get_mode(), graph_unsaved) };
	events.push(graph_events);

	const auto node_list_events{ window_node_list.run() };
	events.push(node_list_events);

	const auto param_editor_events{ window_param_editor.run() };
	events.push(param_editor_events);

	return events;
}

cse::InterfaceEventArray cse::MainWindow::process_event(const InputEvent& event) const
{
	const InputEventType type{ event.type() };
	InterfaceEventArray new_events;

	// First, check if this input work the same regardless of mode
	if (type == InputEventType::KEY) {
		assert(event.details_key().has_value());
		const auto details{ event.details_key().get() };
		const bool press_or_repeat{ details.action == entry::ClickAction::Pressing || details.action == entry::ClickAction::Pressing };
		const bool mod_ctrl{ static_cast<bool>(details.mods & ImGuiKeyModFlags_Ctrl) };
		if (details.key == entry::Key::F12 && press_or_repeat) {
			const InterfaceEvent debug_event{ InterfaceEventType::ENABLE_DEBUG };
			new_events.push(debug_event);
		}
		if (details.key == entry::Key::Left && press_or_repeat) {
			const InterfaceEvent pan_event{ InterfaceEventType::PAN_VIEW, Int2Details(csc::Int2(-1 * GRID_PAN_DISTANCE, 0)) };
			new_events.push(pan_event);
		}
		else if (details.key == entry::Key::Up  && press_or_repeat) {
			const InterfaceEvent pan_event{ InterfaceEventType::PAN_VIEW, Int2Details(csc::Int2(0, -1 * GRID_PAN_DISTANCE)) };
			new_events.push(pan_event);
		}
		else if (details.key == entry::Key::Right && press_or_repeat) {
			const InterfaceEvent pan_event{ InterfaceEventType::PAN_VIEW, Int2Details(csc::Int2(GRID_PAN_DISTANCE, 0)) };
			new_events.push(pan_event);
		}
		else if (details.key == entry::Key::Down && press_or_repeat) {
			const InterfaceEvent pan_event{ InterfaceEventType::PAN_VIEW, Int2Details(csc::Int2(0, GRID_PAN_DISTANCE)) };
			new_events.push(pan_event);
		}
		else if (details.key == entry::Key::KeyF && press_or_repeat) {
			if (details.mods & ImGuiKeyModFlags_Shift) {
				const InterfaceEvent focus_event{ InterfaceEventType::FOCUS_OUTPUT, SubwindowId::GRAPH };
				new_events.push(focus_event);
			}
			else {
				const InterfaceEvent focus_event{ InterfaceEventType::FOCUS_SELECTION, SubwindowId::GRAPH };
				new_events.push(focus_event);
			}
		}
		else if (details.key == entry::Key::KeyZ && mod_ctrl && press_or_repeat) {
			const InterfaceEvent undo_event{ InterfaceEventType::UNDO };
			new_events.push(undo_event);
		}
		else if (details.key == entry::Key::KeyY && mod_ctrl && press_or_repeat) {
			const InterfaceEvent redo_event{ InterfaceEventType::REDO };
			new_events.push(redo_event);
		}
		else if (details.key == entry::Key::KeyD && mod_ctrl && details.action == entry::ClickAction::Pressing) {
			const InterfaceEvent redo_event{ InterfaceEventType::DUPLICATE_SELECTION, SubwindowId::GRAPH };
			new_events.push(redo_event);
		}
		else if (details.key == entry::Key::KeyS && mod_ctrl && details.action == entry::ClickAction::Pressing) {
			const InterfaceEvent save_event{ InterfaceEventType::SAVE_TO_MAX };
			new_events.push(save_event);
		}
	}

	const InterfaceEventArray graph_events{ window_graph.process_event(event, get_mode(), hovered_subwindow == SubwindowId::GRAPH) };
	new_events.push(graph_events);

	switch (get_mode()) {
		case InteractionMode::SELECT:
		{
			const InterfaceEventArray select_events = process_event_select(event);
			new_events.push(select_events);
			break;
		}
		case InteractionMode::CREATE:
		{
			const InterfaceEventArray create_events = process_event_create(event);
			new_events.push(create_events);
			break;
		}
		default:
			// Do nothing
			break;
	}

	return new_events;
}

cse::InterfaceEventArray cse::MainWindow::process_event_select(const InputEvent& event) const
{
	assert(get_mode() == InteractionMode::SELECT);
	InterfaceEventArray new_events;

	if (event.type() == InputEventType::KEY) {
		assert(event.details_key().has_value());
		const auto details{ event.details_key().get() };
        const bool mod_ctrl{ static_cast<bool>(details.mods & ImGuiKeyModFlags_Ctrl) };
        //|| details.key == entry::Key::Backspace
		if ((details.key == entry::Key::Delete ) && details.action == entry::ClickAction::Pressing) {
			const InterfaceEvent delete_event{ InterfaceEventType::DELETE_NODE_SELECTION, SubwindowId::GRAPH };
			new_events.push(delete_event);
		}
        if ((details.key == entry::Key::Backspace ) && mod_ctrl && details.action == entry::ClickAction::Pressing) {
            const InterfaceEvent delete_event{ InterfaceEventType::DELETE_NODE_SELECTION, SubwindowId::GRAPH };
            new_events.push(delete_event);
        }
	}

	return new_events;
}

cse::InterfaceEventArray cse::MainWindow::process_event_create(const InputEvent& event) const
{
	assert(get_mode() == InteractionMode::CREATE);
	InterfaceEventArray new_events;

	if (event.type() == InputEventType::MOUSE_BUTTON) {
		assert(event.details_mouse_button().has_value());
		const auto details{ event.details_mouse_button().get() };
		if (details.button == ImGuiMouseButton_Right && details.action == entry::ClickAction::Pressing) {
			const InterfaceEvent select_event{ InterfaceEventType::SELECT_NODE_TYPE_NONE, SubwindowId::NODE_LIST };
			new_events.push(select_event);
		}
		if (hovered_subwindow == SubwindowId::GRAPH) {
			if (details.button == ImGuiMouseButton_Left && details.action == entry::ClickAction::Pressing) {
				const auto selected_type{ window_node_list.selected_type().get() };
				const InterfaceEvent create_event{ CreateNodeDetails{ selected_type, details.pos } };
				new_events.push(create_event);
				if (static_cast<bool>(details.mods & ImGuiKeyModFlags_Shift) == false) {
					const InterfaceEvent deselect_event{ InterfaceEventType::SELECT_NODE_TYPE_NONE, SubwindowId::NODE_LIST };
					new_events.push(deselect_event);
				}
			}
		}
	}

	return new_events;
}

void cse::MainWindow::do_event(const InterfaceEvent& event)
{
	// Special events that get duplicated to multiple windows are handled here
	if (event.type() == InterfaceEventType::SELECT_SLOT) {
		assert(event.details_as<SlotIdDetails>().has_value());
		InterfaceEvent graph_event{ InterfaceEventType::SELECT_SLOT, SlotIdDetails{ event.details_as<SlotIdDetails>()->value }, SubwindowId::GRAPH };
		window_graph.do_event(graph_event);
		InterfaceEvent param_event{ InterfaceEventType::SELECT_SLOT, SlotIdDetails{ event.details_as<SlotIdDetails>()->value }, SubwindowId::PARAM_EDITOR };
		window_param_editor.do_event(param_event);
		selected_slot = event.details_as<SlotIdDetails>()->value;
		return;
	}
	else if (event.type() == InterfaceEventType::SELECT_SLOT_NONE) {
		InterfaceEvent graph_event{ InterfaceEventType::SELECT_SLOT_NONE, SubwindowId::GRAPH };
		window_graph.do_event(graph_event);
		InterfaceEvent param_event{ InterfaceEventType::SELECT_SLOT_NONE, SubwindowId::PARAM_EDITOR };
		window_param_editor.do_event(param_event);
		selected_slot = boost::none;
		return;
	}

	// Normal event handling begins here
	if (event.target_subwindow()) {
		const SubwindowId target_subwindow{ event.target_subwindow().get() };
		switch (target_subwindow) {
			case SubwindowId::ALERT:
				// No events should be sent to this window
				assert(false);
				break;
			case SubwindowId::GRAPH:
			{
				if (window_graph.do_event(event)) {
					should_do_undo_push = true;
				}
				break;
			}
			case SubwindowId::NODE_LIST:
				window_node_list.do_event(event);
				break;
			case SubwindowId::DEBUG:
				window_debug.do_event(event);
				break;
			case SubwindowId::PARAM_EDITOR:
				window_param_editor.do_event(event);
				break;
			case SubwindowId::MODAL_CURVE_EDITOR:
				modal_curve_editor.do_event(event);
				break;
			case SubwindowId::MODAL_RAMP_COLOR_PICK:
				modal_ramp_color_pick.do_event(event);
				break;
			default:
				assert(false);
		}
	}
	else {
		// No target subwindow, this is for the main window to handle directly
		switch (event.type()) {
			case InterfaceEventType::ENABLE_DEBUG:
				enable_debug_menu = true;
				break;
			case InterfaceEventType::QUIT:
				quit_requested = true;
				break;
//            case InterfaceEventType::NEW_FILE:
//                
//                break;
			case InterfaceEventType::SAVE_TO_MAX:
            {
				csg::complie_graph(the_graph,shared_state,true);
				graph_unsaved = false;
            }
				break;
			case InterfaceEventType::SAVE_TO_FILE:
				Platform::save_graph_dialog();
				break;
			case InterfaceEventType::LOAD_FROM_FILE:
            {
                shared_state->holdWaitInputFile();
                Platform::load_graph_dialog();
            }
				break;
            case InterfaceEventType::NEW_FILE:
            {
                shared_state->new_input_graph();
                auto new_graph = std::make_shared<csg::Graph>(csg::GraphType::MATERIAL);
                load_graph(new_graph->serialize());
                graph_unsaved = false;
            }
                break;
			case InterfaceEventType::MODAL_ALERT_SHOW:
				modal_window = ModalWindow::ALERT;
				if (event.message().has_value()) {
					window_alert.set_message(event.message().value());
				}
				else {
					window_alert.set_message("[No message]");
				}
				break;
			case InterfaceEventType::MODAL_ALERT_CLOSE:
				modal_window = boost::none;
				break;
			case InterfaceEventType::WINDOW_SHOW_ABOUT:
				show_window_about = true;
				break;
			case InterfaceEventType::WINDOW_CLOSE_ABOUT:
				show_window_about = false;
				break;
            case InterfaceEventType::WINDOW_SHOW_SHADER_TEXT:
                show_window_shader_text = true;
                break;
            case InterfaceEventType::WINDOW_CLOSE_SHADER_TEXT:
                show_window_shader_text = false;
                break;
			case InterfaceEventType::WINDOW_SHOW_DEMO:
				show_window_demo = true;
				break;
            case InterfaceEventType::WINDOW_SHOW_EXAMPLES:
                show_window_examples = true;
                break;
			case InterfaceEventType::WINDOW_CLOSE_DEMO:
				show_window_demo = false;
				break;
            case InterfaceEventType::WINDOW_CLOSE_EXAMPLES:
                show_window_examples = false;
                break;
			case InterfaceEventType::WINDOW_SHOW_DEBUG:
				show_window_debug = true;
				break;
			case InterfaceEventType::WINDOW_CLOSE_DEBUG:
				show_window_debug = false;
				break;
			case InterfaceEventType::MODAL_CURVE_EDITOR_SHOW:
			{
				// Fetch the curve from the graph
				if (selected_slot) {
					const csg::SlotId slot_id{ *selected_slot };
					const auto this_slot_rgb{ the_graph->get_slot_value_as<csg::RGBCurveSlotValue>(slot_id) };
					const auto this_slot_vec{ the_graph->get_slot_value_as<csg::VectorCurveSlotValue>(slot_id) };
					if (this_slot_rgb) {
						modal_curve_editor.set_vector(*this_slot_rgb);
						modal_window = ModalWindow::CURVE_EDITOR;
					}
					else if (this_slot_vec) {
						modal_curve_editor.set_vector(*this_slot_vec);
						modal_window = ModalWindow::CURVE_EDITOR;
					}
				}
				break;
			}
			case InterfaceEventType::MODAL_CURVE_EDITOR_COMMIT_RGB:
			{
				assert(selected_slot.has_value());
				const csg::SlotId slot_id{ *selected_slot };
				const boost::optional<csg::RGBCurveSlotValue> rgb_curve{ modal_curve_editor.take_rgb() };
				if (rgb_curve) {
					the_graph->set_curve_rgb(slot_id, *rgb_curve);
                    auto layer = shared_state->getSlotLayer(slot_id.slot_id());
                    SetRGBCurveSlotValue(shared_state,rgb_curve.get_ptr(),layer);
				}
				const InterfaceEvent close_event{ InterfaceEventType::MODAL_CURVE_EDITOR_CLOSE };
				do_event(close_event);
				should_do_undo_push = true;
				break;
			}
			case InterfaceEventType::MODAL_CURVE_EDITOR_COMMIT_VEC:
			{
				assert(selected_slot.has_value());
				const csg::SlotId slot_id{ *selected_slot };
				const boost::optional<csg::VectorCurveSlotValue> vector_curve{ modal_curve_editor.take_vector() };
				if (vector_curve) {
					the_graph->set_curve_vec(slot_id, *vector_curve);
                    auto layer = shared_state->getSlotLayer(slot_id.slot_id());
                    SetVectorCurveSlotValue(shared_state,vector_curve.get_ptr(),layer);
				}
				const InterfaceEvent close_event{ InterfaceEventType::MODAL_CURVE_EDITOR_CLOSE };
				do_event(close_event);
				should_do_undo_push = true;
				break;
			}
			case InterfaceEventType::MODAL_CURVE_EDITOR_CLOSE:
				modal_curve_editor.clear();
				modal_window = boost::none;
				break;
			case InterfaceEventType::MODAL_RAMP_COLOR_PICK_SHOW:
			{
				assert(event.details_as<ModalRampColorPickShowDetails>().has_value());
				const ModalRampColorPickShowDetails details{ event.details_as<ModalRampColorPickShowDetails>().get() };
				// Get the current RGB value from the attached slot and index
				const auto opt_value{ the_graph->get_slot_value_as<csg::ColorRampSlotValue>(details.slot_id) };
				if (opt_value) {
					const csg::ColorRamp ramp{ opt_value->get() };
					if (details.index < ramp.size()) {
						const csg::ColorRampPoint point{ ramp.get(details.index) };
						const csc::Float4 rgba{ point.color, point.alpha };
						modal_ramp_color_pick.attach(details.slot_id, details.index, rgba);
						modal_window = ModalWindow::RAMP_COLOR_PICKER;
					}
				}
				break;
			}
			case InterfaceEventType::MODAL_RAMP_COLOR_PICK_CLOSE:
			{
				assert(modal_window == ModalWindow::RAMP_COLOR_PICKER);
				modal_window = boost::none;
				break;
			}
			case InterfaceEventType::SUBWINDOW_IS_HOVERED:
			{
				const boost::optional<SubwindowIdDetails> details{ event.details_as<SubwindowIdDetails>() };
				if (details) {
					hovered_subwindow = details->value;
				}
				break;
			}
			case InterfaceEventType::SET_THEME_DARK:
				ImGui::StyleColorsDark();
				break;
			case InterfaceEventType::SET_THEME_CLASSIC:
				ImGui::StyleColorsClassic();
				break;
			case InterfaceEventType::IGNORE_INPUT_EVENTS_THIS_FRAME:
				process_input_events = false;
				break;
			case InterfaceEventType::SET_SLOT_BOOL:
			{
				const boost::optional<SetSlotBoolDetails> details{ event.details_as<SetSlotBoolDetails>() };
				assert(details.has_value());
				the_graph->set_bool(details->slot_id, details->new_value);
                shared_state->push_slot_val_change(details->slot_id.slot_id(),details->new_value);
				should_do_undo_push = true;
				break;
			}
			case InterfaceEventType::SET_SLOT_COLOR:
			{
				const boost::optional<SetSlotColorDetails> details{ event.details_as<SetSlotColorDetails>() };
				assert(details.has_value());
				the_graph->set_color(details->slot_id, details->new_value);
                shared_state->push_slot_val_change(details->slot_id.slot_id(),details->new_value);
				should_do_undo_push = true;
				break;
			}
			case InterfaceEventType::SET_SLOT_ENUM:
			{
				const boost::optional<SetSlotEnumDetails> details{ event.details_as<SetSlotEnumDetails>() };
				assert(details.has_value());
				the_graph->set_enum(details->slot_id, details->new_value);
                //re complie shader
                if(shared_state->has_connect_nodes( details->slot_id.node_id())){
                    csg::complie_graph(the_graph,shared_state,false);
                }
				should_do_undo_push = true;
				break;
			}
			case InterfaceEventType::SET_SLOT_FLOAT:
			{
				const boost::optional<SetSlotFloatDetails> details{ event.details_as<SetSlotFloatDetails>() };
				assert(details.has_value());
                the_graph->set_float(details->slot_id, details->new_value);
                shared_state->push_slot_val_change(details->slot_id.slot_id(),details->new_value);
				should_do_undo_push = true;
				break;
			}
			case InterfaceEventType::SET_SLOT_INT:
			{
				const boost::optional<SetSlotIntDetails> details{ event.details_as<SetSlotIntDetails>() };
				assert(details.has_value());
				the_graph->set_int(details->slot_id, details->new_value);
                shared_state->push_slot_val_change(details->slot_id.slot_id(),details->new_value);
				should_do_undo_push = true;
				break;
			}
			case InterfaceEventType::MODIFY_SLOT_RAMP_COLOR:
			{
				const boost::optional<ModifySlotRampColorDetails> details{ event.details_as<ModifySlotRampColorDetails>() };
				assert(details.has_value());
				const boost::optional<csg::ColorRampSlotValue> opt_ramp{ the_graph->get_slot_value_as<csg::ColorRampSlotValue>(details->slot_id) };
				if (opt_ramp) {
					const boost::optional<csg::ColorRampPoint> opt_point{ opt_ramp->get().get(details->point_index) };
					if (opt_point) {
						csg::ColorRampPoint mut_point{ *opt_point };
						const csc::Float3 rgb{ details->new_value.x, details->new_value.y, details->new_value.z };
						mut_point.color = rgb;
						mut_point.alpha = details->new_value.w;
						csg::ColorRamp mut_ramp{ opt_ramp->get() };
						mut_ramp.set(details->point_index, mut_point);
                        csg::ColorRampSlotValue mut_ramp_slot{mut_ramp};
						the_graph->set_color_ramp(details->slot_id, mut_ramp_slot);
                        auto layer = shared_state->getSlotLayer(details->slot_id.slot_id());
                        SetColorRampSlotValue(shared_state,&mut_ramp_slot,layer);
						should_do_undo_push = true;
					}
				}
				break;
			}
			case InterfaceEventType::MODIFY_SLOT_RAMP_POS:
			{
				const boost::optional<ModifySlotRampPosDetails> details{ event.details_as<ModifySlotRampPosDetails>() };
				assert(details.has_value());
				const boost::optional<csg::ColorRampSlotValue> opt_ramp{ the_graph->get_slot_value_as<csg::ColorRampSlotValue>(details->slot_id) };
				if (opt_ramp) {
					const boost::optional<csg::ColorRampPoint> opt_point{ opt_ramp->get().get(details->point_index) };
					if (opt_point) {
						csg::ColorRampPoint mut_point{ *opt_point };
						mut_point.pos = details->new_value;
						csg::ColorRamp mut_ramp{ opt_ramp->get() };
						mut_ramp.set(details->point_index, mut_point);
                        csg::ColorRampSlotValue mut_ramp_slot{mut_ramp};
//                        mut_ramp_slot.setLayer(opt_ramp->getLayer());
						the_graph->set_color_ramp(details->slot_id, mut_ramp_slot);
                        auto layer = shared_state->getSlotLayer(details->slot_id.slot_id());
                        SetColorRampSlotValue(shared_state,&mut_ramp_slot,layer);
//                        shared_state->push_slot_val_change(details->slot_id.slot_id(),details->new_value);
						should_do_undo_push = true;
					}
				}
				break;
			}
			case InterfaceEventType::MODIFY_SLOT_RAMP_NEW:
			{
				const boost::optional<SlotIdDetails> details{ event.details_as<SlotIdDetails>() };
				assert(details.has_value());
				const boost::optional<csg::ColorRampSlotValue> opt_ramp{ the_graph->get_slot_value_as<csg::ColorRampSlotValue>(details->value) };
				if (opt_ramp) {
					const csg::ColorRamp ramp{ opt_ramp->get() };
					std::vector<csg::ColorRampPoint> mut_points{ ramp.get() };
					mut_points.push_back(csg::ColorRampPoint{ 1.0f, csc::Float3{ 1.0f, 1.0f, 1.0f }, 1.0f });
					const csg::ColorRamp new_ramp{ mut_points };
                    csg::ColorRampSlotValue mut_ramp_slot{new_ramp};
//                    mut_ramp_slot.setLayer(opt_ramp->getLayer());
//                    the_graph->set_color_ramp(details->slot_id, mut_ramp_slot);
					the_graph->set_color_ramp(details->value, mut_ramp_slot);
                    auto layer = shared_state->getSlotLayer(details->value.slot_id());

                    SetColorRampSlotValue(shared_state,&mut_ramp_slot,layer);

//                    shared_state->push_slot_val_change(details->slot_id.slot_id(),details->new_value);
					should_do_undo_push = true;
				}
				break;
			}
			case InterfaceEventType::MODIFY_SLOT_RAMP_DELETE:
			{
				const boost::optional<ModifySlotRampDeleteDetails> details{ event.details_as<ModifySlotRampDeleteDetails>() };
				assert(details.has_value());
				const boost::optional<csg::ColorRampSlotValue> opt_ramp{ the_graph->get_slot_value_as<csg::ColorRampSlotValue>(details->slot_id) };
				if (opt_ramp) {
					csg::ColorRamp mut_ramp{ opt_ramp->get() };
					mut_ramp.remove(details->point_index);
                    csg::ColorRampSlotValue mut_ramp_slot{mut_ramp};
//                    mut_ramp_slot.setLayer(opt_ramp->getLayer());
                    the_graph->set_color_ramp(details->slot_id, mut_ramp_slot);
                    auto layer = shared_state->getSlotLayer(details->slot_id.slot_id());

                    SetColorRampSlotValue(shared_state,&mut_ramp_slot,layer);
//                    shared_state->push_slot_val_change(details->slot_id.slot_id(),details->new_value);
					should_do_undo_push = true;
				}
				break;
			}
			case InterfaceEventType::SET_SLOT_VECTOR:
			{
				const boost::optional<SetSlotVectorDetails> details{ event.details_as<SetSlotVectorDetails>() };
				assert(details.has_value());
				the_graph->set_vector(details->slot_id, details->new_value);
                shared_state->push_slot_val_change(details->slot_id.slot_id(),details->new_value);
				should_do_undo_push = true;
				break;
			}
            case InterfaceEventType::SET_SLOT_IMAGE:
            {
                const boost::optional<SetSlotImageDetails> details{ event.details_as<SetSlotImageDetails>() };
                assert(details.has_value());
                the_graph->set_image_value(details->slot_id, details->new_value);
                shared_state->push_slot_sampler_change(details->slot_id.slot_id(),shared_state->AddTextureHandler(details->new_value));
                should_do_undo_push = true;
                break;
            }
			case InterfaceEventType::UNDO:
			{
				if (undo_stack.undo_available()) {
					auto tmp = undo_stack.pop_undo(*the_graph);
                    setGraph(tmp);
				}
				break;
			}
			case InterfaceEventType::REDO:
			{
				if (undo_stack.redo_available()) {
					auto tmp = undo_stack.pop_redo(*the_graph);
                    setGraph(tmp);
				}
				break;
			}
			case InterfaceEventType::RELOAD_GRAPH:
			{
				// Serialize and deserialize the current graph
				const std::string graph_string{ the_graph->serialize() };
				const boost::optional<csg::Graph> opt_graph{ csg::Graph::from(graph_string) };
				assert(opt_graph.has_value());
				if (*opt_graph != *the_graph) {
					std::cout << "Assert failed" << std::endl;
					std::cout << "original:" << std::endl;
					std::cout << the_graph->serialize() << std::endl;
					std::cout << "reloaded:" << std::endl;
					std::cout << opt_graph->serialize() << std::endl;
					assert(false);
				}
				break;
			}
            case InterfaceEventType::OPEN_DIALOG:
//                printf("TODO OPEN_DIALOG");
                Platform::open_image_dialog();
                break;
			default:
                std::cout << "Assert TODO" << std::endl;
				// Do nothing
				break;
		}
	}
}

cse::InteractionMode cse::MainWindow::get_mode() const
{
	// Let the graph dictate the current mode if it wants to
	const auto graph_mode{ window_graph.get_mode() };
	if (graph_mode) {
		return graph_mode.value();
	}
	else if (window_node_list.selected_type()) {
		return InteractionMode::CREATE;
	}
	else {
		return InteractionMode::SELECT;
	}
}
