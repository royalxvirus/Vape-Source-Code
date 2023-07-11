#include "menu.h"
#include "imgui/imgui.h"

void Menu::Render()
{
	//ImGui::Columns(2);
	//ImGui::SetColumnOffset(1, 230);

	{}

	//ImGui::NextColumn();

	{
	}
}

void Menu::Theme()
{
	ImGuiStyle *style = &ImGui::GetStyle();

	style->WindowBorderSize = 0;
	style->WindowTitleAlign = ImVec2(0.5, 0.5);
	style->WindowMinSize = ImVec2(807, 535);

	style->Colors[ImGuiCol_WindowBg] = ImColor(21, 22, 24, 255);
}