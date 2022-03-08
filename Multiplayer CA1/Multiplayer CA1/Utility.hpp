#pragma once
#include "Utility.hpp"

#include <cassert>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <cmath>
#include <random>
#include <string>
#include <sstream>
#include <vector>
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
	static int RandomInt(int exclusive_max);
	enum Direction
	{
		Up,
		Down,
		Left,
		Right
	};
};

