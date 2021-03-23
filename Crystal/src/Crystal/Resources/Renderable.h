#pragma once
#include <d3d12.h>
#include <wrl\client.h>
#include "Crystal/Resources/Buffers.h"

namespace Crystal {

	class Renderable
	{
	public:
		Renderable() = default;
		virtual ~Renderable() = default;

		virtual void Update(float deltaTime) {}
		virtual void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, int submeshIndex)
		{
			commandList->IASetVertexBuffers(0, 1, &m_VertexBuffers[submeshIndex]->GetView());
			commandList->IASetIndexBuffer(&m_IndexBuffers[submeshIndex]->GetView());
			commandList->DrawIndexedInstanced(m_IndexBuffers[submeshIndex]->GetCount(), 1, 0, 0, 0);
		}

		size_t GetVertexbufferCount() const { return m_VertexBuffers.size(); }

	protected:
		std::vector <std::unique_ptr<VertexBuffer>> m_VertexBuffers;
		std::vector <std::unique_ptr<IndexBuffer>> m_IndexBuffers;
	};

	class Line : public Renderable
	{
	public:
		Line()
		{
			float unitLineVertices[] = {
				0.0f, 1.0f
			};

			int indices[] = {
				0, 1
			};

			m_VertexBuffers.push_back(std::make_unique<VertexBuffer>((void*)unitLineVertices,
				sizeof(float) * _countof(unitLineVertices), _countof(unitLineVertices)));
			m_IndexBuffers.push_back(std::make_unique<IndexBuffer>((void*)indices,
				sizeof(int) * _countof(indices), _countof(indices)));
		}
		~Line() override= default;
		
	};

	class LineQuad : public Renderable
	{
	public:
		LineQuad()
		{
			float quadVertices[] = {
				+0.5f, +0.5f, 0.0f,
				+0.5f, -0.5f, 0.0f,
				-0.5f, -0.5f, 0.0f,
				-0.5f, +0.5f, 0.0f
			};

			uint32_t quadIndices[] = {
				0, 1, 1, 2, 2, 3, 3, 0
			};

			m_VertexBuffers.push_back(std::make_unique<VertexBuffer>((void*)quadVertices,
				(UINT)(sizeof(float) * 3), (UINT)_countof(quadVertices)));
			m_IndexBuffers.push_back(std::make_unique<IndexBuffer>((void*)quadIndices,
				(UINT)(sizeof(uint32_t) * _countof(quadIndices)), (UINT)(_countof(quadIndices))));
		}
		~LineQuad() override = default;
	};



	class LineBox : public Renderable
	{
	public:
		LineBox()
		{
			float quadVertices[] = {
				+0.5f, +0.5f, +0.5f,
				+0.5f, -0.5f, +0.5f,
				-0.5f, -0.5f, +0.5f,
				-0.5f, +0.5f, +0.5f,

				+0.5f, +0.5f, -0.5f,
				+0.5f, -0.5f, -0.5f,
				-0.5f, -0.5f, -0.5f,
				-0.5f, +0.5f, -0.5f,

				-0.5f, +0.5f, +0.5f,
				-0.5f, +0.5f, -0.5f,
				-0.5f, -0.5f, -0.5f,
				-0.5f, -0.5f, +0.5f,

				+0.5f, +0.5f, +0.5f,
				+0.5f, +0.5f, -0.5f,
				+0.5f, -0.5f, -0.5f,
				+0.5f, -0.5f, +0.5f,
			};

			uint32_t quadIndices[] = {
				0, 1, 1, 2, 2, 3, 3, 0,
				4, 5, 5, 6, 6, 7, 7, 4,
				8, 9, 10, 11,
				12, 13, 14, 15
			};

			m_VertexBuffers.push_back(std::make_unique<VertexBuffer>((void*)quadVertices,
				(UINT)(sizeof(float) * 3), (UINT)_countof(quadVertices)));
			m_IndexBuffers.push_back(std::make_unique<IndexBuffer>((void*)quadIndices,
				(UINT)(sizeof(uint32_t) * _countof(quadIndices)), (UINT)(_countof(quadIndices))));
		}
		~LineBox() override = default;

	};

	class LineCircle : public Renderable
	{
	public:
		LineCircle()
		{
			const int circleVerticesCount = 20;
			DirectX::XMFLOAT3 circleVertices[circleVerticesCount];
			for (int i = 0; i < circleVerticesCount; i++)
			{
				float angle = 360.0f / (float)circleVerticesCount * i;
				circleVertices[i].x = cos(DirectX::XMConvertToRadians(angle));
				circleVertices[i].y = sin(DirectX::XMConvertToRadians(angle));
				circleVertices[i].z = 0.0f;
			}

			int circleIndices[_countof(circleVertices) * 2];
			for (int i = 0; i < _countof(circleIndices) - 1; i++)
			{
				static int j = 0;
				circleIndices[i] = j;
				circleIndices[i + 1] = j + 1;
				if (i % 2 == 0)
					j++;
			}
			circleIndices[_countof(circleIndices) - 1] = 0;

			m_VertexBuffers.push_back(std::make_unique<VertexBuffer>((void*)circleVertices,
				(UINT)(sizeof(float) * 3), (UINT)_countof(circleVertices)));
			m_IndexBuffers.push_back(std::make_unique<IndexBuffer>((void*)circleIndices,
				(UINT)(sizeof(uint32_t) * _countof(circleIndices)), (UINT)(_countof(circleIndices))));

		}
		~LineCircle() override = default;
	};

	class LineSphere : public Renderable
	{
	public:
		LineSphere()
		{
			const int circleVerticesCount = 60;
			DirectX::XMFLOAT3 circleVertices[circleVerticesCount];
			for (int i = 0; i < circleVerticesCount / 3; i++)
			{
				float angle = 360.0f / (circleVerticesCount / 3.0f) * i;
				circleVertices[i].x = cos(DirectX::XMConvertToRadians(angle));
				circleVertices[i].y = sin(DirectX::XMConvertToRadians(angle));
				circleVertices[i].z = 0.0f;
			}
			for (int i = circleVerticesCount / 3; i < circleVerticesCount / 3 * 2; i++)
			{
				float angle = 360.0f / (circleVerticesCount / 3.0f) * i;
				circleVertices[i].x = sin(DirectX::XMConvertToRadians(angle));
				circleVertices[i].y = 0.0f;
				circleVertices[i].z = cos(DirectX::XMConvertToRadians(angle));
			}
			for (int i = circleVerticesCount / 3 * 2; i < circleVerticesCount; i++)
			{
				float angle = 360.0f / (circleVerticesCount / 3.0f) * i;
				circleVertices[i].x = 0.0f;
				circleVertices[i].y = cos(DirectX::XMConvertToRadians(angle));
				circleVertices[i].z = sin(DirectX::XMConvertToRadians(angle));
			}


			int circleIndices[_countof(circleVertices) * 2];
			for (int i = 0; i < _countof(circleIndices) - 1; i++)
			{
				static int j = 0;
				circleIndices[i] = j;
				circleIndices[i + 1] = j + 1;
				if (i % 2 == 0)
					j++;
			}
			circleIndices[_countof(circleIndices) / 3 - 1] = 0;
			circleIndices[_countof(circleIndices) / 3 * 2 - 1] = circleIndices[_countof(circleIndices) / 3];
			circleIndices[_countof(circleIndices) - 1] = circleIndices[_countof(circleIndices) / 3 * 2];

			m_VertexBuffers.push_back(std::make_unique<VertexBuffer>((void*)circleVertices,
				(UINT)(sizeof(float) * 3), (UINT)_countof(circleVertices)));
			m_IndexBuffers.push_back(std::make_unique<IndexBuffer>((void*)circleIndices,
				(UINT)(sizeof(uint32_t) * _countof(circleIndices)), (UINT)(_countof(circleIndices))));

		}
		~LineSphere() override = default;
	};


}