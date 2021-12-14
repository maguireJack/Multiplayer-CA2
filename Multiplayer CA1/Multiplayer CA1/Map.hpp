#pragma once
#include <string>

#include "SceneNode.hpp"

class Map : public SceneNode
{
public:
	Map(std::string path, std::string textureAtlas, int tilesX);
	void LoadLayer(std::string path, std::string layer, std::string textureAtlas, int tilesX, Category::Type tileCategory);
private:
	void LoadLayer(std::string path, std::string textureAtlas, int tilesX, Category::Type tileCategory);
	sf::Texture* LoadTextureAt(std::string texture, int index, int tilesX);
};
