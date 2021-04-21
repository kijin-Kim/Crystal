#pragma once
#include "Meshes.h"

namespace Crystal {
	class Line : public StaticMesh
	{
	public:
		Line()
		{
			float unitLineVertices[] = {
				0.0f, 0.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
			};

			int indices[] = {
				0, 1
			};

			m_VertexBuffers.push_back(std::make_unique<Buffer>((void*)unitLineVertices,
				sizeof(float) * 3 * _countof(unitLineVertices), _countof(unitLineVertices), false, false));
			m_IndexBuffers.push_back(std::make_unique<Buffer>((void*)indices,
				sizeof(int) * _countof(indices), _countof(indices), false, false));
		}
		~Line() override = default;
	};

	class LineQuad : public StaticMesh
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

			m_VertexBuffers.push_back(std::make_unique<Buffer>((void*)quadVertices,
				(UINT)(sizeof(float) * 3 * _countof(quadVertices)), (UINT)_countof(quadVertices), false, false));
			m_IndexBuffers.push_back(std::make_unique<Buffer>((void*)quadIndices,
				(UINT)(sizeof(uint32_t) * _countof(quadIndices)), (UINT)(_countof(quadIndices)), false, false));
		}
		~LineQuad() override = default;
	};

	class LineBox : public StaticMesh
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

			m_VertexBuffers.push_back(std::make_unique<Buffer>((void*)quadVertices,
				(UINT)(sizeof(float) * 3 * _countof(quadVertices)), (UINT)_countof(quadVertices), false, false));
			m_IndexBuffers.push_back(std::make_unique<Buffer>((void*)quadIndices,
				(UINT)(sizeof(uint32_t) * _countof(quadIndices)), (UINT)(_countof(quadIndices)), false, false));
		}
		~LineBox() override = default;
	};

	class LineCircle : public StaticMesh
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
			int j = 0;
			for (int i = 0; i < _countof(circleIndices) - 1; i++)
			{
				circleIndices[i] = j;
				circleIndices[i + 1] = j + 1;
				if (i % 2 == 0)
					j++;
			}
			circleIndices[_countof(circleIndices) - 1] = 0;

			m_VertexBuffers.push_back(std::make_unique<Buffer>((void*)circleVertices,
				(UINT)(sizeof(float) * 3 * _countof(circleVertices)), (UINT)_countof(circleVertices), false, false));
			m_IndexBuffers.push_back(std::make_unique<Buffer>((void*)circleIndices,
				(UINT)(sizeof(uint32_t) * _countof(circleIndices)), (UINT)(_countof(circleIndices)), false, false));
		}
		~LineCircle() override = default;
	};

	class LineSphere : public StaticMesh
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
			int j = 0;
			for (int i = 0; i < _countof(circleIndices) - 1; i++)
			{
				circleIndices[i] = j;
				circleIndices[i + 1] = j + 1;
				if (i % 2 == 0)
					j++;
			}
			circleIndices[_countof(circleIndices) / 3 - 1] = 0;
			circleIndices[_countof(circleIndices) / 3 * 2 - 1] = circleIndices[_countof(circleIndices) / 3];
			circleIndices[_countof(circleIndices) - 1] = circleIndices[_countof(circleIndices) / 3 * 2];

			m_VertexBuffers.push_back(std::make_unique<Buffer>((void*)circleVertices,
				(UINT)(sizeof(float) * 3 * _countof(circleVertices)), (UINT)_countof(circleVertices), false, false));
			m_IndexBuffers.push_back(std::make_unique<Buffer>((void*)circleIndices,
				(UINT)(sizeof(uint32_t) * _countof(circleIndices)), (UINT)(_countof(circleIndices)), false, false));
		}
		~LineSphere() override = default;
	};

	class PlaneQuad2D : public StaticMesh
	{
	public:
		PlaneQuad2D()
		{
			float quadVertices[] = {
			-1.0f, -1.0f,
			-1.0f, +1.0f,
			+1.0f, +1.0f,
			+1.0f, -1.0f
			};

			uint32_t quadIndices[] = {
				1, 2, 3,
				1, 3, 0
			};

			m_VertexBuffers.push_back(std::make_unique<Buffer>((void*)quadVertices,
				(UINT)(sizeof(float) * 2 * _countof(quadVertices)), (UINT)_countof(quadVertices), false, false));
			m_IndexBuffers.push_back(std::make_unique<Buffer>((void*)quadIndices,
				(UINT)(sizeof(uint32_t) * _countof(quadIndices)), (UINT)(_countof(quadIndices)), false, false));
		}
		~PlaneQuad2D() override = default;
	};


	class PlaneQuad3D : public StaticMesh
	{
	public:
		PlaneQuad3D()
		{
			float quadVertices[] = {
			-1.0f, -1.0f, +0.0f,
			-1.0f, +1.0f, +0.0f,
			+1.0f, +1.0f, +0.0f,
			+1.0f, -1.0f, +0.0f,
			};

			uint32_t quadIndices[] = {
				1, 2, 3,
				1, 3, 0
			};

			m_VertexBuffers.push_back(std::make_unique<Buffer>((void*)quadVertices,
				(UINT)(sizeof(float) * 3 * _countof(quadVertices)), (UINT)_countof(quadVertices), false, false));
			m_IndexBuffers.push_back(std::make_unique<Buffer>((void*)quadIndices,
				(UINT)(sizeof(uint32_t) * _countof(quadIndices)), (UINT)(_countof(quadIndices)), false, false));
		}
		~PlaneQuad3D() override = default;
	};

	class PlaneQuad3DTextured : public StaticMesh
	{
	public:
		PlaneQuad3DTextured()
		{
			float quadVertices[] = {
			-1.0f, -1.0f, +0.0f, +0.0f, +1.0f,
			-1.0f, +1.0f, +0.0f, +0.0f, +0.0f,
			+1.0f, +1.0f, +0.0f, +1.0f, +0.0f,
			+1.0f, -1.0f, +0.0f, +1.0f, +1.0f
			};

			uint32_t quadIndices[] = {
				1, 2, 3,
				1, 3, 0
			};

			m_VertexBuffers.push_back(std::make_unique<Buffer>((void*)quadVertices,
				(UINT)(sizeof(float) * 5 * _countof(quadVertices)), (UINT)_countof(quadVertices), false, false));
			m_IndexBuffers.push_back(std::make_unique<Buffer>((void*)quadIndices,
				(UINT)(sizeof(uint32_t) * _countof(quadIndices)), (UINT)(_countof(quadIndices)), false, false));
		}
		~PlaneQuad3DTextured() override = default;
	};
	
}
