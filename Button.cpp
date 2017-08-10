#include "GuiButton.h"
#include "Utility.h"

namespace GUI
{

GuiButton::GuiButton(const FontManager& fonts, const TextureManager& textures)
: mCallback()
, mSprite(textures.get(Textures::Buttons))
, mText("", fonts.get(Fonts::Main), 16)
, mIsToggle(false)
{
	changeTexture(Normal);

	sf::FloatRect bounds = mSprite.getLocalBounds();
	mText.setPosition(bounds.width / 2.f, bounds.height / 2.f);
}

void GuiButton::setCallback(Callback callback)
{
	mCallback = std::move(callback);
}

void GuiButton::setText(const std::string& text)
{
	mText.setString(text);
	centerOrigin(mText);
}

void GuiButton::setToggle(bool flag)
{
	mIsToggle = flag;
}

bool GuiButton::isSelectable() const
{
    return true;
}

void GuiButton::select()
{
	GuiComponent::select();

	changeTexture(Selected);
}

void GuiButton::deselect()
{
	GuiComponent::deselect();

	changeTexture(Normal);
}

void GuiButton::activate()
{
	GuiComponent::activate();

    // If we are toggle then we should show that the button is pressed and thus "toggled".
	if (mIsToggle)
		changeTexture(Pressed);

	if (mCallback)
		mCallback();

    // If we are not a toggle then deactivate the button since we are just momentarily activated.
	if (!mIsToggle)
		deactivate();
}

void GuiButton::deactivate()
{
	GuiComponent::deactivate();

	if (mIsToggle)
	{
        // Reset texture to right one depending on if we are selected or not.
		if (isSelected())
			changeTexture(Selected);
		else
			changeTexture(Normal);
	}
}

void GuiButton::handleEvent(const sf::Event&)
{
}

void GuiButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(mSprite, states);
	target.draw(mText, states);
}

void GuiButton::changeTexture(Type buttonType)
{
	sf::IntRect textureRect(0, 50*buttonType, 200, 50);
	mSprite.setTextureRect(textureRect);
}

}