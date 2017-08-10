#include "Map.h"

Map::Map()
: mW(0)
, mH(0)
, mTileW(0)
, mTileH(0)
, mRoom(nullptr)
{}

void Map::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
	//draw tile layers
	for (auto layer = begin(mLayers); layer != end(mLayers); ++layer) 
	{
		if(!layer->visible) continue;

		auto originalView = rt.getView();
		auto parallaxView = originalView;

		parallaxView.setCenter(parallaxView.getCenter().x * layer->parallax_x, parallaxView.getCenter().y * layer->parallax_y);

		for(unsigned i = 0; i < layer->vertexArrays.size(); i++)
		{
			//if(it->vertexArrays[i].getBounds().intersects(m_bounds))
			{
				rt.setView(parallaxView);
				states.texture = &mTextures[i];
				rt.draw(layer->vertexArrays[i], states);
				rt.setView(originalView);
			}
		}
	}
}

void Map::update(sf::Time dt) const
{
	for ( int layerInx = mRoom->bgLayers.getCount() - 1; layerInx >= 0; layerInx-- ) {
		int backLayerInx = layerInx + 1;

		// Update the layer:
		RoomBGLayer * bgLayer = mRoom->bgLayers[layerInx];
		//bgLayer->screenWidth  = mWorldView.getSize().x;
		//bgLayer->screenHeight = mWorldView.getSize().y;
	}
}

Room* Map::getRoom() const
{
	return mRoom.get();
}