#include "Entry.hpp"

#include <iostream>

#include "Shared/Geometry/Vec2.hpp"

#include "Client.hpp"

Entry::Entry(const uf::vec2i &size) {
    CustomWidget::SetSize(size);

	canBeActive = true;

    showPos = 0;
    cursorPos = -1;

	hidingSymbols = false;
	hidingSymbol = '*';

    cursorTime = sf::Time::Zero;
}

void Entry::SetOnEnterFunc(std::function<void()> func) {
    onEnterFunc = func;
}

std::vector<float> Entry::getLetterSizes(wchar_t c) {
    std::vector<float> letter_sizes;
    letter_sizes.push_back(style.font->getGlyph(c, style.fontSize, false).advance);
    letter_sizes.push_back(style.font->getGlyph(c, style.fontSize, true).advance);
    return letter_sizes;
}

void Entry::moveCursorLeft(wchar_t c) {
    cursor.setPosition(cursor.getPosition().x - style.font->getGlyph(c, style.fontSize, false).advance, cursor.getPosition().y);
}

void Entry::moveCursorRight(wchar_t c) {
    cursor.setPosition(cursor.getPosition().x + style.font->getGlyph(c, style.fontSize, false).advance, cursor.getPosition().y);
}

void Entry::draw() const {
    buffer.clear(style.backgroundColor);
    buffer.draw(text);

    if (IsActive() && cursorTime >= sf::seconds(0.6f) && cursorTime <= sf::seconds(0.8f)) {
        buffer.draw(cursor);
    }

    buffer.display();
}

void Entry::Update(sf::Time timeElapsed) {
    if (style.updated) {
        //text = sf::Text(entryString, *style.font, style.fontSize);
        text.setFont(*style.font);
        text.setCharacterSize(style.fontSize);
        text.setFillColor(style.textColor);
        text.setOutlineColor(style.textColor);
        cursor.setSize(sf::Vector2f(style.fontSize / 8.0f, float(style.fontSize)));
        cursor.setFillColor(style.textColor);
        style.updated = false;
    }
    if (IsActive()) {
        cursorTime += timeElapsed;

        if (cursorTime > sf::seconds(0.8f))
            cursorTime = sf::Time::Zero;
    }
}

void Entry::setSymbol(wchar_t c) {
    if (hidingSymbols) {
        hidingString += c;
        c = hidingSymbol;
    }
    if (cursorPos == entryString.size() - 1)
        entryString += c;
    else
        entryString.insert(entryString.begin() + 1 + cursorPos, c);

    cursorPos += 1;
    moveCursorRight(c);

    while (cursor.getPosition().x >= text.getGlobalBounds().left + GetSize().x * 0.96f) {
        showPos++;
        text.setString(std::wstring(entryString.c_str() + showPos));
        moveCursorLeft(entryString[showPos - 1]);
        }

    text.setString(std::wstring(entryString.c_str() + showPos));
}

void Entry::deleteSymbol() {
    moveCursorLeft(entryString[cursorPos]);
    entryString.erase(entryString.begin() + cursorPos);
	if (hidingSymbols)
		hidingString.erase(hidingString.begin() + cursorPos);
    text.setString(std::wstring(entryString.c_str() + showPos));

    if (cursorPos--)
    	return;

    int shift_size = 15;
    if (cursorPos + 1 <= showPos && showPos) {
        if (int(showPos) >= shift_size)
            showPos -= shift_size;
        else {
            shift_size = showPos;
            showPos -= shift_size;
        }
        for (unsigned i = showPos; i < showPos + shift_size; i++)
            moveCursorRight(entryString[i]);
    }

    text.setString(std::wstring(entryString.c_str() + showPos));
}

void Entry::moveLeft() {
    moveCursorLeft(entryString[cursorPos]);
    std::vector<float> letter_sizes = getLetterSizes(entryString[cursorPos]);
    cursorPos--;

    if (cursorPos + 1 < showPos) {
        showPos--;
        text.setString(std::wstring(entryString.c_str() + showPos));
        cursor.setPosition(cursor.getPosition().x + letter_sizes[0], cursor.getPosition().y);
    }
}

void Entry::moveRight() {
    if (cursorPos == entryString.size() - 1)
        return;

    cursorPos++;
    moveCursorRight(entryString[cursorPos]);

    while (cursor.getPosition().x >= text.getGlobalBounds().left + GetSize().x * 0.96f) {
        showPos++;
        text.setString(std::wstring(entryString.c_str() + showPos));
        moveCursorLeft(entryString[showPos - 1]);
    }
}

void Entry::Clear() {
    text.setString("");
    entryString.erase(entryString.begin(), entryString.end());
    hidingString.erase(hidingString.begin(), hidingString.end());
    showPos = 0;
    cursorPos = -1;

    cursor.setPosition(text.getGlobalBounds().width, cursor.getPosition().y);
}

bool Entry::Empty() const {
    return entryString.empty();
}

std::string Entry::GetText() {
    std::string returnedString(entryString.size() * 4, 0);
    std::string::iterator end = sf::Utf<8>::fromWide(entryString.begin(), entryString.end(), returnedString.begin());
    if (hidingSymbols)
        end = sf::Utf<8>::fromWide(hidingString.begin(), hidingString.end(), returnedString.begin());
    returnedString.erase(end, returnedString.end());

    text.setString("");
    entryString.erase(entryString.begin(), entryString.end());
    hidingString.erase(hidingString.begin(), hidingString.end());
    showPos = 0;
    cursorPos = -1;

    cursor.setPosition(text.getGlobalBounds().width, cursor.getPosition().y);

    //std::cout << returnedString << std::endl;

    return returnedString;
}

void Entry::HideSymbols(wchar_t hider) {
    hidingSymbol = hider;
    hidingSymbols = true;
    hidingString = entryString;
    entryString = std::wstring(entryString.size(), hidingSymbol);
}

void Entry::ShowSymbols() {
    hidingSymbols = false;
    std::swap(hidingString, entryString);
}

bool Entry::OnMouseButtonPressed(sf::Mouse::Button, uf::vec2i position) {
	if (position >= GetAbsolutePosition() && position < GetAbsolutePosition() + GetSize()) {
		//SetActive(true);
		return true;
	}
	return false;
}

bool Entry::OnKeyPressed(sf::Event::KeyEvent keyEvent) {
	if (IsActive()) {
		switch (keyEvent.code) {
			case sf::Keyboard::BackSpace:
				deleteSymbol();
				return true;
			case sf::Keyboard::Left:
				moveLeft();
				return true;
			case sf::Keyboard::Right:
				moveRight();
				return true;
			case sf::Keyboard::Return:
				onEnterFunc();
				return true;
			default:
				break;
		}
	}
	return false;
}

bool Entry::OnTextEntered(uint32_t unicodeChar) {
	if (IsActive()) {
		wchar_t c = wchar_t(unicodeChar);
		if (c != '\r' && c != '\t' && c != '\b')
			setSymbol(c);
		return true;
	}
	return false;
}
