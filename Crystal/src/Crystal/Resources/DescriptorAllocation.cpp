#include "cspch.h"
#include "DescriptorAllocation.h"
#include "DescriptorAllocator.h"
#include "DescriptorAllocatorPage.h"

namespace Crystal {


	DescriptorAllocation::DescriptorAllocation()
		: m_Descriptor({ 0 }),
		m_HandleCount(0),
		m_DescriptorSize(0),
		m_Page(nullptr)
	{
	}

	DescriptorAllocation::DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, uint32_t handleCount, uint32_t descriptorSize, std::shared_ptr<DescriptorAllocatorPage> page)
		: m_Descriptor(descriptor),
		m_HandleCount(handleCount),
		m_DescriptorSize(descriptorSize),
		m_Page(page)
	{
	}

	DescriptorAllocation::DescriptorAllocation(DescriptorAllocation&& allocation)
		: m_Descriptor(allocation.m_Descriptor),
		m_HandleCount(allocation.m_HandleCount),
		m_DescriptorSize(allocation.m_DescriptorSize),
		m_Page(std::move(allocation.m_Page))
	{
		allocation.m_Descriptor.ptr = 0;
		allocation.m_HandleCount = 0;
		allocation.m_DescriptorSize = 0;
	}

	DescriptorAllocation& DescriptorAllocation::operator=(DescriptorAllocation&& other)
	{
		m_Descriptor = other.m_Descriptor;
		m_HandleCount = other.m_HandleCount;
		m_DescriptorSize = other.m_DescriptorSize;
		m_Page = std::move(other.m_Page);

		other.m_Descriptor.ptr = 0;
		other.m_HandleCount = 0;
		other.m_DescriptorSize = 0;

		return *this;
	}

	DescriptorAllocation::~DescriptorAllocation()
	{
		Free();
	}

	bool DescriptorAllocation::IsNull() const
	{
		return m_Descriptor.ptr == 0;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetDescriptorHandle(uint32_t offset /*= 0*/) const
	{
		CS_FATAL(offset < m_HandleCount, "offset이 HandleCount보다 큽니다");
		return { m_Descriptor.ptr + (m_DescriptorSize * offset) };
	}

	uint32_t DescriptorAllocation::GetDescriptorHandleCount() const
	{
		return m_HandleCount;
	}

	std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocation::GetDescriptorAllocatorPage() const
	{
		return m_Page;
	}

	void DescriptorAllocation::Free()
	{
		if (!IsNull() && m_Page)
		{
			m_Page->Free(std::move(*this));

			m_Descriptor.ptr = 0;
			m_HandleCount = 0;
			m_DescriptorSize = 0;
			m_Page.reset();
		}
	}



	

}




