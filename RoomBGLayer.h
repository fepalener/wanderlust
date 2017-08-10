#ifndef _RoomBGLayer_h_
#define _RoomBGLayer_h_

#include "Matrix.h"
#include "ResourceIdentifiers.h"
#include <SFML\Graphics.hpp>

class RoomBGLayer : public sf::Drawable
{
public:
	float x;
	float y;
	int screenWidth;
	int screenHeight;

	/** The width of each individual tile, in pixels. */
	const int tileWidth;
		
	/** The height of each individual tile, in pixels. */
	const int tileHeight;

	/**
	 * When the parallax factor is equal to 1.0, the layer will appear to
	 * be on the same plane as the obstacle layer; a factor greater than
	 * 1.0 will make it seem closer to the screen; a factor less than 1.0
	 * will make it seem farther away.
	 * 
	 * For instance, a static "sky" layer would need a parallax factor
	 * equal to zero.
	 */
	const float parallaxFactorX;
	const float parallaxFactorY;

	/**
	 * The translation is applied to the entire layer, and is not affected
	 * by the parallax factor.
	 */
	int translateX;
	int translateY;
	
	Matrix<int>	bgData;

	RoomBGLayer ( int tileWidth, int tileHeight,
				  float parallaxFactorX, float parallaxFactorY,
				  int translateX, int translateY,
				  int rows, int columns, const sf::Texture& texture );

	virtual ~RoomBGLayer();

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	void					addVertex(float worldX, float worldY, float texCoordX, float texCoordY, const sf::Color& color) const;

	const sf::Texture&		mTexture;
	mutable sf::VertexArray	mVertexArray;
};

#endif