#ifndef _Environment_h_
#define _Environment_h_

#include "ResourceManager.h"
#include "ResourceIdentifiers.h"
#include "SceneNode.h"
#include "SpriteNode.h"
#include "CommandQueue.h"
#include "Command.h"
#include "Room.h"
#include "Platformer.h"
#include "EffectBloom.h"
#include "EffectTyndall.h"
#include "Map.h"

#include <SFML\Graphics.hpp>
#include <pugixml\pugixml.hpp>

class Environment : private sf::NonCopyable
{
	public:
		explicit							Environment(sf::RenderTarget& outputTarget, TextureManager& textures, FontManager& fonts);
		void								update(sf::Time dt);
		void								draw();
 
		CommandQueue&						getCommandQueue();

	private:
		const Map&							getCurrentMap() const;

	private:
		sf::RenderTarget&					mTarget;
		sf::RenderTexture					mSceneTexture;

		sf::View							mWorldView;
		
		SceneNode							mSceneGraph;

		TextureManager&						mTextures;
		FontManager&						mFonts;
		CommandQueue						mCommandQueue;
		
		Platformer*							mPlayerCharacter;

		EffectBloom							mBloomEffect;
		EffectTyndall						mTyndallEffect;

		std::map<int, std::unique_ptr<Map>> mMaps;
};

#endif