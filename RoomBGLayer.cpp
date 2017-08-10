#include "RoomBGLayer.h"
#include "ResourceManager.h"

RoomBGLayer::RoomBGLayer( int _tileWidth, int _tileHeight, float _parallaxFactorX, float _parallaxFactorY, int _translateX, int _translateY, int _rows, int _columns, const sf::Texture& texture )
: bgData(_rows, _columns)
, mVertexArray(sf::Quads)
, tileWidth(_tileWidth)
, tileHeight(_tileHeight)
, parallaxFactorX(_parallaxFactorX)
, parallaxFactorY(_parallaxFactorY)
, translateX(_translateX)
, translateY(_translateY)
, mTexture(texture)
{
	x = 0;
	y = 0;
	screenWidth  = 0;
	screenHeight = 0;
}

RoomBGLayer::~RoomBGLayer()
{
}

void RoomBGLayer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// Clip hidden tiles:
	// Calculate which portions of the tile matrix are visible:
	int startRow = std::max( 0,              (int) ((-y * parallaxFactorY - translateY) / tileHeight));
	int startCol = std::max( 0,              (int) ((-x * parallaxFactorX - translateX) / tileWidth) );
	int endRow   = std::min( bgData.rows,    (int) ((-y * parallaxFactorY + screenHeight - translateY) / tileHeight) + 1); // startRow + (Environment::getScreenHeight() / tileHeight) + 2 );
	int endCol   = std::min( bgData.columns, (int) ((-x * parallaxFactorX + screenWidth  - translateX) / tileWidth)  + 1); // startCol + (Environment::getScreenWidth()  / tileWidth ) + 2 );
	
	mVertexArray.clear();
	
	sf::Vector2f textureSize(mTexture.getSize());

	// Iterate over the bgData matrix:
	for ( int i = startRow; i < endRow; i++ ) {
		for ( int j = startCol; j < endCol; j++ ) {
			int tileID = bgData.cell(i, j);

			// Ignore "blank tiles":
			if ( tileID != -1 ) {

				// Calculate this tile's position in the tile library:
				int tileRow = (tileID * (int)(tileWidth)) / textureSize.x;
				int tileCol = tileID % ((int)(textureSize.x) / tileWidth);

				// Calculate the four vertices' coordinates:
				float t_x  = (translateX + (int) (parallaxFactorX * x) + j * tileWidth);
				float t_y  = (translateY + (int) (parallaxFactorY * y) + i * tileHeight);
				float t_w  = t_x + tileWidth;
				float t_h  = t_y + tileHeight;

				float texLeft  = (float) (tileCol * tileWidth);
				float texTop   = (float) (tileRow * tileHeight);
				float texRight = texLeft + (float) tileWidth;
				float texBott  = texTop  + (float) tileHeight;

				sf::Color color(255,255,255,255);

				//Add the tile
				addVertex(t_x,				t_y,			 texLeft,   texTop,  color);
				addVertex(t_x + tileWidth,	t_y,			 texRight,  texTop,  color);
				addVertex(t_x + tileWidth,	t_y + tileWidth, texRight,	texBott, color);
				addVertex(t_x,				t_y + tileWidth, texLeft,   texBott, color);
			}
		}
	}

	// Apply texture
	states.texture = &mTexture;
	
	// Draw vertices
	target.draw(mVertexArray, states);
}

void RoomBGLayer::addVertex(float worldX, float worldY, float texCoordX, float texCoordY, const sf::Color& color) const
{
	sf::Vertex vertex;
	vertex.position = sf::Vector2f(worldX, worldY);
	vertex.texCoords = sf::Vector2f(texCoordX, texCoordY);
	vertex.color = color;

	mVertexArray.append(vertex);
}