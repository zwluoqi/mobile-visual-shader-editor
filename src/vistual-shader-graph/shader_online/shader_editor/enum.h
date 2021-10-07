#pragma once

/**
 * @file
 * @brief File used to declare enums used throughout the shader editor.
 */

namespace cse {

	enum class InputEventType {
		MOUSE_BUTTON,
		KEY,
		CHARACTER,
		SCROLL,
        DROP_FILE,
        SAVE_FILE,
	};

	enum class InterfaceEventType {
		// Main window
		ENABLE_DEBUG,
		QUIT,
//        NEW_FILE,
		SAVE_TO_MAX,
		SAVE_TO_FILE,
		LOAD_FROM_FILE,
        NEW_FILE,
		WINDOW_SHOW_ABOUT,
		WINDOW_CLOSE_ABOUT,
		WINDOW_SHOW_DEMO,
        WINDOW_SHOW_EXAMPLES,
		WINDOW_CLOSE_DEMO,
        WINDOW_CLOSE_EXAMPLES,
		WINDOW_SHOW_DEBUG,
		WINDOW_CLOSE_DEBUG,
		MODAL_ALERT_SHOW,
		MODAL_ALERT_CLOSE,
		MODAL_CURVE_EDITOR_SHOW,
		MODAL_CURVE_EDITOR_COMMIT_RGB,
		MODAL_CURVE_EDITOR_COMMIT_VEC,
		MODAL_CURVE_EDITOR_CLOSE,
		MODAL_RAMP_COLOR_PICK_SHOW,
		MODAL_RAMP_COLOR_PICK_CLOSE,
		SUBWINDOW_IS_HOVERED,
		SET_THEME_DARK,
		SET_THEME_CLASSIC,
		IGNORE_INPUT_EVENTS_THIS_FRAME,
		SET_SLOT_BOOL,
		SET_SLOT_COLOR,
		SET_SLOT_ENUM,
		SET_SLOT_FLOAT,
		SET_SLOT_INT,
		SET_SLOT_VECTOR,
        SET_SLOT_IMAGE,
		MODIFY_SLOT_RAMP_COLOR,
		MODIFY_SLOT_RAMP_POS,
		MODIFY_SLOT_RAMP_NEW,
		MODIFY_SLOT_RAMP_DELETE,
		UNDO,
		REDO,
		RELOAD_GRAPH,
        WINDOW_SHOW_SHADER_TEXT,
        WINDOW_CLOSE_SHADER_TEXT,
		// Graph window,
		PAN_VIEW,
		MOUSE_PAN_BEGIN,
		MOUSE_PAN_END,
		MOUSE_MOVE_BEGIN,
		MOUSE_MOVE_END,
		CREATE_NODE,
		SELECT_NODE,
		BOX_SELECT_BEGIN,
		BOX_SELECT_END,
		DELETE_NODE_SELECTION,
		FOCUS_SELECTION,
		FOCUS_OUTPUT,
		CONNECTION_BEGIN,
		CONNECTION_END,
		CONNECTION_ALTER,
		DUPLICATE_SELECTION,
		SELECT_ALL,
		SELECT_NONE,
		SELECT_INVERSE,
		// Node list window
		SELECT_NODE_TYPE,
		SELECT_NODE_TYPE_NONE,
		// Param editor window
		PARAM_EDIT_COLOR_INIT,
		PARAM_EDIT_COLOR_CHANGE,
		// Debug window
		VALIDATE_SET_MESSAGE,
		// Modal curve editor
		CURVE_EDIT_RESET,
		CURVE_EDIT_SET_BOUNDS,
		CURVE_EDIT_SET_INTERP,
		CURVE_EDIT_SET_MODE,
		CURVE_EDIT_SET_TAB,
		CURVE_EDIT_VIEW_CLICK,
		CURVE_EDIT_POINT_MOVE,
		// Modal ramp color picker
		RAMP_COLOR_PICK_UPDATE,
		// Multicast to several windows
		SELECT_SLOT,
		SELECT_SLOT_NONE,
        // dialog manager,
        OPEN_DIALOG,
	};

	enum class InteractionMode {
		SELECT,
		CREATE,
		MOUSE_MOVE,
		MOUSE_PAN,
		BOX_SELECT,
	};

	enum class SubwindowId {
		ALERT,
		GRAPH,
		NODE_LIST,
		PARAM_EDITOR,
		MODAL_CURVE_EDITOR,
		MODAL_RAMP_COLOR_PICK,
		DEBUG,
	};

	enum class SelectMode {
		EXCLUSIVE,
		ADD,
		REMOVE,
		TOGGLE,
	};

	enum class CurveEditorMode {
		SELECT,
		MOVE,
		CREATE,
		DELETE,
	};

	enum class CurveEditorTab {
		DEFAULT,
		X,
		Y,
		Z,
		ALL,
		R,
		G,
		B,
	};

}
