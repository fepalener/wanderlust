#include "MapLoader.h"
#include <sstream>

MapLoader::MapLoader(const std::string& mapDirectory)//, Room* room)
: mMapDirectory(mapDirectory)
//, mWidth(0)
//, mHeight(0)
//, mTileW(0)
//, mTileH(0)
//, mRoom(room)
{
	//check map directory contains trailing slash
	if(!mMapDirectory.empty() && *mMapDirectory.rbegin() != '/')
		mMapDirectory += '/';
}

bool MapLoader::load(const std::string& mapFile, Map& map)
{
	std::string mapPath = mMapDirectory + mapFile;

	//parse map xml, return on error
	pugi::xml_document roomDoc;
	pugi::xml_parse_result result = roomDoc.load_file(mapPath.c_str());
	if(!result)
		throw std::runtime_error("Environment::doLoadRoom - Failed to load " + mapFile + ",reason: " + result.description());

	pugi::xml_node mapNode = roomDoc.child("map");
	if(!mapNode)
		throw std::runtime_error("Map node not found. Map " + mapFile + " not loaded.");

	if(!mParseMapNode(mapNode, map)) return false;
	//load map textures / tilesets
	if(!mParseTilesetNode(mapNode, map)) return false;

	//actually we need to traverse map node children and parse each layer as found
	pugi::xml_node currentNode = mapNode.first_child();
	while(currentNode)
	{
		std::string name = currentNode.name();
		if(name == "layer")
		{
			if(!mParseLayer(currentNode, map)) return false;
		}
		else if(name == "imagelayer")
		{
			if(!mParseImageLayer(currentNode)) return false;
		}

		currentNode = currentNode.next_sibling();
	}

	return true;
}

bool MapLoader::mParseMapNode(const pugi::xml_node& mapNode, Map& map)
{
	//parse tile properties
	if( !(map.mW  = mapNode.attribute("width").as_int()) ||
		!(map.mH  = mapNode.attribute("height").as_int()) ||
		!(map.mTileW  = mapNode.attribute("tilewidth").as_int()) ||
		!(map.mTileH  = mapNode.attribute("tileheight").as_int()))
	{
		throw std::runtime_error("Invalid tile size found, check map data. Map not loaded.");
	}

	map.mRoom = std::unique_ptr<Room>(new Room(map.mTileW,map.mW,map.mH,0,0));

	return true;
}

bool MapLoader::mParseTilesetNode(const pugi::xml_node& mapNode, Map& map)
{
	pugi::xml_node tileset;
	if(!(tileset = mapNode.child("tileset")))
	{
		throw std::runtime_error("No tile sets found.");
	}

	//empty vertex tile
	mTileInfo.push_back(TileInfo());

	//parse tile sets in order so GUIDs match index
	while(tileset)
	{
		sf::Uint16 tileW, tileH, spacing, margin;

		//try and parse tile sizes
		if( !(tileW = tileset.attribute("tilewidth").as_int()) ||
			!(tileH = tileset.attribute("tileheight").as_int()))
		{
			throw std::runtime_error("No tile sets found.");
			return false;
		}

		spacing = (tileset.attribute("spacing")) ? tileset.attribute("spacing").as_int() : 0u;
		margin  = (tileset.attribute("margin"))  ? tileset.attribute("margin").as_int()  : 0u;

		//try parsing image node
		pugi::xml_node imageNode;
		if(!(imageNode = tileset.child("image")) || !imageNode.attribute("source"))
		{
			throw std::runtime_error("Missing image data in tmx file. Map not loaded.");
			return false;
		}

		//process image from disk
		std::string imagePath;
		imagePath = mMapDirectory + imageNode.attribute("source").as_string();

		sf::Image sourceImage = mLoadImage(imagePath);

		//store image as a texture for drawing with vertex array
		sf::Texture texture;
		texture.loadFromImage(sourceImage);
		map.mTextures.push_back(texture);

		//slice into tiles
		int cols = (sourceImage.getSize().x - margin) / (tileW + spacing);
		int rows = (sourceImage.getSize().y - margin) / (tileH + spacing);

		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < cols; x++)
			{
				sf::IntRect rect; //must account for any spacing or margin on the tileset
				rect.top = y * (tileH + spacing);
				rect.top += margin;
				rect.height = tileH;
				rect.left = x * (tileW + spacing);
				rect.left += spacing;
				rect.width = tileW;

				//store texture coords and tileset index for vertex array
				mTileInfo.push_back(TileInfo(rect,sf::Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height)), map.mTextures.size() - 1u));
			}
		}

		//parse any tile properties
		if(pugi::xml_node tileNode = tileset.child("tile"))
		{
			while(tileNode)
			{
				int first_gid = tileset.attribute("firstgid").as_int();
				int id = tileNode.attribute("id").as_int();
			
				if(pugi::xml_node propertiesNode = tileNode.child("properties"))
				{
					pugi::xml_node propertyNode = propertiesNode.child("property");
					while(propertyNode)
					{
						std::string name  = propertyNode.attribute("name").as_string();
						std::string value = propertyNode.attribute("value").as_string();

						mTileInfo[id + first_gid].properties[name] = value;

						propertyNode = propertyNode.next_sibling("property");
					}
				}

				tileNode = tileNode.next_sibling("tile");
			}
		}

		//move on to next tileset node
		tileset = tileset.next_sibling("tileset");
	}

	return true;
}

bool MapLoader::mParseLayer(const pugi::xml_node& layerNode, Map& map)
{
	Map::Layer layer;
	if(layerNode.attribute("name")) layer.name = layerNode.attribute("name").as_string();
	if(layerNode.attribute("parallax_x")) layer.parallax_x = layerNode.attribute("parallax_x").as_float();
	if(layerNode.attribute("parallax_y")) layer.parallax_y = layerNode.attribute("parallax_y").as_float();
	if(layerNode.attribute("opacity")) layer.opacity = layerNode.attribute("opacity").as_float();
	if(layerNode.attribute("visible")) layer.visible = layerNode.attribute("visible").as_bool();

	//make sure there are enough vertex arrays for tile sets
	for(auto arr = map.mTextures.begin(); arr != map.mTextures.end(); ++arr)
		layer.vertexArrays.push_back(sf::VertexArray(sf::Quads));

	pugi::xml_node dataNode;
	if(!(dataNode = layerNode.child("data")))
		throw std::runtime_error("Layer data missing or corrupt. Map not loaded.");

	//tiles
	pugi::xml_node tileNode;
	if(!(tileNode = dataNode.child("tile")))
		throw std::runtime_error("No tile data found. Map not loaded.");

	sf::Uint16 x, y;
	x = y = 0;
	while(tileNode)
	{
		sf::Uint16 gid = tileNode.attribute("gid").as_int();

		mAddTileToLayer(map, layer, x, y, gid);

		if(layer.name == "collision" && gid > 0)
		{
			int c = atoi(mTileInfo[gid].properties["c"].c_str());

			map.mRoom->obstacleLayer.cell(y,x) = static_cast<RoomBlock::Type>(c);
		}

		tileNode = tileNode.next_sibling("tile");
		x++;
		if(x == map.mW)
		{
			x = 0;
			y++;
		}
	}

	map.mLayers.push_back(layer);

	return true;
}

bool MapLoader::mParseImageLayer(const pugi::xml_node& imageLayerNode)
{
	pugi::xml_node imageNode;
	//load image
	if(!(imageNode = imageLayerNode.child("image")) || !imageNode.attribute("source"))
		throw std::runtime_error("Image layer missing image source property. Map not loaded.");

	std::string imageName = mMapDirectory + imageNode.attribute("source").as_string();
	sf::Image sourceImage = mLoadImage(imageName);
	
	//set transparency if required
	if(imageNode.attribute("trans"))
	{
		sourceImage.createMaskFromColor(mColourFromHex(imageNode.attribute("trans").as_string()));
	}

	//load image to texture

	//sf::Texture texture;
	//texture.loadFromImage(sourceImage);
	std::unique_ptr<sf::Texture> texture = std::unique_ptr<sf::Texture>(new sf::Texture());
	texture->loadFromImage(sourceImage);

	mTexturesImageLayer.push_back(std::move(texture));

	//add texture to layer as sprite, set layer properties
	MapImageLayer imageLayer;
	imageLayer.sprite.setTexture(*mTexturesImageLayer.back().get());
	
	if(imageLayerNode.attribute("visible")) imageLayer.visible = imageLayerNode.attribute("visible").as_bool();

	if(imageLayerNode.attribute("opacity"))
	{
		imageLayer.opacity = imageLayerNode.attribute("opacity").as_float();
		sf::Uint8 opacity = static_cast<sf::Uint8>(255.f * imageLayer.opacity);
		imageLayer.sprite.setColor(sf::Color(255u, 255u, 255u, opacity));
	}
	
	//push back layer
	mImageLayers.push_back(imageLayer);

	return true;
}

void MapLoader::mAddTileToLayer(Map& map, Map::Layer& layer, sf::Uint16 x, sf::Uint16 y, sf::Uint16 gid)
{
	sf::Uint8 opacity = static_cast<sf::Uint8>(255.f * layer.opacity);
	sf::Color colour = sf::Color(255u, 255u, 255u, opacity);

	//update the layer's vertex array(s)
	sf::Vertex v0, v1, v2, v3;

	//applying half pixel trick avoids artifacting when scrolling
	v0.texCoords = mTileInfo[gid].vertices[0] + sf::Vector2f(0.5f, 0.5f);
	v1.texCoords = mTileInfo[gid].vertices[1] + sf::Vector2f(-0.5f, 0.5f);
	v2.texCoords = mTileInfo[gid].vertices[2] + sf::Vector2f(-0.5f, -0.5f);
	v3.texCoords = mTileInfo[gid].vertices[3] + sf::Vector2f(0.5f, -0.5f);

	v0.position = sf::Vector2f(static_cast<float>(map.mTileW * x), static_cast<float>(map.mTileH * y));
	v1.position = sf::Vector2f(static_cast<float>(map.mTileW * x) + mTileInfo[gid].size.x, static_cast<float>(map.mTileH * y));
	v2.position = sf::Vector2f(static_cast<float>(map.mTileW * x) + mTileInfo[gid].size.x, static_cast<float>(map.mTileH * y) + mTileInfo[gid].size.y);
	v3.position = sf::Vector2f(static_cast<float>(map.mTileW * x), static_cast<float>(map.mTileH * y) + mTileInfo[gid].size.y);

	//offset tiles with size not equal to map grid size
	sf::Uint16 tileHeight = static_cast<sf::Uint16>(mTileInfo[gid].size.y);
	if(tileHeight != map.mTileH)
	{
		float diff = static_cast<float>(map.mTileH - tileHeight);
		v0.position.y += diff;
		v1.position.y += diff;
		v2.position.y += diff;
		v3.position.y += diff;
	}

	v0.color = colour;
	v1.color = colour;
	v2.color = colour;
	v3.color = colour;

	layer.vertexArrays[mTileInfo[gid].setId].append(v0);
	layer.vertexArrays[mTileInfo[gid].setId].append(v1);
	layer.vertexArrays[mTileInfo[gid].setId].append(v2);
	layer.vertexArrays[mTileInfo[gid].setId].append(v3);
}

sf::Image& MapLoader::mLoadImage(std::string path)
{
	auto i = mCachedImages.find(path);
	if(i != mCachedImages.end())
		return *i->second;

	//else attempt to load
	std::shared_ptr<sf::Image> newImage = std::shared_ptr<sf::Image>(new sf::Image());
	if(path.empty() || !newImage->loadFromFile(path))
	{
		newImage->create(20u, 20u, sf::Color::Yellow);
	}
	mCachedImages[path] = newImage;
	return *mCachedImages[path];
}

//decoding and utility functions
sf::Color MapLoader::mColourFromHex(const char* hexStr) const
{
	//TODO proper checking valid string length
	unsigned int value, r, g, b;
	std::stringstream input(hexStr);
	input >> std::hex >> value;

	r = (value >> 16) & 0xff;
	g = (value >> 8) & 0xff;
	b = value & 0xff;

	return sf::Color(r, g, b);
}