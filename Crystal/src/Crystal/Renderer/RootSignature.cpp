#include "cspch.h"
#include "RootSignature.h"

#include "Renderer.h"

namespace Crystal {


	RootSignature::RootSignature(D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc)
	{
		auto device = Renderer::Get().GetDevice();

		HRESULT hr = E_FAIL;
		ID3DBlob* rootSignatureDataBlob = nullptr;
		ID3DBlob* rootSignatureErrorBlob = nullptr;

		hr = D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
		CS_ASSERT(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다");
		hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		CS_ASSERT(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");

		SAFE_RELEASE(rootSignatureDataBlob);
		SAFE_RELEASE(rootSignatureErrorBlob);
	}

}