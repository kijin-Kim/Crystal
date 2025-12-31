#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "Buffer.h"
#include "Crystal/Core/Core.h"

namespace Crystal {
	class Renderable
	{
	public:
		Renderable() = default;
		virtual ~Renderable() = default;

		virtual void Update(float deltaTime);
		void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, int submeshIndex, int instanceCount = 1);

		size_t GetVertexbufferCount() const;

		const std::vector<Shared<Buffer>>& GetVertexBuffers() { return m_VertexBuffers; }
		const std::vector<Shared<Buffer>>& GetIndexBuffers() { return m_IndexBuffers; }


	protected:
		std::vector<Shared<Buffer>> m_VertexBuffers;
		std::vector<Shared<Buffer>> m_IndexBuffers;
	};
}


