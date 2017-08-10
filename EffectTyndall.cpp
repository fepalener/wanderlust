#include "EffectTyndall.h"

EffectTyndall::EffectTyndall()
: mShaders()
{
	mShaders.load(Shaders::Tyndall, "../resources/Shaders/Tyndall.frag", sf::Shader::Type::Fragment);
}

void EffectTyndall::apply(const sf::RenderTexture& input, sf::RenderTarget& output)
{
	prepareTextures(input.getSize());

	filterTyndall(input, mTexture);
	output.draw(sf::Sprite(input.getTexture())) ;
	output.draw(sf::Sprite(mTexture.getTexture()),sf::RenderStates(sf::BlendAdd));
}

void EffectTyndall::prepareTextures(sf::Vector2u size)
{
	if (mTexture.getSize() != size)
	{
		mTexture.create(size.x, size.y);
	}
}

void EffectTyndall::setLightPosition(sf::Vector2f& position)
{
	mLightPosition = position;
}

void EffectTyndall::filterTyndall(const sf::RenderTexture& input, sf::RenderTexture& output)
{
	sf::Shader& tyndall = mShaders.get(Shaders::Tyndall);

	tyndall.setParameter("exposure", 0.005f);
    tyndall.setParameter("decay", 0.97f);
    tyndall.setParameter("density", 0.97f);
    tyndall.setParameter("weight", 5.5f);
    tyndall.setParameter("lightPositionOnScreen", mLightPosition);
	tyndall.setParameter("source", input.getTexture());

	applyShader(tyndall, output);
	output.display();
}