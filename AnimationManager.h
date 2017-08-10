#ifndef _AnimationManager_h_
#define _AnimationManager_h_

#include <map>
#include <string>
#include <SFML\Graphics.hpp>
#include <pugixml\pugixml.hpp>
#include <cassert>
#include "Anim.h"

class AnimationManager : private sf::NonCopyable
{
public:
	struct Spr
	{
		Spr() : x(0), y(0), z(0), r(0), opacity(1), flipH(false), flipV(false) {}
		Spr(const Spr& s) : rect(s.rect), x(s.x), y(s.y), z(s.z), r(s.r), opacity(1), flipH(s.flipH), flipV(s.flipV) {}

		float x;
		float y;
		float z;
		float r;
		float opacity;
		bool flipH;
		bool flipV;
		sf::FloatRect rect;

		//z order
		bool operator < (const Spr& spr) const
		{
			return (spr.z < z);
		}
	};

	AnimationManager(const std::string& directory);

	bool			load(const std::string& filename);
	bool			loadSprites(const std::string& filename);

	Anim&			get(const std::string& name);
	const Anim&		get(const std::string& name) const;

private:
	sf::Image&	loadImage(std::string path);
	bool		parseAnimationsNode(const pugi::xml_node& animationsNode);
	bool		parseSpritesNode(const pugi::xml_node& spritesNode);


private:
	std::string											mDirectory;
	std::vector<std::unique_ptr<sf::Texture>>			mTextures;
	std::map<std::string, Spr>							mSprites;
	std::map<std::string, std::unique_ptr<Anim>>		mAnimations;
	std::map<std::string, std::shared_ptr<sf::Image>>	mCachedImages;
};

#endif