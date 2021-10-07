#pragma once

/**
 * @file
 * @brief Defines GraphSubwindow.
 */

#include <memory>
#include <set>

#include <boost/optional.hpp>

#include "../shader_core/rect.h"
#include "../shader_core/vector.h"
#include "../shader_graph/node_id.h"
#include "../shader_graph/slot.h"

#include "enum.h"
#include "event.h"
#include "selection.h"

struct ImDrawList;

namespace csg {
	class Graph;
	class SlotId;
}

namespace cse {
    class SharedState;
	class GraphSubwindow {
	public:
        GraphSubwindow(std::shared_ptr<csg::Graph> the_graph,std::shared_ptr<cse::SharedState> state) : the_graph{ the_graph },the_state {state}{}

		InterfaceEventArray run(InteractionMode mode, bool graph_unsaved) const;

		// Returns true if this event cause a change that may affect undo state
		bool do_event(const InterfaceEvent& event);
		InterfaceEventArray process_event(const InputEvent& event, InteractionMode interaction_mode, bool is_hovered) const;

		void set_window_size(csc::Int2 size);
		void update_mouse(csc::Float2 mouse_screen_pos, csc::Float2 mouse_delta);

		bool has_selection() const;
		boost::optional<InteractionMode> get_mode() const;

	private:
		csc::Float2 get_window_pos() const;
		csc::FloatRect get_draw_rect() const;

		void draw_grid(ImDrawList* draw_list) const;
		void draw_grid_layer(ImDrawList* draw_list, float grid_spacing, unsigned int color) const;
		void draw_nodes(ImDrawList* draw_list) const;
		void draw_select_box(ImDrawList* draw_list) const;
        
		
		boost::optional<csc::FloatRect> selection_rect() const;
		std::set<csg::NodeId> get_nodes_in_rect(csc::FloatRect world_rect) const;
		boost::optional<csg::NodeId> get_node_at_pos(csc::Float2 screen_pos) const;
		boost::optional<csg::SlotId> get_pin_at_pos(csc::Float2 screen_pos, csg::SlotDirection direction) const;
		boost::optional<csg::SlotId> get_slot_at_pos(csc::Float2 screen_pos, boost::optional<csg::SlotDirection> direction = boost::none) const;

		csc::Float2 world_to_screen(csc::Float2 world_pos) const;
		csc::Float2 screen_to_world(csc::Float2 screen_pos) const;

		std::shared_ptr<csg::Graph> the_graph;
        std::shared_ptr<cse::SharedState> the_state;
        
		NodeSelection node_selection;

		csc::Int2 window_size{ 1, 1 };

		csc::Int2 view_center;
		
		csc::Float2 mouse_world_pos;

		boost::optional<csc::Int2> box_select_begin;
		boost::optional<csg::SlotId> pending_connection_begin;

		boost::optional<csg::SlotId> selected_slot;

		bool _mouse_move_active{ false };
		bool _mouse_pan_active{ false };
	};
}
