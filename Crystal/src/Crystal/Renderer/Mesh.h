#pragma once
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <vector>

#include "Buffers.h"
#include "Material.h"

struct aiNode;
struct aiScene;
struct aiMesh;

namespace Crystal {
	struct Vertex final
	{
		Vertex() = default;
		Vertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT2 texCoord) : Position(position), Normal(normal), TexCoord(texCoord) {}
		DirectX::XMFLOAT3 Position = {};
		DirectX::XMFLOAT2 TexCoord = {};
		DirectX::XMFLOAT3 Normal = {};
		DirectX::XMFLOAT3 Tangent = {};
		DirectX::XMFLOAT3 BiTangent = {};
	};

	/*실질적인 정점 정보를 담고 있는 클래스입니다.*/
	class SubMesh final
	{
	public:
		SubMesh(aiMesh* mesh, const aiScene* scene);
		virtual ~SubMesh() = default;

		void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);

	private:
		std::unique_ptr<VertexBuffer> m_VertexBuffer = nullptr;
		std::unique_ptr<IndexBuffer> m_IndexBuffer = nullptr;
	};

	/*SubMesh들의 컨테이너 입니다.*/
	class Mesh final
	{
	public:
		Mesh(const std::string& filePath);
		virtual ~Mesh() = default;
		void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);
		void SetMaterial(std::shared_ptr<Material> material) { m_Material = std::move(material); }
		Material* GetMaterial() const { return m_Material.get(); }
	private:
		void processNode(aiNode* node, const aiScene* scene);

	private:
		std::vector<std::unique_ptr<SubMesh>> m_Meshes;
		std::shared_ptr<Material> m_Material = nullptr;
	};
}
