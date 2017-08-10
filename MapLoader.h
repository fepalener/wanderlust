#ifndef _MapLoader_h_
#define _MapLoader_h_

#include "Map.h"
#include "Room.h"
#include <SFML\Graphics.hpp>
#include <pugixml\pugixml.hpp>
#include <array>


class MapLoader : private sf::NonCopyable
{
private:

	struct MapImageLayer
	{
		MapImageLayer() : opacity(1.f), parallax(1.f), visible(true) {};
		sf::Sprite							sprite;
		std::vector<sf::VertexArray>		vertexArrays;
		float								opacity; //range 0 - 1
		float								parallax;
		bool								visible;
	};

	struct TileInfo
	{
		std::array<sf::Vector2f, 4>			vertices;
		sf::Vector2f						size;
		sf::Uint16							setId;
		sf::IntRect							subRect;
		std::map<std::string, std::string>  properties;

		TileInfo() : setId(0){};
		TileInfo(const sf::IntRect& rect, const sf::Vector2f& size, sf::Uint16 tilesetId) : size(size), setId(tilesetId), subRect(rect)
		{
			vertices[0] = sf::Vector2f(static_cast<float>(rect.left),				static_cast<float>(rect.top));
			vertices[1] = sf::Vector2f(static_cast<float>(rect.left + rect.width),	static_cast<float>(rect.top));
			vertices[2] = sf::Vector2f(static_cast<float>(rect.left + rect.width),	static_cast<float>(rect.top + rect.height));
			vertices[3] = sf::Vector2f(static_cast<float>(rect.left),				static_cast<float>(rect.top + rect.height));
		}
	};

public:
	typedef std::unique_ptr<MapLoader> Ptr;

				MapLoader(const std::string& mapDirectory);
	bool		load(const std::string& mapFile, Map& map);

private:
	bool		mParseMapNode(const pugi::xml_node& mapNode, Map& map);
	bool		mParseTilesetNode(const pugi::xml_node& mapNode, Map& map);
	bool		mParseLayer(const pugi::xml_node& layerNode, Map& map);
	bool		mParseImageLayer(const pugi::xml_node& imageLayerNode);
	void		mAddTileToLayer(Map& map, Map::Layer& layer, sf::Uint16 x, sf::Uint16 y, sf::Uint16 gid);
	sf::Image&	mLoadImage(std::string path);
	//utility method for parsing colour values from hex values
	sf::Color	mColourFromHex(const char* hexStr) const;

private:
	std::string											mMapDirectory;

	std::map<std::string, std::shared_ptr<sf::Image>>	mCachedImages;
	std::vector<sf::Texture>							mTextures;
	std::vector<std::unique_ptr<sf::Texture>>			mTexturesImageLayer;
	std::vector<TileInfo>								mTileInfo;
	std::vector<MapImageLayer>							mImageLayers;
};

#endif