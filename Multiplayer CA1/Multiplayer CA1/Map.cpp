#include "Map.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <SFML/Graphics/Texture.hpp>

#include "Tile.hpp"
#include "Utility.hpp"

Map::Map(std::string path, std::string textureAtlas, int tilesX)
{
	//LoadLayer(path, "_BG.csv", textureAtlas, tilesX, Category::kFloorTile);
	LoadLayer(path, "_Border.csv", textureAtlas, tilesX, Category::kWallTile);
	LoadLayer(path, "_Destroyable.csv", textureAtlas, tilesX, Category::kDestroyableTile);
}

void Map::LoadLayer(std::string path, std::string layer, std::string textureAtlas, int tilesX, Category::Type tileCategory)
{
	std::ifstream file(path+layer);
	std::string line;

	int xPos = 0;
	int yPos = 0;

	std::map<int, sf::Texture*> texturemap;

	while (std::getline(file, line))
	{
		for (auto value : Utility::Split(line, ','))
		{
			int elem = std::stoi(value);
			if (elem != -1)
			{
				if (texturemap[elem] == nullptr)
					texturemap[elem] = LoadTextureAt(textureAtlas, elem, tilesX);
				std::unique_ptr<Tile> tile(new Tile(texturemap[elem], tileCategory));
				tile->setPosition(xPos, yPos);
				AttachChild(std::move(tile));
			}

			xPos += 50;
		}
		xPos = 0;
		yPos += 50;
	}
	file.close();
}

sf::Texture* Map::LoadTextureAt(std::string texture, int index, int tilesX)
{
	int x = index % tilesX;
	int y = index / tilesX;

	sf::Texture* tex = new sf::Texture();
	if(!tex->loadFromFile(texture, sf::IntRect((x+1) + x*10, y+1 + y*10, 10,10)))
	{
		std::cout << "TEXTURE WAS NOT FOUND IN MAP.CPP";
	}

	return tex;
}
