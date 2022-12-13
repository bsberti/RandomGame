#include "BlocksLoader.h"

BlocksLoader::BlocksLoader(unsigned int height, unsigned int width) {
	this->blocks_height = height;
	this->blocks_width = width;
	
	g_blockMap = new std::vector<std::vector<std::string>>(blocks_height, std::vector<std::string>(blocks_width));

	if (!readFile("assets/Dungeon1.txt")) {
		std::cout << "Failed to read base block file." << std::endl;
	}
}

BlocksLoader::~BlocksLoader() {
	delete[] g_blockMap;
}

bool BlocksLoader::LoadNewFile(std::string filePath) {
	g_blockMap = new std::vector<std::vector<std::string>>(blocks_height, std::vector<std::string>(blocks_width));

	if (!readFile(filePath)) {
		return false;
	}

	return true;
}

bool BlocksLoader::readJsonFile(std::string filePath) {
	return false;
}

// Here's a simple way to load the comma delimited files:
bool BlocksLoader::readFile(std::string filePath) {
	
	// Open the file
	std::ifstream namesFile2(filePath);
	if (!namesFile2.is_open()) {
		return false;
	}

	std::string theLine2;
	std::string currentString;
	for (int i = 0; i < g_blockMap->size(); i++)
	{
		if (std::getline(namesFile2, theLine2)) {
			std::stringstream ssLine(theLine2);
			std::string token;
			for (int j = 0; j < g_blockMap->at(i).size(); j++) {
				if (std::getline(ssLine, token, ' ')) {
					g_blockMap->at(i).at(j) = token;
				}
			}
		}
	}

	return true;
}

