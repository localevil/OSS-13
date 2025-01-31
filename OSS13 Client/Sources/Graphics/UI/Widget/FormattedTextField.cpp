#include "FormattedTextField.hpp"

#include "Client.hpp"

using namespace std;

FormattedTextField::FormattedTextField(const sf::Vector2f &size)
{
    SetSize(size);

    scrolled = 0;

    textXShift = GetSize().x * 0.01f;

    resized = false;

    canBeActive = true;
}

void FormattedTextField::scrollUp() {
    int totalSize = 0;
    for (auto &message : text)
        totalSize += int(message.text.size());
    if (scrolled < totalSize)
        scrolled++;
}

void FormattedTextField::scrollDown() {
    if (scrolled > 0)
        scrolled--;
}

void FormattedTextField::AddInscription(const std::string &inscription) {
    wstring result(inscription.size(), 0);
    wstring::iterator end = sf::Utf<8>::toWide(inscription.begin(), inscription.end(), result.begin());
    result.erase(end, result.end());

    text.push_back(Inscription(result));
}

void FormattedTextField::parse(Inscription &inscription, sf::Text &tempText) {
    unsigned startPos = 0, shiftPos = 0;

    wstring playerName;
    float nameWidth = 0;
    if (inscription.stringText[0] == '<') {
        int counter = 0;
        while (inscription.stringText[counter] != '>')
            counter++;
        playerName = inscription.stringText.substr(1, counter - 1);
        playerName += L": ";
        startPos = counter + 1;
    }
    if (!playerName.empty()) {
        tempText.setStyle(sf::Text::Bold);
        tempText.setString(playerName);
        inscription.text.push_back(vector<sf::Text>({ tempText }));
        nameWidth = tempText.getGlobalBounds().width;
    }

    tempText.setStyle(sf::Text::Regular);
    size_t string_counter = 0;
    float size = nameWidth + textXShift;
    while (startPos < inscription.stringText.size()) {
        while (startPos + shiftPos < unsigned(inscription.stringText.size())) {
            float advance = GetStyle().font->getGlyph(inscription.stringText[startPos + shiftPos], GetStyle().fontSize, false).advance;
            if (size + advance >= GetSize().x * 0.96f)
                break;
            size += advance;
            shiftPos++;
        }

        size = 0;

        bool spaces = true;
        if (startPos + shiftPos < inscription.stringText.size())
            if (inscription.stringText[startPos + shiftPos] != ' ') {
                spaces = false;
                for (unsigned i = startPos + shiftPos; i > startPos; i--) {
                    if (inscription.stringText[i] == ' ') {
                        spaces = true;
                        shiftPos = i - startPos;
                        break;
                    }
                }
            }

        tempText.setString(inscription.stringText.substr(startPos, shiftPos));

        if (inscription.text.size() < ++string_counter)
            inscription.text.push_back(vector<sf::Text>({ tempText }));
        else
            inscription.text[string_counter - 1].push_back(tempText);

        startPos += shiftPos;
        if (spaces)
            startPos++;
        shiftPos = 0;
    }
}

void FormattedTextField::draw() const {
    buffer.clear(style.backgroundColor);

    int cur_scroll = scrolled;

    float size = 0;
    for (int i = int(text.size() - 1); i >= 0; i--) {
        for (int j = int(text[i].text.size() - 1); j >= 0; j--) {
            if (!cur_scroll) {
                size += text[i].text[j][0].getGlobalBounds().height + style.fontSize - 5;
                if (size > GetSize().y)
                    break;
                float boxTextXShift = GetSize().x * 0.01f;
                for (size_t k = 0; k < text[i].text[j].size(); k++) {
                    text[i].text[j][k].setPosition(boxTextXShift, GetSize().y - size);
                    buffer.draw(text[i].text[j][k]);
                    boxTextXShift += text[i].text[j][k].getGlobalBounds().width;
                }
            }
            else
                cur_scroll--;
        }
        if (size > GetSize().y)
            break;
    }

    buffer.display();
}

bool FormattedTextField::OnMouseButtonPressed(sf::Mouse::Button, uf::vec2i position) {
	if (position >= GetAbsolutePosition() && position < GetAbsolutePosition() + GetSize())
		return true;
	return false;
}

bool FormattedTextField::OnKeyPressed(sf::Event::KeyEvent keyEvent) {
	switch (keyEvent.code) {
		case sf::Keyboard::Up:
			scrollUp();
			return true;
		case sf::Keyboard::Down:
			scrollDown();
			return true;
		default:
			break;
	}
	return false;
}

void FormattedTextField::Update(sf::Time /*timeElapsed*/) {
    sf::Text tempText;
    tempText.setFont(*style.font);
    tempText.setCharacterSize(style.fontSize);
    tempText.setFillColor(style.textColor);

    int iter = int(text.size());
    if (!iter)
        return;

    while (text[--iter].text.empty()) {
        parse(text[iter], tempText);
        if (!iter)
            break;
    }

    while (text.size() > 100)
        text.erase(text.begin());

    if (resized) {
        for (auto &message : text) {
            if (message.text.size() > 1) {
                message.text.erase(message.text.begin(), message.text.end());
                parse(message, tempText);
            }
            else {
                float size = 0;
                for (auto &i : message.text[0])
                    size += i.getGlobalBounds().width;
                if (size >= GetSize().x) {
                    message.text.erase(message.text.begin());
                    parse(message, tempText);
                }
            }
        }
        resized = false;
    }
}

void FormattedTextField::SetSize(const sf::Vector2f &size) {
	if (size.x != this->GetSize().x)
		resized = true;
	CustomWidget::SetSize(size);
}
