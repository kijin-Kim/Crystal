#pragma once
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <vector>
#include <array>

#include "Crystal/Resources/Buffers.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Math/Math.h"
#include "Crystal/Renderer/Renderable.h"

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiAnimation;
struct aiNodeAnim;

namespace Assimp { class Importer; }


namespace Crystal {

	struct StaticVertex
	{
		StaticVertex() = default;
		DirectX::XMFLOAT3 Position = {};
		DirectX::XMFLOAT2 TexCoord = {};
		DirectX::XMFLOAT3 Normal = {};
		DirectX::XMFLOAT3 Tangent = {};
		DirectX::XMFLOAT3 BiTangent = {};
	};

	struct SkeletalVertex
	{
		SkeletalVertex() = default;
		DirectX::XMFLOAT3 Position = {};
		DirectX::XMFLOAT2 TexCoord = {};
		DirectX::XMFLOAT3 Normal = {};
		DirectX::XMFLOAT3 Tangent = {};
		DirectX::XMFLOAT3 BiTangent = {};

		uint32_t IDs[4] = {};
		float Weights[4] = {};

		void AddBoneData(uint32_t boneID, float weight)
		{
			for (unsigned int i = 0; i < 4; i++) 
			{
				if (Weights[i] == 0.0) 
				{
					IDs[i] = boneID;
					Weights[i] = weight;
					return;
				}
			}
			CS_DEBUG_INFO("최대 정점의 영향을 주는 본의 갯수를 초과하였습니다. 갯수를 초과하는 본의 Weight는 삭제 됩니다");
		}
	};

	/*실질적인 정점 정보를 담고 있는 클래스입니다.*/
	struct SubMesh
	{
		SubMesh() = default;
		virtual ~SubMesh() = default;

		std::vector<UINT> Indices;
	};

	struct StaticSubMesh : public SubMesh
	{
		StaticSubMesh(aiMesh* mesh);
		virtual ~StaticSubMesh() = default;

		std::vector<StaticVertex> StaticVertices;
	};

	struct SkeletalSubMesh : public SubMesh
	{
		SkeletalSubMesh(aiMesh* mesh);
		virtual ~SkeletalSubMesh() = default;

		std::vector<SkeletalVertex> SkeletalVertices;
	};

	class Mesh : public Renderable
	{
	public:
		Mesh(const std::string& filePath);
		virtual ~Mesh();

		virtual void Update(float deltaTime) override;
		void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, int submeshIndex);
		void SetMaterial(std::shared_ptr<Material> material, int index = 0) { m_Materials[index] = std::move(material); }
		size_t GetSubmeshCount() const { return m_Submeshes.size(); }
		Material* GetMaterial(int index = 0) const 
		{ 
			if (!m_Materials[index])
				CS_FATAL(false, "%s번째 Material이 존재 하지 않습니다.", index);
			return m_Materials[index].get(); 
		}

		const std::array<std::shared_ptr<Material>, 5>& GetMaterials() const { return m_Materials; }

	protected:
		void processNode(aiNode* rootNode, const aiScene* scene, bool bIsSkeletal = false);

	protected:
		std::vector <std::unique_ptr<SubMesh>> m_Submeshes;
		std::vector <std::unique_ptr<VertexBuffer>> m_VertexBuffers;
		std::vector <std::unique_ptr<IndexBuffer>> m_IndexBuffers;

		std::shared_ptr <Material> m_Material = nullptr;
		std::array<std::shared_ptr<Material>, 5> m_Materials;

		const aiScene* m_MeshScene = nullptr;

	private:
		Assimp::Importer* m_Importer;

	};

	/*SubMesh들의 컨테이너 입니다.*/
	class StaticMesh : public Mesh
	{
	public:
		StaticMesh(const std::string& filePath);
		virtual ~StaticMesh() = default;
	};

	class SkeletalMesh : public Mesh 
	{
	public:
		SkeletalMesh(const std::string& meshFilePath, const std::string& animationFilePath = "");
		virtual ~SkeletalMesh();

		virtual void Update(float deltaTime) override;
		//TEMP//
		const std::vector<DirectX::XMFLOAT4X4>& GetBoneTransforms() const { return m_BoneTransforms; }
	private:
		void boneTransform(float deltaTime);
		void readNodeHierarchy(float animationTime, const aiNode* pNode, const DirectX::XMFLOAT4X4& parentTransform);
		int findNodeAnimIndex(unsigned int animationIndex, const std::string& nodeName);

		DirectX::XMFLOAT3 interpolateScale(float animationTime, aiNodeAnim* nodeAnim);
		DirectX::XMFLOAT4 interpolateRotation(float animationTime, aiNodeAnim* nodeAnim);
		DirectX::XMFLOAT3 interpolateTranslation(float animationTime, aiNodeAnim* nodeAnim);

		uint32_t findScale(float animationTime, aiNodeAnim* nodeAnim);
		uint32_t findRotation(float animationTime, aiNodeAnim* nodeAnim);
		uint32_t findTranslation(float animationTime, aiNodeAnim* nodeAnim);


	private:
		const aiScene* m_AnimationScene = nullptr;

		std::vector <DirectX::XMFLOAT4X4> m_BoneOffsets; // Bone의 Local Transform
		std::vector <DirectX::XMFLOAT4X4> m_BoneTransforms; // 최종적으로 변환된 Bone들의 값 (World Space)
		std::unordered_map <std::string, uint32_t> m_BoneMap; // m_BoneTransforms의 Index를 가지고 있음. [노드의 이름, m_BoneTransforms의 인덱스]
		using AnimationChannelMap = std::map<std::string, uint32_t>;
		std::vector <AnimationChannelMap> m_AnimationChannelMaps;
		DirectX::XMFLOAT4X4 m_InverseGlobalTransform = Matrix4x4::Identity();
		bool m_bMeshHasBones = false;
		bool m_bMeshHasAnimations = false;
		float m_AnimationTime = 0.0f;


		Assimp::Importer* m_AnimationFileImporter = nullptr;


	};
}
