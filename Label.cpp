#include "GuiLabel.h"
#include "Utility.h"

#include <SFML\Graphics.hpp>

namespace GUI
{

GuiLabel::GuiLabel(const std::string& text, const FontManager& fonts)
: mText(text, fonts.get(Fonts::Main), 16)
{
}

bool GuiLabel::isSelectable() const
{
    return false;
}

void GuiLabel::handleEvent(const sf::Event&)
{
}

void GuiLabel::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(mText, states);
}

void GuiLabel::setText(const std::string& text)
{
	mText.setString(text);
}

}