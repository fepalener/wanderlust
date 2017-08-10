#ifndef _ResourceIdentifiers_h_
#define _ResourceIdentifiers_h_

// Forward declaration of SFML classes
namespace sf
{
	class Texture;
	class Font;
	class Shader;
}

namespace Textures
{
	enum ID
	{
		Entities,
		Jungle,
		TitleScreen,
		Buttons,
		Explosion,
		Particle,
		Tileset,
		FinishLine,
		Gui,
		Collision,
		Water,
		Player,
		Background,
		TestImage,
		TestLight,
	};
}

namespace Shaders
{
	enum ID
	{
		BrightnessPass,
		DownSamplePass,
		GaussianBlurPass,
		AddPass,
		Tyndall,
	};
}

namespace Fonts
{
	enum ID
	{
		Main,
	};
}

// Forward declaration and a few type definitions
template<typename Resource, typename Identifier>
class ResourceManager;

typedef ResourceManager<sf::Texture, Textures::ID>	TextureManager;
typedef ResourceManager<sf::Font, Fonts::ID>		FontManager;
typedef ResourceManager<sf::Shader, Shaders::ID>	ShaderManager;

#endif