#include "subwindow_node_list.h"

#include "imgui/imgui.h"

#include "../shader_graph/node_type.h"

#include "enum.h"

cse::NodeListSubwindow::NodeListSubwindow()
{

}

cse::InterfaceEventArray cse::NodeListSubwindow::run() const
{
	InterfaceEventArray events;
	ImGui::SetNextWindowSizeConstraints(ImVec2{ 160.f, 0.f }, ImVec2{ 1000.f, 600.f });
	if (ImGui::Begin("Nodes", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		constexpr float WINDOW_WIDTH{ 180.0f };
		for (const auto this_category : csg::NodeCategoryList{}) {
			const auto category_info{ csg::NodeCategoryInfo::from(this_category) };
			if (category_info && category_info->allow_creation()) {
				if (ImGui::CollapsingHeader(category_info->name())) {
					for (const csg::NodeType node_type : csg::NodeTypeList()) {
						const auto node_info{ csg::NodeTypeInfo::from(node_type) };
						if (node_info->category() == this_category) {
							if (node_info->type() == _selected_type) {
								ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ButtonActive));
								if (ImGui::Button(node_info->disp_name(), ImVec2{ WINDOW_WIDTH, 0.f })) {
									const InterfaceEvent select_event{ InterfaceEventType::SELECT_NODE_TYPE_NONE, SubwindowId::NODE_LIST };
									events.push(select_event);
								}
								ImGui::PopStyleColor();
							}
							else if (ImGui::Button(node_info->disp_name(), ImVec2{ WINDOW_WIDTH, 0.f })) {
								events.push(InterfaceEvent{ InterfaceEventType::SELECT_NODE_TYPE, NodeTypeDetails{ node_info->type() } });
							}
						}
					}
					//ImGui::TreePop();
				}
				else if (ImGui::IsItemDeactivated() && _selected_type) {
					// When a tree section is closed, deselect the current button if it is part of that section
					if (const auto node_info{ csg::NodeTypeInfo::from(*_selected_type) }) {
						if (node_info->category() == this_category) {
							const InterfaceEvent select_event{ InterfaceEventType::SELECT_NODE_TYPE_NONE, SubwindowId::NODE_LIST };
							events.push(select_event);
						}
					}
				}
			}
		}
	}
	ImGui::End();

	return events;
}

void cse::NodeListSubwindow::do_event(const InterfaceEvent& event)
{
	if (event.target_subwindow() && event.target_subwindow() == SubwindowId::NODE_LIST) {
		// Event is targeting this subwindow
		switch (event.type()) {
			case InterfaceEventType::SELECT_NODE_TYPE:
			{
				if (event.details_as<NodeTypeDetails>()) {
					_selected_type = event.details_as<NodeTypeDetails>()->value;
				}
				break;
			}
			case InterfaceEventType::SELECT_NODE_TYPE_NONE:
			{
				_selected_type = boost::none;
				break;
			}
			default:
				// Do nothing
				break;
		}
	}
}

boost::optional<csg::NodeType> cse::NodeListSubwindow::selected_type() const
{
	return _selected_type;
}
