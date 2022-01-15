//Alex Nogueira / D00242564
#pragma once
#include <map>
#include <string>

#include "SceneNode.hpp"

class Map : public SceneNode
{
public:
	Map(std::string path, std::string textureAtlas, int tilesX);
	void LoadLayer(std::string path, std::string layer, std::string textureAtlas, int tilesX, Category::Type tileCategory);

private:
	void LoadLayer(std::string path, std::string textureAtlas, int tilesX, Category::Type tileCategory);
	void LoadTextureAt(sf::Texture& texture, std::string texture_string, int index, int tilesX);

private:
	std::map<int, sf::Texture> texture_map;
};
