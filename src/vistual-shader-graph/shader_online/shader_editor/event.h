#pragma once

/**
 * @file
 * @brief Defines classes used to keep track of events.
 */

#include <array>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <string>
#include <type_traits>

#include <boost/optional.hpp>

#include "../shader_core/config.h"
#include "../shader_core/rect.h"
#include "../shader_core/stack_vector.h"
#include "../shader_core/util_enum.h"
#include "../shader_core/vector.h"
#include "../shader_graph/curves.h"
#include "../shader_graph/node_id.h"
#include "../shader_graph/node_type.h"
#include "../shader_graph/slot_id.h"

#include "enum.h"

#include <bx/filepath.h>

namespace cse {

	struct InputCharacterDetails {
		InputCharacterDetails(unsigned int codepoint) : codepoint{ codepoint } {}

		unsigned int codepoint;
	};

	struct InputKeyDetails {
		InputKeyDetails(const int key, const int scancode, const int action, const int mods) :
			key{ key }, scancode{ scancode }, action{ action }, mods{ mods } {}

		int key;
		int scancode;
		int action;
		int mods;
	};

	struct InputMouseButtonDetails {
		InputMouseButtonDetails(const int button, const int action, const int mods, const csc::Float2 pos) :
			button{ button }, action{ action }, mods{ mods }, pos{ pos } {}

		int button;
		int action;
		int mods;

		csc::Float2 pos;
	};

	struct InputScrollDetails {
		InputScrollDetails(const double xoffset, const double yoffset) : xoffset{ xoffset }, yoffset{ yoffset } {}
		
		double xoffset;
		double yoffset;
	};

    struct InputFilePathDetails{
        InputFilePathDetails(const bx::FilePath& _filePath,const uint8_t _result) : filePath{ _filePath } ,result{_result}{}

        bx::FilePath filePath;
        uint8_t result;
        uint8_t callback;
    };

    struct InputSaveFilePathDetails{
        InputSaveFilePathDetails(const bx::FilePath& _filePath,const uint8_t _result) : filePath{ _filePath } ,result{_result}{}

        bx::FilePath filePath;
        uint8_t result;
        uint8_t callback;
    };

	/**
	 * @brief A class representing some raw user input. Instances of this class require additional processing to be turned into some number of InterfaceEvents.
	 */
	class InputEvent {
	public:
		static size_t sizeof_details();

		InputEvent(const InputCharacterDetails& input_character_details);
		InputEvent(const InputKeyDetails& input_key_details);
		InputEvent(const InputMouseButtonDetails& input_mouse_button_details);
		InputEvent(const InputScrollDetails& input_scroll_details);
        InputEvent(const InputFilePathDetails& input_drop_file_details);
        InputEvent(const InputSaveFilePathDetails& input_drop_file_details);
        
		
        
		inline InputEventType type() const { return _type; }

		boost::optional<InputCharacterDetails> details_character() const;
		boost::optional<InputKeyDetails> details_key() const;
		boost::optional<InputMouseButtonDetails> details_mouse_button() const;
		boost::optional<InputScrollDetails> details_scroll() const;
        boost::optional<InputFilePathDetails> details_dropfile() const;
        boost::optional<InputSaveFilePathDetails> details_savefile() const;

	private:
		union InputEventDetails {
			InputEventDetails(const InputCharacterDetails& details) : character{ details } {}
			InputEventDetails(const InputKeyDetails& details) : key{ details } {}
			InputEventDetails(const InputMouseButtonDetails& details) : mouse_button{ details } {}
			InputEventDetails(const InputScrollDetails& details) : scroll{ details } {}
            InputEventDetails(const InputFilePathDetails& details) : dropfile{ details } {}
            InputEventDetails(const InputSaveFilePathDetails& details) : savefile{ details } {}

			InputCharacterDetails character;
			InputKeyDetails key;
			InputMouseButtonDetails mouse_button;
			InputScrollDetails scroll;
            InputFilePathDetails dropfile;
            InputSaveFilePathDetails savefile;
		};

		InputEventType _type;
		InputEventDetails details;
	};
	
	/**
	 * @brief Template class that can be used to store a single object as details.
	 */
	template <typename T, InterfaceEventType... types>
	struct SimpleDetails {
		SimpleDetails(T value) : value{ value } {}

        static bool matches(InterfaceEventType type) {
            return csc::EnumMatcher<InterfaceEventType, types...>{}.matches(type);
		}

		T value;
	};

	typedef SimpleDetails<CurveEditorMode,   InterfaceEventType::CURVE_EDIT_SET_MODE>     CurveEditorModeDetails;
	typedef SimpleDetails<CurveEditorTab,    InterfaceEventType::CURVE_EDIT_SET_TAB>      CurveEditorTabDetails;
	typedef SimpleDetails<SelectMode,        InterfaceEventType::BOX_SELECT_END>          SelectModeDetails;
	typedef SimpleDetails<SubwindowId,       InterfaceEventType::SUBWINDOW_IS_HOVERED>    SubwindowIdDetails;
	typedef SimpleDetails<csg::NodeType,     InterfaceEventType::SELECT_NODE_TYPE>        NodeTypeDetails;
	typedef SimpleDetails<csg::SlotId,
		InterfaceEventType::CONNECTION_BEGIN,
		InterfaceEventType::CONNECTION_ALTER,
		InterfaceEventType::SELECT_SLOT,
		InterfaceEventType::MODIFY_SLOT_RAMP_NEW,
        InterfaceEventType::OPEN_DIALOG
		> SlotIdDetails;
	typedef SimpleDetails<csc::Float3,
		InterfaceEventType::PARAM_EDIT_COLOR_INIT,
		InterfaceEventType::PARAM_EDIT_COLOR_CHANGE
	> Float3Details;
	typedef SimpleDetails<csc::Float4,
		InterfaceEventType::RAMP_COLOR_PICK_UPDATE
	> Float4Details;
	typedef SimpleDetails<csc::FloatRect,
		InterfaceEventType::CURVE_EDIT_SET_BOUNDS
	> FloatRectDetails;
	typedef SimpleDetails<csc::Int2, InterfaceEventType::PAN_VIEW> Int2Details;


	struct CreateNodeDetails {
		CreateNodeDetails(csg::NodeType type, csc::Float2 screen_pos) : type{ type }, screen_pos{ screen_pos } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::CREATE_NODE>{}.matches(type);
		}

		csg::NodeType type;
		csc::Float2 screen_pos;
	};

	struct SelectNodeDetails {
		SelectNodeDetails(SelectMode mode, csg::NodeId id) : mode{ mode }, id{ id } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::SELECT_NODE>{}.matches(type);
		}

		SelectMode mode;
		csg::NodeId id;
	};

	struct SetSlotBoolDetails {
		SetSlotBoolDetails(csg::SlotId slot_id, bool new_value) : slot_id{ slot_id }, new_value{ new_value } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::SET_SLOT_BOOL>{}.matches(type);
		}

		csg::SlotId slot_id;
		bool new_value;
	};

	struct SetSlotColorDetails {
		SetSlotColorDetails(csg::SlotId slot_id, csc::Float3 new_value) : slot_id{ slot_id }, new_value{ new_value } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::SET_SLOT_COLOR>{}.matches(type);
		}

		csg::SlotId slot_id;
		csc::Float3 new_value;
	};

	struct SetSlotEnumDetails {
		SetSlotEnumDetails(csg::SlotId slot_id, size_t new_value) : slot_id{ slot_id }, new_value{ new_value } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::SET_SLOT_ENUM>{}.matches(type);
		}

		csg::SlotId slot_id;
		size_t new_value;
	};

	struct SetSlotFloatDetails {
		SetSlotFloatDetails(csg::SlotId slot_id, float new_value) : slot_id{ slot_id }, new_value{ new_value } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::SET_SLOT_FLOAT>{}.matches(type);
		}

		csg::SlotId slot_id;
		float new_value;
	};

	struct SetSlotIntDetails {
		SetSlotIntDetails(csg::SlotId slot_id, int new_value) : slot_id{ slot_id }, new_value{ new_value } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::SET_SLOT_INT>{}.matches(type);
		}

		csg::SlotId slot_id;
		int new_value;
	};

	struct SetSlotVectorDetails {
		SetSlotVectorDetails(csg::SlotId slot_id, csc::Float3 new_value) : slot_id{ slot_id }, new_value{ new_value } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::SET_SLOT_VECTOR>{}.matches(type);
		}

		csg::SlotId slot_id;
		csc::Float3 new_value;
	};

    struct SetSlotImageDetails {
        SetSlotImageDetails(csg::SlotId slot_id, const char* _new_value) : slot_id{ slot_id }  {
            strcpy(new_value, _new_value);
        }

        static bool matches(InterfaceEventType type) {
            return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::SET_SLOT_IMAGE>{}.matches(type);
        }

        csg::SlotId slot_id;
        char new_value[256];
    };

	struct ModifySlotRampColorDetails {
		ModifySlotRampColorDetails(csg::SlotId slot_id, size_t point_index, csc::Float4 new_value) : slot_id{ slot_id }, point_index{ point_index }, new_value{ new_value } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::MODIFY_SLOT_RAMP_COLOR>{}.matches(type);
		}

		csg::SlotId slot_id;
		size_t point_index;
		csc::Float4 new_value;
	};

	struct ModifySlotRampPosDetails {
		ModifySlotRampPosDetails(csg::SlotId slot_id, size_t point_index, float new_value) : slot_id{ slot_id }, point_index{ point_index }, new_value{ new_value } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::MODIFY_SLOT_RAMP_POS>{}.matches(type);
		}

		csg::SlotId slot_id;
		size_t point_index;
		float new_value;
	};

	struct ModifySlotRampDeleteDetails {
		ModifySlotRampDeleteDetails(csg::SlotId slot_id, size_t point_index) : slot_id{ slot_id }, point_index{ point_index } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::MODIFY_SLOT_RAMP_DELETE>{}.matches(type);
		}

		csg::SlotId slot_id;
		size_t point_index;
	};

	struct CurveEditorViewClickDetails {
		CurveEditorViewClickDetails(csc::Float2 pos, CurveEditorMode mode) : pos{ pos }, mode{ mode } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::CURVE_EDIT_VIEW_CLICK>{}.matches(type);
		}

		csc::Float2 pos;
		CurveEditorMode mode;
	};

	struct CurveEditorPointMoveDetails {
		CurveEditorPointMoveDetails(size_t index, csc::Float2 new_pos) : index{ index }, new_pos{ new_pos } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::CURVE_EDIT_POINT_MOVE>{}.matches(type);
		}

		size_t index;
		csc::Float2 new_pos;
	};

	struct CurveEditorSetInterpDetails {
		CurveEditorSetInterpDetails(size_t index, csg::CurveInterp interp) : index{ index }, interp{ interp } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::CURVE_EDIT_SET_INTERP>{}.matches(type);
		}

		size_t index;
		csg::CurveInterp interp;
	};

	struct ModalRampColorPickShowDetails {
		ModalRampColorPickShowDetails(csg::SlotId slot_id, size_t index) : slot_id{ slot_id }, index{ index } {}

		static bool matches(InterfaceEventType type) {
			return csc::EnumMatcher<InterfaceEventType, InterfaceEventType::MODAL_RAMP_COLOR_PICK_SHOW>{}.matches(type);
		}

		csg::SlotId slot_id;
		size_t index;
	};

	/**
	 * @brief A class representing some action that has been taken in the interface. An instance of this class reprensents a clear logical action within the program.
	 */
	class InterfaceEvent {
	public:
		static size_t sizeof_details();

		InterfaceEvent(
			InterfaceEventType type,
			boost::optional<SubwindowId> target = boost::none,
			boost::optional<std::string> message = boost::none
		);
		InterfaceEvent(InterfaceEventType type, const CurveEditorModeDetails& curve_mode_details);
		InterfaceEvent(InterfaceEventType type, const CurveEditorTabDetails& curve_tab_details);
		InterfaceEvent(InterfaceEventType type, const SelectModeDetails& select_mode_details, boost::optional<SubwindowId> target);
		InterfaceEvent(InterfaceEventType type, const SubwindowIdDetails& subwindow_is_hovered_details, boost::optional<SubwindowId> target);
		InterfaceEvent(InterfaceEventType type, const NodeTypeDetails& node_type_details);
		InterfaceEvent(InterfaceEventType type, const SlotIdDetails& slot_id_details, boost::optional<SubwindowId> target);
		InterfaceEvent(InterfaceEventType type, const Float3Details& float3_details, boost::optional<SubwindowId> target);
		InterfaceEvent(InterfaceEventType type, const Float4Details& float4_details, boost::optional<SubwindowId> target);
		InterfaceEvent(InterfaceEventType type, const FloatRectDetails& float_rect_details, boost::optional<SubwindowId> target);
		InterfaceEvent(InterfaceEventType type, const Int2Details& int2_details);

        InterfaceEvent(const CreateNodeDetails& create_node_details);
		InterfaceEvent(const SelectNodeDetails& select_node_details);
		InterfaceEvent(const SetSlotBoolDetails& set_slot_bool_details);
		InterfaceEvent(const SetSlotColorDetails& set_slot_color_details);
		InterfaceEvent(const SetSlotEnumDetails& set_slot_enum_details);
		InterfaceEvent(const SetSlotFloatDetails& set_slot_float_details);
		InterfaceEvent(const SetSlotIntDetails& set_slot_float_details);
		InterfaceEvent(const SetSlotVectorDetails& set_slot_float_details);
        InterfaceEvent(const SetSlotImageDetails& set_slot_image_details);
		InterfaceEvent(const ModifySlotRampColorDetails& mod_slot_ramp_color_details);
		InterfaceEvent(const ModifySlotRampPosDetails& mod_slot_ramp_pos_details);
		InterfaceEvent(const ModifySlotRampDeleteDetails& mod_slot_ramp_delete_details);
		InterfaceEvent(const CurveEditorViewClickDetails& curve_edit_view_details);
		InterfaceEvent(const CurveEditorPointMoveDetails& curve_edit_point_move_details);
		InterfaceEvent(const CurveEditorSetInterpDetails& curve_edit_set_interp_details);
		InterfaceEvent(const ModalRampColorPickShowDetails& modal_ramp_color_pick_show_details);

		inline InterfaceEventType type() const { return _type; }
		inline boost::optional<SubwindowId> target_subwindow() const { return _target_subwindow; }
		inline boost::optional<std::string> message() const { return _message; }

		template <typename T> boost::optional<T> details_as() const
		{
			if (T::matches(_type)) {
				return details->as<T>();
			}
			else {
				return boost::none;
			}
		}

	private:

		union InterfaceEventDetails {
			InterfaceEventDetails(const CurveEditorModeDetails& details) : curve_editor_mode{ details } {}
			InterfaceEventDetails(const CurveEditorTabDetails& details) : curve_editor_tab{ details } {}
			InterfaceEventDetails(const SelectModeDetails& details) : select_mode{ details } {}
			InterfaceEventDetails(const SubwindowIdDetails& details) : subwindow_id{ details } {}
			InterfaceEventDetails(const NodeTypeDetails& details) : node_type{ details } {}
			InterfaceEventDetails(const SlotIdDetails& details) : slot_id{ details } {}

			InterfaceEventDetails(const Float3Details& details) : float3{ details } {}
			InterfaceEventDetails(const Float4Details& details) : float4{ details } {}
			InterfaceEventDetails(const FloatRectDetails& details) : float_rect{ details } {}
			InterfaceEventDetails(const Int2Details& details) : int2{ details } {}
            

            
			InterfaceEventDetails(const CreateNodeDetails& details) : create_node{ details } {}
			InterfaceEventDetails(const SelectNodeDetails& details) : select_node{ details } {}
			InterfaceEventDetails(const SetSlotBoolDetails& details) : set_slot_bool{ details } {}
			InterfaceEventDetails(const SetSlotColorDetails& details) : set_slot_color{ details } {}
			InterfaceEventDetails(const SetSlotEnumDetails& details) : set_slot_enum{ details } {}
			InterfaceEventDetails(const SetSlotFloatDetails& details) : set_slot_float{ details } {}
			InterfaceEventDetails(const SetSlotIntDetails& details) : set_slot_int{ details } {}
			InterfaceEventDetails(const SetSlotVectorDetails& details) : set_slot_vector{ details } {}
            InterfaceEventDetails(const SetSlotImageDetails& details) : set_slot_image{ details } {}
			InterfaceEventDetails(const ModifySlotRampColorDetails& details) : mod_slot_ramp_color{ details } {}
			InterfaceEventDetails(const ModifySlotRampPosDetails& details) : mod_slot_ramp_pos{ details } {}
			InterfaceEventDetails(const ModifySlotRampDeleteDetails& details) : mod_slot_ramp_delete{ details } {}
			InterfaceEventDetails(const CurveEditorViewClickDetails& details) : curve_edit_view_click{ details } {}
			InterfaceEventDetails(const CurveEditorPointMoveDetails& details) : curve_edit_point_move{ details } {}
			InterfaceEventDetails(const CurveEditorSetInterpDetails& details) : curve_edit_set_interp{ details } {}
			InterfaceEventDetails(const ModalRampColorPickShowDetails& details) : modal_ramp_color_pick_show{ details } {}

			template <typename T> T as() const { static_assert(std::is_same<T, T>() == false, "This function must be specialized"); }

		private:
			CurveEditorModeDetails curve_editor_mode;
			CurveEditorTabDetails curve_editor_tab;
			SelectModeDetails select_mode;
			SubwindowIdDetails subwindow_id;
			NodeTypeDetails node_type;
			SlotIdDetails slot_id;

			Float3Details float3;
			Float4Details float4;
			FloatRectDetails float_rect;
			Int2Details int2;

            
			CreateNodeDetails create_node;
			SelectNodeDetails select_node;
			SetSlotBoolDetails set_slot_bool;
			SetSlotColorDetails set_slot_color;
			SetSlotEnumDetails set_slot_enum;
			SetSlotFloatDetails set_slot_float;
			SetSlotIntDetails set_slot_int;
			SetSlotVectorDetails set_slot_vector;
            SetSlotImageDetails set_slot_image;
			ModifySlotRampColorDetails mod_slot_ramp_color;
			ModifySlotRampPosDetails mod_slot_ramp_pos;
			ModifySlotRampDeleteDetails mod_slot_ramp_delete;
			CurveEditorViewClickDetails curve_edit_view_click;
			CurveEditorPointMoveDetails curve_edit_point_move;
			CurveEditorSetInterpDetails curve_edit_set_interp;
			ModalRampColorPickShowDetails modal_ramp_color_pick_show;
		};

		InterfaceEventType _type;
		boost::optional<SubwindowId> _target_subwindow;
		boost::optional<std::string> _message;
		boost::optional<InterfaceEventDetails> details;
	};

	// Template specializations for the above union
	template <> CurveEditorModeDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> CurveEditorTabDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> SelectModeDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> SubwindowIdDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> NodeTypeDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> SlotIdDetails InterfaceEvent::InterfaceEventDetails::as() const;

	template <> Float3Details InterfaceEvent::InterfaceEventDetails::as() const;
	template <> Float4Details InterfaceEvent::InterfaceEventDetails::as() const;
	template <> FloatRectDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> Int2Details InterfaceEvent::InterfaceEventDetails::as() const;


	template <> CreateNodeDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> SelectNodeDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> SetSlotBoolDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> SetSlotColorDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> SetSlotEnumDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> SetSlotFloatDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> SetSlotIntDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> ModifySlotRampColorDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> ModifySlotRampPosDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> ModifySlotRampDeleteDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> SetSlotVectorDetails InterfaceEvent::InterfaceEventDetails::as() const;
    template <> SetSlotImageDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> CurveEditorViewClickDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> CurveEditorPointMoveDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> CurveEditorSetInterpDetails InterfaceEvent::InterfaceEventDetails::as() const;
	template <> ModalRampColorPickShowDetails InterfaceEvent::InterfaceEventDetails::as() const;

	/**
	 * @brief Utility class to allow easy creation of a fixed-size stack-allocated collection of InterfaceEvents
	 */
	class InterfaceEventArray {
	public:
		static std::atomic_size_t max_used;

		void push(const InterfaceEvent& new_event);
		void push(const InterfaceEventArray& array);

		csc::StackVectorIterator<InterfaceEvent> begin() const { return storage.begin(); }
		csc::StackVectorIterator<InterfaceEvent> end() const { return storage.end(); }

	private:
		void update_usage_info();

		csc::StackVector<InterfaceEvent, 6> storage;
	};
}
