#include "cspch.h"
#include "Device.h"
#include "Crystal/Renderer/CommandQueue.h"
#include "Crystal/Resources/DescriptorAllocator.h"

namespace Crystal {

	CommandQueue* Device::GetCommandQueue() const
	{
		return m_CommandQueue.get();
	}

	DescriptorAllocation Device::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t descriptorCount)
	{
		return m_DescriptorAllocators[type]->Allocate(descriptorCount);
	}

	void Device::ReleaseStaleDescriptors()
	{
		for (auto& DescriptorAllocator : m_DescriptorAllocators)
		{
			DescriptorAllocator->ReleaseStaleDescriptors();
		}
	}

	Device::Device()
	{
		HRESULT hr = E_FAIL;
		UINT createFactoryDebugFlags = 0;


#if defined CS_DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
		CS_FATAL(SUCCEEDED(hr), "D3D디버그 인터페이스를 가져오는데 실패하였습니다.");
		debugController->EnableDebugLayer();
		createFactoryDebugFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
		// #DirectX Create DXGI factory
		hr = CreateDXGIFactory2(createFactoryDebugFlags, IID_PPV_ARGS(m_Factory.GetAddressOf()));
		CS_FATAL(SUCCEEDED(hr), "DXGI Factory를 생성하는데 실패하였습니다.");

		// #DirectX Create Grahpics Adapter
		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter = nullptr;
		for (int i = 0; m_Factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++)
		{
			if (adapter)
			{
				DXGI_ADAPTER_DESC1 adapterDesc;
				adapter->GetDesc1(&adapterDesc);

				// #DirectX Create Device
				hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device));
				m_Device->SetName(L"D3D12 Device");
				CS_FATAL(SUCCEEDED(hr), "D3D Device를 생성하는데 실패하였습니다.");

				break;
			}
		}

#if defined CS_DEBUG
		Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
		if (SUCCEEDED(m_Device.As(&infoQueue)))
		{
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

			D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };

			D3D12_MESSAGE_ID denyIDs[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE
			};

			D3D12_INFO_QUEUE_FILTER newFilter = {};
			newFilter.DenyList.NumSeverities = _countof(severities);
			newFilter.DenyList.pSeverityList = severities;
			newFilter.DenyList.NumIDs = _countof(denyIDs);
			newFilter.DenyList.pIDList = denyIDs;
			infoQueue->PushStorageFilter(&newFilter);
		}
#endif
		m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
		m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);

		m_CommandQueue = std::make_unique<CommandQueue>(m_Device, D3D12_COMMAND_LIST_TYPE_DIRECT);


		for (int i = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++)
		{
			m_DescriptorAllocators[i]
				= std::make_unique<DescriptorAllocator>(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
		}

	}
	
}


