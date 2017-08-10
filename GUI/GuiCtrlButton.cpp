#include "GuiCtrlButton.h"

namespace GUI
{

GuiCtrlButton::GuiCtrlButton(sf::Vector2f& position, const TextureManager& textures) : GuiCtrl(position)
, mSprite(textures.get(Textures::Gui))
, mIsToggle(false)
{
	changeTexture(Unchecked);
}

void GuiCtrlButton::handleEvent(const sf::Event& event)
{
	if(event.type == sf::Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			if(mSprite.getGlobalBounds().contains(
				static_cast< float >(event.mouseButton.x), 
				static_cast< float >(event.mouseButton.y)))
			{
				mIsToggle = !mIsToggle;
				changeTexture(mIsToggle ? Checked : Unchecked);

				if(mIsToggle && mCallback)
				{
					mCallback();
				}
			}
		}
	}
}

void GuiCtrlButton::update(sf::Time dt)
{
}

void GuiCtrlButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(mSprite, states);
}

void GuiCtrlButton::changeTexture(Type buttonType)
{
	sf::IntRect textureRect(53*buttonType, 0, 53, 53);
	mSprite.setTextureRect(textureRect);
	mSprite.setPosition(mPosition);
}

}