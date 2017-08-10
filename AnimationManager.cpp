#include "AnimationManager.h"
#include "Def.h"

AnimationManager::AnimationManager(const std::string& directory)
: mDirectory(directory)
{
	//check map directory contains trailing slash
	if(!mDirectory.empty() && *mDirectory.rbegin() != '/')
		mDirectory += '/';
}

bool AnimationManager::load(const std::string& filename)
{
	std::string filePath = mDirectory + filename;

	//parse anim xml, return on error
	pugi::xml_document animDoc;
	pugi::xml_parse_result result = animDoc.load_file(filePath.c_str());
	if(!result)
		throw std::runtime_error("AnimationManager::load - Failed to load " + filename + ",reason: " + result.description());

	pugi::xml_node animationsNode = animDoc.child("animations");
	if(!animationsNode)
		throw std::runtime_error("Animations node not found. Animations " + filename + " not loaded.");

	if(!parseAnimationsNode(animationsNode)) return false;

	return true;
}

bool AnimationManager::loadSprites(const std::string& filename)
{
	std::string filePath = mDirectory + filename;

	//parse anim xml, return on error
	pugi::xml_document spriteDoc;
	pugi::xml_parse_result result = spriteDoc.load_file(filePath.c_str());
	if(!result)
		throw std::runtime_error("AnimationManager::load - Failed to load " + filename + ",reason: " + result.description());

	pugi::xml_node spritesNode = spriteDoc.child("img");
	if(!spritesNode)
		throw std::runtime_error("Sprites img node not found. Sprites " + filename + " not loaded.");

	//load sprite sheet image from disk
	std::string imagePath = mDirectory + spritesNode.attribute("name").as_string();

	sf::Image sourceImage = loadImage(imagePath);

	//store image as a texture for drawing with vertex array
	std::unique_ptr<sf::Texture> texture = std::unique_ptr<sf::Texture>(new sf::Texture());
	texture->loadFromImage(sourceImage);
	mTextures.push_back(std::move(texture));
	
	pugi::xml_node definitionsNode;
	if(!(definitionsNode = spritesNode.child("definitions")))
		throw std::runtime_error("No definitions node found.");

	parseSpritesNode(definitionsNode);

	return true;
}

bool AnimationManager::parseAnimationsNode(const pugi::xml_node& animationsNode)
{
	//parse tile properties
	if( !animationsNode.attribute("spriteSheet"))
	{
		throw std::runtime_error("No 'spriteSheet' attribute found, check anim data. Animation not loaded.");
	}

	//load sprites
	if(!loadSprites(animationsNode.attribute("spriteSheet").as_string())) return false;

	//load animations
	pugi::xml_node animationNode;
	if(!(animationNode = animationsNode.child("anim")))
		throw std::runtime_error("No 'anim' node found.");

	while(animationNode)
	{
		std::unique_ptr<Anim> animation = std::unique_ptr<Anim>(new Anim());
		animation->setLoops(animationNode.attribute("loops").as_uint());
		animation->setTexture(*mTextures.back().get());

		std::string animationName = animationNode.attribute("name").as_string();

		//parse animation cells
		if(pugi::xml_node cellNode = animationNode.child("cell"))
		{
			while(cellNode)
			{
				Anim::Frame frame;

				frame.duration = sf::milliseconds(cellNode.attribute("delay").as_int() * 30);

				//parse animation spr
				if(pugi::xml_node sprNode = cellNode.child("spr"))
				{
					std::vector<Spr> sprites;

					while(sprNode)
					{
						Spr s;
						//find sprite def
						auto found = mSprites.find(sprNode.attribute("name").as_string());
						assert(found != mSprites.end());

						s = found->second;

						s.flipH = sprNode.attribute("flipH").as_bool();
						s.flipV = sprNode.attribute("flipV").as_bool();

						s.x = sprNode.attribute("x").as_float();
						s.y = sprNode.attribute("y").as_float();
						s.z = sprNode.attribute("z").as_float();
						s.r = sprNode.attribute("angle").as_float();

						sprites.push_back(s);

						//move on to next spr node
						sprNode = sprNode.next_sibling("spr");
					}

					//order by z
					std::sort(sprites.begin(), sprites.end());

					//prepare vertex array
					for(std::vector<Spr>::iterator it = sprites.begin(); it != sprites.end(); ++it) 
					{
						sf::Vertex v0, v1, v2, v3;

						v0.texCoords = sf::Vector2f(it->rect.left,					it->rect.top);
						v1.texCoords = sf::Vector2f(it->rect.left,					it->rect.top + it->rect.height);
						v2.texCoords = sf::Vector2f(it->rect.left + it->rect.width,	it->rect.top + it->rect.height);
						v3.texCoords = sf::Vector2f(it->rect.left + it->rect.width,	it->rect.top);

						if (it->flipH)
						{
							it->rect.left   += it->rect.width;
							it->rect.width  *= -1.0;
						}
						if (it->flipV)
						{
							it->rect.top	+= it->rect.height;
							it->rect.height *= -1.0;
						}

						float hot_x = it->rect.width  / 2;
						float hot_y = it->rect.height / 2;

						if(it->r != 0.f)
						{
							it->r = it->r * M_PI / 180.f; 

							float sint = std::sinf(it->r);
							float cost = std::cosf(it->r);
						
							float tx1, tx2 ,ty1, ty2;
							
							tx1 = -hot_x;
							ty1 = -hot_y;
							tx2 = it->rect.width  - hot_x;
							ty2 = it->rect.height - hot_y;

							v0.position = sf::Vector2f(tx1*cost - ty1*sint + it->x,	tx1*sint + ty1*cost + it->y);
							v1.position = sf::Vector2f(tx1*cost - ty2*sint + it->x,	tx1*sint + ty2*cost + it->y);
							v2.position = sf::Vector2f(tx2*cost - ty2*sint + it->x,	tx2*sint + ty2*cost + it->y);
							v3.position = sf::Vector2f(tx2*cost - ty1*sint + it->x,	tx2*sint + ty1*cost + it->y);
						}
						else
						{
							v0.position = sf::Vector2f(it->x - hot_x,					it->y - hot_y);
							v1.position = sf::Vector2f(it->x - hot_x,					it->y + it->rect.height - hot_y);
							v2.position = sf::Vector2f(it->x + it->rect.width - hot_x,	it->y + it->rect.height - hot_y);
							v3.position = sf::Vector2f(it->x + it->rect.width - hot_x,	it->y - hot_y);
						}

						sf::Uint8 opacity = static_cast<sf::Uint8>(255.f * it->opacity);
						sf::Color colour  = sf::Color(255u, 255u, 255u, opacity);

						v0.color = colour;
						v1.color = colour;
						v2.color = colour;
						v3.color = colour;

						frame.vertexArray.append(v0);
						frame.vertexArray.append(v1);
						frame.vertexArray.append(v2);
						frame.vertexArray.append(v3);
					}
				}

				animation->addFrame(frame);

				//move on to next cell node
				cellNode = cellNode.next_sibling("cell");
			}
		}
		

		mAnimations.insert(std::make_pair(animationName, std::move(animation)));

		//move on to next anim node
		animationNode = animationNode.next_sibling("anim");
	}

	return true;
}

bool AnimationManager::parseSpritesNode(const pugi::xml_node& spritesNode)
{
	if(strcmp(spritesNode.name(), "spr") == 0)
	{
		//find sprite name
		std::string spriteName;
		pugi::xml_node node = spritesNode;
		
		while(strcmp(node.parent().name(), "dir") == 0) 
		{
			std::string parentName = node.parent().attribute("name").as_string();
			if(!parentName.empty() && *parentName.rbegin() != '/')
				parentName += '/';

			spriteName.insert(0, parentName);
			
			node = node.parent();
		}
		
		spriteName += spritesNode.attribute("name").as_string();

		//parse sprite
		Spr sprite;
		sprite.rect.left	= spritesNode.attribute("x").as_float();
		sprite.rect.top		= spritesNode.attribute("y").as_float();
		sprite.rect.width	= spritesNode.attribute("w").as_float();
		sprite.rect.height  = spritesNode.attribute("h").as_float();

		mSprites.insert(std::make_pair(spriteName, sprite));
	}

    for(pugi::xml_node child = spritesNode.first_child(); child; child = child.next_sibling())
    {
        parseSpritesNode(child);
    }

	return true;
}

Anim& AnimationManager::get(const std::string& name)
{
	auto found = mAnimations.find(name);
	assert(found != mAnimations.end());

	return *found->second;
}

const Anim& AnimationManager::get(const std::string& name) const
{
	auto found = mAnimations.find(name);
	assert(found != mAnimations.end());

	return *found->second;
}

sf::Image& AnimationManager::loadImage(std::string path)
{
	auto i = mCachedImages.find(path);
	if(i != mCachedImages.end())
		return *i->second;

	//else attempt to load
	std::shared_ptr<sf::Image> newImage = std::shared_ptr<sf::Image>(new sf::Image());
	if(path.empty() || !newImage->loadFromFile(path))
	{
		newImage->create(20u, 20u, sf::Color::Yellow);
	}
	mCachedImages[path] = newImage;
	return *mCachedImages[path];
}