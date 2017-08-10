#ifndef _Effect_h_
#define _Effect_h_

#include <SFML/System/NonCopyable.hpp>

namespace sf
{
	class RenderTarget;
	class RenderTexture;
	class Shader;
}

class Effect : sf::NonCopyable
{
	public:
		virtual					~Effect();
		virtual void			apply(const sf::RenderTexture& input, sf::RenderTarget& output) = 0;

		static bool				isSupported();
		

	protected:
		static void				applyShader(const sf::Shader& shader, sf::RenderTarget& output);
};

#endif