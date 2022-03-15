//Alex Nogueira / D00242564
#pragma once
#include <map>
#include <string>

#include "SceneNode.hpp"

class Map : public SceneNode
{
public:
	Map(std::string path, std::string textureAtlas, int tilesX, std::map<int, sf::Vector2f>& tank_spawns, sf::Vector2f world_center, float spawn_offset);	

private:
	void LoadTextureAt(sf::Texture& texture, std::string texture_string, int index, int tilesX);
	void LoadSpawns(std::string path, std::string layer, std::map<int, sf::Vector2f>& tank_spawns, sf::Vector2f world_center, float spawn_offset);
	void LoadLayer(std::string path, std::string layer, std::string textureAtlas, int tilesX, Category::Type tileCategory);
	void LoadFile(std::string path, std::string layer, std::function<void(int, int, int)> func);
	void LoadSpawnAtPos(int xPos, int yPos, int elem, std::map<int, sf::Vector2f>& tank_spawns, sf::Vector2f world_center, float spawn_offset);
	void LoadTextureAtPos(int xPos, int yPos, int elem, std::string textureAtlas, int tilesX,
		Category::Type tileCategory);

private:
	std::map<int, sf::Texture> texture_map;
};
