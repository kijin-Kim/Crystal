#pragma once
#include <memory>
#include <d3d12.h>
#include <map>
#include <queue>
#include <wrl/client.h>
#include "DirectXTex/d3dx12.h"

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

	class DescriptorAllocatorPage : public std::enable_shared_from_this<DescriptorAllocatorPage>
	{
	public:
		DescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t descriptorCount);

		D3D12_DESCRIPTOR_HEAP_TYPE GetDescriptorHeapType() const { return m_DescriptorHeapType; }

		bool HasSpace(uint32_t descriptorCount) // descriptor count ��ŭ�� space�� �ִ��� üũ�մϴ�.
		{
			return m_FreeListBySize.lower_bound(descriptorCount) != m_FreeListBySize.end();
		}

		uint32_t GetFreeHandleCount() const { return m_FreeHandleCount; }

		/*
		* ���� descriptor heap�� descriptorCount��ŭ�� descriptor�� �Ҵ�
		* descriptorCount��ŭ �Ҵ��� �� �� ������ null DescriptorAllocation�� ����
		* Free Handle Count���� descriptorCount�� �۾Ƶ� ����ȭ�� ���� ���� �� �� ����.
		*/
		DescriptorAllocation Allocate(uint32_t descriptorCount);

		/*
		* Descriptor�� heap�� �ٽ� �����ݴϴ�.
		* Stale Allocation�� ���������� �������� �ʰ� �ٸ� �����̳ʷ� �Ű����� ���� �˴ϴ�.
		* r-value reference�� DescriptorAllocation�� �ޱ� ������ Free �Լ� ���Ŀ�
		* DescriptorAllocation�� empty�� �˴ϴ�.
		*/
		void Free(DescriptorAllocation&& descriptor);

		void ReleaseStaleDescriptors();

	protected:
		uint32_t ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle);

		void AddNewBlock(uint32_t offset, uint32_t descriptorCount);

		/*
		* Block�� Free�մϴ�.
		* Free�� Block�� Free list���� merge�Ǿ� ū Block�� ����ϴ�.
		*/
		void FreeBlock(uint32_t offset, uint32_t descriptorCount);

	private:
		using OffsetType = uint32_t;
		using SizeType = uint32_t;

		struct FreeBlockInfo;

		// Bimap
		using FreeListByOffset = std::map<OffsetType, FreeBlockInfo>;
		using FreeListBySize = std::multimap<SizeType, FreeListByOffset::iterator>;

		/*
		* FreeListByOffset�� ������Ʈ.
		*/
		struct FreeBlockInfo
		{
			FreeBlockInfo(SizeType size) : Size(size)
			{
			}

			SizeType Size;
			FreeListBySize::iterator FreeListBySizeIt; // Merging�ϱ� ���� 
		};

		struct StaleDescriptorInfo
		{
			StaleDescriptorInfo(OffsetType offset, SizeType size)
				: Offset(offset),
				Size(size)
			{
			}


			OffsetType Offset;
			SizeType Size;
		};

		using StaleDescriptorQueue = std::queue<StaleDescriptorInfo>;

		FreeListByOffset m_FreeListByOffset;
		FreeListBySize m_FreeListBySize;
		StaleDescriptorQueue m_StaleDescriptors;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_D3d12DescriptorHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE m_DescriptorHeapType;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_BaseDescriptor;

		uint32_t m_DescriptorHandleIncrementSize;
		uint32_t m_DescriptorCountInHeap;
		uint32_t m_FreeHandleCount;


	};

}