#include "mcpch.h"
#include "HeapMem.h"

namespace MarC
{
	HeapMem::HeapMem(uint32_t size)
		: HeapMem(size, calcNPages(size))
	{
	}
	HeapMem::~HeapMem()
	{
		if (!m_allocations.empty())
		{
			std::cout << "Detected memory leak(s)!:" << std::endl;

			uint32_t nBytes = 0;

			std::ios_base::fmtflags fmtFlags(std::cout.flags());

			for (auto a : m_allocations)
			{
				uint32_t firstPage = a.first;
				uint32_t lastPage = firstPage + a.second - 1;
				uint32_t firstAddr = addrFromPage(firstPage);
				uint32_t lastAddr = firstAddr + a.second * s_pageSize - 1;

				std::cout << 
					"  0x" << std::setfill('0') << std::setw(8) << std::right << std::hex <<
					firstPage << " - 0x" << std::setfill('0') << std::setw(8) << std::right << std::hex <<
					lastPage << "  (0x" << std::setfill('0') << std::setw(8) << std::right << std::hex <<
					firstAddr << " - 0x" << std::setfill('0') << std::setw(8) << std::right << std::hex <<
					lastAddr << ")" << std::endl;
				std::cout << std::dec;
			}

			std::cout.flags(fmtFlags);
		}
		else
		{
			std::cout << "No memory leaks detected!" << std::endl;
		}
	}
	HeapMem::HeapMem(uint32_t size, Page nPages)
		: m_size(nPages * s_pageSize), m_nPages(nPages), m_nFree(nPages * s_pageSize), m_nAlloc(0), m_freePages(nPages)
	{
		m_heap.resize(m_size);
	}
	Address HeapMem::alloc(uint32_t size)
	{
		Page nPages = calcNPages(size);
		PageBlock pb = m_freePages.popBlock(nPages);

		if (pb.nPages == 0)
			return 0;

		uint32_t nBytes = pb.nPages * s_pageSize;
		m_nFree -= nBytes;
		m_nAlloc += nBytes;

		m_allocations.insert(std::make_pair(pb.firstPage, pb.nPages));

		return addrFromPage(pb.firstPage);
	}
	void HeapMem::free(Address addr)
	{
		auto it = m_allocations.find(pageFromAddr(addr));
		PageBlock pb{ it->first, it->second };
		m_allocations.erase(it);
		m_freePages.pushBlock(pb);
	}
	void HeapMem::free(Address* addr)
	{
		free(*addr);
		*addr = 0;
	}
	void* HeapMem::operator[](Address addr)
	{
		return virtToReal(addr);
	}
	uint32_t HeapMem::getSize() const
	{
		return m_size;
	}
	uint32_t HeapMem::getNFree() const
	{
		return m_nFree;
	}
	uint32_t HeapMem::getNAlloc() const
	{
		return m_nAlloc;
	}
	uint32_t HeapMem::calcNPages(uint32_t nBytes)
	{
		return (nBytes + s_partMask) / s_pageSize;
	}
	Page HeapMem::pageFromAddr(Address addr)
	{
		return ((addr - 1) & s_pageMask) >> s_pageShift;
	}
	Address HeapMem::addrFromPage(Page page)
	{
		return (page << s_pageShift) + 1;
	}
	void* HeapMem::virtToReal(Address addr)
	{
		uint32_t rAddr = addr - 1;
		return m_heap.data() + rAddr;
	}
	PageBlockList::PageBlockList(Page nPages)
	{
		m_pBegin = new Container();
		m_pEnd = new Container();
		Container* pTemp = new Container();

		m_pBegin->pNextElem = m_pEnd;
		m_pEnd->pPrevElem = m_pBegin;
		
		pTemp->pageBlock.firstPage = 0;
		pTemp->pageBlock.nPages = nPages;

		pushContainer(m_pBegin, pTemp);
	}
	PageBlockList::~PageBlockList()
	{
		while (m_pBegin)
		{
			Container* temp = m_pBegin->getNext();
			delete m_pBegin;
			m_pBegin = temp;
		}
	}
	PageBlock PageBlockList::popBlock(Page nPages)
	{
		Container* pMostFitting = nullptr;
		for (auto& pCurr : *this)
		{
			if (nPages <= pCurr.pageBlock.nPages && (!pMostFitting ||
				pCurr.pageBlock.nPages < pMostFitting->pageBlock.nPages)
				)
			{
				pMostFitting = &pCurr;
				if (nPages == pCurr.pageBlock.nPages)
					break;
			}
		}

		if (!pMostFitting)
			return PageBlock();

		PageBlock pb;
		pb.firstPage = pMostFitting->pageBlock.firstPage;
		pb.nPages = nPages;

		if (nPages == pMostFitting->pageBlock.nPages)
		{
			popContainer(pMostFitting);
		}
		else
		{
			pMostFitting->pageBlock.firstPage += nPages;
			pMostFitting->pageBlock.nPages -= nPages;
		}

		return pb;
	}
	void PageBlockList::pushBlock(PageBlock pageBlock)
	{
		Container* pFit = nullptr;
		for (auto& pCurr : *this)
		{
			if (pageBlock.firstPage > pCurr.pageBlock.firstPage)
			{
				pFit = &pCurr;
				break;
			}
		}

		Container* temp = new Container();
		temp->pageBlock = pageBlock;
		pushContainer(pFit ? pFit : m_pBegin, temp);
	}
	PageBlockList::Iterator PageBlockList::begin()
	{
		return m_pBegin->getNext();
	}
	PageBlockList::Iterator PageBlockList::end()
	{
		return m_pEnd;
	}
	void PageBlockList::popContainer(Container* pCon)
	{
		pCon->getPrev()->pNextElem = pCon->getNext();
		pCon->getNext()->pPrevElem = pCon->getPrev();

		delete pCon;
	}
	void PageBlockList::pushContainer(Container* pPrev, Container* pCon)
	{
		pCon->pPrevElem = pPrev;
		pCon->pNextElem = pPrev->getNext();
		pCon->pPrevElem->pNextElem = pCon;
		pCon->pNextElem->pPrevElem = pCon;

		if (pCon->pPrevElem != m_pBegin)
		{
			if (pCon->pPrevElem->pageBlock.firstPage + pCon->pPrevElem->pageBlock.nPages == pCon->pageBlock.firstPage)
			{
				pCon->pageBlock.firstPage = pCon->pPrevElem->pageBlock.firstPage;
				pCon->pageBlock.nPages += pCon->pPrevElem->pageBlock.nPages;
				popContainer(pCon->pPrevElem);
			}
		}
		if (pCon->pNextElem != m_pEnd)
		{
			if (pCon->pageBlock.firstPage + pCon->pageBlock.nPages == pCon->pNextElem->pageBlock.firstPage)
			{
				pCon->pageBlock.nPages += pCon->pNextElem->pageBlock.nPages;
				popContainer(pCon->pNextElem);
			}
		}
	}
	PageBlockList::Iterator::Iterator(Container* pCon)
		: m_pCon(pCon)
	{}
	PageBlockList::Iterator& PageBlockList::Iterator::operator++()
	{
		m_pCon = m_pCon->getNext();
		return *this;
	}
	PageBlockList::Iterator PageBlockList::Iterator::operator++(int)
	{
		Iterator temp = *this;
		++(*this);
		return temp;
	}
	bool PageBlockList::Iterator::operator!=(const Iterator& other) const
	{
		return m_pCon != other.m_pCon;
	}
	PageBlockList::Container& PageBlockList::Iterator::operator*()
	{
		return *m_pCon;
	}
}