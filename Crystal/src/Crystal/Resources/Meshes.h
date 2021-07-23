#pragma once
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <vector>
#include <array>

#include "Animation.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Math/Math.h"
#include "Crystal/Resources/Renderable.h"

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiAnimation;
struct aiNodeAnim;

namespace Assimp { class Importer; }

namespace Crystal {
	struct PositionVertex
	{
		PositionVertex() = default;
		DirectX::XMFLOAT3 Position = {};
	};

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
			CS_DEBUG_INFO("�ִ� ������ ������ �ִ� ���� ������ �ʰ��Ͽ����ϴ�. ������ �ʰ��ϴ� ���� Weight�� ���� �˴ϴ�");
		}
	};

	/*�������� ���� ������ ��� �ִ� Ŭ�����Դϴ�.*/
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
		Mesh() = default;
		Mesh(const std::string& filePath);
		virtual ~Mesh();

	private:
		virtual void ProcessNode(aiNode* rootNode, const aiScene* scene) {};

	protected:
		std::vector <Unique<SubMesh>> m_Submeshes;
		const aiScene* m_MeshScene = nullptr;

	private:
		Assimp::Importer* m_Importer = nullptr;
	};

	/*SubMesh���� �����̳� �Դϴ�.*/
	class StaticMesh : public Mesh
	{
	public:
		StaticMesh() = default;
		StaticMesh(const std::string& filePath);
		~StaticMesh() override = default;
	private:
		void ProcessNode(aiNode* rootNode, const aiScene* scene) override;
	};

	

	class SkeletalMesh : public Mesh
	{
	public:
		SkeletalMesh(const std::string& meshFilePath);
		~SkeletalMesh() override = default;

		//TEMP//

		std::vector <DirectX::XMFLOAT4X4>& GetBoneOffset() { return m_BoneOffsets; }
		std::unordered_map <std::string, uint32_t>& GetBoneMap() { return m_BoneMap; }

	private:
		void ProcessNode(aiNode* rootNode, const aiScene* scene) override;

	private:
		std::vector <DirectX::XMFLOAT4X4> m_BoneOffsets; // Bone�� Local Transform
		std::unordered_map <std::string, uint32_t> m_BoneMap; // m_BoneTransforms�� Index�� ������ ����. [����� �̸�, m_BoneTransforms�� �ε���]
	};
}
