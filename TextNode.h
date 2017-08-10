#ifndef _TextNode_h_
#define _TextNode_h_

#include "ResourceManager.h"
#include "ResourceIdentifiers.h"
#include "SceneNode.h"

#include <SFML\Graphics.hpp>

class TextNode : public SceneNode
{
	public:
		explicit			TextNode(const FontManager& fonts, const std::string& text);

		void				setString(const std::string& text);


	private:
		virtual void		drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;


	private:
		sf::Text			mText;
};

#endif