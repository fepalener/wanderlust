#ifndef _EffectTyndall_h_
#define _EffectTyndall_h_

#include "Effect.h"
#include "ResourceIdentifiers.h"
#include "ResourceManager.h"

#include <SFML\Graphics.hpp>
#include <array>

class EffectTyndall : public Effect
{
public:
					EffectTyndall();

	virtual void	apply(const sf::RenderTexture& input, sf::RenderTarget& output);
			void	setLightPosition(sf::Vector2f& position);
private:
			void	prepareTextures(sf::Vector2u size);
			void	filterTyndall(const sf::RenderTexture& input, sf::RenderTexture& output);

private:
	ShaderManager		mShaders;

	sf::RenderTexture	mTexture;
	sf::Vector2f		mLightPosition;
};

#endif