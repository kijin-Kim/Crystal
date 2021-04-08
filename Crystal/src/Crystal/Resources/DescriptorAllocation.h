#pragma once

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

	class DescriptorAllocatorPage;

	class DescriptorAllocation
	{
	public:
		DescriptorAllocation(); // for null allocation
		DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, uint32_t handleCount, uint32_t descriptorSize,
			std::shared_ptr<DescriptorAllocatorPage> page);
		~DescriptorAllocation();


		// move only class
		DescriptorAllocation(const DescriptorAllocation&) = delete;
		DescriptorAllocation& operator=(const DescriptorAllocation&) = delete;
		
		DescriptorAllocation(DescriptorAllocation&& allocation);
		DescriptorAllocation& operator=(DescriptorAllocation&& other);


		bool IsNull() const;

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(uint32_t offset = 0) const;
		uint32_t GetDescriptorHandleCount() const;

		std::shared_ptr<DescriptorAllocatorPage> GetDescriptorAllocatorPage() const;

	private:
		void Free();

		D3D12_CPU_DESCRIPTOR_HANDLE m_Descriptor;
		uint32_t m_HandleCount;
		uint32_t m_DescriptorSize;

		std::shared_ptr<DescriptorAllocatorPage> m_Page;
	};
}
