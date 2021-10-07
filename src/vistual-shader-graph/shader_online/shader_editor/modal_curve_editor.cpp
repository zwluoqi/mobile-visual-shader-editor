#include "modal_curve_editor.h"

#include <array>
#include <cassert>
#include <memory>
#include <vector>

#include "imgui/imgui.h"

#include "../shader_core/rect.h"
#include "../shader_core/vector.h"

#include "wrapper_imgui_func.h"

static const ImU32 COLOR_VIEW_OUTLINE{ ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f,  1.0f,  1.0f,  0.5f)) };
static const ImU32 COLOR_VIEW_GRID   { ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f,  1.0f,  1.0f,  0.25f)) };
static const ImU32 COLOR_VIEW_VALUE  { ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f,  1.0f,  1.0f,  0.7f)) };

void cse::ModalCurveEditor::clear()
{
	rgb_curve = boost::none;
	vector_curve = boost::none;
	cached_curve.fill(0.0f);
	selected_tab = CurveEditorTab::DEFAULT;
	selected_point = boost::none;
}

void cse::ModalCurveEditor::set_vector(const csg::RGBCurveSlotValue& value)
{
	clear();
	rgb_curve = value;
	select_tab(CurveEditorTab::DEFAULT);
}

void cse::ModalCurveEditor::set_vector(const csg::VectorCurveSlotValue& value)
{
	clear();
	vector_curve = value;
	select_tab(CurveEditorTab::DEFAULT);
}

void cse::ModalCurveEditor::reset_curve()
{
	if (rgb_curve) {
		assert(vector_curve.has_value() == false);
		rgb_curve = csg::RGBCurveSlotValue{};
		rebuild_curve_cache();
	}
	if (vector_curve) {
		assert(rgb_curve.has_value() == false);
		vector_curve = csg::VectorCurveSlotValue{ csc::Float2{-1.0f, -1.0f}, csc::Float2{1.0f, 1.0f} };
		rebuild_curve_cache();
	}
}

cse::InterfaceEventArray cse::ModalCurveEditor::run() const
{
	InterfaceEventArray result;

	const csg::Curve current_curve{ get_current_curve() };

	if (ImGui::Begin("Curve Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Columns(2, "CurveEditor", false);
		ImGui::SetColumnWidth(-1, 200.0f);

		ImGui::Text("Edit Mode:");
		const bool mode_select = mode == CurveEditorMode::SELECT;
		if (ImGui::RadioButton("Select", mode_select)) {
			const InterfaceEvent mode_event{ InterfaceEventType::CURVE_EDIT_SET_MODE, CurveEditorModeDetails{ CurveEditorMode::SELECT } };
			result.push(mode_event);
		}
		const bool mode_move = mode == CurveEditorMode::MOVE;
		if (ImGui::RadioButton("Select/Move", mode_move)) {
			const InterfaceEvent mode_event{ InterfaceEventType::CURVE_EDIT_SET_MODE, CurveEditorModeDetails{ CurveEditorMode::MOVE } };
			result.push(mode_event);
		}
		const bool mode_create = mode == CurveEditorMode::CREATE;
		if (ImGui::RadioButton("Create", mode_create)) {
			const InterfaceEvent mode_event{ InterfaceEventType::CURVE_EDIT_SET_MODE, CurveEditorModeDetails{ CurveEditorMode::CREATE } };
			result.push(mode_event);
		}
		const bool mode_delete = mode == CurveEditorMode::DELETE;
		if (ImGui::RadioButton("Delete", mode_delete)) {
			const InterfaceEvent mode_event{ InterfaceEventType::CURVE_EDIT_SET_MODE, CurveEditorModeDetails{ CurveEditorMode::DELETE } };
			result.push(mode_event);
		}

		// Do not show bounds controls for RGB
		if (rgb_curve.has_value() == false) {
			ImGui::NewLine();
			ImGui::Text("Bounds:");
			const std::array<float, 2> bounds_min{ current_curve.min().x, current_curve.min().y };
			std::array<float, 2> bounds_min_mut{ bounds_min };
			ImGui::InputFloat2("Min", bounds_min_mut.data(), "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);
			const std::array<float, 2> bounds_max{ current_curve.max().x, current_curve.max().y };
			std::array<float, 2> bounds_max_mut{ bounds_max };
			ImGui::InputFloat2("Max", bounds_max_mut.data(), "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);

			if (bounds_min != bounds_min_mut || bounds_max != bounds_max_mut) {
				const csc::FloatRect bounds_rect{ csc::Float2{ bounds_min_mut[0], bounds_min_mut[1] }, csc::Float2{ bounds_max_mut[0], bounds_max_mut[1] } };
				const InterfaceEvent bounds_event{
					InterfaceEventType::CURVE_EDIT_SET_BOUNDS,
					FloatRectDetails{ bounds_rect },
					SubwindowId::MODAL_CURVE_EDITOR
				};
				result.push(bounds_event);
			}
		}

		ImGui::NewLine();

		if (selected_point && *selected_point < current_curve.control_points_size()) {
			const csg::CurvePoint control_point{ current_curve.control_points()[*selected_point] };

			ImGui::Text("Point Position:");
			const float point_x{ control_point.pos.x };
			float point_x_mut{ point_x };
			ImGui::InputFloat("X", &point_x_mut, 0.1f, 0.5f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);
			const float point_y{ control_point.pos.y };
			float point_y_mut{ point_y };
			ImGui::InputFloat("Y", &point_y_mut, 0.1f, 0.5f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);

			if (point_x != point_x_mut || point_y != point_y_mut) {
				const csc::Float2 new_pos{ point_x_mut, point_y_mut };
				const InterfaceEvent move_event{ CurveEditorPointMoveDetails{ *selected_point, new_pos } };
				result.push(move_event);
			}

			ImGui::NewLine();

			ImGui::Text("Point Mode:");
			const bool hermite_selected{ control_point.interp == csg::CurveInterp::CUBIC_HERMITE };
			if (ImGui::RadioButton("Cubic Hermite", hermite_selected)) {
				const InterfaceEvent interp_event{ CurveEditorSetInterpDetails{ *selected_point, csg::CurveInterp::CUBIC_HERMITE } };
				result.push(interp_event);
			}
			const bool linear_selected{ control_point.interp == csg::CurveInterp::LINEAR };
			if (ImGui::RadioButton("Linear", linear_selected)) {
				const InterfaceEvent interp_event{ CurveEditorSetInterpDetails{ *selected_point, csg::CurveInterp::LINEAR } };
				result.push(interp_event);
			}
		}

		ImGui::NextColumn();

		ImGui::SetColumnWidth(-1, 500.0f);

		if (ImGui::BeginTabBar("Curve Select")) {
			if (rgb_curve) {
				assert(vector_curve.has_value() == false);
				const bool selected_all{ selected_tab == CurveEditorTab::ALL || selected_tab == CurveEditorTab::DEFAULT };
				if (ImGui::BeginTabItem("All")) {
					if (selected_all == false) {
						const InterfaceEvent tab_event{ InterfaceEventType::CURVE_EDIT_SET_TAB, CurveEditorTabDetails{ CurveEditorTab::ALL } };
						result.push(tab_event);
					}
					ImGui::EndTabItem();
				}
				const bool selected_r{ selected_tab == CurveEditorTab::R };
				if (ImGui::BeginTabItem("R")) {
					if (selected_r == false) {
						const InterfaceEvent tab_event{ InterfaceEventType::CURVE_EDIT_SET_TAB, CurveEditorTabDetails{ CurveEditorTab::R } };
						result.push(tab_event);
					}
					ImGui::EndTabItem();
				}
				const bool selected_g{ selected_tab == CurveEditorTab::G };
				if (ImGui::BeginTabItem("G")) {
					if (selected_g == false) {
						const InterfaceEvent tab_event{ InterfaceEventType::CURVE_EDIT_SET_TAB, CurveEditorTabDetails{ CurveEditorTab::G } };
						result.push(tab_event);
					}
					ImGui::EndTabItem();
				}
				const bool selected_b{ selected_tab == CurveEditorTab::B };
				if (ImGui::BeginTabItem("B")) {
					if (selected_b == false) {
						const InterfaceEvent tab_event{ InterfaceEventType::CURVE_EDIT_SET_TAB, CurveEditorTabDetails{ CurveEditorTab::B } };
						result.push(tab_event);
					}
					ImGui::EndTabItem();
				}
			}
			if (vector_curve) {
				assert(rgb_curve.has_value() == false);
				const bool selected_x{ selected_tab == CurveEditorTab::X || selected_tab == CurveEditorTab::DEFAULT };
				if (ImGui::BeginTabItem("X")) {
					if (selected_x == false) {
						const InterfaceEvent tab_event{ InterfaceEventType::CURVE_EDIT_SET_TAB, CurveEditorTabDetails{ CurveEditorTab::X } };
						result.push(tab_event);
					}
					ImGui::EndTabItem();
				}
				const bool selected_y{ selected_tab == CurveEditorTab::Y };
				if (ImGui::BeginTabItem("Y")) {
					if (selected_y == false) {
						const InterfaceEvent tab_event{ InterfaceEventType::CURVE_EDIT_SET_TAB, CurveEditorTabDetails{ CurveEditorTab::Y } };
						result.push(tab_event);
					}
					ImGui::EndTabItem();
				}
				const bool selected_z{ selected_tab == CurveEditorTab::Z };
				if (ImGui::BeginTabItem("Z")) {
					if (selected_z == false) {
						const InterfaceEvent tab_event{ InterfaceEventType::CURVE_EDIT_SET_TAB, CurveEditorTabDetails{ CurveEditorTab::Z } };
						result.push(tab_event);
					}
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}

		const ImVec2 im_widget_pos{ ImGui::GetCursorScreenPos() };
		const csc::Float2 widget_pos{ im_widget_pos.x, im_widget_pos.y };
		const csc::FloatRect view_area{ widget_pos, widget_pos + csc::Float2{ 490.0f, 490.0f } };

		ImGui::InvisibleButton("View", ImVec2{ 500.0f, 500.0f });
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			const ImVec2 im_mouse_screen_pos{ ImGui::GetMousePos() };
			const csc::Float2 normalized_pos{ view_area.normalized_pos(csc::Float2{ im_mouse_screen_pos.x, im_mouse_screen_pos.y }) };
			// Invert Y, the graphics system makes (0,0) the top-left, but we want it to be the bottom-left here
			const csc::Float2 corrected_pos{ normalized_pos.x, 1.0f - normalized_pos.y };
			const InterfaceEvent click_event{ CurveEditorViewClickDetails{corrected_pos, mode} };
			result.push(click_event);
		}

		ImGui::Columns(1);
		// Columns are weird about width, we need another dummy here
		ImGui::Dummy(ImVec2{ 700.0f, 1.0f });
		if (ImGui::Button("Save")) {
			if (rgb_curve) {
				InterfaceEvent save_event{ InterfaceEventType::MODAL_CURVE_EDITOR_COMMIT_RGB };
				result.push(save_event);
			}
			if (vector_curve) {
				InterfaceEvent save_event{ InterfaceEventType::MODAL_CURVE_EDITOR_COMMIT_VEC };
				result.push(save_event);
			}
		}
		ImGui::SameLine();
		ImGui::Dummy(ImVec2{ 10.0f, 1.0f });
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			InterfaceEvent save_event{ InterfaceEventType::MODAL_CURVE_EDITOR_CLOSE };
			result.push(save_event);
		}
		ImGui::SameLine();
		ImGui::Dummy(ImVec2{ 10.0f, 1.0f });
		ImGui::SameLine();
		if (ImGui::Button("Reset to Default")) {
			InterfaceEvent reset_event{ InterfaceEventType::CURVE_EDIT_RESET, SubwindowId::MODAL_CURVE_EDITOR };
			result.push(reset_event);
		}
		draw_view(view_area);
	}
	ImGui::End();

	return result;
}

void cse::ModalCurveEditor::do_event(const InterfaceEvent& event)
{
	if (event.target_subwindow() && event.target_subwindow() == SubwindowId::MODAL_CURVE_EDITOR) {
		switch (event.type()) {
		case InterfaceEventType::CURVE_EDIT_RESET:
			reset_curve();
			break;
		case InterfaceEventType::CURVE_EDIT_SET_BOUNDS:
		{
			const boost::optional<FloatRectDetails> details{ event.details_as<FloatRectDetails>() };
			assert(details.has_value());
			if (vector_curve) {
				vector_curve->set_bounds(details->value);
			}
			rebuild_curve_cache();
			break;
		}
		case InterfaceEventType::CURVE_EDIT_SET_INTERP:
		{
			const boost::optional<CurveEditorSetInterpDetails> details{ event.details_as<CurveEditorSetInterpDetails>() };
			assert(details.has_value());
			csg::Curve this_curve{ get_current_curve() };
			this_curve.set_interp(details->index, details->interp);
			commit_curve(this_curve);
			rebuild_curve_cache();
			break;
		}
		case InterfaceEventType::CURVE_EDIT_SET_MODE:
		{
			const boost::optional<CurveEditorModeDetails> details{ event.details_as<CurveEditorModeDetails>() };
			assert(details.has_value());
			mode = details->value;
			break;
		}
		case InterfaceEventType::CURVE_EDIT_SET_TAB:
		{
			const boost::optional<CurveEditorTabDetails> details{ event.details_as<CurveEditorTabDetails>() };
			assert(details.has_value());
			select_tab(details->value);
			break;
		}
		case InterfaceEventType::CURVE_EDIT_VIEW_CLICK:
		{
			const boost::optional<CurveEditorViewClickDetails> details{ event.details_as<CurveEditorViewClickDetails>() };
			assert(details.has_value());
			switch (details->mode) {
			case CurveEditorMode::SELECT:
				view_select_point(details->pos);
				break;
			case CurveEditorMode::MOVE:
				view_select_move_point(details->pos);
				break;
			case CurveEditorMode::CREATE:
				view_create_point(details->pos.x);
				break;
			case CurveEditorMode::DELETE:
				view_delete_point(details->pos);
				break;
			default:
				assert(false);
			}
			break;
		}
		case InterfaceEventType::CURVE_EDIT_POINT_MOVE:
		{
			const boost::optional<CurveEditorPointMoveDetails> details{ event.details_as<CurveEditorPointMoveDetails>() };
			assert(details.has_value());
			csg::Curve mutable_curve{ get_current_curve() };
			selected_point = mutable_curve.move_point(details->index, details->new_pos);
			commit_curve(mutable_curve);
			rebuild_curve_cache();
			break;
		}
		default:
			// Do nothing
			break;
		}
	}
}

boost::optional<csg::RGBCurveSlotValue> cse::ModalCurveEditor::take_rgb()
{
	if (rgb_curve) {
		assert(vector_curve.has_value() == false);
		const boost::optional<csg::RGBCurveSlotValue> result{ *rgb_curve };
		clear();
		return result;
	}

	return boost::none;
}

boost::optional<csg::VectorCurveSlotValue> cse::ModalCurveEditor::take_vector()
{
	if (vector_curve) {
		assert(rgb_curve.has_value() == false);
		const boost::optional<csg::VectorCurveSlotValue> result{ *vector_curve };
		clear();
		return result;
	}

	return boost::none;
}

void cse::ModalCurveEditor::draw_view(const csc::FloatRect area) const
{
	ImDrawList* const draw_list{ ImGui::GetWindowDrawList() };
	assert(draw_list != nullptr);
	ImGui::DrawList::PushClipRect(draw_list, area);
	// Draw the grid
	{
		constexpr size_t SEGMENT_COUNT{ 8 };
		// Draw Horizontal lines
		for (size_t i = 1; i < SEGMENT_COUNT; i++) {
			const float segment_y{ (static_cast<float>(i) / static_cast<float>(SEGMENT_COUNT)) * area.size().y + area.begin().y };
			const float thickness{ (i == SEGMENT_COUNT / 2) ? 3.0f : 1.0f };
			const csc::Float2 p1{ area.begin().x, segment_y };
			const csc::Float2 p2{ area.end().x, segment_y };
			ImGui::DrawList::AddLine(draw_list, p1.floor(), p2.floor(), COLOR_VIEW_GRID, thickness);
		}
		// Draw vertical lines
		for (size_t i = 1; i < SEGMENT_COUNT; i++) {
			const float segment_x{ (static_cast<float>(i) / static_cast<float>(SEGMENT_COUNT)) * area.size().x + area.begin().x };
			const float thickness{ (i == SEGMENT_COUNT / 2) ? 3.0f : 1.0f };
			const csc::Float2 p1{ segment_x, area.begin().y };
			const csc::Float2 p2{ segment_x, area.end().y };
			ImGui::DrawList::AddLine(draw_list, p1.floor(), p2.floor(), COLOR_VIEW_GRID, thickness);
		}
	}
	// Draw the computed curve
	{
		// Each iteration of the loop draws the line between points[i] and points[i+1]
		for (size_t i = 0; i < (CURVE_CACHE_ARRAY_SIZE - 1); i++) {
			const float fraction_p1{ static_cast<float>(i)   / static_cast<float>(CURVE_CACHE_ARRAY_SIZE - 1) };
			const float fraction_p2{ static_cast<float>(i+1) / static_cast<float>(CURVE_CACHE_ARRAY_SIZE - 1) };
			// X values are easy to calculate because bounds are automagically taken care of
			const float p1_x{ area.begin().x + fraction_p1 * area.size().x };
			const float p2_x{ area.begin().x + fraction_p2 * area.size().x };
			// Y values are already scaled to the range [0, 1]
			const float p1_y{ area.end().y - cached_curve[i] * area.size().y };
			const float p2_y{ area.end().y - cached_curve[i+1] * area.size().y };
			const csc::Float2 p1{ p1_x, p1_y };
			const csc::Float2 p2{ p2_x, p2_y };
			ImGui::DrawList::AddLine(draw_list, p1, p2, COLOR_VIEW_VALUE, 2.0f);
		}
	}
	// Draw the control points
	{
		const csg::Curve this_curve{ get_current_curve() };
		const std::vector<csg::CurvePoint> points{ this_curve.control_points() };
		for (size_t i = 0; i < points.size(); i++) {
			const csg::CurvePoint this_point = points[i];

			// Draw circle
			const float x_normalized{ (this_point.pos.x - this_curve.min().x) / (this_curve.max().x - this_curve.min().x) };
			const float y_normalized{ (this_point.pos.y - this_curve.min().y) / (this_curve.max().y - this_curve.min().y) };
			const float x{ area.begin().x + x_normalized * area.size().x };
			const float y{ area.end().y - y_normalized * area.size().y };
			const csc::Float2 point{ x, y };
			ImGui::DrawList::AddCircleFilled(draw_list, point, 5.0f, COLOR_VIEW_VALUE);

			// Draw outer circle if this point is selected
			if (selected_point && *selected_point == i) {
				ImGui::DrawList::AddCircle(draw_list, point, 9.0f, COLOR_VIEW_VALUE, 12, 2.0f);
			}
		}
		for (const csg::CurvePoint& this_point : this_curve.control_points()) {
			const float x_normalized{ (this_point.pos.x - this_curve.min().x) / (this_curve.max().x - this_curve.min().x) };
			const float y_normalized{ (this_point.pos.y - this_curve.min().y) / (this_curve.max().y - this_curve.min().y) };
			const float x{ area.begin().x + x_normalized * area.size().x };
			const float y{ area.end().y - y_normalized * area.size().y };
			const csc::Float2 point{ x, y };
			ImGui::DrawList::AddCircleFilled(draw_list, point, 5.0f, COLOR_VIEW_VALUE);
		}
	}
	ImGui::DrawList::PopClipRect(draw_list);
	ImGui::DrawList::AddRect(draw_list, area.grow(1.0f), COLOR_VIEW_OUTLINE);
}

void cse::ModalCurveEditor::select_tab(const CurveEditorTab tab)
{
	selected_tab = tab;
	selected_point = boost::none;
	rebuild_curve_cache();
}

void cse::ModalCurveEditor::rebuild_curve_cache()
{
	if (rgb_curve) {
		assert(vector_curve.has_value() == false);
		cached_curve = get_current_curve().eval_curve<CURVE_CACHE_ARRAY_SIZE>();
		// Output is already in the range [0, 1] for RGB values
	}
	if (vector_curve) {
		assert(rgb_curve.has_value() == false);
		cached_curve = get_current_curve().eval_curve<CURVE_CACHE_ARRAY_SIZE>();
		// Scale output to the range [0, 1] for display
		const csc::Float2 min{ vector_curve->get_min() };
		const csc::Float2 max{ vector_curve->get_max() };
		for (float& val : cached_curve) {
			val = (val - min.y) / (max.y - min.y);
		}
	}
}

void cse::ModalCurveEditor::view_create_point(const float x)
{
	csg::Curve mutable_curve{ get_current_curve() };
	const csc::FloatRect curve_rect{ mutable_curve.min(), mutable_curve.max() };
	// Map x from [0, 1] to [min, max]
	const float real_x{ curve_rect.scaled_pos(csc::Float2{ x, 0.0f }).x };
	selected_point = mutable_curve.create_point(real_x);
	commit_curve(mutable_curve);
	rebuild_curve_cache();
}

void cse::ModalCurveEditor::view_delete_point(const csc::Float2 pos)
{
	constexpr float DELETE_RADIUS{ 0.020f };
	const boost::optional<size_t> point_index{ view_get_point(pos, DELETE_RADIUS) };
	if (point_index) {
		csg::Curve mutable_curve{ get_current_curve() };
		mutable_curve.delete_point(*point_index);
		commit_curve(mutable_curve);
		rebuild_curve_cache();
		selected_point = boost::none;
	}
}

void cse::ModalCurveEditor::view_select_point(const csc::Float2 pos)
{
	constexpr float SELECT_RADIUS{ 0.025f };
	const boost::optional<size_t> point_index{ view_get_point(pos, SELECT_RADIUS) };
	if (point_index) {
		selected_point = point_index;
	}
	else {
		selected_point = boost::none;
	}
}

void cse::ModalCurveEditor::view_select_move_point(const csc::Float2 pos)
{
	constexpr float SELECT_RADIUS{ 0.025f };
	const boost::optional<size_t> point_index{ view_get_point(pos, SELECT_RADIUS) };
	if (point_index) {
		selected_point = point_index;
	}
	else if (selected_point) {
		// No point is in range to select, instead move the currently selected point to the click position
		csg::Curve mutable_curve{ get_current_curve() };
		const csc::FloatRect curve_rect{ mutable_curve.min(), mutable_curve.max() };
		// Map pos from [0, 1] to [min, max]
		const csc::Float2 scaled_pos{ curve_rect.scaled_pos(pos) };
		selected_point = mutable_curve.move_point(*selected_point, scaled_pos);
		commit_curve(mutable_curve);
		rebuild_curve_cache();
	}
}

boost::optional<size_t> cse::ModalCurveEditor::view_get_point(const csc::Float2 pos, const float radius) const
{
	const csg::Curve curve{ get_current_curve() };
	const std::vector<csg::CurvePoint> points{ curve.control_points() };
	const csc::FloatRect bounds{ curve.min(), curve.max() };

	boost::optional<float> closest_distance;
	boost::optional<size_t> closest_index;
	for (size_t i = 0; i < points.size(); i++) {
		const csc::Float2 normalized_point_pos{ bounds.normalized_pos(points[i].pos) };
		const float distance{ normalized_point_pos.distance(pos) };
		if (closest_index && closest_distance) {
			if (distance < *closest_distance) {
				closest_distance = distance;
				closest_index = i;
			}
		}
		else {
			closest_distance = distance;
			closest_index = i;
		}
	}

	if (closest_index && closest_distance && *closest_distance <= radius) {
		return *closest_index;
	}
	else {
		return boost::none;
	}
}

csg::Curve cse::ModalCurveEditor::get_current_curve() const
{
	if (rgb_curve) {
		assert(vector_curve.has_value() == false);
		switch (selected_tab) {
		case CurveEditorTab::DEFAULT:
		case CurveEditorTab::ALL:
			return rgb_curve->get_all();
		case CurveEditorTab::R:
			return rgb_curve->get_r();
		case CurveEditorTab::G:
			return rgb_curve->get_g();
		case CurveEditorTab::B:
			return rgb_curve->get_b();
		default:
			// A non-vector tab is selected when a vector curve is set
			assert(false);
		}
	}
	if (vector_curve) {
		assert(rgb_curve.has_value() == false);
		switch (selected_tab) {
		case CurveEditorTab::DEFAULT:
		case CurveEditorTab::X:
			return vector_curve->get_x();
		case CurveEditorTab::Y:
			return vector_curve->get_y();
		case CurveEditorTab::Z:
			return vector_curve->get_z();
		default:
			// A non-vector tab is selected when a vector curve is set
			assert(false);
		}
	}
	assert(false);
	// Unreachable return to avoid compiler warning
	return csg::Curve{ csc::Float2{ 0.0f, 0.0f }, csc::Float2{ 1.0f, 1.0f } };
}

bool cse::ModalCurveEditor::commit_curve(const csg::Curve& curve)
{
	if (rgb_curve) {
		assert(vector_curve.has_value() == false);
		switch (selected_tab) {
		case CurveEditorTab::DEFAULT:
		case CurveEditorTab::ALL:
			return rgb_curve->set_all(curve);
		case CurveEditorTab::R:
			return rgb_curve->set_r(curve);
		case CurveEditorTab::G:
			return rgb_curve->set_g(curve);
		case CurveEditorTab::B:
			return rgb_curve->set_b(curve);
		default:
			// A non-rgb tab is selected when a rgb curve is set
			assert(false);
		}
	}
	if (vector_curve) {
		assert(rgb_curve.has_value() == false);
		switch (selected_tab) {
		case CurveEditorTab::DEFAULT:
		case CurveEditorTab::X:
			return vector_curve->set_x(curve);
		case CurveEditorTab::Y:
			return vector_curve->set_y(curve);
		case CurveEditorTab::Z:
			return vector_curve->set_z(curve);
		default:
			// A non-vector tab is selected when a vector curve is set
			assert(false);
		}
	}
	// Unreachable return to avoid compiler warning
	return false;
}
