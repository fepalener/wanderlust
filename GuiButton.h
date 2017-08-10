#ifndef _GuiButton_h_
#define _GuiButton_h_

#include "GuiComponent.h"
#include "ResourceIdentifiers.h"
#include "ResourceManager.h"

#include <SFML/Graphics.hpp>

#include <vector>
#include <string>
#include <memory>
#include <functional>

namespace GUI
{

class GuiButton : public GuiComponent
{
    public:
        typedef std::shared_ptr<GuiButton>	Ptr;
        typedef std::function<void()>		Callback;

		enum Type
		{
			Normal,
			Selected,
			Pressed,
			ButtonCount
		};


	public:
								GuiButton(const FontManager& fonts, const TextureManager& textures);

        void					setCallback(Callback callback);
        void					setText(const std::string& text);
        void					setToggle(bool flag);

        virtual bool			isSelectable() const;
        virtual void			select();
        virtual void			deselect();

        virtual void			activate();
        virtual void			deactivate();

        virtual void			handleEvent(const sf::Event& event);


    private:
        virtual void			draw(sf::RenderTarget& target, sf::RenderStates states) const;
		void					changeTexture(Type buttonType);


    private:
        Callback				mCallback;
        sf::Sprite				mSprite;
        sf::Text				mText;
        bool					mIsToggle;
};

}

#endif