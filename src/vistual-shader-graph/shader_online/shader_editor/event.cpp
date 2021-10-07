#include "event.h"

#include <cassert>

size_t cse::InputEvent::sizeof_details()
{
	return sizeof(InputEventDetails);
}

cse::InputEvent::InputEvent(const InputCharacterDetails& input_character_details) :
	_type{ InputEventType::CHARACTER },
	details{ input_character_details }
{

}

cse::InputEvent::InputEvent(const InputKeyDetails& input_key_details) :
	_type{ InputEventType::KEY },
	details{ input_key_details }
{

}

cse::InputEvent::InputEvent(const InputMouseButtonDetails& input_mouse_button_details) :
	_type{ InputEventType::MOUSE_BUTTON },
	details{ input_mouse_button_details }
{

}

cse::InputEvent::InputEvent(const InputScrollDetails& input_scroll_details) :
	_type{ InputEventType::SCROLL },
	details{ input_scroll_details }
{

}


cse::InputEvent::InputEvent(const InputFilePathDetails& input_scroll_details) :
    _type{ InputEventType::DROP_FILE },
    details{ input_scroll_details }
{

}

cse::InputEvent::InputEvent(const InputSaveFilePathDetails& input_scroll_details) :
    _type{ InputEventType::SAVE_FILE },
    details{ input_scroll_details }
{

}


boost::optional<cse::InputCharacterDetails> cse::InputEvent::details_character() const
{
	if (_type == InputEventType::CHARACTER) {
		return details.character;
	}
	else {
		return boost::none;
	}
}

boost::optional<cse::InputKeyDetails> cse::InputEvent::details_key() const
{
	if (_type == InputEventType::KEY) {
		return details.key;
	}
	else {
		return boost::none;
	}
}

boost::optional<cse::InputMouseButtonDetails> cse::InputEvent::details_mouse_button() const
{
	if (_type == InputEventType::MOUSE_BUTTON) {
		return details.mouse_button;
	}
	else {
		return boost::none;
	}
}

boost::optional<cse::InputScrollDetails> cse::InputEvent::details_scroll() const
{
	if (_type == InputEventType::SCROLL) {
		return details.scroll;
	}
	else {
		return boost::none;
	}
}

boost::optional<cse::InputFilePathDetails> cse::InputEvent::details_dropfile() const
{
    if (_type == InputEventType::DROP_FILE) {
        return details.dropfile;
    }
    else {
        return boost::none;
    }
}

boost::optional<cse::InputSaveFilePathDetails> cse::InputEvent::details_savefile() const
{
    if (_type == InputEventType::SAVE_FILE) {
        return details.savefile;
    }
    else {
        return boost::none;
    }
}

size_t cse::InterfaceEvent::sizeof_details()
{
	return sizeof(InterfaceEventDetails);
}

cse::InterfaceEvent::InterfaceEvent(
	InterfaceEventType type,
	boost::optional<SubwindowId> target,
	boost::optional<std::string> message) :
	_type{ type },
	_target_subwindow{ target },
	_message{ message }
{

}

cse::InterfaceEvent::InterfaceEvent(const InterfaceEventType type, const CurveEditorModeDetails& curve_mode_details) :
	_type{ type },
	_target_subwindow{ SubwindowId::MODAL_CURVE_EDITOR },
	details{ curve_mode_details }
{
	assert(CurveEditorModeDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const InterfaceEventType type, const CurveEditorTabDetails& curve_tab_details) :
	_type{ type },
	_target_subwindow{ SubwindowId::MODAL_CURVE_EDITOR },
	details{ curve_tab_details }
{
	assert(CurveEditorTabDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const InterfaceEventType type, const SelectModeDetails& select_mode_details, const boost::optional<SubwindowId> target) :
	_type{ type },
	_target_subwindow{ target },
	details{ select_mode_details }
{
	assert(SelectModeDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const InterfaceEventType type, const SubwindowIdDetails& subwindow_id_details, const boost::optional<SubwindowId> target) :
	_type{ type },
	_target_subwindow{ target },
	details{ subwindow_id_details }
{
	assert(SubwindowIdDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const InterfaceEventType type, const NodeTypeDetails& node_type_details) :
	_type{ type },
	_target_subwindow{ SubwindowId::NODE_LIST },
	details{ node_type_details }
{
	assert(NodeTypeDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const InterfaceEventType type, const SlotIdDetails& slot_id_details, const boost::optional<SubwindowId> target) :
	_type{ type },
	_target_subwindow{ target },
	details{ slot_id_details }
{
	assert(SlotIdDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const InterfaceEventType type, const Float3Details& float3_details, const boost::optional<SubwindowId> target) :
	_type{ type },
	_target_subwindow{ target },
	details{ float3_details }
{
	assert(Float3Details::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const InterfaceEventType type, const Float4Details& float4_details, const boost::optional<SubwindowId> target) :
	_type{ type },
	_target_subwindow{ target },
	details{ float4_details }
{
	assert(Float4Details::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const InterfaceEventType type, const FloatRectDetails& float_rect_details, const boost::optional<SubwindowId> target) :
	_type{ type },
	_target_subwindow{ target },
	details{ float_rect_details }
{
	assert(FloatRectDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const InterfaceEventType type, const Int2Details& int2_details) :
	_type{ type },
	_target_subwindow{ SubwindowId::GRAPH },
	details{ int2_details }
{
	assert(Int2Details::matches(_type));
}


cse::InterfaceEvent::InterfaceEvent(const CreateNodeDetails& create_node_details) :
	_type{ InterfaceEventType::CREATE_NODE },
	_target_subwindow{ SubwindowId::GRAPH },
	details{ create_node_details }
{
	assert(CreateNodeDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const SelectNodeDetails& select_node_details) :
	_type{ InterfaceEventType::SELECT_NODE },
	_target_subwindow{ SubwindowId::GRAPH },
	details{ select_node_details }
{
	assert(SelectNodeDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const SetSlotBoolDetails& set_slot_bool_details) :
	_type{ InterfaceEventType::SET_SLOT_BOOL },
	_target_subwindow{ boost::none },
	details{ set_slot_bool_details }
{
	assert(SetSlotBoolDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const SetSlotColorDetails& set_slot_color_details) :
	_type{ InterfaceEventType::SET_SLOT_COLOR },
	_target_subwindow{ boost::none },
	details{ set_slot_color_details }
{
	assert(SetSlotColorDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const SetSlotEnumDetails& set_slot_enum_details) :
	_type{ InterfaceEventType::SET_SLOT_ENUM },
	_target_subwindow{ boost::none },
	details{ set_slot_enum_details }
{
	assert(SetSlotEnumDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const SetSlotFloatDetails& set_slot_float_details) :
	_type{ InterfaceEventType::SET_SLOT_FLOAT },
	_target_subwindow{ boost::none },
	details{ set_slot_float_details }
{
	assert(SetSlotFloatDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const SetSlotIntDetails& set_slot_int_details) :
	_type{ InterfaceEventType::SET_SLOT_INT },
	_target_subwindow{ boost::none },
	details{ set_slot_int_details }
{
	assert(SetSlotIntDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const SetSlotVectorDetails& set_slot_vector_details) :
	_type{ InterfaceEventType::SET_SLOT_VECTOR },
	_target_subwindow{ boost::none },
	details{ set_slot_vector_details }
{
	assert(SetSlotVectorDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const SetSlotImageDetails& set_slot_image_details) :
    _type{ InterfaceEventType::SET_SLOT_IMAGE },
    _target_subwindow{ boost::none },
    details{ set_slot_image_details }
{
    assert(SetSlotImageDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const ModifySlotRampColorDetails& mod_slot_ramp_color_details) :
	_type{ InterfaceEventType::MODIFY_SLOT_RAMP_COLOR },
	_target_subwindow{ boost::none },
	details{ mod_slot_ramp_color_details }
{
	assert(ModifySlotRampColorDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const ModifySlotRampPosDetails& mod_slot_ramp_pos_details) :
	_type{ InterfaceEventType::MODIFY_SLOT_RAMP_POS },
	_target_subwindow{ boost::none },
	details{ mod_slot_ramp_pos_details }
{
	assert(ModifySlotRampPosDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const ModifySlotRampDeleteDetails& mod_slot_ramp_delete_details) :
	_type{ InterfaceEventType::MODIFY_SLOT_RAMP_DELETE },
	_target_subwindow{ boost::none },
	details{ mod_slot_ramp_delete_details }
{
	assert(ModifySlotRampDeleteDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const CurveEditorViewClickDetails& curve_edit_view_details) :
	_type{ InterfaceEventType::CURVE_EDIT_VIEW_CLICK },
	_target_subwindow{ SubwindowId::MODAL_CURVE_EDITOR },
	details{ curve_edit_view_details }
{
	assert(CurveEditorViewClickDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const CurveEditorPointMoveDetails& curve_edit_point_move_details) :
	_type{ InterfaceEventType::CURVE_EDIT_POINT_MOVE },
	_target_subwindow{ SubwindowId::MODAL_CURVE_EDITOR },
	details{ curve_edit_point_move_details }
{
	assert(CurveEditorPointMoveDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const CurveEditorSetInterpDetails& curve_edit_set_interp_details) :
	_type{ InterfaceEventType::CURVE_EDIT_SET_INTERP },
	_target_subwindow{ SubwindowId::MODAL_CURVE_EDITOR },
	details{ curve_edit_set_interp_details }
{
	assert(CurveEditorSetInterpDetails::matches(_type));
}

cse::InterfaceEvent::InterfaceEvent(const ModalRampColorPickShowDetails& modal_ramp_color_pick_show_details) :
	_type{ InterfaceEventType::MODAL_RAMP_COLOR_PICK_SHOW },
	_target_subwindow{ boost::none },
	details{ modal_ramp_color_pick_show_details }
{
	assert(ModalRampColorPickShowDetails::matches(_type));
}

template <> cse::CurveEditorModeDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return curve_editor_mode;
}

template <> cse::CurveEditorTabDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return curve_editor_tab;
}

template <> cse::SelectModeDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return select_mode;
}

template <> cse::SubwindowIdDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return subwindow_id;
}

template <> cse::NodeTypeDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return node_type;
}

template <> cse::SlotIdDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return slot_id;
}

template <> cse::Float3Details cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return float3;
}

template <> cse::Float4Details cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return float4;
}

template <> cse::FloatRectDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return float_rect;
}

template <> cse::Int2Details cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return int2;
}

template <> cse::CreateNodeDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return create_node;
}

template <> cse::SelectNodeDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return select_node;
}

template <> cse::SetSlotBoolDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return set_slot_bool;
}

template <> cse::SetSlotColorDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return set_slot_color;
}

template <> cse::SetSlotEnumDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return set_slot_enum;
}

template <> cse::SetSlotFloatDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return set_slot_float;
}

template <> cse::SetSlotIntDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return set_slot_int;
}

template <> cse::ModifySlotRampColorDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return mod_slot_ramp_color;
}

template <> cse::ModifySlotRampPosDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return mod_slot_ramp_pos;
}

template <> cse::ModifySlotRampDeleteDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return mod_slot_ramp_delete;
}

template <> cse::SetSlotVectorDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return set_slot_vector;
}

template <> cse::SetSlotImageDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
    return set_slot_image;
}

template <> cse::CurveEditorViewClickDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return curve_edit_view_click;
}

template <> cse::CurveEditorPointMoveDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return curve_edit_point_move;
}

template <> cse::CurveEditorSetInterpDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return curve_edit_set_interp;
}

template <> cse::ModalRampColorPickShowDetails cse::InterfaceEvent::InterfaceEventDetails::as() const
{
	return modal_ramp_color_pick_show;
}

void cse::InterfaceEventArray::push(const InterfaceEvent& new_event)
{
	storage.push(new_event);
	update_usage_info();
}

void cse::InterfaceEventArray::push(const InterfaceEventArray& array)
{
	for (const InterfaceEvent& this_event : array) {
		push(this_event);
	}
}

std::atomic_size_t cse::InterfaceEventArray::max_used{ static_cast<size_t>(0) };

void cse::InterfaceEventArray::update_usage_info()
{
	const size_t vec_size{ storage.size() };
	while (vec_size > max_used.load()) {
		size_t current = max_used.load();
		if (vec_size > current) {
			max_used.compare_exchange_strong(current, vec_size);
		}
	}
}
