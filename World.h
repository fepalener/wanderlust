#ifndef _World_h_
#define _World_h_

#include "ResourceManager.h"
#include "ResourceIdentifiers.h"
#include "SceneNode.h"
#include "SpriteNode.h"
#include "Character.h"
#include "CommandQueue.h"
#include "Command.h"
#include "EffectBloom.h"
#include "Physics.h"

#include <SFML\Graphics.hpp>
#include <array>
#include <queue>

// Forward declaration
namespace sf
{
	class RenderTarget;
}

class World : private sf::NonCopyable
{
	public:
		explicit							World(sf::RenderTarget& outputTarget, FontManager& fonts);
		void								update(sf::Time dt);
		void								draw();
		
		CommandQueue&						getCommandQueue();

		bool 								hasAlivePlayer() const;
		bool 								hasPlayerReachedEnd() const;


	private:
		void								loadTextures();
		void								adaptPlayerPosition();
		void								adaptPlayerVelocity();
		void								handleCollisions();
		
		void								buildScene();
		void								addEnemies();
		void								addEnemy(Character::Type type, float relX, float relY);
		void								spawnEnemies();
		void								destroyEntitiesOutsideView();
		void								guideMissiles();
		sf::FloatRect						getViewBounds() const;
		sf::FloatRect						getBattlefieldBounds() const;


	private:
		enum Layer
		{
			Background,
			LowerAir,
			UpperAir,
			LayerCount
		};

		struct SpawnPoint 
		{
			SpawnPoint(Character::Type type, float x, float y)
			: type(type)
			, x(x)
			, y(y)
			{
			}

			Character::Type type;
			float x;
			float y;
		};


	private:
		sf::RenderTarget&					mTarget;
		sf::RenderTexture					mSceneTexture;
		sf::View							mWorldView;
		TextureManager						mTextures;
		FontManager&						mFonts;

		SceneNode							mSceneGraph;
		std::array<SceneNode*, LayerCount>	mSceneLayers;
		CommandQueue						mCommandQueue;

		sf::FloatRect						mWorldBounds;
		sf::Vector2f						mSpawnPosition;
		float								mScrollSpeed;
		Character*							mPlayerAircraft;

		std::vector<SpawnPoint>				mEnemySpawnPoints;
		std::vector<Character*>				mActiveEnemies;

		EffectBloom							mBloomEffect;
};

#endif