#include "cspch.h"
#include "Meshes.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/Vertex.h>
#include <assimp/postprocess.h>

namespace Crystal {
	DirectX::XMFLOAT4X4 ToDirectXMathMatrix4x4(const aiMatrix4x4& matrix)
	{
		return Matrix4x4::Transpose(*(DirectX::XMFLOAT4X4*)&matrix);
	}

	StaticSubMesh::StaticSubMesh(aiMesh* mesh)
	{
		StaticVertices.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			StaticVertex vertex = {};
			vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

			if (mesh->HasNormals())
			{
				vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			}

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
				vertex.BiTangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			}

			if (mesh->HasTextureCoords(0))
			{
				vertex.TexCoord = { mesh->mTextureCoords[0][i][0], mesh->mTextureCoords[0][i][1] };
			}
			StaticVertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			Indices.reserve(mesh->mFaces[i].mNumIndices);
			for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			{
				Indices.push_back(mesh->mFaces[i].mIndices[j]);
			}
		}
	}

	SkeletalSubMesh::SkeletalSubMesh(aiMesh* mesh)
	{
		SkeletalVertices.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			SkeletalVertex vertex = {};
			vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

			if (mesh->HasNormals())
			{
				vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			}

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
				vertex.BiTangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			}

			if (mesh->HasTextureCoords(0))
			{
				vertex.TexCoord = { mesh->mTextureCoords[0][i][0], mesh->mTextureCoords[0][i][1] };
			}
			SkeletalVertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			Indices.reserve(mesh->mFaces[i].mNumIndices);
			for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			{
				Indices.push_back(mesh->mFaces[i].mIndices[j]);
			}
		}
	}

	Mesh::Mesh(const std::string& filePath)
	{
		m_Importer = new Assimp::Importer();
		m_MeshScene = m_Importer->ReadFile(filePath,
			aiProcess_ConvertToLeftHanded |
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_OptimizeMeshes |
			aiProcess_ValidateDataStructure);
		CS_FATAL(!(!m_MeshScene || m_MeshScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_MeshScene->mRootNode),
			"%s 메쉬를 로드하는데 실패하였습니다", filePath.c_str());
	}

	Mesh::~Mesh()
	{
		if (m_Importer)
			delete m_Importer;
	}

	StaticMesh::StaticMesh(const std::string& filePath) : Mesh(filePath)
	{
		CS_INFO("%s 메쉬 불러오는 중 ...", filePath.c_str());
		ProcessNode(m_MeshScene->mRootNode, m_MeshScene);
		for (const auto& submesh : m_Submeshes)
		{
			m_VertexBuffers.push_back(std::make_unique<VertexBuffer>(((StaticSubMesh*)(submesh.get()))->StaticVertices.data(),
				(UINT)sizeof(StaticVertex),
				(UINT)((StaticSubMesh*)(submesh.get()))->StaticVertices.size()));
			m_IndexBuffers.push_back(std::make_unique<IndexBuffer>(submesh->Indices.data(),
				(UINT)(sizeof(UINT) * submesh->Indices.size()), (UINT)submesh->Indices.size()));
		}

		CS_INFO("%s 메쉬 불러오기 완료", filePath.c_str());
	}

	void StaticMesh::ProcessNode(aiNode* rootNode, const aiScene* scene)
	{
		for (unsigned int i = 0; i < rootNode->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[rootNode->mMeshes[i]];
			m_Submeshes.push_back(std::make_unique<StaticSubMesh>(mesh));
		}

		for (unsigned int i = 0; i < rootNode->mNumChildren; i++)
		{
			ProcessNode(rootNode->mChildren[i], scene);
		}
	}

	SkeletalMesh::SkeletalMesh(const std::string& meshFilePath, const std::string& animationFilePath /*= ""*/) : Mesh(meshFilePath)
	{
		/*만약 animation file의 path가 따로 전달 되었다면*/
		CS_INFO("%s 스켈레탈 메쉬 불러오는 중 ...", meshFilePath.c_str());
		ProcessNode(m_MeshScene->mRootNode, m_MeshScene);
		/*Skeletal Mesh를 로드합니다.*/
		m_bMeshHasBones = m_MeshScene->HasMeshes() && m_MeshScene->mMeshes[0]->HasBones();
		m_bMeshHasAnimations = m_MeshScene->HasAnimations();

		bool bHasSeperateFileForAnimation = !animationFilePath.empty();
		if (bHasSeperateFileForAnimation)
		{
			m_AnimationFileImporter = new Assimp::Importer();

			m_AnimationScene = m_AnimationFileImporter->ReadFile(animationFilePath,
				aiProcess_ConvertToLeftHanded);

			CS_FATAL(!(!m_AnimationScene || !m_AnimationScene->mRootNode),
				"%s 애니메이션 데이터를 로드하는데 실패하였습니다", animationFilePath.c_str());
		}
		else if (m_bMeshHasAnimations) // 별도의 Animation File Path가 전달되지 않았고, 애니메이션 데이터가 Mesh File에 존재하면
		{
			m_AnimationScene = m_MeshScene;
		}
		else
		{
			CS_FATAL(false, "Skeletal Mesh : %s는 본의 데이터 또는 애니메이션의 데이터가 전달 되지 않았습니다.", meshFilePath.c_str());
		}

		m_AnimationChannelMaps.resize(m_AnimationScene->mNumAnimations);
		for (unsigned int i = 0; i < m_AnimationScene->mNumAnimations; i++)
		{
			auto animation = m_AnimationScene->mAnimations[i];
			for (unsigned int j = 0; j < animation->mNumChannels; j++)
			{
				auto animationChannel = animation->mChannels[j];
				m_AnimationChannelMaps[i][animationChannel->mNodeName.C_Str()] = j;
			}
		}

		// Bones

		m_InverseGlobalTransform = Matrix4x4::Inverse(ToDirectXMathMatrix4x4(m_MeshScene->mRootNode->mTransformation));

		if (m_bMeshHasBones)
		{
			m_BoneTransforms.resize(m_MeshScene->mMeshes[0]->mNumBones);
			m_BoneOffsets.resize(m_MeshScene->mMeshes[0]->mNumBones);
			for (unsigned int m = 0; m < m_MeshScene->mNumMeshes; m++)
			{
				aiMesh* mesh = m_MeshScene->mMeshes[m];
				for (unsigned int i = 0; i < mesh->mNumBones; i++)
				{
					aiBone* bone = mesh->mBones[i];
					std::string boneName = bone->mName.C_Str();
					int boneIndex = i;
					if (m_BoneMap.find(boneName) == m_BoneMap.end()) // 처음 등장한 Bone이면
					{
						m_BoneOffsets[boneIndex] = ToDirectXMathMatrix4x4(bone->mOffsetMatrix);
						m_BoneMap[boneName] = boneIndex;
					}
					else // 기존에 Bonemap에 있었으면
					{
						boneIndex = m_BoneMap[boneName]; // 현재 등장한 Bone으로 대체
					}

					for (unsigned int j = 0; j < bone->mNumWeights; j++)
					{
						int vertexID = bone->mWeights[j].mVertexId;
						float weight = bone->mWeights[j].mWeight;
						((SkeletalSubMesh*)(m_Submeshes[m].get()))->SkeletalVertices[vertexID].AddBoneData(boneIndex, weight);
					}
				}
			}
		}

		for (const auto& submesh : m_Submeshes)
		{
			m_VertexBuffers.push_back(std::make_unique<VertexBuffer>(((SkeletalSubMesh*)(submesh.get()))->SkeletalVertices.data(),
				(UINT)sizeof(SkeletalVertex),
				(UINT)((SkeletalSubMesh*)(submesh.get()))->SkeletalVertices.size()));
			m_IndexBuffers.push_back(std::make_unique<IndexBuffer>(submesh->Indices.data(),
				(UINT)(sizeof(UINT) * submesh->Indices.size()), (UINT)submesh->Indices.size()));
		}

		CS_INFO("%s 스켈레탈 메쉬 불러오기 완료", meshFilePath.c_str());
	}

	SkeletalMesh::~SkeletalMesh()
	{
		delete m_AnimationFileImporter;
	}

	void SkeletalMesh::Update(float deltaTime)
	{
		Mesh::Update(deltaTime);

		float ticksPerSecond = m_AnimationScene->mAnimations[0]->mTicksPerSecond != 0
			? (float)m_AnimationScene->mAnimations[0]->mTicksPerSecond : 25.0f;
		m_AnimationTime += deltaTime * ticksPerSecond;
		m_AnimationTime = fmod(m_AnimationTime, (float)m_AnimationScene->mAnimations[0]->mDuration);
		boneTransform(m_AnimationTime);
	}

	void SkeletalMesh::ProcessNode(aiNode* rootNode, const aiScene* scene)
	{
		for (unsigned int i = 0; i < rootNode->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[rootNode->mMeshes[i]];
			m_Submeshes.push_back(std::make_unique<SkeletalSubMesh>(mesh));
		}

		for (unsigned int i = 0; i < rootNode->mNumChildren; i++)
		{
			ProcessNode(rootNode->mChildren[i], scene);
		}
	}

	void SkeletalMesh::boneTransform(float deltaTime)
	{
		readNodeHierarchy(deltaTime, m_AnimationScene->mRootNode, Matrix4x4::Identity());
	}

	void SkeletalMesh::readNodeHierarchy(float animationTime, const aiNode* pNode, const DirectX::XMFLOAT4X4& parentTransform)
	{
		// Reference : http://ogldev.atspace.co.uk/www/tutorial38/tutorial38.html
		/*부모노드부터 자식노드 까지 내려가면서 애니메이션을 계산*/
		std::string nodeName = pNode->mName.C_Str();
		unsigned int animationIndex = 0;
		const aiAnimation* animation = m_AnimationScene->mAnimations[animationIndex]; // animationIndex번째 animation
		DirectX::XMFLOAT4X4 nodeTransform = ToDirectXMathMatrix4x4(pNode->mTransformation);
		aiNodeAnim* nodeAnim = nullptr;
		/* Node의 이름과, animationIndex를 가지고 변환 정보를 가지고 있는 채널의 Index를 검색*/
		int channelIndex = findNodeAnimIndex(animationIndex, nodeName);
		if (channelIndex != -1)
			nodeAnim = animation->mChannels[channelIndex];

		if (nodeAnim)
		{
			DirectX::XMFLOAT3 scale = interpolateScale(animationTime, nodeAnim);
			DirectX::XMFLOAT4X4 scaleMatrix = Matrix4x4::Scale(scale);

			DirectX::XMFLOAT4 rotation = interpolateRotation(animationTime, nodeAnim);
			DirectX::XMFLOAT4X4 rotationMatrix = Matrix4x4::RotationQuaternion(rotation);

			DirectX::XMFLOAT3 translation = interpolateTranslation(animationTime, nodeAnim);
			DirectX::XMFLOAT4X4 translationMatrix = Matrix4x4::Translation(translation);

			nodeTransform = Matrix4x4::Multiply(Matrix4x4::Multiply(scaleMatrix, rotationMatrix), translationMatrix);
		}

		DirectX::XMFLOAT4X4 transform = Matrix4x4::Multiply(nodeTransform, parentTransform);

		if (m_BoneMap.find(nodeName) != m_BoneMap.end())
		{
			uint32_t boneIndex = m_BoneMap[nodeName];
			/* Local x parentTransform(ToWorld) (부모부터 차례대로 계산하는 이유.)*/
			m_BoneTransforms[boneIndex] = Matrix4x4::Transpose(
				Matrix4x4::Multiply(Matrix4x4::Multiply(m_BoneOffsets[boneIndex], transform), m_InverseGlobalTransform));
		}

		for (uint32_t i = 0; i < pNode->mNumChildren; i++)
			readNodeHierarchy(animationTime, pNode->mChildren[i], transform);
	}

	int SkeletalMesh::findNodeAnimIndex(unsigned int animationIndex, const std::string& nodeName)
	{
		auto& animationChannelMap = m_AnimationChannelMaps[animationIndex];
		if (animationChannelMap.count(nodeName))
		{
			return animationChannelMap[nodeName];
		}

		return -1;
	}

	DirectX::XMFLOAT3 SkeletalMesh::interpolateScale(float animationTime, aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumScalingKeys == 1)
			return { nodeAnim->mScalingKeys[0].mValue.x, nodeAnim->mScalingKeys[0].mValue.y,
			nodeAnim->mScalingKeys[0].mValue.z };

		uint32_t index = findScale(animationTime, nodeAnim);
		uint32_t nextIndex = index + 1;
		float deltaTime = (float)(nodeAnim->mScalingKeys[nextIndex].mTime - nodeAnim->mScalingKeys[index].mTime);
		float factor = (float)(animationTime - nodeAnim->mScalingKeys[index].mTime) / (float)deltaTime;
		factor = max(factor, 0.0f);

		// interpolate
		auto startValue = nodeAnim->mScalingKeys[index].mValue;
		auto endValue = nodeAnim->mScalingKeys[nextIndex].mValue;
		auto delta = endValue - startValue;

		auto result = startValue + factor * delta;
		return { result.x, result.y, result.z };
	}

	DirectX::XMFLOAT4 SkeletalMesh::interpolateRotation(float animationTime, aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumRotationKeys == 1)
			return { nodeAnim->mRotationKeys[0].mValue.x, nodeAnim->mRotationKeys[0].mValue.y,
			nodeAnim->mRotationKeys[0].mValue.z,  nodeAnim->mRotationKeys[0].mValue.w };

		uint32_t index = findRotation(animationTime, nodeAnim);
		uint32_t nextIndex = index + 1;
		float deltaTime = (float)(nodeAnim->mRotationKeys[nextIndex].mTime - nodeAnim->mRotationKeys[index].mTime);
		float factor = (float)((animationTime - nodeAnim->mRotationKeys[index].mTime) / deltaTime);
		factor = max(factor, 0.0f);

		// Slerp
		auto startValue = nodeAnim->mRotationKeys[index].mValue;
		auto endValue = nodeAnim->mRotationKeys[nextIndex].mValue;

		aiQuaternion result;
		aiQuaternion::Interpolate(result, startValue, endValue, factor);
		result.Normalize();

		return { result.x, result.y, result.z, result.w };
	}

	DirectX::XMFLOAT3 SkeletalMesh::interpolateTranslation(float animationTime, aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumPositionKeys == 1)
			return { nodeAnim->mPositionKeys[0].mValue.x, nodeAnim->mPositionKeys[0].mValue.y, nodeAnim->mPositionKeys[0].mValue.z };

		uint32_t index = findTranslation(animationTime, nodeAnim);
		uint32_t nextIndex = index + 1;
		float deltaTime = (float)(nodeAnim->mPositionKeys[nextIndex].mTime - nodeAnim->mPositionKeys[index].mTime);
		float factor = (float)((animationTime - nodeAnim->mPositionKeys[index].mTime) / deltaTime);
		factor = max(factor, 0.0f);

		// interpolate
		auto startValue = nodeAnim->mPositionKeys[index].mValue;
		auto endValue = nodeAnim->mPositionKeys[nextIndex].mValue;
		auto delta = endValue - startValue;

		auto result = startValue + factor * delta;
		return { result.x, result.y, result.z };
	}

	uint32_t SkeletalMesh::findScale(float animationTime, aiNodeAnim* nodeAnim)
	{
		for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
		{
			if (animationTime < nodeAnim->mScalingKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}

	uint32_t SkeletalMesh::findRotation(float animationTime, aiNodeAnim* nodeAnim)
	{
		for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
		{
			if (animationTime < nodeAnim->mRotationKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}

	uint32_t SkeletalMesh::findTranslation(float animationTime, aiNodeAnim* nodeAnim)
	{
		for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
		{
			if (animationTime < nodeAnim->mPositionKeys[i + 1].mTime)
				return i;
		}
		return 0;
	}
}