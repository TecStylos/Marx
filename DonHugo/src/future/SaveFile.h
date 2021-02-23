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

#pragma pack(push, 1)
struct SaveFileHeader
{
	char identifier[5] = "DHSF";
};

struct BlockHeader
{
	uint16_t keyID;
	uint16_t nSubBlocks;
	uint16_t nVars;
};

struct BlockVarHeader
{
	uint16_t keyID;
	uint16_t varSize;
};

struct KeyListHeader
{
	uint16_t nKeys;
};
#pragma pack(pop)

struct Block;
class BlockVar;
class KeyList;
typedef std::shared_ptr<Block> BlockRef;
typedef std::shared_ptr<BlockVar> BlockVarRef;
typedef std::shared_ptr<KeyList> KeyListRef;
typedef std::unordered_map<uint16_t, BlockRef> BlockMap;
typedef std::unordered_map<uint16_t, BlockVarRef> VarMap;

class BlockVar
{
public:
	BlockVar(uint16_t keyID, uint32_t size);
	BlockVar(uint16_t keyID, uint32_t size, const void* pData);
	~BlockVar();
public:
	uint16_t getKeyID() const;
	uint32_t getSize() const;
	uint32_t getDataSize() const;
	void* getData();
	const void* getData() const;
public:
	void saveToFile(std::ofstream& file) const;
public:
	static BlockVarRef loadFromFile(std::ifstream& file);
private:
	uint16_t m_keyID;
	uint32_t m_size;
	void* m_pData;
};

class KeyList
{
public:
	KeyList() = default;
public:
	uint16_t get(const std::string& strID);
	const std::string& get(uint16_t keyID) const;
	uint32_t getSize() const;
public:
	void saveToFile(std::ofstream& file) const;
public:
	static KeyListRef loadFromFile(std::ifstream& file);
private:
	uint16_t m_currIndex = 0;
	std::vector<std::string> m_keyStrings;
	std::unordered_map<std::string, uint16_t> m_keys;
};

class BlockChain
{
public:
	BlockChain() = default;
	BlockChain(std::vector<uint16_t> chain);
	BlockChain(KeyListRef pKeyList, const std::string& chainStr);
public:
	uint32_t getDepth() const;
	uint16_t operator[](uint32_t index) const;
	BlockChain operator+(const BlockChain& other) const;
private:
	std::vector<uint16_t> m_chain;
};

struct Block
{
public:
	Block(uint16_t keyID);
public:
	uint16_t keyID;
	BlockMap subBlocks;
	VarMap vars;
	bool hasVar(uint16_t keyID) const;
	template <typename T> void setVar(uint16_t keyID, const T& value);
	template <typename T> T getVar(uint16_t keyID, const T& defaultValue);
public:
	uint32_t getSize() const;
	void saveToFile(std::ofstream& file);
public:
	static BlockRef loadFromFile(std::ifstream& file);
};

class SaveFile
{
public:
	SaveFile();
public:
	bool hasBlockChain(const std::string& blockChain) const;
	bool hasVar(const std::string& blockChain, const std::string& varName) const;
	template <typename T> void setVar(const std::string& blockChain, const std::string& varName, const T& value);
	template <typename T> T getVar(const std::string& blockChain, const std::string& varName, const T& defaultValue);
public:
	void saveToFile(const std::string& filepath) const;
	std::vector<std::string> getSubBlockNames(const std::string& blockChain) const;
public:
	static std::shared_ptr<SaveFile> loadFromFile(const std::string& filepath);
private:
	BlockRef addBlock(const BlockChain& blockChain);
	BlockRef getBlock(const BlockChain& blockChain);
	const BlockRef getBlock(const BlockChain& blockChain) const;
private:
	BlockRef m_pRootBlock;
	KeyListRef m_pKeyList;
};

template<typename T>
inline void Block::setVar(uint16_t keyID, const T& value)
{
	auto it = vars.find(keyID);
	if (it == vars.end())
	{
		vars.insert(std::make_pair(keyID, std::make_shared<BlockVar>(keyID, (uint32_t)sizeof(T), &value)));
	}
	else
	{
		if (it->second->getDataSize() == sizeof(T))
		{
			memcpy(it->second->getData(), &value, sizeof(T));
		}
		else
		{
			it->second = std::make_shared<BlockVar>(keyID, (uint32_t)sizeof(T), &value);
		}
	}
}

template<>
inline void Block::setVar<std::string>(uint16_t keyID, const std::string& value)
{
	auto it = vars.find(keyID);
	if (it == vars.end())
	{
		vars.insert(std::make_pair(keyID, std::make_shared<BlockVar>(keyID, (uint32_t)value.size() + 1, value.c_str())));
	}
	else
	{
		if (it->second->getDataSize() == value.size() + 1)
		{
			memcpy(it->second->getData(), value.c_str(), value.size() + 1);
		}
		else
		{
			it->second = std::make_shared<BlockVar>(keyID, (uint32_t)value.size() + 1, value.c_str());
		}
	}
}

template<typename T>
inline T Block::getVar(uint16_t keyID, const T& defaultValue)
{
	auto it = vars.find(keyID);
	if (it == vars.end())
	{
		vars.insert(std::make_pair(keyID, std::make_shared<BlockVar>(keyID, (uint32_t)sizeof(T), &defaultValue)));
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
			it->second = std::make_shared<BlockVar>(keyID, (uint32_t)sizeof(T), &defaultValue);
			return *(T*)it->second->getData();
		}
	}
}

template<>
inline std::string Block::getVar<std::string>(uint16_t keyID, const std::string& defaultValue)
{
	auto it = vars.find(keyID);
	if (it == vars.end())
	{
		vars.insert(std::make_pair(keyID, std::make_shared<BlockVar>(keyID, (uint32_t)defaultValue.size() + 1, &defaultValue)));
		return defaultValue;
	}
	else
	{
		return std::string((char*)it->second->getData());
	}
}

template<typename T>
inline void SaveFile::setVar(const std::string& blockChain, const std::string& varName, const T& value)
{
	getBlock(BlockChain(m_pKeyList, blockChain))->setVar<T>(m_pKeyList->get(varName), value);
}

template<typename T>
inline T SaveFile::getVar(const std::string& blockChain, const std::string& varName, const T& defaultValue)
{
	return getBlock(BlockChain(m_pKeyList, blockChain))->getVar<T>(m_pKeyList->get(varName), defaultValue);
}
