#include "SaveFile.h"

BlockData::BlockData(uint32_t size)
	: m_size(size)
{
	m_pData = malloc(size);
}

BlockData::BlockData(uint32_t size, void* pData)
	: BlockData(size)
{
	memcpy(m_pData, pData, size);
}

BlockData::BlockData(uint32_t size, std::istream& is)
	: BlockData(size)
{
	is.read((char*)m_pData, size);
}

BlockData::~BlockData()
{
	free(m_pData);
}

uint32_t BlockData::getSize() const
{
	return m_size;
}

void* BlockData::getData()
{
	return m_pData;
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

std::vector<std::string> SaveFile::getSubBlockNames(const BlockChain& blockChain)
{
	auto pBlock = getBlock(blockChain);
	if (!pBlock)
		return std::vector<std::string>();

	std::vector<std::string> names;
	for (auto& elem : pBlock->subBlocks)
		names.push_back(elem.first);
	return names;
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

std::shared_ptr<BlockData> SaveFile::getBlockData(const BlockChain& blockChain)
{
	auto pBlock = getBlock(blockChain);
	if (!pBlock)
		return nullptr;
	return pBlock->pBlockData;
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
	DH_ASSERT(file.good());

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

uint32_t Block::getSize() const
{
	uint32_t headerSize = sizeof(BlockHeader);
	uint32_t dataSize = pBlockData ? pBlockData->getSize() : 0;
	uint32_t subSize = 0;
	for (auto elem : subBlocks)
		subSize += elem.second->getSize();

	return headerSize + dataSize + subSize;
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
	bh.blockSize = pBlockData ? pBlockData->getSize() : 0;

	file.write((const char*)&bh, sizeof(BlockHeader));

	uint32_t subOffset = currOffset + sizeof(BlockHeader);
	for (auto elem : subBlocks)
	{
		elem.second->saveToFile(file, subOffset);
		subOffset += elem.second->getSize();
	}

	if (pBlockData)
		file.write((const char*)pBlockData->getData(), pBlockData->getSize());
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

	if (bh.blockSize > 0)
		pBlock->pBlockData = std::make_shared<BlockData>(bh.blockSize, file);

	return pBlock;
}
