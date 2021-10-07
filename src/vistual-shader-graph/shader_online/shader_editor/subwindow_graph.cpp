#include "subwindow_graph.h"

#include <array>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <list>
#include <map>
#include <set>
#include <vector>

#include <boost/optional.hpp>
#include <boost/range/adaptor/reversed.hpp>
//#include <GLFW/glfw3.h>
#include "imgui/imgui.h"

#include "../shader_core/rect.h"
#include "../shader_core/vector.h"
#include "../shader_graph/graph.h"
#include "../shader_graph/node.h"
#include "../shader_graph/node_type.h"
#include "../shader_graph/slot.h"
#include "../shader_graph/slot_id.h"

#include "alt_slot_names.h"
#include "event.h"
#include "graph_display.h"
#include "node_geometry.h"
#include "wrapper_imgui_func.h"

#include "entry/entry.h"

static ImU32 get_category_color(const csg::NodeCategory category)
{
	switch (category) {
		case csg::NodeCategory::OUTPUT:
			return COLOR_NODE_CATEGORY_OUTPUT;
		case csg::NodeCategory::COLOR:
			return COLOR_NODE_CATEGORY_COLOR;
		case csg::NodeCategory::CONVERTER:
			return COLOR_NODE_CATEGORY_CONVERTER;
		case csg::NodeCategory::INPUT:
			return COLOR_NODE_CATEGORY_INPUT;
		case csg::NodeCategory::SHADER:
			return COLOR_NODE_CATEGORY_SHADER;
		case csg::NodeCategory::TEXTURE:
			return COLOR_NODE_CATEGORY_TEXTURE;
		case csg::NodeCategory::VECTOR:
			return COLOR_NODE_CATEGORY_VECTOR;
		default:
			return COLOR_NODE_CATEGORY_DEFAULT;
	}
}

static ImU32 get_slot_type_color(const csg::SlotType type)
{
	switch (type) {
		case csg::SlotType::COLOR:
			return COLOR_NODE_SLOT_COLOR;
		case csg::SlotType::CLOSURE:
			return COLOR_NODE_SLOT_CLOSURE;
		case csg::SlotType::FLOAT:
			return COLOR_NODE_SLOT_FLOAT;
		case csg::SlotType::VECTOR:
			return COLOR_NODE_SLOT_VECTOR;
		default:
			return COLOR_NODE_SLOT_DEFAULT;
	}
}

cse::InterfaceEventArray cse::GraphSubwindow::run(const InteractionMode mode, const bool graph_unsaved) const
{
	InterfaceEventArray result;

	const csc::Float2 window_size_float{ window_size };
	const csc::Float2 imgui_window_pos{ get_window_pos() };
	const csc::Float2 imgui_window_size{ window_size_float - imgui_window_pos };

	ImGui::SetNextWindowPos(imgui_window_pos);
	ImGui::SetNextWindowSize(imgui_window_size);

	const ImGuiWindowFlags flags{
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus
	};

	if (ImGui::Begin("Background", nullptr, flags)) {
		if (ImGui::IsWindowHovered()) {
			result.push(InterfaceEvent{ InterfaceEventType::SUBWINDOW_IS_HOVERED, SubwindowIdDetails{ SubwindowId::GRAPH }, boost::none });
		}

		ImGui::BeginTabBar("Bar");

		if (graph_unsaved) {
			if (ImGui::BeginTabItem("Graph*###Graph")) {
				ImGui::EndTabItem();
			}
		}
		else {
			if (ImGui::BeginTabItem("Graph###Graph")) {
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();

		ImDrawList* const draw_list{ ImGui::GetWindowDrawList() };
		const csc::FloatRect clip_region{ get_draw_rect() };
		ImGui::DrawList::PushClipRect(draw_list, clip_region, true);
		ImGui::DrawList::AddRectFilled(draw_list, clip_region, COLOR_GRID_BACKGROUND);
		draw_grid(draw_list);
		draw_nodes(draw_list);
		draw_select_box(draw_list);
        ImGui::DrawList::PopClipRect(draw_list);

		// Status bar text
		const csc::Float2 text_pos{ 8.0f, clip_region.end().y + 2.0f };
		if (mode == InteractionMode::SELECT) {
			ImGui::DrawList::AddText(draw_list, text_pos, ImGui::GetColorU32(ImGuiCol_Text), "Mode: Select");
		}
		else if (mode == InteractionMode::CREATE) {
			ImGui::DrawList::AddText(draw_list, text_pos, ImGui::GetColorU32(ImGuiCol_Text), "Mode: Create");
		}
		else if (mode == InteractionMode::MOUSE_PAN) {
			ImGui::DrawList::AddText(draw_list, text_pos, ImGui::GetColorU32(ImGuiCol_Text), "Mode: Mouse Pan");
		}
		else if (mode == InteractionMode::MOUSE_MOVE) {
			ImGui::DrawList::AddText(draw_list, text_pos, ImGui::GetColorU32(ImGuiCol_Text), "Mode: Mouse Move");
		}
		else if (mode == InteractionMode::BOX_SELECT) {
			ImGui::DrawList::AddText(draw_list, text_pos, ImGui::GetColorU32(ImGuiCol_Text), "Mode: Box Select");
		}
		else {
			ImGui::DrawList::AddText(draw_list, text_pos, ImGui::GetColorU32(ImGuiCol_Text), "Mode: Unknown");
		}
	}
	ImGui::End();

	return result;
}

bool cse::GraphSubwindow::do_event(const InterfaceEvent& event)
{
	bool graph_altered{ false };

	if (event.target_subwindow() && event.target_subwindow() == SubwindowId::GRAPH) {
		switch (event.type()) {
			case InterfaceEventType::PAN_VIEW:
			{
				const boost::optional<Int2Details> details{ event.details_as<Int2Details>() };
				assert(details.has_value());
				if (details) {
					view_center = view_center + details->value;
				}
				break;
			}
			case InterfaceEventType::MOUSE_PAN_BEGIN:
				_mouse_pan_active = true;
				break;
			case InterfaceEventType::MOUSE_PAN_END:
				_mouse_pan_active = false;
				break;
			case InterfaceEventType::MOUSE_MOVE_BEGIN:
				_mouse_move_active = true;
				break;
			case InterfaceEventType::MOUSE_MOVE_END:
				_mouse_move_active = false;
				graph_altered = true;
				break;
			case InterfaceEventType::CREATE_NODE:
			{
				const boost::optional<CreateNodeDetails> details{ event.details_as<CreateNodeDetails>() };
				assert(details.has_value());
				the_graph->add(details->type, csc::Int2{ screen_to_world(details->screen_pos) });
				graph_altered = true;
				break;
			}
			case InterfaceEventType::SELECT_NODE:
			{
				const boost::optional<SelectNodeDetails> details{ event.details_as<SelectNodeDetails>() };
				assert(details.has_value());
				node_selection.select(details->mode, details->id);
				if (details->mode == SelectMode::EXCLUSIVE) {
					the_graph->raise(details->id);
				}
				break;
			}
			case InterfaceEventType::BOX_SELECT_BEGIN:
				box_select_begin = csc::Int2{ mouse_world_pos };
				break;
			case InterfaceEventType::BOX_SELECT_END:
			{
				const boost::optional<SelectModeDetails> details{ event.details_as<SelectModeDetails>() };
				assert(details.has_value());
				const auto rect{ selection_rect() };
				box_select_begin = boost::none;
				if (rect) {
					const std::set<csg::NodeId> selected_nodes{ get_nodes_in_rect(rect.value()) };
					const SelectMode select_mode{ details->value };
					// Selecting all as exclusive individually won't work, so we clear and add in that case
					if (select_mode == SelectMode::EXCLUSIVE) {
						node_selection.clear();
						for (const auto this_node_id : selected_nodes) {
							node_selection.select(SelectMode::ADD, this_node_id);
						}
					}
					else {
						for (const auto this_node_id : selected_nodes) {
							node_selection.select(select_mode, this_node_id);
						}
					}
				}
				break;
			}
			case InterfaceEventType::DELETE_NODE_SELECTION:
            {
                auto selectNodeIds = node_selection.selected();
				the_graph->remove(selectNodeIds);
                bool hasDeleteShaderNode = false;
                for (auto nodeId :selectNodeIds) {
                    if(the_state->has_connect_nodes(nodeId)){
                        hasDeleteShaderNode = true;
                        break;
                    }
                }
                if(hasDeleteShaderNode){
                    csg::complie_graph(the_graph, the_state, false);
                }
				graph_altered = true;
            }
				break;
			case InterfaceEventType::FOCUS_SELECTION:
			{
				boost::optional<csc::FloatRect> bounding_rect;
				for (const auto& this_node : the_graph->nodes()) {
					const NodeGeometry node_geom{ *this_node };
					if (node_selection.is_selected(this_node->id())) {
						if (bounding_rect) {
							bounding_rect = bounding_rect->with_point(node_geom.pos()).with_point(node_geom.end());
						}
						else {
							bounding_rect = node_geom.rect();
						}
					}
				}
				if (bounding_rect) {
					view_center = csc::Int2{ bounding_rect->center() };
				}
				break;
			}
			case InterfaceEventType::FOCUS_OUTPUT:
			{
				for (const auto& this_node : the_graph->nodes()) {
					const auto type_info{ csg::NodeTypeInfo::from(this_node->type()) };
					const NodeGeometry node_geom{ *this_node };
					assert(type_info.has_value());
					if (type_info->category() == csg::NodeCategory::OUTPUT) {
						view_center = this_node->position + csc::Int2{ node_geom.size() / 2.0f };
					}
				}
				break;
			}
			case InterfaceEventType::CONNECTION_BEGIN:
			{
				const boost::optional<SlotIdDetails> details{ event.details_as<SlotIdDetails>() };
				assert(details.has_value());
				const csg::SlotId slot_id{ details->value };
				const auto node = the_graph->get(slot_id.node_id());
				if (node && node->has_pin(slot_id.index(), csg::SlotDirection::OUTPUT)) {
					pending_connection_begin = slot_id;
				}
				break;
			}
			case InterfaceEventType::CONNECTION_END:
			{
				const boost::optional<csg::SlotId> pin{ get_pin_at_pos( world_to_screen(mouse_world_pos), csg::SlotDirection::INPUT) };
				if (pin && pending_connection_begin) {
					bool success = the_graph->add_connection(*pending_connection_begin, *pin);
                    if(success){
                        if(the_state->has_connect_nodes( (*pin).node_id())){
                            csg::complie_graph(the_graph, the_state, false);
                        }
                    }
					graph_altered = true;
				}
				pending_connection_begin = boost::none;
				break;
			}
			case InterfaceEventType::CONNECTION_ALTER:
			{
				const boost::optional<SlotIdDetails> details{ event.details_as<SlotIdDetails>() };
				assert(details.has_value());
				const csg::SlotId slot_id{ details->value };
				const auto old_conn{ the_graph->remove_connection(slot_id) };
                if(the_state->has_connect_nodes( slot_id.node_id())){
                    csg::complie_graph(the_graph, the_state, false);
                }
				if (old_conn) {
					pending_connection_begin = old_conn->source();
					graph_altered = true;
				}
				break;
			}
			case InterfaceEventType::DUPLICATE_SELECTION:
			{
				std::map<csg::NodeId, csg::NodeId> old_to_new;
				const std::set<csg::NodeId> original_selection{ node_selection.selected() };
				node_selection.clear();
				// Duplicate each node and track the id mapping
				for (const csg::NodeId original_id : original_selection) {
					const boost::optional<csg::NodeId> opt_new_id { the_graph->duplicate(original_id) };
					if (opt_new_id.has_value()) {
						old_to_new[original_id] = *opt_new_id;
						node_selection.select(SelectMode::ADD, *opt_new_id);
					}
				}
				// Duplicate connections
				const std::list<csg::Connection> original_connections{ the_graph->connections() };
				for (const csg::Connection& this_conn : original_connections) {
					if (old_to_new.count(this_conn.source().node_id()) + old_to_new.count(this_conn.dest().node_id()) < 2) {
						// Both nodes for this connection were not duplicated, go next
						continue;
					}
					const csg::SlotId new_source{ old_to_new[this_conn.source().node_id()],  this_conn.source().index() };
					const csg::SlotId new_dest{ old_to_new[this_conn.dest().node_id()],  this_conn.dest().index() };
					the_graph->add_connection(new_source, new_dest);
				}
				graph_altered = true;
				break;
			}
			case InterfaceEventType::SELECT_ALL:
			{
				node_selection.clear();
				for (const auto& this_node : the_graph->nodes()) {
					node_selection.select(SelectMode::ADD, this_node->id());
				}
				break;
			}
			case InterfaceEventType::SELECT_NONE:
			{
				node_selection.clear();
				break;
			}
			case InterfaceEventType::SELECT_INVERSE:
			{
				std::set<csg::NodeId> original_selection{ node_selection.selected() };
				node_selection.clear();
				for (const auto& this_node : the_graph->nodes()) {
					if (original_selection.count(this_node->id()) == 0) {
						node_selection.select(SelectMode::ADD, this_node->id());
					}
				}
				break;
			}
			case InterfaceEventType::SELECT_SLOT:
			{
				const boost::optional<SlotIdDetails> details{ event.details_as<SlotIdDetails>() };
				assert(details.has_value());
				selected_slot = details->value;
				break;
			}
			case InterfaceEventType::SELECT_SLOT_NONE:
			{
				selected_slot = boost::none;
				break;
			}
			default:
				break;
		}
	}

	return graph_altered;
}

cse::InterfaceEventArray cse::GraphSubwindow::process_event(const InputEvent& event, const InteractionMode interaction_mode, const bool is_hovered) const
{
	InterfaceEventArray new_events;
	// Things that happen even if the window is not hovered first
	if (event.type() == InputEventType::MOUSE_BUTTON) {
		assert(event.details_mouse_button().has_value());
		const auto details{ event.details_mouse_button().get() };
		const bool mod_ctrl{ static_cast<bool>(details.mods & ImGuiKeyModFlags_Ctrl) };
		const bool mod_shift{ static_cast<bool>(details.mods & ImGuiKeyModFlags_Shift) };
		if (details.button == ImGuiMouseButton_Middle && details.action == entry::ClickAction::Release) {
			const InterfaceEvent pan_event{ InterfaceEventType::MOUSE_PAN_END, SubwindowId::GRAPH };
			new_events.push(pan_event);
		}
		else if (details.button == ImGuiMouseButton_Left && details.action == entry::ClickAction::Release) {
			const InterfaceEvent move_event{ InterfaceEventType::MOUSE_MOVE_END, SubwindowId::GRAPH };
			new_events.push(move_event);
			const InterfaceEvent connection_event{ InterfaceEventType::CONNECTION_END, SubwindowId::GRAPH };
			new_events.push(connection_event);
			if (mod_ctrl) {
				const InterfaceEvent box_select_event{ InterfaceEventType::BOX_SELECT_END, SelectMode::ADD,       SubwindowId::GRAPH };
				new_events.push(box_select_event);
			}
			else if (mod_shift) {
				const InterfaceEvent box_select_event{ InterfaceEventType::BOX_SELECT_END, SelectMode::TOGGLE,    SubwindowId::GRAPH };
				new_events.push(box_select_event);
			}
			else {
				const InterfaceEvent box_select_event{ InterfaceEventType::BOX_SELECT_END, SelectMode::EXCLUSIVE, SubwindowId::GRAPH };
				new_events.push(box_select_event);
			}
		}
	}
    else if (event.type() == InputEventType::DROP_FILE) {
        
        const auto details{ event.details_dropfile().get() };
        if(details.result == 0){
            if(the_state->getWaitInputFile()){
                the_state->releaseWaitInputFile();
                the_state->setStatePath(details.filePath,true);
                the_state->set_input_reload();
            }else{
                if(selected_slot.has_value()){
                    const SetSlotImageDetails add_detail{ selected_slot.value()
                        , details.filePath.getCPtr() };
                    const InterfaceEvent add_event{ add_detail };
                    new_events.push(add_event);
                }
            }
        }else{
            the_state->releaseWaitInputFile();
        }
    }
    else if (event.type() == InputEventType::SAVE_FILE) {
        const auto details{ event.details_savefile().get() };
        if(details.result == 0){
            the_state->setStatePath(details.filePath,true);
            new_events.push(InterfaceEventType::SAVE_TO_MAX);
        }
    }
    
	if (is_hovered) {
		// Things that happen regardless of mode first
		if (event.type() == InputEventType::MOUSE_BUTTON) {
			assert(event.details_mouse_button().has_value());
			const auto details{ event.details_mouse_button().get() };
			if (details.button == ImGuiMouseButton_Middle && details.action == entry::ClickAction::Pressing) {
				const InterfaceEvent pan_event{ InterfaceEventType::MOUSE_PAN_BEGIN, SubwindowId::GRAPH };
				new_events.push(pan_event);
			}
		}
		if (interaction_mode == InteractionMode::SELECT) {
			if (event.type() == InputEventType::MOUSE_BUTTON) {
				assert(event.details_mouse_button().has_value());
				const auto details{ event.details_mouse_button().get() };
				const bool mod_ctrl{ static_cast<bool>(details.mods & ImGuiKeyModFlags_Ctrl) };
				const bool mod_shift{ static_cast<bool>(details.mods & ImGuiKeyModFlags_Shift) };
				if (details.button == ImGuiMouseButton_Left && details.action == entry::ClickAction::Pressing) {
					const auto in_pin{ get_pin_at_pos(details.pos, csg::SlotDirection::INPUT) };
					const auto out_pin{ get_pin_at_pos(details.pos, csg::SlotDirection::OUTPUT) };
					const auto node_id{ get_node_at_pos(details.pos) };
					// Slot can only be set when node is also set
					const auto slot{ get_slot_at_pos(details.pos, csg::SlotDirection::INPUT) };
					if (in_pin) {
						const InterfaceEvent connection_event{ InterfaceEventType::CONNECTION_ALTER, SlotIdDetails{ *in_pin }, SubwindowId::GRAPH };
						new_events.push(connection_event);
					}
					else if (out_pin) {
						const InterfaceEvent connection_event{ InterfaceEventType::CONNECTION_BEGIN, SlotIdDetails{ *out_pin }, SubwindowId::GRAPH };
						new_events.push(connection_event);
					}
					else if (node_id) {
						if (slot) {
							// Check if the slot has a value
							// Slots with no value are unselectable
							if (the_graph->get_slot_value(*slot)) {
								// Select the slot
								const InterfaceEvent slot_event{ InterfaceEventType::SELECT_SLOT, *slot, boost::none };
								new_events.push(slot_event);
							}
						}

						// Select the node
						if (mod_ctrl) {
							const InterfaceEvent select_event{ SelectNodeDetails{ SelectMode::ADD, node_id.get() } };
							new_events.push(select_event);
						}
						else if (mod_shift) {
							const InterfaceEvent select_event{ SelectNodeDetails{ SelectMode::TOGGLE, node_id.get() } };
							new_events.push(select_event);
						}
						else {
							// No modifiers, this does an exclusive select and begins movement
							const InterfaceEvent select_event{ SelectNodeDetails{ SelectMode::EXCLUSIVE, node_id.get() } };
							new_events.push(select_event);
							const InterfaceEvent move_event{ InterfaceEventType::MOUSE_MOVE_BEGIN, SubwindowId::GRAPH };
							new_events.push(move_event);
						}
					}
					else {
						// Select no slot
						const InterfaceEvent slot_event{ InterfaceEventType::SELECT_SLOT_NONE, boost::none };
						new_events.push(slot_event);
						// Begin box selection
						const InterfaceEvent box_select_event{ InterfaceEventType::BOX_SELECT_BEGIN, SubwindowId::GRAPH };
						new_events.push(box_select_event);
					}
				}
			}
		}
	}

	return new_events;
}

void cse::GraphSubwindow::set_window_size(const csc::Int2 size)
{
	window_size = size;
}

void cse::GraphSubwindow::update_mouse(const csc::Float2 mouse_screen_pos, csc::Float2 mouse_delta)
{
	mouse_world_pos = screen_to_world(mouse_screen_pos);
	if (get_mode() == InteractionMode::MOUSE_PAN) {
		view_center = view_center - csc::Int2{ mouse_delta };
	}
	else if (get_mode() == InteractionMode::MOUSE_MOVE) {
		the_graph->move(node_selection.selected(), mouse_delta);
	}
}

bool cse::GraphSubwindow::has_selection() const
{
	return (node_selection.count() > 0);
}

boost::optional<cse::InteractionMode> cse::GraphSubwindow::get_mode() const
{
	if (_mouse_pan_active) {
		return InteractionMode::MOUSE_PAN;
	}
	else if (_mouse_move_active) {
		return InteractionMode::MOUSE_MOVE;
	}
	else if (box_select_begin.has_value()) {
		return InteractionMode::BOX_SELECT;
	}
	else {
		return boost::none;
	}
}

csc::Float2 cse::GraphSubwindow::get_window_pos() const
{
	return csc::Float2{ 0.0f, 20.0f };
}

csc::FloatRect cse::GraphSubwindow::get_draw_rect() const
{
	const csc::Float2 begin{ get_window_pos() + csc::Float2{ 0.0f, 27.0f } };
	const csc::Float2 end = csc::Float2{ window_size } - csc::Float2{ 0.0f, 20.0f };
	return csc::FloatRect{ begin, end };
}

void cse::GraphSubwindow::draw_grid(ImDrawList* const draw_list) const
{
	// All draws are in (non-imgui) window space and must be offset by the widget begin position
	const csc::FloatRect draw_rect{ get_draw_rect() };
	const csc::Float2 limits{ draw_rect.end() };
	
	draw_grid_layer(draw_list, GRID_MINOR_SPACING, COLOR_GRID_LINE_2);
	draw_grid_layer(draw_list, GRID_MAJOR_SPACING, COLOR_GRID_LINE_1);

	// Always draw cross at the origin
	{
		const csc::Float2 draw_origin{ world_to_screen(csc::Float2{}) };
		{
			const csc::Float2 horizontal_begin{ csc::Float2{ 0.0f, draw_origin.y } };
			const csc::Float2 horizontal_end{ csc::Float2{ limits.x, draw_origin.y } };
			ImGui::DrawList::AddLine(draw_list, horizontal_begin, horizontal_end, COLOR_GRID_LINE_0, 2.2f);
			const csc::Float2 vertical_begin = csc::Float2{ draw_origin.x, 0.0f };
			const csc::Float2 vertical_end = csc::Float2{ draw_origin.x, limits.y };
			ImGui::DrawList::AddLine(draw_list, vertical_begin, vertical_end, COLOR_GRID_LINE_0, 2.2f);
		}
	}
}

void cse::GraphSubwindow::draw_grid_layer(ImDrawList* const draw_list, const float grid_spacing, const unsigned int color) const
{
	// All draws are in (non-imgui) window space and must be offset by the widget begin position
	const csc::FloatRect draw_rect{ get_draw_rect() };
	const csc::Float2 limits{ draw_rect.end() };
	const csc::Float2 lowest_world{ csc::Float2{ view_center } - draw_rect.size() / 2.0f };

	// Horizontal
	{
		// From the lowest point, find the next line
		const float first_line_y_world{ std::ceil(lowest_world.y / grid_spacing) * grid_spacing };
		float next_line_y_local{ world_to_screen(csc::Float2{ 0.0f, first_line_y_world }).y };
		while (next_line_y_local < limits.y) {
			ImGui::DrawList::AddLine(draw_list, csc::Float2{ 0.0f, next_line_y_local }, csc::Float2{ limits.x, next_line_y_local }, color);
			next_line_y_local += grid_spacing;
		}
	}

	// Vertical
	{
		// From the lowest point, find the next line
		const float first_line_x_world{ std::ceil(lowest_world.x / grid_spacing) * grid_spacing };
		float next_line_x_local{ world_to_screen(csc::Float2{ first_line_x_world, 0.0f }).x };
		while (next_line_x_local < limits.x) {
			ImGui::DrawList::AddLine(draw_list, csc::Float2{ next_line_x_local, 0.0f }, csc::Float2{ next_line_x_local, limits.y }, color);
			next_line_x_local += grid_spacing;
		}
	}
}

void cse::GraphSubwindow::draw_nodes(ImDrawList* const draw_list) const
{
	if (static_cast<bool>(the_graph) == false) {
		// No graph, nothing to render
		return;
	}
	
	const csc::FloatRect draw_rect{ get_draw_rect() };

	// Use this to track the geometry of whichever node is the pending connection source
	boost::optional<NodeGeometry> connection_src_node_geom;

	// Draw nodes (reverse order so nodes at the beginning of the list are drawn last)
	for (const auto& node : boost::adaptors::reverse(the_graph->nodes())) {
		const auto opt_node_type_info{ csg::NodeTypeInfo::from(node->type()) };
		assert(opt_node_type_info.has_value());
		const csg::NodeTypeInfo node_type_info{ opt_node_type_info.get() };

		// Get node geometry and convert to screen space
		const NodeGeometry node_geom_world{ *node };
		const NodeGeometry node_geom{ node_geom_world.with_pos( world_to_screen(node_geom_world.pos()) ) };

		// Store geometry if this node is the pending connection source
		if (pending_connection_begin && pending_connection_begin->node_id() == node->id()) {
			connection_src_node_geom = node_geom;
		}

		// Skip to the next node if this is out of the view
		if (draw_rect.overlaps(node_geom.rect()) == false) {
			continue;
		}

		const bool node_has_selected_slot = selected_slot ? selected_slot->node_id() == node->id() : false;

		const bool is_selected{ node_selection.is_selected(node->id()) };
		const ImU32 outline_color = is_selected ? COLOR_NODE_OUTLINE_SELECTED : COLOR_NODE_OUTLINE_DEFAULT;

		// Draw background
		ImGui::DrawList::AddRectFilled(draw_list, node_geom.rect(), COLOR_NODE_BG, NODE_CORNER_RADIUS);

		// Draw header
		{
			const csc::Float2 header_end{ node_geom.end().x, node_geom.pos().y + NODE_HEADER_HEIGHT };
			const csc::FloatRect header_rect{ node_geom.pos(), header_end };
			{
				const ImU32 header_color{ get_category_color(node_type_info.category()) };
				ImGui::DrawList::AddRectFilled(draw_list, header_rect, header_color, NODE_CORNER_RADIUS, ImDrawFlags_RoundCornersTop);
			}
			const csc::Float2 text_pos{ node_geom.pos() + csc::Float2{ 8.0f, 6.0f } };
			ImGui::DrawList::AddText(draw_list, text_pos, COLOR_NODE_TEXT, node_type_info.disp_name());
			const csc::Float2 header_line_0{ csc::Float2{ node_geom.pos().x, header_end.y } };
			const csc::Float2 header_line_1{ csc::Float2{ node_geom.end().x, header_end.y } };
			ImGui::DrawList::AddLine(draw_list, header_line_0, header_line_1, COLOR_NODE_OUTLINE_DEFAULT);
		}

		// Draw slots
		{
			const csc::Float2 body_begin{ node_geom.pos() + csc::Float2{ 0.0f, NODE_HEADER_HEIGHT} };
			// Lines between slots
			for (unsigned int i = 1; i < node->slots().size(); i++) {
				const csc::Float2 p0{ body_begin + csc::Float2{ 0.0f, i * NODE_ROW_HEIGHT } };
				const csc::Float2 p1{ node_geom.end().x, body_begin.y + NODE_ROW_HEIGHT * i };
				ImGui::DrawList::AddLine(draw_list, p0, p1, COLOR_NODE_OUTLINE_DEFAULT);
			}
			csc::Float2 next_slot_begin{ body_begin };

			// Draw outline
			ImGui::DrawList::AddRect(draw_list, node_geom.rect(), outline_color, NODE_CORNER_RADIUS);

			// Main body of slot
			size_t slots_drawn{ 0 };
			for (const auto& slot : node->slots()) {

				const bool highlight_this_slot = node_has_selected_slot && selected_slot->index() == slots_drawn;
				if (highlight_this_slot) {
					const csc::FloatRect highlight_rect = node_geom.slot_rect(slots_drawn).grow(-2.0f);
					ImGui::DrawList::AddRectFilled(draw_list, highlight_rect, COLOR_NODE_SLOT_SELECTED, 6.0f);
				}

				// Label
				{
					const csc::Float2 label_pos{ next_slot_begin + csc::Float2{ 8.0f, 4.0f } };
                    
					const boost::string_view slot_disp_name_view{ get_alt_slot_name(*node, slot.disp_name()) };
					const char* const slot_disp_name{ slot_disp_name_view.data() };
					std::array<char, 96> label_text;
					label_text.fill('\0');
					if (slot.value) {
						if (slot.type() == csg::SlotType::BOOL) {
							const auto bool_value{ slot.value->as<csg::BoolSlotValue>() };
							assert(bool_value.has_value());
							if (bool_value->get()) {
								snprintf(label_text.data(), label_text.size() - 1, "%s: True", slot_disp_name);
							}
							else {
								snprintf(label_text.data(), label_text.size() - 1, "%s: False", slot_disp_name);
							}
						}
						else if (slot.type() == csg::SlotType::COLOR) {
							const auto color_value = slot.value->as<csg::ColorSlotValue>();
							assert(color_value.has_value());
							snprintf(label_text.data(), label_text.size() - 1, "%s: ", slot_disp_name);
							const ImVec2 text_size{ ImGui::CalcTextSize(label_text.data()) };
							const csc::Float2 color_rect_offset{ label_pos + csc::Float2{ text_size.x, 0.0f } };
							const csc::Float2 color_rect_size{ 24.0f, 14.0f };
							const csc::FloatRect color_rect{ color_rect_offset, color_rect_offset + color_rect_size };
							ImGui::DrawList::AddRectFilled(draw_list, color_rect, COLOR_NODE_TEXT);
							const csc::FloatRect inner_rect{ color_rect.grow(-1.0f) };
							const ImU32 inner_color{ ImGui::ColorConvertFloat4ToU32(
								ImVec4(color_value->get().x, color_value->get().y, color_value->get().z, 1.0f))
							};
							ImGui::DrawList::AddRectFilled(draw_list, inner_rect, inner_color);
						}
						else if (slot.type() == csg::SlotType::ENUM) {
                            const auto enum_value{ slot.value->as<csg::EnumSlotValue>() };
							snprintf(label_text.data(), label_text.size() - 1, "%s:%s", slot_disp_name,enum_value.get().internal_name());
						}
						else if (slot.type() == csg::SlotType::FLOAT) {
							const auto float_value = slot.value->as<csg::FloatSlotValue>();
							assert(float_value.has_value());
							// Use snprintf to generate a pattern for another snprintf to get the label
							// This is so the precision held by the slot is respected
							std::array<char, 24> pattern_text;
							pattern_text.fill('\0');
							snprintf(pattern_text.data(), pattern_text.size() - 1, "%%s: %%.%zuf", float_value->precision());
							// pattern_text will look like "%s: %.2f"
							snprintf(label_text.data(), label_text.size() - 1, pattern_text.data(), slot_disp_name, float_value->get());
						}
						else if (slot.type() == csg::SlotType::INT) {
							const auto int_value{ slot.value->as<csg::IntSlotValue>() };
							assert(int_value.has_value());
							snprintf(label_text.data(), label_text.size() - 1, "%s: %d", slot_disp_name, int_value->get());
						}
						else if (slot.type() == csg::SlotType::VECTOR) {
                            const auto vec_value{ slot.value->as<csg::VectorSlotValue>() };
                            const auto vec = vec_value.get().get();
							snprintf(label_text.data(), label_text.size() - 1, "%s: [%f,%f,%f]", slot_disp_name,vec.x,vec.y,vec.z);
						}
						else if (slot.type() == csg::SlotType::CURVE_RGB) {
							snprintf(label_text.data(), label_text.size() - 1, "%s: [Curves]", slot_disp_name);
						}
						else if (slot.type() == csg::SlotType::CURVE_VECTOR) {
							snprintf(label_text.data(), label_text.size() - 1, "%s: [Curves]", slot_disp_name);
						}
						else if (slot.type() == csg::SlotType::COLOR_RAMP) {
							snprintf(label_text.data(), label_text.size() - 1, "%s: [Ramp]", slot_disp_name);
						}
                        else if (slot.type() == csg::SlotType::IMAGE) {
                            const auto image_value{ slot.value->as<csg::ImageSlotValue>() };
                            const auto imagePath = image_value.get().get();
                            auto bxFilePath = bx::FilePath(imagePath);
                            auto fileNam = bxFilePath.getFileName();
                            snprintf(label_text.data(), label_text.size() - 1, "%s: %s", slot_disp_name,fileNam.getPtr());
                        }
						else {
							snprintf(label_text.data(), label_text.size() - 1, "%s: ?", slot_disp_name);
						}
					}
					else {
						snprintf(label_text.data(), label_text.size() - 1, "%s", slot_disp_name);
					}
                    ImGui::DrawList::AddText(draw_list, label_pos, COLOR_NODE_TEXT, label_text.data());

                    if(slot.type() == csg::SlotType::IMAGE){
                        const auto image_value{ slot.value->as<csg::ImageSlotValue>() };
                        const auto imagePath = image_value.get().get();
                        bgfx::TextureInfo* textureInfo;
                        bgfx::TextureHandle uth = the_state->AddTextureHandler(imagePath,&textureInfo);
                        Float2 showTextSize = Float2(50*(float(textureInfo->width)/float(textureInfo->height)),50);
                        ImGui::DrawList::AddImage(draw_list, label_pos+csc::Float2(0.0f, NODE_ROW_HEIGHT),showTextSize, COLOR_NODE_TEXT,uth);
                        next_slot_begin = next_slot_begin + showTextSize;
                    }
				}
				// Pins
				if (slot.has_pin()) {
					const float circle_x_offset = (slot.dir() == csg::SlotDirection::INPUT) ? 0.0f : node_geom.size().x;
					const csc::Float2 circle_pos{ next_slot_begin + csc::Float2{ circle_x_offset, NODE_ROW_HEIGHT / 2.0f } };
					ImGui::DrawList::AddCircleFilled(draw_list, circle_pos, NODE_PIN_RADIUS, get_slot_type_color(slot.type()));
					ImGui::DrawList::AddCircle(draw_list, circle_pos, NODE_PIN_RADIUS, COLOR_NODE_SLOT_CONNECTION_OUTLINE);
				}

				next_slot_begin = next_slot_begin + csc::Float2(0.0f, NODE_ROW_HEIGHT);
				slots_drawn++;
			}
		}

	}

	// Draw connections
	for (const csg::Connection& conn : the_graph->connections()) {
		const boost::optional<std::shared_ptr<const csg::Node>> node_src{ the_graph->get(conn.source().node_id()) };
		if (node_src.has_value() == false || node_src->use_count() == 0) {
			continue;
		}
		const boost::optional<std::shared_ptr<const csg::Node>> node_dest = the_graph->get(conn.dest().node_id());
		if (node_dest.has_value() == false || node_dest->use_count() == 0) {
			continue;
		}
		const NodeGeometry geom_src_world{ **node_src };
		const NodeGeometry geom_src_screen{ geom_src_world.with_pos(world_to_screen(geom_src_world.pos())) };
		const NodeGeometry geom_dest_world{ **node_dest };
		const NodeGeometry geom_dest_screen{ geom_dest_world.with_pos(world_to_screen(geom_dest_world.pos())) };

		const csc::Float2 begin{ geom_src_screen.pin_pos(conn.source().index(), csg::SlotDirection::OUTPUT) };
		const csc::Float2 end{ geom_dest_screen.pin_pos(conn.dest().index(),   csg::SlotDirection::INPUT) };
		ImGui::DrawList::AddLine(draw_list, begin, end, COLOR_CONNECTION, 1.5f);
	}

	// Draw connection in progress
	if (connection_src_node_geom) {
		const size_t pin_index = pending_connection_begin->index();
		const csc::Float2 pin_pos{ connection_src_node_geom->pin_pos(pin_index, csg::SlotDirection::OUTPUT) };
		const csc::Float2 mouse_screen_pos{ world_to_screen(mouse_world_pos) };
		ImGui::DrawList::AddLine(draw_list, pin_pos, mouse_screen_pos, COLOR_CONNECTION_PENDING, 2.0f);
	}
}

void cse::GraphSubwindow::draw_select_box(ImDrawList* const draw_list) const
{
	if (get_mode() != InteractionMode::BOX_SELECT) {
		return;
	}
	assert(box_select_begin.has_value());
	const csc::Float2 begin_screen_pos{ world_to_screen(csc::Float2(box_select_begin.value())) };
	const csc::Float2 end_screen_pos{ world_to_screen(mouse_world_pos) };
	const csc::FloatRect select_rect{ begin_screen_pos, end_screen_pos };
	ImGui::DrawList::AddRect(draw_list, select_rect, COLOR_SELECTION_BOX);
}

boost::optional<csc::FloatRect> cse::GraphSubwindow::selection_rect() const
{
	if (get_mode() != InteractionMode::BOX_SELECT) {
		return boost::none;
	}
	assert(box_select_begin.has_value());
	const csc::Float2 box_select_end{ mouse_world_pos };
	return csc::FloatRect{ csc::Float2{ box_select_begin.value() }, box_select_end };
}

std::set<csg::NodeId> cse::GraphSubwindow::get_nodes_in_rect(const csc::FloatRect world_rect) const
{
	std::set<csg::NodeId> result;
	for (const auto& node : the_graph->nodes()) {
		const NodeGeometry node_geom{ *node };
		if (world_rect.overlaps(node_geom.rect())) {
			result.insert(node->id());
		}
	}
	return result;
}

boost::optional<csg::NodeId> cse::GraphSubwindow::get_node_at_pos(const csc::Float2 screen_pos) const
{
	const csc::Float2 world_pos{ screen_to_world(screen_pos) };

	for (const auto& node : the_graph->nodes()) {
		const NodeGeometry node_geom{ *node };
		if (node_geom.rect().contains(world_pos)) {
			return node->id();
		}
	}

	return boost::none;
}

boost::optional<csg::SlotId> cse::GraphSubwindow::get_pin_at_pos(const csc::Float2 screen_pos, const csg::SlotDirection direction) const
{
	const csc::Float2 world_pos{ screen_to_world(screen_pos) };

	for (const auto& node : the_graph->nodes()) {
		const NodeGeometry node_geom{ *node };
		const boost::optional<size_t> maybe_pin{ node_geom.pin_at_pos(world_pos, direction) };
		if (maybe_pin) {
			if (node->has_pin(maybe_pin.value(), direction)) {
				// A match has been found
				return csg::SlotId{ node->id(), maybe_pin.value() };
			}
			else {
				return boost::none;
			}
		}
		else if (node_geom.rect().contains(world_pos)) {
			// The node itself was clicked, abort checking lower nodes
			return boost::none;
		}
	}

	return boost::none;
}

boost::optional<csg::SlotId> cse::GraphSubwindow::get_slot_at_pos(const csc::Float2 screen_pos, const boost::optional<csg::SlotDirection> direction) const
{
	const csc::Float2 world_pos{ screen_to_world(screen_pos) };

	for (const auto& node : the_graph->nodes()) {
		const NodeGeometry node_geom{ *node };
		if (node_geom.rect().contains(world_pos)) {
			const boost::optional<size_t> slot_id{ node_geom.slot_at_pos(world_pos) };
			if (slot_id) {
				const boost::optional<csg::Slot> slot{ node->slot(*slot_id) };
				if (slot) {
					// We have found a real slot, check that the direction matches before returning
					if (direction) {
						if (slot->dir() == *direction) {
							return csg::SlotId{ node->id(), *slot_id };
						}
					}
					else {
						return csg::SlotId{ node->id(), *slot_id };
					}
				}
			}
			// Break here so you can not click "through" nodes
			// Once we have found a node that the mouse is over, we should not look for any more
			break;
		}
	}
	return boost::none;
}

csc::Float2 cse::GraphSubwindow::world_to_screen(const csc::Float2 world_pos) const
{
	const csc::Int2 middle_offset{ get_draw_rect().size() / 2.0f };
	const csc::Float2 local_pos{ world_pos - csc::Float2{ view_center - middle_offset } };
	return local_pos + get_draw_rect().begin();
}

csc::Float2 cse::GraphSubwindow::screen_to_world(const csc::Float2 screen_pos) const
{
	const csc::Float2 local_pos{ screen_pos - get_draw_rect().begin() };
	const csc::Int2 middle_offset{ get_draw_rect().size() / 2.0f };
	const csc::Float2 before_translate{ local_pos - csc::Float2{ middle_offset } };
	return before_translate + csc::Float2{ view_center };
}
