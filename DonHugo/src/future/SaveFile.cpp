#include "SaveFile.h"

BlockVar::BlockVar(uint16_t keyID, uint32_t size)
	: m_keyID(keyID), m_size(size), m_pData(malloc(size))
{
}

BlockVar::BlockVar(uint16_t keyID, uint32_t size, const void* pData)
	: BlockVar(keyID, size)
{
	memcpy(m_pData, pData, size);
}

BlockVar::~BlockVar()
{
	free(m_pData);
}

uint16_t BlockVar::getKeyID() const
{
	return m_keyID;
}

uint32_t BlockVar::getSize() const
{
	uint32_t headerSize = sizeof(BlockVarHeader);
	uint32_t dataSize = getDataSize();
	return headerSize + dataSize;
}

uint32_t BlockVar::getDataSize() const
{
	return m_size;
}

void* BlockVar::getData()
{
	return m_pData;
}

const void* BlockVar::getData() const
{
	return m_pData;
}

void BlockVar::saveToFile(std::ofstream& file) const
{
	BlockVarHeader bvh;

	bvh.keyID = m_keyID;
	bvh.varSize = m_size;

	file.write((const char*)&bvh, sizeof(BlockVarHeader));
	file.write((const char*)getData(), bvh.varSize);
}

BlockVarRef BlockVar::loadFromFile(std::ifstream& file)
{
	BlockVarRef pVar;

	BlockVarHeader bvh;
	file.read((char*)&bvh, sizeof(BlockVarHeader));

	pVar = std::make_shared<BlockVar>(bvh.keyID, bvh.varSize);
	
	file.read((char*)pVar->getData(), bvh.varSize);

	return pVar;
}

BlockChain::BlockChain(std::vector<uint16_t> chain)
	: m_chain(chain)
{}

BlockChain::BlockChain(KeyListRef pKeyList, const std::string& chainStr)
{
	std::string token;
	for (char c : chainStr)
	{
		if (c == '.')
		{
			m_chain.push_back(pKeyList->get(token));
			token.clear();
			continue;
		}
		token.push_back(c);
	}
	if (!token.empty())
		m_chain.push_back(pKeyList->get(token));
}

uint32_t BlockChain::getDepth() const
{
	return (uint32_t)m_chain.size();
}

uint16_t BlockChain::operator[](uint32_t index) const
{
	return m_chain[index];
}

BlockChain BlockChain::operator+(const BlockChain& other) const
{
	auto nChain = m_chain;
	for (uint16_t i : other.m_chain)
		nChain.push_back(i);
	
	return nChain;
}

SaveFile::SaveFile()
{
	m_pKeyList = std::make_shared<KeyList>();
	m_pRootBlock = std::make_shared<Block>(m_pKeyList->get("root"));
}

bool SaveFile::hasBlockChain(const std::string& blockChain) const
{
	return !!getBlock(BlockChain(m_pKeyList, blockChain));
}

bool SaveFile::hasVar(const std::string& blockChain, const std::string& varName) const
{
	auto pBlock = getBlock(BlockChain(m_pKeyList, blockChain));

	return pBlock ? pBlock->hasVar(m_pKeyList->get(varName)) : false;
}

BlockRef SaveFile::addBlock(const BlockChain& blockChain)
{
	auto pBlock = m_pRootBlock;
	for (uint32_t i = 1; i < blockChain.getDepth(); ++i)
	{
		auto& res = pBlock->subBlocks.find(blockChain[i]);
		if (res == pBlock->subBlocks.end())
			pBlock = pBlock->subBlocks.insert(std::make_pair(blockChain[i], std::make_shared<Block>(blockChain[i]))).first->second;
		else
			pBlock = res->second;
	}

	return pBlock;
}

BlockRef SaveFile::getBlock(const BlockChain& blockChain)
{
	return addBlock(blockChain);
}

const BlockRef SaveFile::getBlock(const BlockChain& blockChain) const
{
	auto pBlock = m_pRootBlock;
	for (uint32_t i = 1; i < blockChain.getDepth(); ++i)
	{
		auto& res = pBlock->subBlocks.find(blockChain[i]);
		if (res == pBlock->subBlocks.end())
			return nullptr;
		pBlock = res->second;
	}
	return pBlock;
}

void SaveFile::saveToFile(const std::string& filepath) const
{
	SaveFileHeader sfh;
	std::ofstream file(filepath, std::ios::out | std::ios::binary);
	DH_ASSERT(file.good());

	file.write((const char*)&sfh, sizeof(SaveFileHeader));
	m_pKeyList->saveToFile(file);

	m_pRootBlock->saveToFile(file);
	file.close();
}

std::vector<std::string> SaveFile::getSubBlockNames(const std::string& blockChain) const
{
	auto pBlock = getBlock(BlockChain(m_pKeyList, blockChain));

	if (!pBlock)
		return std::vector<std::string>();

	std::vector<std::string> names;
	for (auto& elem : pBlock->subBlocks)
	{
		names.push_back(m_pKeyList->get(elem.first));
	}

	return names;
}

std::shared_ptr<SaveFile> SaveFile::loadFromFile(const std::string& filepath)
{
	SaveFileHeader sfh;
	std::ifstream file(filepath, std::ios::in | std::ios::binary);
	if (!file.good())
		return nullptr;

	file.read((char*)&sfh, sizeof(SaveFileHeader));
	DH_ASSERT(!strcmp(sfh.identifier, "DHSF"));

	std::shared_ptr<SaveFile> psf = std::make_shared<SaveFile>();

	psf->m_pKeyList = KeyList::loadFromFile(file);

	psf->m_pRootBlock = Block::loadFromFile(file);

	return psf;
}

Block::Block(uint16_t keyID)
	: keyID(keyID)
{}

bool Block::hasVar(uint16_t keyID) const
{
	return vars.find(keyID) != vars.end();
}

uint32_t Block::getSize() const
{
	uint32_t headerSize = sizeof(BlockHeader);
	uint32_t subSize = 0;
	for (auto elem : subBlocks)
		subSize += elem.second->getSize();
	uint32_t varSize = 0;
	for (auto elem : vars)
		varSize += elem.second->getSize();

	return headerSize + subSize + varSize;
}

void Block::saveToFile(std::ofstream& file)
{
	uint32_t subSize = 0;
	for (auto elem : subBlocks)
		subSize += elem.second->getSize();

	BlockHeader bh;

	bh.keyID = keyID;
	bh.nSubBlocks = (uint32_t)subBlocks.size();
	bh.nVars = (uint32_t)vars.size();

	file.write((const char*)&bh, sizeof(BlockHeader));

	for (auto elem : subBlocks)
	{
		elem.second->saveToFile(file);
	}

	for (auto elem : vars)
	{
		elem.second->saveToFile(file);
	}
}

BlockRef Block::loadFromFile(std::ifstream& file)
{
	BlockHeader bh;
	
	file.read((char*)&bh, sizeof(BlockHeader));

	BlockRef pBlock = std::make_shared<Block>(bh.keyID);

	for (uint32_t i = 0; i < bh.nSubBlocks; ++i)
	{
		BlockRef pSubBlock = Block::loadFromFile(file);
		pBlock->subBlocks.insert(std::make_pair(pSubBlock->keyID, pSubBlock));
	}

	for (uint32_t i = 0; i < bh.nVars; ++i)
	{
		BlockVarRef pVar = BlockVar::loadFromFile(file);
		pBlock->vars.insert(std::make_pair(pVar->getKeyID(), pVar));
	}

	return pBlock;
}

uint16_t KeyList::get(const std::string& strID)
{
	auto it = m_keys.find(strID);
	if (it != m_keys.end())
		return it->second;

	m_keys.insert(std::make_pair(strID, m_currIndex));
	m_keyStrings.push_back(strID);
	return m_currIndex++;
}

const std::string& KeyList::get(uint16_t keyID) const
{
	return m_keyStrings[keyID];
}

uint32_t KeyList::getSize() const
{
	uint32_t size = 0;
	size += sizeof(KeyListHeader);
	for (auto& elem : m_keys)
	{
		size += (uint32_t)elem.first.size() + 1;
	}
	return size;
}

void KeyList::saveToFile(std::ofstream& file) const
{
	KeyListHeader klh;
	klh.nKeys = (uint16_t)m_keys.size();
	file.write((const char*)&klh, sizeof(KeyListHeader));

	for (auto& elem : m_keyStrings)
	{
		file.write(elem.c_str(), elem.size() + 1);
	}
}

KeyListRef KeyList::loadFromFile(std::ifstream& file)
{
	auto pKl = std::make_shared<KeyList>();

	KeyListHeader klh;
	file.read((char*)&klh, sizeof(KeyListHeader));

	for (uint16_t i = 0; i < klh.nKeys; ++i)
	{
		std::string str;
		char c;
		while ((c = file.get()) != '\0')
			str.push_back(c);
		pKl->get(str);
	}

	return pKl;
}
