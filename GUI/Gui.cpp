#include "Gui.h"
#include "../Foreach.h"

namespace GUI
{

Gui::Gui() : mChildren()
{
}

void Gui::addCtrl(GuiCtrl::Ptr ctrl)
{
	mChildren.push_back(ctrl);
}

GuiCtrl::Ptr Gui::getCtrl(std::size_t index)
{
	return 0;
}

void Gui::handleEvent(const sf::Event& event)
{
	FOREACH(const GuiCtrl::Ptr& child, mChildren)
		child->handleEvent(event);
}

void Gui::update(sf::Time dt)
{
	FOREACH(const GuiCtrl::Ptr& child, mChildren)
		child->update(dt);
}

void Gui::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	FOREACH(const GuiCtrl::Ptr& child, mChildren)
		target.draw(*child, states);
}

}