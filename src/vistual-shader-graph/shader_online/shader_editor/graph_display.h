#pragma once

/**
 * @file
 * @brief Used to store configurable parameters of the graph view.
 */

#include "imgui/imgui.h"

// General stuff

static const ImU32 COLOR_SELECTION_BOX{ ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)) };

// Grid stuff

static constexpr float GRID_MINOR_SPACING{ 32.0f };
static constexpr float GRID_MAJOR_SPACING{ GRID_MINOR_SPACING * 6 };
static constexpr int GRID_PAN_DISTANCE{ static_cast<int>(GRID_MINOR_SPACING) };

static const ImU32 COLOR_GRID_LINE_0    { ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,  0.0f,  0.0f,  1.0f)) };
static const ImU32 COLOR_GRID_LINE_1    { ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,  0.0f,  0.0f,  0.95f)) };
static const ImU32 COLOR_GRID_LINE_2    { ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,  0.0f,  0.0f,  0.18f)) };
static const ImU32 COLOR_GRID_BACKGROUND{ ImGui::ColorConvertFloat4ToU32(ImVec4(0.30f, 0.30f, 0.30f, 1.0f)) };

// Node stuff

static constexpr float NODE_DEFAULT_WIDTH{ 170.0f };
static constexpr float NODE_CORNER_RADIUS{ 4.0f };
static constexpr float NODE_HEADER_HEIGHT{ 24.0f };
static constexpr float NODE_ROW_HEIGHT{ 22.0f };
static constexpr float NODE_PIN_RADIUS{ 5.5f };

static const ImU32 COLOR_NODE_BG              { ImGui::ColorConvertFloat4ToU32(ImVec4(0.35f, 0.35f, 0.35f, 1.0f)) };
static const ImU32 COLOR_NODE_OUTLINE_DEFAULT { ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,  0.0f,  0.0f,  1.0f)) };
static const ImU32 COLOR_NODE_OUTLINE_SELECTED{ ImGui::ColorConvertFloat4ToU32(ImVec4(0.9f,  0.9f,  0.9f,  1.0f)) };
static const ImU32 COLOR_NODE_TEXT            { ImGui::ColorConvertFloat4ToU32(ImVec4(0.9f,  0.9f,  0.9f,  1.0f)) };
static const ImU32 COLOR_NODE_SLOT_SELECTED   { ImGui::ColorConvertFloat4ToU32(ImVec4(0.2f,  0.2f,  0.2f,  1.0f)) };

static const ImU32 COLOR_NODE_CATEGORY_DEFAULT  { ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,  1.0f , 0.0f,  1.0f)) };
static const ImU32 COLOR_NODE_CATEGORY_OUTPUT   { ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f,  0.2f,  0.2f,  1.0f)) };
static const ImU32 COLOR_NODE_CATEGORY_COLOR    { ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f,  0.4f,  0.1f,  1.0f)) };
static const ImU32 COLOR_NODE_CATEGORY_CONVERTER{ ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,  0.35f, 0.45f, 1.0f)) };
static const ImU32 COLOR_NODE_CATEGORY_INPUT    { ImGui::ColorConvertFloat4ToU32(ImVec4(0.5f,  0.3f,  0.3f,  1.0f)) };
static const ImU32 COLOR_NODE_CATEGORY_SHADER   { ImGui::ColorConvertFloat4ToU32(ImVec4(0.15f, 0.45f, 0.3f,  1.0f)) };
static const ImU32 COLOR_NODE_CATEGORY_TEXTURE  { ImGui::ColorConvertFloat4ToU32(ImVec4(0.5f,  0.35f, 0.15f, 1.0f)) };
static const ImU32 COLOR_NODE_CATEGORY_VECTOR   { ImGui::ColorConvertFloat4ToU32(ImVec4(0.35f, 0.25f, 0.5f,  1.0f)) };

static const ImU32 COLOR_NODE_SLOT_CONNECTION_OUTLINE{ ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)) };
static const ImU32 COLOR_NODE_SLOT_DEFAULT{ ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f,  1.0f,  0.0f,  1.0f)) };
static const ImU32 COLOR_NODE_SLOT_COLOR  { ImGui::ColorConvertFloat4ToU32(ImVec4(0.65f, 0.65f, 0.2f,  1.0f)) };
static const ImU32 COLOR_NODE_SLOT_CLOSURE{ ImGui::ColorConvertFloat4ToU32(ImVec4(0.35f, 0.6f,  0.35f, 1.0f)) };
static const ImU32 COLOR_NODE_SLOT_FLOAT  { ImGui::ColorConvertFloat4ToU32(ImVec4(0.65f, 0.65f, 0.65f, 1.0f)) };
static const ImU32 COLOR_NODE_SLOT_VECTOR { ImGui::ColorConvertFloat4ToU32(ImVec4(0.35f, 0.35f, 0.6f,  1.0f)) };

static const ImU32 COLOR_CONNECTION{         ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 0.8f)) };
static const ImU32 COLOR_CONNECTION_PENDING{ ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)) };
