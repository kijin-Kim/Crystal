#pragma once
#include <d3d12.h>

namespace Crystal {

	class RootSignature
	{
	public:
		RootSignature(D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc);

		ID3D12RootSignature* GetRaw() const { return m_RootSignature.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;
	};
}
