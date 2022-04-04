//Alex Nogueira / D00242564 
#include "Map.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <SFML/Graphics/Texture.hpp>

#include "Tile.hpp"
#include "Utility.hpp"

Map::Map(World* world, std::string path, std::string textureAtlas, int tilesX, std::map<int, sf::Vector2f>& tank_spawns,
         std::vector<sf::Vector2f>& pickup_spawners, sf::Vector2f world_center, float spawn_offset, bool load_map)
	         :SceneNode(world)
{
	//LoadLayer(path, "_BG.csv", textureAtlas, tilesX, Category::kFloorTile);
	if (load_map)
	{
		LoadLayer(path, "_Border.csv", textureAtlas, tilesX, Category::kWallTile);
		return;
	}
	LoadLayer(path, "_Destroyable.csv", textureAtlas, tilesX, Category::kDestroyableTile);
	LoadSpawns(path, "_Spawns.csv", tank_spawns, world_center, spawn_offset);

	LoadFile(path, "_Pickups.csv", [this, &pickup_spawners](int xPos, int yPos, int elem)
	{
		if (elem == -1) return;
		sf::Vector2f pos(xPos, yPos);
		pickup_spawners.push_back(pos);
	});
}

void Map::LoadTextureAtPos(int xPos, int yPos, int elem, std::string textureAtlas, int tilesX,
                           Category::Type tileCategory)
{
	if (texture_map.find(elem) == texture_map.end())
		LoadTextureAt(texture_map[elem], textureAtlas, elem, tilesX);
	std::unique_ptr<Tile> tile(new Tile(m_world, texture_map[elem], tileCategory));
	tile->setPosition(xPos, yPos);
	AttachChild(std::move(tile));
}

void Map::LoadSpawns(std::string path, std::string layer, std::map<int, sf::Vector2f>& tank_spawns,
                     sf::Vector2f world_center, float spawn_offset)
{
	LoadFile(path, layer, [this, &tank_spawns, world_center, spawn_offset](int xPos, int yPos, int elem)
	{
		LoadSpawnAtPos(xPos, yPos, elem, tank_spawns, world_center, spawn_offset);
	});
}

void Map::LoadLayer(std::string path, std::string layer, std::string textureAtlas, int tilesX,
                    Category::Type tileCategory)
{
	LoadFile(path, layer, [this, textureAtlas, tilesX, tileCategory](int xPos, int yPos, int elem)
	{
		LoadTextureAtPos(xPos, yPos, elem, textureAtlas, tilesX, tileCategory);
	});
}

void Map::LoadFile(std::string path, std::string layer, std::function<void(int, int, int)> func)
{
	std::ifstream file(path + layer);
	std::string line;

	int xPos = 0;
	int yPos = 0;

	while (std::getline(file, line))
	{
		for (auto value : Utility::Split(line, ','))
		{
			int elem = std::stoi(value);
			if (elem != -1)
			{
				func(xPos, yPos, elem);
			}

			xPos += 50;
		}
		xPos = 0;
		yPos += 50;
	}
	file.close();
}

void Map::LoadSpawnAtPos(int xPos, int yPos, int elem, std::map<int, sf::Vector2f>& tank_spawns,
                         sf::Vector2f world_center, float spawn_offset)
{
	if (elem == -1) return;
	sf::Vector2f pos(xPos, yPos);
	sf::Vector2f offsetDir = Utility::UnitVector(world_center - pos);
	pos = pos + offsetDir * spawn_offset;
	tank_spawns[elem] = pos;
}

void Map::LoadTextureAt(sf::Texture& texture, std::string texture_string, int index, int tilesX)
{
	int x = index % tilesX;
	int y = index / tilesX;

	if (!texture.loadFromFile(texture_string, sf::IntRect((x + 1) + x * 10, y + 1 + y * 10, 10, 10)))
	{
		std::cout << "TEXTURE WAS NOT FOUND IN MAP.CPP";
	}
}
