#ifndef _GuiCtrlButton_h_
#define _GuiCtrlButton_h_

#include "GuiCtrl.h"
#include "../ResourceIdentifiers.h"
#include "../ResourceManager.h"

#include <SFML/Graphics.hpp>

namespace GUI
{

class GuiCtrlButton : public GuiCtrl
{
	public:
        typedef std::shared_ptr<GuiCtrlButton>	Ptr;

		enum Type
		{
			Unchecked,
			Checked,
			ButtonCount
		};

	public:
		GuiCtrlButton(sf::Vector2f& position, const TextureManager& textures);

		virtual void	handleEvent(const sf::Event& event);
		virtual void	update(sf::Time dt);

	private:
        virtual void	draw(sf::RenderTarget& target, sf::RenderStates states) const;
		void			changeTexture(Type buttonType);

	private:
		sf::Sprite		mSprite;
		bool			mIsToggle;
};

}
#endif