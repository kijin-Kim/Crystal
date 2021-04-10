#pragma once
#include <d3d12.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <set>
#include "DescriptorAllocatorPage.h"

// This is based on tutorial from https://www.3dgep.com/learning-directx-12-3/

/*
 *  Copyright(c) 2018 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */


namespace Crystal {

	class DescriptorAllocation;
	
	class DescriptorAllocator
	{
	public:
		DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t maxDescriptorCountPerHeap = 256);
		virtual ~DescriptorAllocator() = default;

		DescriptorAllocation Allocate(uint32_t descriptorCount);

		void ReleaseStaleDescriptors();

	private:
		std::shared_ptr<DescriptorAllocatorPage> CreateDescriptorAllocatorPage();

	private:
		using DescriptorHeapPool = std::vector<std::shared_ptr<DescriptorAllocatorPage>>;
		DescriptorHeapPool m_HeapPool;
		
		D3D12_DESCRIPTOR_HEAP_TYPE m_DescriptorHeapType;
		uint32_t m_MaxDescriptorCountPerHeap;

		std::set<size_t> m_FreeHeaps; // indices of free heap
	};


}
