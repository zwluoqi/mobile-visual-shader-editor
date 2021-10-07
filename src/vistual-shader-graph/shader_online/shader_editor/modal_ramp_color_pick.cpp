#include "modal_ramp_color_pick.h"

#include <cassert>

#include "imgui/imgui.h"

void cse::ModalRampColorPicker::attach(const csg::SlotId slot_id, const size_t index, const csc::Float4 rgba_in)
{
	rgba = rgba_in.as_array();
	attached_location = ColorLocation{ slot_id, index };
}

cse::InterfaceEventArray cse::ModalRampColorPicker::run() const
{
	InterfaceEventArray result;

	assert(attached_location.has_value());

	if (ImGui::Begin("Ramp Color Picker", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

		std::array<float, 4> mut_rgba{ rgba };
		ImGui::ColorPicker4("Color", mut_rgba.data(), ImGuiColorEditFlags_Float);
		if (rgba != mut_rgba) {
			const csc::Float4 rgba_f4{ mut_rgba[0], mut_rgba[1], mut_rgba[2], mut_rgba[3] };
			InterfaceEvent color_event{ InterfaceEventType::RAMP_COLOR_PICK_UPDATE, Float4Details{ rgba_f4 }, SubwindowId::MODAL_RAMP_COLOR_PICK };
			result.push(color_event);
		}

		if (ImGui::Button("OK")) {
			const csc::Float4 rgba_f4{ mut_rgba[0], mut_rgba[1], mut_rgba[2], mut_rgba[3] };
			InterfaceEvent ramp_color_event{ ModifySlotRampColorDetails{ attached_location->slot_id, attached_location->index, rgba_f4 } };
			result.push(ramp_color_event);
			InterfaceEvent close_event{ InterfaceEventType::MODAL_RAMP_COLOR_PICK_CLOSE };
			result.push(close_event);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			InterfaceEvent close_event{ InterfaceEventType::MODAL_RAMP_COLOR_PICK_CLOSE };
			result.push(close_event);
		}

		ImGui::End();
	}

	return result;
}

void cse::ModalRampColorPicker::do_event(const InterfaceEvent& event)
{
	assert(event.target_subwindow() && event.target_subwindow() == SubwindowId::MODAL_RAMP_COLOR_PICK);
	switch (event.type()) {
	case InterfaceEventType::RAMP_COLOR_PICK_UPDATE:
	{
		const boost::optional<Float4Details> opt_details{ event.details_as<Float4Details>() };
		assert(opt_details.has_value());
		rgba[0] = opt_details->value.x;
		rgba[1] = opt_details->value.y;
		rgba[2] = opt_details->value.z;
		rgba[3] = opt_details->value.w;
		break;
	}
	default:
		// Every event sent here should be handled
		assert(false);
	}
}
