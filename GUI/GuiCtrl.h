#ifndef _GuiCtrl_h_
#define _GuiCtrl_h_

#include <SFML\Graphics.hpp>
#include <functional>

namespace sf
{
	class Event;
}

namespace GUI
{

class GuiCtrl : public sf::Drawable, public sf::Transformable, private sf::NonCopyable
{
public:
	typedef std::shared_ptr<GuiCtrl>	Ptr;
	typedef std::function<void()>		Callback;

public:
						GuiCtrl(sf::Vector2f &position);
	virtual				~GuiCtrl();

	void				setCallback(Callback callback);
	virtual void		handleEvent(const sf::Event& event) = 0;
	virtual void		update(sf::Time dt) = 0;

protected:
	sf::Vector2f		mPosition;
	Callback			mCallback;
};

}

#endif