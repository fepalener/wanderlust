#include "GuiComponent.h"

namespace GUI
{

GuiComponent::GuiComponent()
	: mIsSelected(false)
	, mIsActive(false)
{
}

GuiComponent::~GuiComponent()
{
}

bool GuiComponent::isSelected() const
{
	return mIsSelected;
}

void GuiComponent::select()
{
	mIsSelected = true;
}

void GuiComponent::deselect()
{
	mIsSelected = false;
}

bool GuiComponent::isActive() const
{
	return mIsActive;
}

void GuiComponent::activate()
{
	mIsActive = true;
}

void GuiComponent::deactivate()
{
	mIsActive = false;
}

}