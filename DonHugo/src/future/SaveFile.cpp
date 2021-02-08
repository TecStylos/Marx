#include "SaveFile.h"

BlockVar::BlockVar(const std::string& name, uint32_t size)
	: m_name(name), m_size(size)
{
	m_pData = malloc(size);
}

BlockVar::BlockVar(const std::string& name, uint32_t size, const void* pData)
	: BlockVar(name, size)
{
	memcpy(m_pData, pData, size);
}

BlockVar::~BlockVar()
{
	free(m_pData);
}

const std::string& BlockVar::getName() const
{
	return m_name;
}

uint32_t BlockVar::getSize() const
{
	return sizeof(BlockVarHeader) + getDataSize();
}

uint32_t BlockVar::getDataSize() const
{
	return m_size;
}

void* BlockVar::getData()
{
	return m_pData;
}

void BlockVar::saveToFile(std::ofstream& file, uint32_t currOffset)
{
	BlockVarHeader bvh;

	memcpy(bvh.varName, m_name.c_str(), m_name.size() + 1);

	bvh.varSize = m_size;

	file.write((const char*)&bvh, sizeof(BlockVarHeader));

	file.write((const char*)getData(), m_size);
}

BlockVarRef BlockVar::loadFromFile(std::ifstream& file, uint32_t currOffset)
{
	BlockVarHeader bvh;
	file.read((char*)&bvh, sizeof(BlockVarHeader));

	BlockVarRef pVar = std::make_shared<BlockVar>(bvh.varName, bvh.varSize);
	
	file.read((char*)pVar->getData(), bvh.varSize);

	return pVar;
}

BlockChain::BlockChain(const char* chainStr)
	: BlockChain(std::string(chainStr))
{}

BlockChain::BlockChain(const std::string& chainStr)
	: m_chainStr(chainStr)
{
	m_depth = chainStr.empty() ? 0 : 1;

	for (char c : chainStr)
		if (c == '.')
			++m_depth;
}

uint32_t BlockChain::getDepth() const
{
	return m_depth;
}

std::string BlockChain::operator[](uint32_t index) const
{
	uint32_t begin = 0;
	uint32_t end = 0;
	for (uint32_t i = 0; i < m_chainStr.size(); ++i)
	{
		if (m_chainStr[i] == '.')
		{
			if (index == 0)
			{
				end = i;
				break;
			}
			if (index == 1)
			{
				begin = i + 1;
			}
			--index;
		}
	}

	return m_chainStr.substr(begin, end - begin);
}

BlockChain BlockChain::operator+(const BlockChain& other) const
{
	if (m_chainStr.size() > 0 && other.m_chainStr.size() > 0)
		return m_chainStr + "." + other.m_chainStr;
	return m_chainStr + other.m_chainStr;
}

SaveFile::SaveFile()
	: m_pRootBlock(std::make_shared<Block>())
{
	m_pRootBlock->name = "root";
}

bool SaveFile::hasBlockChain(const BlockChain& blockChain) const
{
	return !!getBlock(blockChain);
}

std::vector<std::string> SaveFile::getSubBlockNames(const BlockChain& blockChain) const
{
	auto pBlock = getBlock(blockChain);
	if (!pBlock)
		return std::vector<std::string>();

	std::vector<std::string> names;
	for (auto& elem : pBlock->subBlocks)
		names.push_back(elem.first);
	return names;
}

std::vector<std::string> SaveFile::getVarNames(const BlockChain& blockChain) const
{
	auto pBlock = getBlock(blockChain);

	if (pBlock)
		return pBlock->getVarNames();

	return std::vector<std::string>();
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
	/*auto pBlock = m_pRootBlock;
	for (uint32_t i = 1; i < blockChain.getDepth(); ++i)
	{
		auto& res = pBlock->subBlocks.find(blockChain[i]);
		if (res == pBlock->subBlocks.end())
			return nullptr;
		pBlock = res->second;
	}
	return pBlock;*/
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

	uint32_t firstBlockOffset = sizeof(SaveFileHeader);

	file.write((const char*)&sfh, sizeof(SaveFileHeader));
	m_pRootBlock->saveToFile(file, firstBlockOffset);
	file.close();
}

std::shared_ptr<SaveFile> SaveFile::loadFromFile(const std::string& filepath)
{
	SaveFileHeader sfh;
	std::ifstream file(filepath, std::ios::in | std::ios::binary);
	if (!file.good())
		return nullptr;

	uint32_t firstBlockOffset = sizeof(SaveFileHeader);

	file.read((char*)&sfh, sizeof(SaveFileHeader));
	DH_ASSERT(!strcmp(sfh.identifier, "DonHugoSF"));

	std::shared_ptr<SaveFile> psf = std::make_shared<SaveFile>();

	psf->m_pRootBlock = Block::loadFromFile(file, firstBlockOffset);

	return psf;
}

Block::Block(const std::string& name)
	: name(name)
{}

std::vector<std::string> Block::getVarNames() const
{
	std::vector<std::string> names;

	for (auto elem : vars)
		names.push_back(elem.second->getName());

	return names;
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

void Block::saveToFile(std::ofstream& file, uint32_t currOffset)
{
	uint32_t subSize = 0;
	for (auto elem : subBlocks)
		subSize += elem.second->getSize();

	BlockHeader bh;

	memcpy(bh.blockName, name.c_str(), name.size() + 1);

	bh.nSubBlocks = (uint32_t)subBlocks.size();
	bh.blockOffset = currOffset + sizeof(BlockHeader) + subSize;
	bh.nVars = (uint32_t)vars.size();

	file.write((const char*)&bh, sizeof(BlockHeader));

	uint32_t subOffset = currOffset + sizeof(BlockHeader);
	for (auto elem : subBlocks)
	{
		elem.second->saveToFile(file, subOffset);
		subOffset += elem.second->getSize();
	}

	for (auto elem : vars)
	{
		elem.second->saveToFile(file, subOffset);
		subOffset += elem.second->getSize();
	}
}

BlockRef Block::loadFromFile(std::ifstream& file, uint32_t currOffset)
{
	BlockRef pBlock = std::make_shared<Block>();

	BlockHeader bh;
	
	file.read((char*)&bh, sizeof(BlockHeader));

	pBlock->name = bh.blockName;

	uint32_t subOffset = currOffset + sizeof(BlockHeader);
	for (uint32_t i = 0; i < bh.nSubBlocks; ++i)
	{
		BlockRef pSubBlock = Block::loadFromFile(file, subOffset);
		pBlock->subBlocks.insert(std::make_pair(pSubBlock->name, pSubBlock));
		subOffset += pSubBlock->getSize();
	}

	for (uint32_t i = 0; i < bh.nVars; ++i)
	{
		BlockVarRef pVar = BlockVar::loadFromFile(file, subOffset);
		pBlock->vars.insert(std::make_pair(pVar->getName(), pVar));
		subOffset += pVar->getSize();
	}

	return pBlock;
}
