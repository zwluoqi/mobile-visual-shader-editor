#include "subwindow_param_editor.h"

#include <array>
#include <cassert>
#include <cstdio>

#include "imgui/imgui.h"

#include "../shader_core/vector.h"
#include "../shader_graph/graph.h"
#include "../shader_graph/node.h"
#include "../shader_graph/node_enums.h"
#include "../shader_graph/node_type.h"
#include "../shader_graph/ramp.h"
#include "../shader_graph/slot.h"
#include "../shader_graph/slot_id.h"

#include "enum.h"
#include "event.h"
#include "wrapper_imgui_func.h"

cse::InterfaceEventArray cse::ParamEditorSubwindow::run() const
{
	InterfaceEventArray result;

	if (selected_slot.has_value() == false) {
		return result;
	}

	const auto selected_node{ the_graph->get(selected_slot->node_id()) };
	if (selected_node.use_count() == 0) {
		return result;
	}

	const auto opt_type_info{ csg::NodeTypeInfo::from(selected_node->type()) };
	assert(opt_type_info.has_value());
	const csg::NodeTypeInfo type_info{ opt_type_info.value() };

	const auto opt_slot{ selected_node->slot(selected_slot->index()) };
	if (opt_slot.has_value() == false) {
		return result;
	}

//    if(opt_slot->type() == csg::SlotType::BOOL){
//        return result;
//    }
//
//    if(opt_slot->type() == csg::SlotType::FLOAT){
//        return result;
//    }
    
	ImGui::SetNextWindowSizeConstraints(ImVec2{ 0.f, 0.f }, ImVec2{ 1000.f, 600.f });
	if (ImGui::Begin("Parameter Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

		//std::stringstream namestream;
		//namestream << NodeTypeInfo::from(dest_node->type())->name();
		//namestream << nodeOrder;
		//namestream << "_";
		//namestream << type_prefixname_str(opt_slot->type());
		//namestream << slotId.index();
		//return namestream.str();

		ImGui::Text("Node Type: %s", type_info.disp_name());
		ImGui::Text("Param Name: %s", the_graph->GetSlotName(selected_slot.value()).c_str());
		ImGui::Text("Param Type: %s", csg::Slot::type_name_str(opt_slot->type()));
		ImGui::Dummy(ImVec2{ 230.0f, 1.0f });
		ImGui::Separator();
		if (opt_slot->dir() == csg::SlotDirection::INPUT) {
			if (opt_slot->value.has_value()) {
				if (opt_slot->type() == csg::SlotType::BOOL) {
					const auto opt_bool_val{ opt_slot->value->as<csg::BoolSlotValue>() };
					if (opt_bool_val) {
						const InterfaceEventArray bool_event{ run_bool(*selected_slot, *opt_bool_val) };
						result.push(bool_event);
					}
					else {
						ImGui::Text("Error: Failed to find editable bool.");
					}
				}
				else if (opt_slot->type() == csg::SlotType::COLOR) {
					const auto opt_color_val{ opt_slot->value->as<csg::ColorSlotValue>() };
					if (opt_color_val) {
						const InterfaceEventArray color_event{ run_color(*selected_slot, *opt_color_val) };
						result.push(color_event);
					}
					else {
						ImGui::Text("Error: Failed to find editable color.");
					}
				}
				else if (opt_slot->type() == csg::SlotType::ENUM) {
					const auto opt_enum_val{ opt_slot->value.value().as<csg::EnumSlotValue>() };
					if (opt_enum_val) {
						const InterfaceEventArray enum_event{ run_enum(*selected_slot, *opt_enum_val) };
						result.push(enum_event);
					}
					else {
						ImGui::Text("Error: Failed to find editable enum.");
					}
				}
				else if (opt_slot->type() == csg::SlotType::FLOAT) {
					const auto opt_float_val{ opt_slot->value.value().as<csg::FloatSlotValue>() };
					if (opt_float_val) {
						const InterfaceEventArray float_event{ run_float(*selected_slot, *opt_float_val) };
						result.push(float_event);
					}
					else {
						ImGui::Text("Error: Failed to find editable float.");
					}
				}
				else if (opt_slot->type() == csg::SlotType::INT) {
					const auto opt_int_val{ opt_slot->value.value().as<csg::IntSlotValue>() };
					if (opt_int_val) {
						const InterfaceEventArray int_event{ run_int(*selected_slot, *opt_int_val) };
						result.push(int_event);
					}
					else {
						ImGui::Text("Error: Failed to find editable float.");
					}
				}
				else if (opt_slot->type() == csg::SlotType::VECTOR) {
					const auto opt_vec_val{ opt_slot->value.value().as<csg::VectorSlotValue>() };
					if (opt_vec_val) {
						const InterfaceEventArray vec_event{ run_vector(*selected_slot, *opt_vec_val) };
						result.push(vec_event);
					}
					else {
						ImGui::Text("Error: Failed to find editable vector.");
					}
				}
				else if (opt_slot->type() == csg::SlotType::CURVE_RGB) {
					if (ImGui::Button("Open RGB Curve Editor")) {
						const InterfaceEvent new_event{ InterfaceEventType::MODAL_CURVE_EDITOR_SHOW  };
						result.push(new_event);
					}
				}
				else if (opt_slot->type() == csg::SlotType::CURVE_VECTOR) {
					if (ImGui::Button("Open Vector Curve Editor")) {
						const InterfaceEvent new_event{ InterfaceEventType::MODAL_CURVE_EDITOR_SHOW };
						result.push(new_event);
					}
				}
				else if (opt_slot->type() == csg::SlotType::COLOR_RAMP) {
					const auto opt_ramp_val{ opt_slot->value.value().as<csg::ColorRampSlotValue>() };
					if (opt_ramp_val) {
						const InterfaceEventArray enum_events{ run_color_ramp(*selected_slot, *opt_ramp_val) };
						result.push(enum_events);
					}
					else {
						ImGui::Text("Error: Failed to find editable ramp.");
					}
				}
                else if (opt_slot->type() == csg::SlotType::IMAGE) {
                    const auto opt_image_val{ opt_slot->value.value().as<csg::ImageSlotValue>() };
                    if (opt_image_val) {
                        const InterfaceEventArray enum_events{ run_image(*selected_slot, *opt_image_val) };
                        result.push(enum_events);
                    }
                    else {
                        ImGui::Text("Error: Failed to find editable ramp.");
                    }
                }
				else {
					ImGui::Text("TODO");
				}
			}
			else {
				ImGui::Text("This slot cannot be modified.");
			}
		}
		else {
			ImGui::Text("Unable to edit output slots.");
		}
	}
	ImGui::End();

	return result;
}

cse::InterfaceEventArray cse::ParamEditorSubwindow::run_bool(const csg::SlotId slot_id, const csg::BoolSlotValue slot_value) const
{
	InterfaceEventArray result;

	const bool current_value{ slot_value.get() };

	const bool true_selected{ current_value };
	const bool false_selected{ !true_selected };

	if (ImGui::RadioButton("True", true_selected)) {
		if (current_value != true) {
			const InterfaceEvent bool_event{ SetSlotBoolDetails{ slot_id, true } };
			result.push(bool_event);
		}
	}
	if (ImGui::RadioButton("False", false_selected)) {
		if (current_value != false) {
			const InterfaceEvent bool_event{ SetSlotBoolDetails{ slot_id, false } };
			result.push(bool_event);
		}
	}

	return result;
}

cse::InterfaceEventArray cse::ParamEditorSubwindow::run_color(const csg::SlotId slot_id, const csg::ColorSlotValue slot_value) const
{
	const ImGuiColorEditFlags flags{ ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_Float };

	InterfaceEventArray result;

	if (edit_color_color.has_value() == false) {
		// We have not captured this slot's color yet, do this now and render nothing
		const InterfaceEvent new_event{ InterfaceEventType::PARAM_EDIT_COLOR_INIT, Float3Details{ slot_value.get() }, SubwindowId::PARAM_EDITOR };
		result.push(new_event);
		return result;
	}

	assert(edit_color_color.has_value());

	std::array<float, 3> color_buffer{ edit_color_color->as_array() };
	ImGui::ColorPicker3("Color", color_buffer.data(), flags);
	const csc::Float3 new_color{ color_buffer };
	if (new_color != edit_color_color.value()) {
		const InterfaceEvent new_event{ InterfaceEventType::PARAM_EDIT_COLOR_CHANGE, Float3Details{ new_color }, SubwindowId::PARAM_EDITOR };
		result.push(new_event);
        
        const InterfaceEvent new_slot_event{ SetSlotColorDetails{ slot_id, new_color } };
        result.push(new_slot_event);
	}
	ImGui::Separator();

//	if (ImGui::Button("Submit")) {
//		const InterfaceEvent new_event{ SetSlotColorDetails{ slot_id, new_color } };
//		result.push(new_event);
//	}

	return result;
}

cse::InterfaceEventArray cse::ParamEditorSubwindow::run_enum(const csg::SlotId slot_id, const csg::EnumSlotValue slot_value) const
{
	InterfaceEventArray result;

	const int current_value{ static_cast<int>(slot_value.get()) };
	int new_value{ current_value };
	for (size_t i = 0; i <= slot_value.max(); i++) {
		const boost::optional<csg::NodeEnumOptionInfo> opt_info{
			csg::NodeEnumOptionInfo::from(slot_value.get_meta(), i)
		};
		if (opt_info) {
			ImGui::RadioButton(opt_info->display_name(), &new_value, static_cast<int>(i));
		}
	}
	if (new_value != current_value) {
		const size_t new_size_t{ static_cast<size_t>(new_value) };
		const InterfaceEvent new_event{ SetSlotEnumDetails{ slot_id, new_size_t } };
		result.push(new_event);
	}

	return result;
}

cse::InterfaceEventArray cse::ParamEditorSubwindow::run_float(const csg::SlotId slot_id, const csg::FloatSlotValue slot_value) const
{
	InterfaceEventArray result;

	std::array<char, 8> pattern_text;
	pattern_text.fill('\0');
	snprintf(pattern_text.data(), pattern_text.size() - 1, "%%.%zuf", slot_value.precision());

	const float start_val{ slot_value.get() };
    const float min_val{ slot_value.getMin() };
    const float max_val{ slot_value.getMax() };
    
	float mutable_val{ start_val };
    


//    ImGui::InputFloat("Value", &mutable_val, 0.05f, 0.1f, pattern_text.data(), ImGuiInputTextFlags_EnterReturnsTrue);

    if(max_val >= FLT_MAX*0.5){
        ImGui::InputFloat("Value", &mutable_val, 0.05f, 0.1f, pattern_text.data(), ImGuiInputTextFlags_EnterReturnsTrue);
    }else{
        float showMax =1.0;
        float showMin =0.0;
        float abs_val =  std::abs(mutable_val)+1;
        if(max_val >= FLT_MAX*0.5)
        {
            showMax = max_val-100>abs_val?abs_val+100:max_val;
        }else{
            showMax = max_val;
        }
        if(min_val <= -FLT_MAX*0.5)
        {
            showMin = min_val-100<-abs_val?-abs_val-100:min_val;
        }else{
            showMin = min_val;
        }
        ImGui::SliderFloat("Value", &mutable_val, showMin, showMax,pattern_text.data());
    }

	csg::FloatSlotValue new_value{ slot_value };
	new_value.set(mutable_val);

	if (new_value != slot_value) {
		const SetSlotFloatDetails details{ slot_id, new_value.get() };
		const InterfaceEvent event{ details };
		result.push(event);
	}

	return result;
}

cse::InterfaceEventArray cse::ParamEditorSubwindow::run_int(const csg::SlotId slot_id, const csg::IntSlotValue slot_value) const
{
	InterfaceEventArray result;

	const int start_val{ slot_value.get() };
	int mutable_val{ start_val };

	ImGui::InputInt("Value", &mutable_val, 1, 4, ImGuiInputTextFlags_EnterReturnsTrue);

	csg::IntSlotValue new_value{ slot_value };
	new_value.set(mutable_val);

	if (new_value != slot_value) {
		const SetSlotIntDetails details{ slot_id, new_value.get() };
		const InterfaceEvent event{ details };
		result.push(event);
	}

	return result;
}

cse::InterfaceEventArray cse::ParamEditorSubwindow::run_vector(const csg::SlotId slot_id, const csg::VectorSlotValue slot_value) const
{
	InterfaceEventArray result;

	std::array<char, 8> pattern_text;
	pattern_text.fill('\0');
	snprintf(pattern_text.data(), pattern_text.size() - 1, "%%.%zuf", slot_value.precision());

	const float start_x{ slot_value.get().x };
	const float start_y{ slot_value.get().y };
	const float start_z{ slot_value.get().z };
	float mut_x{ start_x };
	float mut_y{ start_y };
	float mut_z{ start_z };

	ImGui::InputFloat("X", &mut_x, 0.05f, 0.1f, pattern_text.data(), ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::InputFloat("Y", &mut_y, 0.05f, 0.1f, pattern_text.data(), ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::InputFloat("Z", &mut_z, 0.05f, 0.1f, pattern_text.data(), ImGuiInputTextFlags_EnterReturnsTrue);

	csg::VectorSlotValue new_value{ slot_value };
	new_value.set(csc::Float3{ mut_x, mut_y, mut_z });

	if (new_value != slot_value) {
		const SetSlotVectorDetails details{ slot_id, new_value.get() };
		const InterfaceEvent event{ details };
		result.push(event);
	}

	return result;
}

cse::InterfaceEventArray cse::ParamEditorSubwindow::run_color_ramp(csg::SlotId slot_id, csg::ColorRampSlotValue slot_value) const
{
	InterfaceEventArray result;

	const char* const float_format{ "%.3f" };

	const csg::ColorRamp points{ slot_value.get() };
	for (size_t i = 0; i < points.size(); i++) {
		const csg::ColorRampPoint this_point{ points.get(i) };

		// Each UI item needs a unique ID string
		std::array<char, 16> name_pos;
		name_pos.fill('\0');
		snprintf(name_pos.data(), name_pos.size(), "##Pos%3zu", i);
		std::array<char, 16> name_color;
		name_color.fill('\0');
		snprintf(name_color.data(), name_color.size(), "##Color%3zu", i);

		float mut_pos{ this_point.pos };
		ImGui::SetNextItemWidth(140.0f);
		ImGui::InputFloat(name_pos.data(), &mut_pos, 0.05f, 0.1f, float_format, ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::SameLine();

		csc::Float4 col{ this_point.color, this_point.alpha };
		if (ImGui::ColorButton(name_color.data(), col)) {
			InterfaceEvent ramp_event{ ModalRampColorPickShowDetails{ slot_id, i } };
			result.push(ramp_event);
		}
		else if (mut_pos != this_point.pos) {
			InterfaceEvent ramp_event{ ModifySlotRampPosDetails{ slot_id, i, mut_pos } };
			result.push(ramp_event);
		}
		ImGui::SameLine();

		std::array<char, 16> name_delete;
		name_delete.fill('\0');
		snprintf(name_delete.data(), name_delete.size(), "Delete##%3zu", i);
		if (ImGui::Button(name_delete.data())) {
			InterfaceEvent delete_event{ ModifySlotRampDeleteDetails{ slot_id, i } };
			result.push(delete_event);
		}
	}

	if (ImGui::Button("Add Control Point")) {
		InterfaceEvent add_event{ InterfaceEventType::MODIFY_SLOT_RAMP_NEW, SlotIdDetails{ slot_id }, boost::none };
		result.push(add_event);
	}

	return result;
}


cse::InterfaceEventArray cse::ParamEditorSubwindow::run_image(csg::SlotId slot_id, csg::ImageSlotValue slot_value) const
{

    
        InterfaceEventArray result;
    
        auto filePath = slot_value.get();
        auto bxFilePath = bx::FilePath(filePath);
        auto fileNam = bxFilePath.getFileName();
        ImGui::Text("image:%s",fileNam.getPtr());
        bgfx::TextureInfo* textureInfo;
        bgfx::TextureHandle uth = the_state->AddTextureHandler(filePath,&textureInfo);
        ImVec2 _size = ImVec2(128*(float(textureInfo->width)/float(textureInfo->height)),128);
        ImGui::Image(uth, _size);
        ImGui::Separator();

        if (ImGui::Button("Change")) {
            InterfaceEvent add_event{ InterfaceEventType::OPEN_DIALOG, SlotIdDetails{ slot_id }, boost::none };
            result.push(add_event);
        }

        return result;

}


void cse::ParamEditorSubwindow::do_event(const InterfaceEvent& event)
{
	if (event.target_subwindow() && event.target_subwindow() == SubwindowId::PARAM_EDITOR) {
		switch (event.type()) {
			case InterfaceEventType::SELECT_SLOT:
			{
				const boost::optional<SlotIdDetails> details{ event.details_as<SlotIdDetails>() };
				assert(details.has_value());
				if (selected_slot != details->value) {
					clear_state();
					selected_slot = details->value;
				}
				break;
			}
			case InterfaceEventType::SELECT_SLOT_NONE:
			{
				selected_slot = boost::none;
				break;
			}
			case InterfaceEventType::PARAM_EDIT_COLOR_INIT:
			{
				const boost::optional<Float3Details> details{ event.details_as<Float3Details>() };
				assert(details.has_value());
				edit_color_color = details->value;
				break;
			}
			case InterfaceEventType::PARAM_EDIT_COLOR_CHANGE:
			{
				const boost::optional<Float3Details> details{ event.details_as<Float3Details>() };
				assert(details.has_value());
				edit_color_color = details->value;
				break;
			}
			default:
				break;
		}
	}
}

void cse::ParamEditorSubwindow::clear_state()
{
	edit_color_color = boost::none;
}
