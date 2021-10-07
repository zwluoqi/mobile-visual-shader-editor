#pragma once

/**
 * @file
 * @brief Wrapper functions to make ImGui work with internal data types (mostly vectors).
 */

#include "imgui/imgui.h"

#include "../shader_core/rect.h"
#include "../shader_core/vector.h"

inline ImVec2 as_imvec(const csc::Float2& float2)
{
	return ImVec2{ float2.x, float2.y };
}

inline ImVec4 as_imvec(const csc::Float4& float4)
{
	return ImVec4{ float4.x, float4.y, float4.z, float4.w };
}

namespace ImGui {
	inline void SetNextWindowPos(const csc::Float2 pos, ImGuiCond cond = 0, const csc::Float2 pivot = csc::Float2(0.0f, 0.0f))
	{
		SetNextWindowPos(as_imvec(pos), cond, as_imvec(pivot));
	}

	inline void SetNextWindowSize(const csc::Float2 size, ImGuiCond cond = 0)
	{
		SetNextWindowSize(as_imvec(size), cond);
	}

	inline bool ColorButton(const char* desc_id, const csc::Float4& col, ImGuiColorEditFlags flags = 0, csc::Float2 size = csc::Float2{ 0, 0 })
	{
		return ImGui::ColorButton(desc_id, as_imvec(col), flags, as_imvec(size));
	}

	namespace DrawList {
		inline void PushClipRect(ImDrawList* draw_list, csc::FloatRect clip_rect, bool intersect_with_current_clip_rect = false)
		{
			draw_list->PushClipRect(as_imvec(clip_rect.begin()), as_imvec(clip_rect.end()), intersect_with_current_clip_rect);
		}

		inline void PopClipRect(ImDrawList* draw_list)
		{
			draw_list->PopClipRect();
		}

		inline void AddLine(ImDrawList* draw_list, csc::Float2 p1, csc::Float2 p2, ImU32 col, float thickness = 1.0f)
		{
			draw_list->AddLine(as_imvec(p1), as_imvec(p2), col, thickness);
		}

		inline void AddRect(ImDrawList* draw_list, csc::FloatRect rect, ImU32 col, float rounding = 0.0f, ImDrawFlags rounding_corners = ImDrawFlags_RoundCornersAll, float thickness = 1.0f)
		{
			draw_list->AddRect(as_imvec(rect.begin()), as_imvec(rect.end()), col, rounding, rounding_corners, thickness);
		}

		inline void AddRectFilled(ImDrawList* draw_list, csc::FloatRect rect, ImU32 col, float rounding = 0.0f, ImDrawFlags rounding_corners = ImDrawFlags_RoundCornersAll)
		{
			draw_list->AddRectFilled(as_imvec(rect.begin()), as_imvec(rect.end()), col, rounding, rounding_corners);
		}

		inline void AddCircle(ImDrawList* draw_list, csc::Float2 center, float radius, ImU32 col, int num_segments = 8, float thickness = 1.0f)
		{
			draw_list->AddCircle(as_imvec(center), radius, col, num_segments, thickness);
		}

		inline void AddCircleFilled(ImDrawList* draw_list, csc::Float2 center, float radius, ImU32 col, int num_segments = 8)
		{
			draw_list->AddCircleFilled(as_imvec(center), radius, col, num_segments);
		}

		inline void AddText(ImDrawList* const draw_list, const csc::Float2 pos, const ImU32 col, const char* const text_begin, const char* const text_end = NULL)
		{
			draw_list->AddText(as_imvec(pos), col, text_begin, text_end);
		}
    
    inline void AddImage(ImDrawList* const draw_list, const csc::Float2 pos,const csc::Float2 size, const ImU32 col,bgfx::TextureHandle handle)
        {
            union {  struct { bgfx::TextureHandle handle; uint8_t flags; uint8_t mip; } s; ImTextureID ptr; } tex;
            tex.s.handle = handle;
            tex.s.flags  = IMGUI_FLAGS_ALPHA_BLEND;
            tex.s.mip    = 0;

            ImTextureID user_texture_id = tex.ptr;
            
            ImVec2 _uv0       = ImVec2(0.0f, 0.0f);
            ImVec2 _uv1       = ImVec2(1.0f, 1.0f);
//            ImRect bb(as_imvec(pos), as_imvec(pos) + size);
            
            draw_list->AddImage(user_texture_id, as_imvec(pos), as_imvec(pos+size), _uv0, _uv1, col);
        }
    
	}
}
