#include "Chat.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>

#include <Shared/Network/Protocol/ClientCommand.h>

#include "Network.hpp"

using namespace network::protocol;

Chat::Chat() {}

void Chat::Update(sf::Time timeElapsed) {
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
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
		ImGui::TextWrapped(row.c_str());
		ImGui::PopStyleColor();
	}

	ImGui::EndChild();

	ImGui::PushItemWidth(-1.0f);
	if (ImGui::InputText("", &buffer, ImGuiInputTextFlags_EnterReturnsTrue)) {
		send();
		text.push_back(std::move(buffer));

		ImGui::SetKeyboardFocusHere(-1);
	}
	ImGui::PopItemWidth();

	ImGui::End();
}

void Chat::Receive(const string& message) {
	text.push_back(message);
}

void Chat::send() {
	auto* p = new SendChatMessageClientCommand();
	p->message = buffer;
	Connection::commandQueue.Push(p);
}