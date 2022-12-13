#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

class BlocksLoader {
private:
	unsigned int blocks_height;
	unsigned int blocks_width;
public:
	BlocksLoader(unsigned int height, unsigned int width);
	~BlocksLoader();

	std::vector<std::vector<std::string>>* g_blockMap;

	bool LoadNewFile(std::string filePath);

	bool readJsonFile(std::string filePath);
	bool readFile(std::string filePath);
};