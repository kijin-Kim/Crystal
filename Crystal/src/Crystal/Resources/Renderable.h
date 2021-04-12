#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "Buffer.h"

namespace Crystal {
	class Renderable
	{
	public:
		Renderable() = default;
		virtual ~Renderable() = default;

		virtual void Update(float deltaTime);
		virtual void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, int submeshIndex, int instanceCount = 1);

		size_t GetVertexbufferCount() const;


	protected:
		std::vector<std::unique_ptr<Buffer>> m_VertexBuffers;
		std::vector<std::unique_ptr<Buffer>> m_IndexBuffers;
	};
}