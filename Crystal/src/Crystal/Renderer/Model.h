#pragma once
#include <string>
#include <vector>
#include <memory>

#include <DirectXMath.h>

struct aiNode;
struct aiScene;
struct aiMesh;

namespace Crystal {


	struct Vertex
	{
		Vertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT2 texCoord) : Position(position), Normal(normal), TexCoord(texCoord) {}
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 TexCoord;
	};

	class Mesh
	{
	public:
		Mesh(aiMesh* mesh, const aiScene* scene);
		void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);

	private:
		std::unique_ptr<class VertexBuffer> m_VertexBuffer = nullptr;
		std::unique_ptr<class IndexBuffer> m_IndexBuffer = nullptr;
	};

	class Model 
	{
	public:
		Model(const std::string& filePath);
		void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);

	private:
		void processNode(aiNode* node, const aiScene* scene);

	private:
		std::vector<std::unique_ptr<class Mesh>> m_Meshes;
	};
}
