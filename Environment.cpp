#include "Environment.h"
#include "Animation.h"
#include "MapLoader.h"

Environment::Environment(sf::RenderTarget& outputTarget, TextureManager& textures, FontManager& fonts)
: mTarget(outputTarget)
, mSceneTexture()
, mWorldView(outputTarget.getDefaultView())
, mSceneGraph()
, mTextures(textures) 
, mFonts(fonts)
, mPlayerCharacter(nullptr)
{
	//load maps
	std::unique_ptr<Map> map(new Map());

	std::unique_ptr<MapLoader> mapLoader = std::unique_ptr<MapLoader>(new MapLoader("../resources"));
	if(mapLoader->load("test.tmx", *map.get()))
	{
		auto inserted = mMaps.insert(std::make_pair(0, std::move(map)));
		assert(inserted.second);
	}

	mSceneTexture.create(mTarget.getSize().x, mTarget.getSize().y);

	//create background
	mTextures.load(Textures::Background, "../resources/Textures/Background.png");
	
	sf::Texture& backgroundTexture = mTextures.get(Textures::Background);
	backgroundTexture.setRepeated(true);

	float viewHeight = mWorldView.getSize().y;
	sf::IntRect textureRect(0,0,800,600);

	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(backgroundTexture, textureRect));
	backgroundSprite->setPosition(0,0);
	//mSceneGraph.attachChild(std::move(backgroundSprite));

	//create player platformer
	std::unique_ptr<Platformer> player(new Platformer(mTextures, mFonts, getCurrentMap().getRoom()));
	mPlayerCharacter = player.get();
	mSceneGraph.attachChild(std::move(player));

	mWorldView.zoom(1.0f);
}

void Environment::update(sf::Time dt)
{
	//set screen position
	sf::Vector2f screenPosition;
	screenPosition.x = std::ceilf(mPlayerCharacter->getPos().x) - (mWorldView.getSize().x / 2);
	screenPosition.y = std::ceilf(mPlayerCharacter->getPos().y) - (mWorldView.getSize().y / 2);

	if(screenPosition.x < 0)
		screenPosition.x = 0;

	if(screenPosition.y < 0)
		screenPosition.y = 0;

	mWorldView.reset(sf::FloatRect(screenPosition.x, screenPosition.y, mWorldView.getSize().x, mWorldView.getSize().y));

	getCurrentMap().update(dt);

	// Forward commands to scene graph
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);

	// Regular update step, adapt position
	mSceneGraph.update(dt, mCommandQueue);
}

void Environment::draw()
{
	/*
	// DRAW LAYERS:

	// (In reverse order; lower-index layers must appear in front!)
	for ( int layerInx = mRoom->bgLayers.getCount() - 1; layerInx >= 0; layerInx-- ) {
		int backLayerInx = layerInx + 1;

		// Render the layer:
		mTarget.draw(*mRoom->bgLayers[layerInx]);
	}

	// DRAW WATER
	if(mRoom->waterLevel > 0)
	{
		Animation anim(mTextures.get(Textures::Water));
		anim.setFrameSize(sf::Vector2i(32,32));
		anim.setNumFrames(4);

		//top surface
		int level = mRoom->waterLevel;
		for(int x = 0; x < mWorldView.getSize().x; x += 32)
		{
			anim.setCurrentFrame(0);
			anim.setPosition(x, level);
			mTarget.draw(anim); 
		}
	}

	// DEBUG DRAW
	Animation anim(mTextures.get(Textures::Collision));
	anim.setFrameSize(sf::Vector2i(48,48));
	anim.setNumFrames(36);

	for(int j = 0; j < mRoom->obstacleLayer.rows; j++)
	{
		for(int i = 0; i < mRoom->obstacleLayer.columns; i++)
		{
			RoomBlock::Type block = static_cast<RoomBlock::Type>(mRoom->obstacleLayer.cell(j,i));
			
			if(block != RoomBlock::BLK_EMPTY)
			{
				anim.setCurrentFrame(block);
				anim.setPosition(i * 48.f, j * 48.f);
				mTarget.draw(anim); 
			}
		}
	}
	*/
	
	
	/*
	if(Effect::isSupported())
	{
		mSceneTexture.clear();
		mSceneTexture.setView(mWorldView);
		mSceneTexture.draw(*mMapLoader);
		mSceneTexture.draw(mSceneGraph);
		mSceneTexture.display();
		mBloomEffect.apply(mSceneTexture, mTarget);
	}
	else
	*/
	{
		mTarget.setView(mWorldView);
		//draw map
		//mTarget.draw(*mMapLoader);
		mTarget.draw(getCurrentMap());
		//draw scene
		mTarget.draw(mSceneGraph);
		
	}
	

	//TEST
	/*
	mSceneTexture.clear();

	mSceneTexture.draw(testLight);
	mSceneTexture.draw(testImage);
	mSceneTexture.draw(*mMapLoader);
	mSceneTexture.display();

	mTyndallEffect.apply(mSceneTexture, mTarget);
	*/
}

CommandQueue& Environment::getCommandQueue()
{
	return mCommandQueue;
}

const Map& Environment::getCurrentMap() const
{
	auto found = mMaps.find(0);
	assert(found != mMaps.end());

	return *found->second;
}