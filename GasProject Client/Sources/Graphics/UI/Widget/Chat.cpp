#include "Chat.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>

Chat::Chat() {}

void Chat::Update(sf::Time timeElapsed) {
	ImGui::ShowDemoWindow();

	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);
	if (!ImGui::Begin("Chat"))
	{
		ImGui::End();
		return;
	}

	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText

	for (const auto &row : text) {
		ImGui::TextUnformatted(row.c_str());
	}

	ImGui::EndChild();

	ImGui::PushItemWidth(-1.0f);
	if (ImGui::InputText("", &buffer, ImGuiInputTextFlags_EnterReturnsTrue)) {
		text.push_back(std::move(buffer));

		ImGui::SetKeyboardFocusHere(-1);
	}
	ImGui::PopItemWidth();

	ImGui::End();
}