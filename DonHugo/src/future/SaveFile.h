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
	char blockName[16] = { 0 };
	uint16_t nSubBlocks;
	uint32_t blockOffset;
	uint16_t nVars;
};

struct BlockVarHeader
{
	char varName[16] = { 0 };
	uint16_t varSize;
};

struct Block;
class BlockVar;
typedef std::shared_ptr<Block> BlockRef;
typedef std::shared_ptr<BlockVar> BlockVarRef;
typedef std::unordered_map<std::string, BlockRef> BlockMap;
typedef std::unordered_map<std::string, BlockVarRef> VarMap;

class BlockVar
{
public:
	BlockVar(const std::string& name, uint32_t size);
	BlockVar(const std::string& name, uint32_t size, const void* pData);
	~BlockVar();
public:
	const std::string& getName() const;
	uint32_t getSize() const;
	uint32_t getDataSize() const;
	void* getData();
public:
	void saveToFile(std::ofstream& file, uint32_t currOffset);
public:
	static BlockVarRef loadFromFile(std::ifstream& file, uint32_t currOffset);
private:
	std::string m_name;
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

struct Block
{
public:
	Block() = default;
	Block(const std::string& name);
public:
	std::string name;
	BlockMap subBlocks;
	VarMap vars;
	std::vector<std::string> getVarNames() const;
	template <typename T> void setVar(const std::string& varName, const T& value);
	template <typename T> T getVar(const std::string& varName, const T& defaultValue);
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
	std::vector<std::string> getSubBlockNames(const BlockChain& blockChain) const;
	std::vector<std::string> getVarNames(const BlockChain& blockChain) const;
	template <typename T> void setVar(const BlockChain& blockChain, const std::string& varName, const T& value);
	template <typename T> T getVar(const BlockChain& blockChain, const std::string& varName, const T& defaultValue);
public:
	void saveToFile(const std::string& filepath) const;
public:
	static std::shared_ptr<SaveFile> loadFromFile(const std::string& filepath);
private:
	BlockRef addBlock(const BlockChain& blockChain);
	BlockRef getBlock(const BlockChain& blockChain);
	const BlockRef getBlock(const BlockChain& blockChain) const;
private:
	BlockRef m_pRootBlock;
};

template<typename T>
inline void Block::setVar(const std::string& varName, const T& value)
{
	auto it = vars.find(varName);
	if (it == vars.end())
	{
		vars.insert(std::make_pair(varName, std::make_shared<BlockVar>(varName, (uint32_t)sizeof(T), &value)));
	}
	else
	{
		if (it->second->getDataSize() == sizeof(T))
		{
			memcpy(it->second->getData(), &value, sizeof(T));
		}
		else
		{
			it->second = std::make_shared<BlockVar>(varName, (uint32_t)sizeof(T), &value);
		}
	}
}

template<>
inline void Block::setVar<std::string>(const std::string& varName, const std::string& value)
{
	auto it = vars.find(varName);
	if (it == vars.end())
	{
		vars.insert(std::make_pair(varName, std::make_shared<BlockVar>(varName, (uint32_t)value.size() + 1, value.c_str())));
	}
	else
	{
		if (it->second->getDataSize() == value.size() + 1)
		{
			memcpy(it->second->getData(), value.c_str(), value.size() + 1);
		}
		else
		{
			it->second = std::make_shared<BlockVar>(varName, (uint32_t)value.size() + 1, value.c_str());
		}
	}
}

template<typename T>
inline T Block::getVar(const std::string& varName, const T& defaultValue)
{
	auto it = vars.find(varName);
	if (it == vars.end())
	{
		vars.insert(std::make_pair(varName, std::make_shared<BlockVar>(varName, (uint32_t)sizeof(T), &defaultValue)));
		return defaultValue;
	}
	else
	{
		if (it->second->getDataSize() == sizeof(T))
		{
			return *(T*)it->second->getData();
		}
		else
		{
			it->second = std::make_shared<BlockVar>(varName, (uint32_t)sizeof(T), &defaultValue);
			return *(T*)it->second->getData();
		}
	}
}

template<>
inline std::string Block::getVar<std::string>(const std::string& varName, const std::string& defaultValue)
{
	auto it = vars.find(varName);
	if (it == vars.end())
	{
		vars.insert(std::make_pair(varName, std::make_shared<BlockVar>(varName, (uint32_t)defaultValue.size() + 1, &defaultValue)));
		return defaultValue;
	}
	else
	{
		return std::string((char*)it->second->getData());
	}
}

template<typename T>
inline void SaveFile::setVar(const BlockChain& blockChain, const std::string& varName, const T& value)
{
	getBlock(blockChain)->setVar<T>(varName, value);
}

template<typename T>
inline T SaveFile::getVar(const BlockChain& blockChain, const std::string& varName, const T& defaultValue)
{
	return getBlock(blockChain)->getVar<T>(varName, defaultValue);
}
