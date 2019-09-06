#include "ControlUI.h"

#include <Server.hpp>
#include <Player.hpp>
#include <World/Objects/Control.hpp>
#include <Resources/ResourceManager.hpp>

#include <Shared/Network/Protocol/ServerToClient/Commands.h>

void ControlUIElement::OnClick() {
	callback();
}

void ControlUIElement::RegistrateCallback(std::function<void()> callback) {
	this->callback = callback;
}

uf::vec2i ControlUIElement::GetPosition() const { return position; }
void ControlUIElement::SetPosition(uf::vec2i pos) {
	position = pos;
	updated = true;
}

void ControlUIElement::AddIcon(const std::string &icon) {
	auto iconId = GServer->RM()->GetIconInfo(icon).id;
	spritesIds.push_back(iconId);
	updated = true;
}

void ControlUIElement::ClearIcons() {
	spritesIds.clear();
	updated = true;
}

void ControlUIElement::SetId(const std::string &id) { elementId = id; }
const std::string &ControlUIElement::GetId() const { return elementId; }

bool ControlUIElement::GetAndDropUpdatedState() {
	bool updated = this->updated;
	this->updated = false;
	return updated;
}

ControlUI::ControlUI(Control *control) :
	control(control) 
{
	EXPECT(control);
}

void ControlUI::Update(std::chrono::microseconds /*timeElapsed*/) {
	if (!updated)
		return;
	updated = false;

	auto command = std::make_unique<network::protocol::server::ControlUIUpdateCommand>();

	for (auto &[key, element] : elements) {
		bool updated = element->GetAndDropUpdatedState();
		if (updated)
			command->elements.push_back(*element);
	}

	if (command->elements.size())
		control->GetPlayer()->AddCommandToClient(command.release());
}

void ControlUI::OnClick(const std::string &key) {
	auto clicked = elements.find(key);
	if (clicked != elements.end()) {
		clicked->second->OnClick();
	}
}

void ControlUI::UpdateElement(std::shared_ptr<ControlUIElement> element) {
	elements[element->GetId()] = std::move(element);
	updated = true;
}

void ControlUI::RemoveElement(const std::string &key) {
	elements.erase(key);
	updated = true;
}

uf::vec2i ControlUI::GetResolution() const { return { Global::control_ui::SIDE_SIZE, Global::control_ui::SIDE_SIZE }; }
uf::vec2i ControlUI::GetCenter() const { return GetResolution() / 2; }
uf::vec2i ControlUI::GetIconSize() const { return { Global::control_ui::ITEM_SIZE, Global::control_ui::ITEM_SIZE }; }
