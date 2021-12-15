#pragma once
#include "Label.hpp"
#include <functional>

namespace GUI {
	class BoundLabel : public Label
	{
	public:
		typedef std::shared_ptr<BoundLabel> Ptr;
	public:
		BoundLabel(const FontHolder& fonts, int characterSize, std::string prefix, std::function<std::string()> update_action, sf::Color text_color = sf::Color::Black);
		void Update();
	private:
		std::string m_prefix;
		std::function<std::string()> m_update_action;
	};
}

