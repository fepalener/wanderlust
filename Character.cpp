#include "Character.h"
#include "DataTables.h"
#include "Utility.h"
#include "Pickup.h"
#include "CommandQueue.h"
#include "ResourceManager.h"

#include <SFML\Graphics.hpp>
#include <cmath>

namespace
{
	const std::vector<CharacterData> Table = initializeCharacterData();
}

Character::Character(Type type, const TextureManager& textures, const FontManager& fonts)
: Entity(Table[type].hitpoints)
, mType(type)
, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
, mExplosion(textures.get(Textures::Explosion))
, mFireCommand()
, mMissileCommand()
, mFireCountdown(sf::Time::Zero)
, mIsFiring(false)
, mIsLaunchingMissile(false)
, mShowExplosion(true)
, mSpawnedPickup(false)
, mFireRateLevel(1)
, mSpreadLevel(1)
, mMissileAmmo(2)
, mDropPickupCommand()
, mTravelledDistance(0.f)
, mDirectionIndex(0)
, mHealthDisplay(nullptr)
, mMissileDisplay(nullptr)
{
	mExplosion.setFrameSize(sf::Vector2i(256, 256));
	mExplosion.setNumFrames(16);
	mExplosion.setDuration(sf::seconds(1));

	centerOrigin(mSprite);
	centerOrigin(mExplosion);

	mFireCommand.category = CommandCategory::SceneSpaceLayer;
	mFireCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createBullets(node, textures);
	};

	mMissileCommand.category = CommandCategory::SceneSpaceLayer;
	mMissileCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createProjectile(node, Projectile::Missile, 0.f, 0.5f, textures);
	};

	mDropPickupCommand.category = CommandCategory::SceneSpaceLayer;
	mDropPickupCommand.action   = [this, &textures] (SceneNode& node, sf::Time)
	{
		createPickup(node, textures);
	};

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));

	if (getCategory() == CommandCategory::PlayerShip)
	{
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		missileDisplay->setPosition(0, 70);
		mMissileDisplay = missileDisplay.get();
		attachChild(std::move(missileDisplay));
	}

	updateTexts();
}

void Character::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isDestroyed() && mShowExplosion)
		target.draw(mExplosion, states);
	else
		target.draw(mSprite, states);
}

void Character::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Update texts and roll animation
	updateTexts();
	updateRollAnimation();

	// Entity has been destroyed: Possibly drop pickup, mark for removal
	if (isDestroyed())
	{
		checkPickupDrop(commands);
		mExplosion.update(dt);
		return;
	}

	// Check if bullets or missiles are fired
	checkProjectileLaunch(dt, commands);

	// Update enemy movement pattern; apply velocity
	updateMovementPattern(dt);
	Entity::updateCurrent(dt, commands);
}

unsigned int Character::getCategory() const
{
	if (isAllied())
		return CommandCategory::PlayerShip;
	else
		return CommandCategory::EnemyShip;
}

sf::FloatRect Character::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Character::isMarkedForRemoval() const
{
	return isDestroyed() && (mExplosion.isFinished() || !mShowExplosion);
}

void Character::remove()
{
	Entity::remove();
	mShowExplosion = false;
}

bool Character::isAllied() const
{
	return mType == Eagle;
}

float Character::getMaxSpeed() const
{
	return Table[mType].speed;
}

void Character::increaseFireRate()
{
	if (mFireRateLevel < 10)
		++mFireRateLevel;
}

void Character::increaseSpread()
{
	if (mSpreadLevel < 3)
		++mSpreadLevel;
}

void Character::collectMissiles(unsigned int count)
{
	mMissileAmmo += count;
}

void Character::fire()
{
	// Only ships with fire interval != 0 are able to fire
	if (Table[mType].fireInterval != sf::Time::Zero)
		mIsFiring = true;
}

void Character::launchMissile()
{
	if (mMissileAmmo > 0)
	{
		mIsLaunchingMissile = true;
		--mMissileAmmo;
	}
}

void Character::updateMovementPattern(sf::Time dt)
{
	// Enemy airplane: Movement pattern
	const std::vector<Direction>& directions = Table[mType].directions;
	if (!directions.empty())
	{
		// Moved long enough in current direction: Change direction
		if (mTravelledDistance > directions[mDirectionIndex].distance)
		{
			mDirectionIndex = (mDirectionIndex + 1) % directions.size();
			mTravelledDistance = 0.f;
		}

		// Compute velocity from direction
		float radians = toRadian(directions[mDirectionIndex].angle + 90.f);
		float vx = getMaxSpeed() * std::cos(radians);
		float vy = getMaxSpeed() * std::sin(radians);

		setVelocity(vx, vy);

		mTravelledDistance += getMaxSpeed() * dt.asSeconds();
	}
}

void Character::checkPickupDrop(CommandQueue& commands)
{
	if (!isAllied() && randomInt(3) == 0 && !mSpawnedPickup)
		commands.push(mDropPickupCommand);

	mSpawnedPickup = true;
}

void Character::checkProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	// Enemies try to fire all the time
	if (!isAllied())
		fire();

	// Check for automatic gunfire, allow only in intervals
	if (mIsFiring && mFireCountdown <= sf::Time::Zero)
	{
		// Interval expired: We can fire a new bullet
		commands.push(mFireCommand);
		mFireCountdown += Table[mType].fireInterval / (mFireRateLevel + 1.f);
		mIsFiring = false;
	}
	else if (mFireCountdown > sf::Time::Zero)
	{
		// Interval not expired: Decrease it further
		mFireCountdown -= dt;
		mIsFiring = false;
	}

	// Check for missile launch
	if (mIsLaunchingMissile)
	{
		commands.push(mMissileCommand);
		mIsLaunchingMissile = false;
	}
}

void Character::createBullets(SceneNode& node, const TextureManager& textures) const
{
	Projectile::Type type = isAllied() ? Projectile::AlliedBullet : Projectile::EnemyBullet;

	switch (mSpreadLevel)
	{
		case 1:
			createProjectile(node, type, 0.0f, 0.5f, textures);
			break;

		case 2:
			createProjectile(node, type, -0.33f, 0.33f, textures);
			createProjectile(node, type, +0.33f, 0.33f, textures);
			break;

		case 3:
			createProjectile(node, type, -0.5f, 0.33f, textures);
			createProjectile(node, type,  0.0f, 0.5f, textures);
			createProjectile(node, type, +0.5f, 0.33f, textures);
			break;
	}
}

void Character::createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureManager& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

	sf::Vector2f offset(xOffset * mSprite.getGlobalBounds().width, yOffset * mSprite.getGlobalBounds().height);
	sf::Vector2f velocity(0, projectile->getMaxSpeed());

	float sign = isAllied() ? -1.f : +1.f;
	projectile->setPosition(getWorldPosition() + offset * sign);
	projectile->setVelocity(velocity * sign);
	node.attachChild(std::move(projectile));
}

void Character::createPickup(SceneNode& node, const TextureManager& textures) const
{
	auto type = static_cast<Pickup::Type>(randomInt(Pickup::TypeCount));

	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(getWorldPosition());
	pickup->setVelocity(0.f, 1.f);
	node.attachChild(std::move(pickup));
}

void Character::updateTexts()
{
	// Display hitpoints
	if (isDestroyed())
		mHealthDisplay->setString("");
	else
		mHealthDisplay->setString(toString(getHitpoints()) + " HP");
	mHealthDisplay->setPosition(0.f, 50.f);
	mHealthDisplay->setRotation(-getRotation());

	// Display missiles, if available
	if (mMissileDisplay)
	{
		if (mMissileAmmo == 0 || isDestroyed())
			mMissileDisplay->setString("");
		else
			mMissileDisplay->setString("M: " + toString(mMissileAmmo));
	}
}

void Character::updateRollAnimation()
{
	if (Table[mType].hasRollAnimation)
	{
		sf::IntRect textureRect = Table[mType].textureRect;

		// Roll left: Texture rect offset once
		if (getVelocity().x < 0.f)
			textureRect.left += textureRect.width;

		// Roll right: Texture rect offset twice
		else if (getVelocity().x > 0.f)
			textureRect.left += 2 * textureRect.width;

		mSprite.setTextureRect(textureRect);
	}
}