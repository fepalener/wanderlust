#include "Anim.h"
#include <cassert>

Anim::Anim()
: mNumFrames(0)
, mCurrentFrame(0)
, mLoops(0)
, mElapsedTime(sf::Time::Zero)
, mRepeat(false)
, mFlipH(false)
, mFlipV(false)
{
}

void Anim::setTexture(const sf::Texture& texture)
{
	mTexture = texture;
}

const sf::Texture* Anim::getTexture() const
{
	return &mTexture;
}

void Anim::setCurrentFrame(std::size_t frame)
{
	assert(mFrames.size() <= frame);
	mCurrentFrame = frame;
}

std::size_t Anim::getNumFrames() const
{
	return mNumFrames;
}

void Anim::setRepeating(bool flag)
{
	mRepeat = flag;
}

bool Anim::isRepeating() const
{
	return mRepeat;
}

void Anim::setLoops(unsigned int loops)
{
	mLoops = loops;
}

void Anim::setFlipH(bool flip)
{
	mFlipH = flip;
}

void Anim::setFlipV(bool flip)
{
	mFlipV = flip;
}

void Anim::restart()
{
	mCurrentFrame = 0;
}

bool Anim::isFinished() const
{
	return mCurrentFrame >= mNumFrames;
}

void Anim::addFrame(const Frame& frame)
{
	mFrames.push_back(frame);
	mNumFrames++;
}

void Anim::update(sf::Time dt)
{
	if(mNumFrames == 0) return;

	mElapsedTime += dt;

	sf::Time frameTime = mFrames[mCurrentFrame].duration;

	// While we have a frame to process
	while (mElapsedTime >= frameTime && (mCurrentFrame <= mNumFrames || mRepeat))
	{
		// And progress to next frame
		mElapsedTime -= frameTime;
		mCurrentFrame = (mCurrentFrame + 1) % mNumFrames;
	}
}

void Anim::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if(mNumFrames == 0) return;

	states.transform *= getTransform();
	states.transform.scale(mFlipH ? -1.f : 1.f, mFlipV ? -1.f : 1.f);

	states.texture = &mTexture;
	target.draw(mFrames[mCurrentFrame].vertexArray, states);
}