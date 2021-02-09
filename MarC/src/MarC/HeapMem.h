#pragma once

#include <stdint.h>

namespace MarC
{
	typedef uint32_t Address;

	class HeapMem
	{
	public:
		Address alloc(uint32_t size);
		void free(Address addr);
	public:
		void* realPtr(Address addr);
	};
}