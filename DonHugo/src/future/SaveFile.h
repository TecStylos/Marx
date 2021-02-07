#pragma once

#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "Tools.h"

#ifndef DH_ASSERT
  #define DH_ASSERT(r) { if (!(r)) __debugbreak(); }
#endif

struct SaveFileHeader
{
	char identifier[10] = "DonHugoSF";
};

struct BlockHeader
{
	char blockName[16];
	uint32_t nSubBlocks;
	uint32_t blockOffset;
	uint32_t blockSize;
};

class BlockData
{
public:
	BlockData(uint32_t size);
	BlockData(uint32_t size, void* pData);
	BlockData(uint32_t size, std::istream& is);
	~BlockData();
public:
	uint32_t getSize() const;
	void* getData();
private:
	uint32_t m_size;
	void* m_pData;
};

class BlockChain
{
public:
	BlockChain() = default;
	BlockChain(const char* chainStr);
	BlockChain(const std::string& chainStr);
public:
	uint32_t getDepth() const;
	std::string operator[](uint32_t index) const;
	BlockChain operator+(const BlockChain& other) const;
private:
	uint32_t m_depth = 0;
	std::string m_chainStr;
};

struct Block;
typedef std::shared_ptr<Block> BlockRef;
typedef std::unordered_map<std::string, BlockRef> BlockMap;

struct Block
{
public:
	Block() = default;
	Block(const std::string& name);
public:
	std::string name;
	BlockMap subBlocks;
	std::shared_ptr<BlockData> pBlockData;
public:
	uint32_t getSize() const;
	void saveToFile(std::ofstream& file, uint32_t currOffset);
public:
	static BlockRef loadFromFile(std::ifstream& file, uint32_t currOffset);
};

class SaveFile
{
public:
	SaveFile();
public:
	bool hasBlockChain(const BlockChain& blockChain) const;
	std::vector<std::string> getSubBlockNames(const BlockChain& blockChain);
	BlockRef addBlock(const BlockChain& blockChain);
	BlockRef getBlock(const BlockChain& blockChain);
	const BlockRef getBlock(const BlockChain& blockChain) const;
	std::shared_ptr<BlockData> getBlockData(const BlockChain& blockChain);
public:
	void saveToFile(const std::string& filepath) const;
public:
	static std::shared_ptr<SaveFile> loadFromFile(const std::string& filepath);
private:
	BlockRef m_pRootBlock;
};