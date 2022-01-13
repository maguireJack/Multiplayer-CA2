#pragma once
#include <string>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "Animation.hpp"


namespace sf
{
	class Sprite;
	class Text;
}

class Utility
{
public:
	static std::vector<std::string> Split(const std::string& s, char delim);
	static void CentreOrigin(sf::Sprite& sprite);
	static void CentreOrigin(sf::Text& text);
	static void CentreOrigin(Animation& animation);
	static std::string toString(sf::Keyboard::Key key);
	static double ToRadians(int degrees);
	static sf::Vector2f UnitVector(sf::Vector2f vector);
	static float Length(sf::Vector2f vector);
	static float ToDegrees(float angle);

	enum Direction
	{
		Up,
		Down,
		Left,
		Right
	};
};

