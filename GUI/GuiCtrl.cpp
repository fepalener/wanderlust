#include "GuiCtrl.h"

namespace GUI
{

GuiCtrl::GuiCtrl(sf::Vector2f &position) 
: mPosition(position)
, mCallback()
{
}

void GuiCtrl::setCallback(Callback callback)
{
	mCallback = std::move(callback);
}

GuiCtrl::~GuiCtrl()
{
}

}