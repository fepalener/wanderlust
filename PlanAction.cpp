#include "PlanAction.h"

PlanAction::PlanAction(sf::Vector2f point, ACTION type, PlanAction* next)
: mPoint(point)
, mType(type)
, mNext(next)
{
}