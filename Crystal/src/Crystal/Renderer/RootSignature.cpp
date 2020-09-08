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
		CS_ASSERT(SUCCEEDED(hr), "Root Signature�� �ø���ȭ�ϴµ� �����Ͽ����ϴ�");
		hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		CS_ASSERT(SUCCEEDED(hr), "Root Signature�� �����ϴµ� �����Ͽ����ϴ�");

		SAFE_RELEASE(rootSignatureDataBlob);
		SAFE_RELEASE(rootSignatureErrorBlob);
	}

}