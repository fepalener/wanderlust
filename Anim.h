#ifndef _Anim_h_
#define _Anim_h_

#include <SFML\Graphics.hpp>

class Anim : public sf::Drawable, public sf::Transformable
{
public:
	struct Frame
	{
		Frame() 
		: duration(sf::Time::Zero)
		, vertexArray(sf::Quads, 4) 
		{}

		sf::VertexArray	vertexArray;
		sf::Time 		duration;					
	};

public:
							Anim();

	void 					setTexture(const sf::Texture& texture);
	const sf::Texture* 		getTexture() const;

	void					setCurrentFrame(std::size_t frame);
	std::size_t 			getNumFrames() const;

	void 					setRepeating(bool flag);
	bool 					isRepeating() const;

	void					setLoops(unsigned int loops);

	void					setFlipH(bool flip);
	void					setFlipV(bool flip);

	void 					restart();
	bool 					isFinished() const;

	void					addFrame(const Frame& frame);

	void 					update(sf::Time dt);


private:
	void 	draw(sf::RenderTarget& target, sf::RenderStates states) const;

	std::size_t 			mNumFrames;
	std::size_t 			mCurrentFrame;
	unsigned int			mLoops;
	bool 					mRepeat;
	bool					mFlipH;
	bool					mFlipV;
	sf::Texture				mTexture;
	sf::Time 				mElapsedTime;
	std::vector<Frame>		mFrames;
};

#endif