#ifndef _EffectBloom_h_
#define _EffectBloom_h_

#include "Effect.h"
#include "ResourceIdentifiers.h"
#include "ResourceManager.h"

#include <SFML\Graphics.hpp>
#include <array>

class EffectBloom : public Effect
{
	public:
							EffectBloom();

		virtual void		apply(const sf::RenderTexture& input, sf::RenderTarget& output);


	private:
		typedef std::array<sf::RenderTexture, 2> RenderTextureArray;


	private:
		void				prepareTextures(sf::Vector2u size);

		void				filterBright(const sf::RenderTexture& input, sf::RenderTexture& output);
		void				blurMultipass(RenderTextureArray& renderTextures);
		void				blur(const sf::RenderTexture& input, sf::RenderTexture& output, sf::Vector2f offsetFactor);
		void				downsample(const sf::RenderTexture& input, sf::RenderTexture& output);
		void				add(const sf::RenderTexture& source, const sf::RenderTexture& bloom, sf::RenderTarget& target);


	private:
		ShaderManager		mShaders;

		sf::RenderTexture	mBrightnessTexture;
		RenderTextureArray	mFirstPassTextures;
		RenderTextureArray	mSecondPassTextures;
};

#endif