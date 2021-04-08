#include "cspch.h"
#include "DescriptorAllocator.h"
#include "Crystal/Renderer/Renderer.h"
#include "DescriptorAllocation.h"

namespace Crystal {


	DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t maxDescriptorCountPerHeap /*= 256*/)
		: m_DescriptorHeapType(heapType), 
		m_MaxDescriptorCountPerHeap(maxDescriptorCountPerHeap)
	{
	}

	DescriptorAllocation DescriptorAllocator::Allocate(uint32_t descriptorCount)
	{
		DescriptorAllocation allocation;
		
		for (auto it = m_FreeHeaps.begin(); it != m_FreeHeaps.end(); it++)
		{
			auto allocatorPage = m_HeapPool[*it];


			allocation = allocatorPage->Allocate(descriptorCount); // null allocation return if not available

			if (!allocatorPage->GetFreeHandleCount())
			{
				it = m_FreeHeaps.erase(it);
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

	DescriptorAllocatorPage::DescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t descriptorCount)
		: m_DescriptorHeapType(type),
		m_DescriptorCountInHeap(descriptorCount)
	{
		auto device = Renderer::Instance().GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = m_DescriptorHeapType;
		heapDesc.NumDescriptors = m_DescriptorCountInHeap;

		HRESULT hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_D3d12DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "Descriptor Heap을 생성하는데 실패하였습니다");

		m_BaseDescriptor = m_D3d12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_DescriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_FreeHandleCount = m_DescriptorCountInHeap;

		AddNewBlock(0, m_FreeHandleCount); // Add block in offset 0
	}

	DescriptorAllocation DescriptorAllocatorPage::Allocate(uint32_t descriptorCount)
	{
		if (descriptorCount > m_FreeHandleCount)
		{
			return DescriptorAllocation(); // Failed Null Allocation
		}

		auto minBlockIt = m_FreeListBySize.lower_bound(descriptorCount); // Find first meet free-list can afford descriptor count
		if (minBlockIt == m_FreeListBySize.end())
		{
			return DescriptorAllocation();  // Failed Null Allocation
		}

		auto blockSize = minBlockIt->first;
		auto offsetIt = minBlockIt->second;
		auto offset = offsetIt->first;

		// Erase
		m_FreeListBySize.erase(minBlockIt);
		m_FreeListByOffset.erase(offsetIt);

		auto newOffset = offset + descriptorCount;
		auto newSize = blockSize - descriptorCount;

		// Re-insert if left size is not zero
		if (newSize > 0)
		{
			AddNewBlock(newOffset, newSize);
		}

		m_FreeHandleCount -= descriptorCount;

		return { CD3DX12_CPU_DESCRIPTOR_HANDLE(m_BaseDescriptor, offset, m_DescriptorHandleIncrementSize),
		descriptorCount, m_DescriptorHandleIncrementSize, shared_from_this() };
	}

	void DescriptorAllocatorPage::Free(DescriptorAllocation&& descriptor)
	{
		auto offset = ComputeOffset(descriptor.GetDescriptorHandle());
		m_StaleDescriptors.emplace(offset, descriptor.GetDescriptorHandleCount());
	}

	void DescriptorAllocatorPage::ReleaseStaleDescriptors()
	{
		while (!m_StaleDescriptors.empty())
		{
			auto& staleDescriptor = m_StaleDescriptors.front();
			auto offset = staleDescriptor.Offset;
			auto descriptorCount = staleDescriptor.Size;

			FreeBlock(offset, descriptorCount);

			m_StaleDescriptors.pop();
		}
	}

	uint32_t DescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		// increment to handle
		return (handle.ptr - m_BaseDescriptor.ptr) / m_DescriptorHandleIncrementSize;
	}

	void DescriptorAllocatorPage::AddNewBlock(uint32_t offset, uint32_t descriptorCount)
	{
		// insert in bimap
		auto offsetIt = m_FreeListByOffset.emplace(offset, descriptorCount);
		auto sizeIt = m_FreeListBySize.emplace(descriptorCount, offsetIt.first);
		offsetIt.first->second.FreeListBySizeIt = sizeIt;
	}

	void DescriptorAllocatorPage::FreeBlock(uint32_t offset, uint32_t descriptorCount)
	{
		auto nextBlockIt = m_FreeListByOffset.upper_bound(offset);
		auto previousBlockIt = nextBlockIt;
		if (previousBlockIt != m_FreeListByOffset.begin())
		{
			--previousBlockIt;
		}
		else
		{
			previousBlockIt = m_FreeListByOffset.end();
		}

		m_FreeHandleCount += descriptorCount;

		if (previousBlockIt != m_FreeListByOffset.end() 
			&& offset == previousBlockIt->first + previousBlockIt->second.Size) // Previous block is freed
		{
			offset = previousBlockIt->first;
			descriptorCount += previousBlockIt->second.Size;

			// Remove previous block
			m_FreeListBySize.erase(previousBlockIt->second.FreeListBySizeIt);
			m_FreeListByOffset.erase(previousBlockIt);
		}

		if (nextBlockIt != m_FreeListByOffset.end() &&
			offset + descriptorCount == nextBlockIt->first) // Nex block is freed
		{
			descriptorCount += nextBlockIt->second.Size;

			// Remove next block
			m_FreeListBySize.erase(nextBlockIt->second.FreeListBySizeIt);
			m_FreeListByOffset.erase(nextBlockIt);
		}

		// Add new merged block
		AddNewBlock(offset, descriptorCount);

	}

}

