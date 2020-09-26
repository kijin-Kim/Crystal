#pragma once
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <vector>

#include "Buffers.h"

struct aiNode;
struct aiScene;
struct aiMesh;

namespace Crystal {

	class Drawable 
	{
	public:
		Drawable() = default;
		virtual ~Drawable() = default;

		virtual void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) = 0;
	};

	struct Vertex final
	{
		Vertex() = default;
		Vertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT2 texCoord) : Position(position), Normal(normal), TexCoord(texCoord) {}
		DirectX::XMFLOAT3 Position = {};
		DirectX::XMFLOAT3 Normal = {};
		DirectX::XMFLOAT2 TexCoord = {};
	};

	class Mesh final : public Drawable
	{
	public:
		Mesh(aiMesh* mesh, const aiScene* scene);
		virtual ~Mesh() = default;

		virtual void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;

	private:
		std::unique_ptr<VertexBuffer> m_VertexBuffer = nullptr;
		std::unique_ptr<IndexBuffer> m_IndexBuffer = nullptr;
	};

	class Model final : public Drawable
	{
	public:
		Model(const std::string& filePath);
		virtual ~Model() = default;
		virtual void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;

	private:
		void processNode(aiNode* node, const aiScene* scene);

	private:
		std::vector<std::unique_ptr<Mesh>> m_Meshes;
	};
}
