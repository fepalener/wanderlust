#include "Pickup.h"
#include "DataTables.h"
#include "CommandCategory.h"
#include "CommandQueue.h"
#include "Utility.h"
#include "ResourceManager.h"

#include <SFML\Graphics.hpp>

namespace
{
	const std::vector<PickupData> Table = initializePickupData();
}

Pickup::Pickup(Type type, const TextureManager& textures)
: Entity(1)
, mType(type)
, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
{
	centerOrigin(mSprite);
}

unsigned int Pickup::getCategory() const
{
	return CommandCategory::Pickup;
}

sf::FloatRect Pickup::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

void Pickup::apply(Character& player) const
{
	Table[mType].action(player);
}

void Pickup::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
}