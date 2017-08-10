#ifndef _GuiLabel_h_
#define _GuiLabel_h_

#include "GuiComponent.h"
#include "ResourceIdentifiers.h"
#include "ResourceManager.h"

#include <SFML\Graphics.hpp>

namespace GUI
{

class GuiLabel : public GuiComponent
{
    public:
        typedef std::shared_ptr<GuiLabel> Ptr;
            

	public:
							GuiLabel(const std::string& text, const FontManager& fonts);

        virtual bool		isSelectable() const;
		void				setText(const std::string& text);

        virtual void		handleEvent(const sf::Event& event);


    private:
        void				draw(sf::RenderTarget& target, sf::RenderStates states) const;


    private:
        sf::Text			mText;
};

}

#endif