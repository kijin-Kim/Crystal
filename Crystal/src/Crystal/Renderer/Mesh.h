#pragma once
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <vector>

#include "Buffers.h"
#include "Material.h"
#include "Crystal/Math/Math.h"

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

	struct AnimatedVertex
	{
		AnimatedVertex() = default;
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
			CS_ERROR("최대 정점의 영향을 주는 본의 갯수를 초과하였습니다. 갯수를 초과하는 본의 Weight는 삭제 됩니다");
		}
	};

	/*실질적인 정점 정보를 담고 있는 클래스입니다.*/
	struct SubMesh
	{
		SubMesh(aiMesh* mesh, const aiScene* scene);
		virtual ~SubMesh() = default;
	
		std::vector<StaticVertex> StaticVertices;
		std::vector<AnimatedVertex> AnimatedVertices;
		std::vector<UINT> Indices;
		DirectX::XMFLOAT4X4 InverseBindPoseMatrix = Matrix4x4::Identity();
		DirectX::XMFLOAT4X4 Transform = Matrix4x4::Identity();
	};

	/*SubMesh들의 컨테이너 입니다.*/
	class Mesh final
	{
	public:
		Mesh(const std::string& filePath);
		virtual ~Mesh();
		void Update(float deltaTime);
		void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);
		void SetMaterial(std::shared_ptr<Material> material) { m_Material = std::move(material); }
		Material* GetMaterial() const { return m_Material.get(); }

		
		//TEMP//
		const DirectX::XMFLOAT4X4& GetGlobalTransform() const { return m_InverseGlobalTransform; }
		bool IsAnimated() const { return m_bIsAnimated; }
		const std::vector<DirectX::XMFLOAT4X4>& GetBoneTransfroms() const { return m_BoneTransforms; }
	private:
		void processNode(aiNode* node, const aiScene* scene);
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
		using AnimationChannelMap = std::map<std::string, uint32_t>;
		
		std::vector <std::unique_ptr<SubMesh>> m_Submeshes;
		std::vector <std::unique_ptr<VertexBuffer>> m_VertexBuffers;
		std::vector <std::unique_ptr<IndexBuffer>> m_IndexBuffers;
		std::vector<DirectX::XMFLOAT4X4> m_BoneOffsets;
		std::vector<DirectX::XMFLOAT4X4> m_BoneTransforms;

		std::unordered_map<std::string, uint32_t> m_BoneMap;
		std::vector<AnimationChannelMap> m_AnimationChannelMaps;
		Assimp::Importer* m_Importer;
		std::shared_ptr<Material> m_Material = nullptr;
		DirectX::XMFLOAT4X4 m_InverseGlobalTransform = Matrix4x4::Identity();

		const aiScene* m_Scene = nullptr;
		bool m_bIsAnimated = false;

		float m_AnimationTime = 0.0f;
	};
}
