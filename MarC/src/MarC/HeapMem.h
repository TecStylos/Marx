#pragma once

#include <stdint.h>

#include <vector>
#include <unordered_map>

namespace MarC
{
	typedef uint32_t Address;
	typedef uint32_t Page;

	static constexpr uint32_t s_pageShift = 3 * 4;
	static constexpr Address s_pageMask = 0xFFFFF000;
	static constexpr Address s_partMask = 0x00000FFF;
	static constexpr uint32_t s_pageSize = s_partMask + 1;
	static constexpr uint32_t s_nPages = (s_pageMask | s_partMask) / (s_partMask + 1);

	struct PageBlock
	{
		Page firstPage = 0;
		Page nPages = 0;
	};

	class PageBlockList
	{
		class Container;
	public:
		class Iterator;
	public:
		PageBlockList(Page nPages);
		~PageBlockList();
	public:
		PageBlock popBlock(Page nPages);
		void pushBlock(PageBlock pageBlock);
	public:
		Iterator begin();
		Iterator end();
	public:
		class Iterator
		{
		public:
			Iterator(Container* pCon);
		public:
			Iterator& operator++();
			Iterator operator++(int);
			bool operator!=(const Iterator& other) const;
			Container& operator*();
		private:
			Container* m_pCon;
		};
	private:
		void popContainer(Container* pCon);
		void pushContainer(Container* pPrev, Container* pCon);
	private:
		class Container
		{
			friend PageBlockList;
			Container* pPrevElem = nullptr;
			Container* pNextElem = nullptr;
			PageBlock pageBlock;
		public:
			bool hasPrev() { return getPrev() != nullptr; }
			Container* getPrev() { return pPrevElem; }
			bool hasNext() { return getNext() != nullptr; }
			Container* getNext() { return pNextElem; }
		};
	private:
		Container* m_pBegin = nullptr;
		Container* m_pEnd = nullptr;
	};

	typedef std::unordered_map<Page, Page> AllocationMap;

	class HeapMem
	{
	public:
		HeapMem(uint32_t size);
		~HeapMem();
	private:
		HeapMem(uint32_t size, Page nPages);
	public:
		Address alloc(uint32_t size);
		void free(Address addr);
		void free(Address* addr);
	public:
		void* operator[](Address addr);
	public:
		uint32_t getSize() const;
		uint32_t getNFree() const;
		uint32_t getNAlloc() const;
	public:
		static uint32_t calcNPages(uint32_t nBytes);
		static Page pageFromAddr(Address addr);
		static Address addrFromPage(Page page);
	private:
		void* virtToReal(Address addr);
	private:
		std::vector<char> m_heap;
	private:
		AllocationMap m_allocations;
		PageBlockList m_freePages;
	private:
		uint32_t m_size;
		uint32_t m_nFree;
		uint32_t m_nAlloc;
		Page m_nPages;
	};
}