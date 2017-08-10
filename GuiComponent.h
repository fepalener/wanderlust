#ifndef _GuiComponent_h_
#define _GuiComponent_h_

#include <SFML\Graphics.hpp>

namespace sf
{
	class Event;
}

namespace GUI
{

class GuiComponent : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
public:
	typedef std::shared_ptr<GuiComponent> Ptr;

public:
						GuiComponent();
	virtual				~GuiComponent();

	virtual bool		isSelectable() const = 0;
	bool				isSelected() const;

	virtual void		select();
	virtual void		deselect();

	virtual bool		isActive() const;

	virtual void		activate();
	virtual void		deactivate();

	virtual void		handleEvent(const sf::Event& event) = 0;

private:
	bool				mIsSelected;
	bool				mIsActive;
};

}

#endif