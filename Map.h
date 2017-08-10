#ifndef _Map_h_
#define _Map_h_

#include "Room.h"

// Forward declaration of classes
class MapLoader;

class Map : public sf::Drawable
{
public:
	friend class MapLoader;

	struct Layer
	{
		Layer() : opacity(1.f), parallax_x(1.f), parallax_y(1.f), visible(true) {};

		std::string						name;
		float							opacity;
		float							parallax_y;
		float							parallax_x;
		bool							visible;
		std::vector<sf::VertexArray>	vertexArrays;
	};

	Map();
	void			draw(sf::RenderTarget& rt, sf::RenderStates states) const;
	void			update(sf::Time dt) const;

	Room*			getRoom() const;

private:
	sf::Uint16					mW, mH;				//tile count
	sf::Uint16					mTileW, mTileH;		//width / height of tiles

	std::vector<Layer>			mLayers;
	std::vector<sf::Texture>	mTextures;
	std::unique_ptr<Room>		mRoom;
};

#endif