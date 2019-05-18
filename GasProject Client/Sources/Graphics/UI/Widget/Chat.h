#pragma once

#include <Graphics/UI/Widget/ImGuiWidget.h>

class Chat : public ImGuiWidget {
public:
	Chat();

	virtual void Update(sf::Time timeElapsed) final;

private:
	std::vector<std::string> text;
	std::string buffer;
};