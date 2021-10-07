#include "subwindow_alert.h"

#include <boost/optional.hpp>
#include "imgui/imgui.h"

#include "enum.h"

void cse::AlertSubwindow::set_message(const std::string& message_in)
{
	this->message = message_in;
}

cse::InterfaceEventArray cse::AlertSubwindow::run() const
{
	InterfaceEventArray events;

	events.push(InterfaceEvent{ InterfaceEventType::SUBWINDOW_IS_HOVERED, SubwindowIdDetails{ SubwindowId::ALERT }, boost::none });

	if (ImGui::Begin("Alert", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("%s", message.c_str());
		ImGui::Separator();
		if (ImGui::Button("Close")) {
			events.push(InterfaceEvent{ InterfaceEventType::MODAL_ALERT_CLOSE });
		}
	}
	ImGui::End();

	return events;
}
