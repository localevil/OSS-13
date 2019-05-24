#include "Chat.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_stdlib.h>

#include <Shared/Network/Protocol/ClientCommand.h>

#include "Graphics/Window.hpp"
#include "Client.hpp"
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

	for (const auto &message : messages) {
		ImGui::PushFont(CC::Get()->GetWindow()->GetFont("arial_bold"));
//		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
		ImGui::TextWrapped(message.playerName.c_str());
		ImGui::SameLine(0, 0);
//		ImGui::PopStyleColor();
		ImGui::PopFont();
		ImGui::PushFont(CC::Get()->GetWindow()->GetFont("Arialuni"));
		ImGui::TextWrapped(message.text.c_str());
		ImGui::PopFont();
	}

	ImGui::EndChild();

	ImGui::PushItemWidth(-1.0f);
	ImGui::PushFont(CC::Get()->GetWindow()->GetFont("Arialuni"));
	if (ImGui::InputText("", &buffer, ImGuiInputTextFlags_EnterReturnsTrue)) {
		send();
		buffer.clear();

		ImGui::SetKeyboardFocusHere(-1);
	}
	ImGui::PopFont();
	ImGui::PopItemWidth();

	ImGui::End();
}

void Chat::Receive(const string& message) {
	size_t startPos = 0;
	string playerName;
	if (message[0] == '<') {
		size_t counter = 0;
		while (message[counter] != '>')
			counter++;
		playerName = message.substr(1, counter - 1) + ": ";
		startPos = counter + 1;
	}
	string text = message.substr(startPos);

	messages.emplace_back(Inscription(move(playerName), move(text)));
}

void Chat::send() {
	auto* p = new SendChatMessageClientCommand();
	p->message = buffer;
	Connection::commandQueue.Push(p);
}