#pragma once
#include "imgui.h"

namespace GUIutils {
	/*
		Hoverable tooltip, copied from imgui_demo.cpp
			desc: tooltip message
			symb: display symbol
	*/
	static void HoverTip(const char* desc, const char* symb)
	{
		ImGui::TextDisabled(symb);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}	
}