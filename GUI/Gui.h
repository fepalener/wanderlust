#ifndef _Gui_h_
#define _Gui_h_

#include "GuiCtrl.h"

#include <SFML\Graphics.hpp>

namespace sf
{
	class Event;
}

namespace GUI
{

class Gui : public sf::Drawable, private sf::NonCopyable
{
	public:
		typedef std::shared_ptr<Gui> Ptr;

							Gui();

		void				addCtrl(GuiCtrl::Ptr ctrl);
	    GuiCtrl::Ptr		getCtrl(std::size_t index);

		void				handleEvent(const sf::Event& event);
		void				update(sf::Time dt);

	private:
		virtual void		draw(sf::RenderTarget& target, sf::RenderStates states) const;

	private:
		std::vector<GuiCtrl::Ptr>	mChildren;
};

}

#endif