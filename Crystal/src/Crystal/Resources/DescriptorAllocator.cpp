#include "cspch.h"
#include "DescriptorAllocator.h"
#include "Crystal/Renderer/Renderer.h"
#include "DescriptorAllocation.h"
#include "DescriptorAllocatorPage.h"

namespace Crystal {


	DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t maxDescriptorCountPerHeap /*= 256*/)
		: m_DescriptorHeapType(heapType), 
		m_MaxDescriptorCountPerHeap(maxDescriptorCountPerHeap)
	{
	}

	DescriptorAllocation DescriptorAllocator::Allocate(uint32_t descriptorCount)
	{
		DescriptorAllocation allocation;
		
		auto it = m_FreeHeaps.begin();

		while (it != m_FreeHeaps.end())
		{
			auto allocatorPage = m_HeapPool[*it];

			allocation = allocatorPage->Allocate(descriptorCount); // null allocation return if not available

			if (!allocatorPage->GetFreeHandleCount())
			{
				it = m_FreeHeaps.erase(it);
			}
			else
			{
				++it;
			}

			if (!allocation.IsNull())
			{
				break;
			}
		}


		if (allocation.IsNull()) // 현재 Page에 descriptorCount를 충족할 만한 Page가 없으면,
		{
			m_MaxDescriptorCountPerHeap = max(m_MaxDescriptorCountPerHeap, descriptorCount);

			auto newPage = CreateDescriptorAllocatorPage();
			allocation = newPage->Allocate(descriptorCount);
			
		}
		return allocation;

	}

	void DescriptorAllocator::ReleaseStaleDescriptors()
	{
		for (int i = 0; i < m_HeapPool.size(); i++)
		{
			auto page = m_HeapPool[i];
 			page->ReleaseStaleDescriptors();

			if (page->GetFreeHandleCount() > 0) // Stale Descriptor를 삭제하고 남은 Descriptor Count을 다시 insert한다. 
			{
				m_FreeHeaps.insert(i);
			}

		}
	}

	std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocator::CreateDescriptorAllocatorPage()
	{
		auto page = std::make_shared<DescriptorAllocatorPage>(m_DescriptorHeapType, m_MaxDescriptorCountPerHeap);

		m_HeapPool.emplace_back(page);
		m_FreeHeaps.insert(m_HeapPool.size() - 1);

		return page;
	}
}

