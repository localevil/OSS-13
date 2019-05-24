#pragma once

#include <Graphics/UI/Widget/ImGuiWidget.h>
#include <imgui.h>

class Chat : public ImGuiWidget {
public:
	struct Inscription
	{
		std::string playerName;
		std::string text;

		Inscription(std::string &&playerName, std::string &&text) : playerName(playerName), text(text) {}
	};

	Chat();

	virtual void Update(sf::Time timeElapsed) final;

	void Receive(const std::string &message);
private:
	void send();

	std::vector<Inscription> messages;
	std::string buffer;
};