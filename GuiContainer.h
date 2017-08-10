#ifndef _GuiContainer_h_
#define _GuiContainer_h_

#include "GuiComponent.h"

namespace GUI
{
	class GuiContainer : public GuiComponent
	{
		public:
			typedef std::shared_ptr<GuiContainer> Ptr;
            

		public:
								GuiContainer();

			void				pack(GuiComponent::Ptr component);

			virtual bool		isSelectable() const;
			virtual void		handleEvent(const sf::Event& event);


		private:
			virtual void		draw(sf::RenderTarget& target, sf::RenderStates states) const;

			bool				hasSelection() const;
			void				select(std::size_t index);
			void				selectNext();
			void				selectPrevious();


		private:
			std::vector<GuiComponent::Ptr>	mChildren;
			int								mSelectedChild;
	};
}

#endif