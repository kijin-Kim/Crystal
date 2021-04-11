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
			"%s �޽��� �ε��ϴµ� �����Ͽ����ϴ�", filePath.c_str());
	}

	Mesh::~Mesh()
	{
		if (m_Importer)
			delete m_Importer;
	}

	StaticMesh::StaticMesh(const std::string& filePath) : Mesh(filePath)
	{
		CS_INFO("%s �޽� �ҷ����� �� ...", filePath.c_str());
		ProcessNode(m_MeshScene->mRootNode, m_MeshScene);
		for (const auto& submesh : m_Submeshes)
		{
			m_VertexBuffers.push_back(std::make_unique<VertexBuffer>(((StaticSubMesh*)(submesh.get()))->StaticVertices.data(),
				(UINT)sizeof(StaticVertex),
				(UINT)((StaticSubMesh*)(submesh.get()))->StaticVertices.size()));
			m_IndexBuffers.push_back(std::make_unique<IndexBuffer>(submesh->Indices.data(),
				(UINT)(sizeof(UINT) * submesh->Indices.size()), (UINT)submesh->Indices.size()));
		}

		CS_INFO("%s �޽� �ҷ����� �Ϸ�", filePath.c_str());
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
		/*���� animation file�� path�� ���� ���� �Ǿ��ٸ�*/
		CS_INFO("%s ���̷�Ż �޽� �ҷ����� �� ...", meshFilePath.c_str());
		ProcessNode(m_MeshScene->mRootNode, m_MeshScene);
		/*Skeletal Mesh�� �ε��մϴ�.*/
		m_bMeshHasBones = m_MeshScene->HasMeshes() && m_MeshScene->mMeshes[0]->HasBones();
		m_bMeshHasAnimations = m_MeshScene->HasAnimations();

		bool bHasSeperateFileForAnimation = !animationFilePath.empty();
		if (bHasSeperateFileForAnimation)
		{
			m_AnimationFileImporter = new Assimp::Importer();

			m_AnimationScene = m_AnimationFileImporter->ReadFile(animationFilePath,
				aiProcess_ConvertToLeftHanded);

			CS_FATAL(!(!m_AnimationScene || !m_AnimationScene->mRootNode),
				"%s �ִϸ��̼� �����͸� �ε��ϴµ� �����Ͽ����ϴ�", animationFilePath.c_str());
		}
		else if (m_bMeshHasAnimations) // ������ Animation File Path�� ���޵��� �ʾҰ�, �ִϸ��̼� �����Ͱ� Mesh File�� �����ϸ�
		{
			m_AnimationScene = m_MeshScene;
		}
		else
		{
			CS_FATAL(false, "Skeletal Mesh : %s�� ���� ������ �Ǵ� �ִϸ��̼��� �����Ͱ� ���� ���� �ʾҽ��ϴ�.", meshFilePath.c_str());
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
					if (m_BoneMap.find(boneName) == m_BoneMap.end()) // ó�� ������ Bone�̸�
					{
						m_BoneOffsets[boneIndex] = ToDirectXMathMatrix4x4(bone->mOffsetMatrix);
						m_BoneMap[boneName] = boneIndex;
					}
					else // ������ Bonemap�� �־�����
					{
						boneIndex = m_BoneMap[boneName]; // ���� ������ Bone���� ��ü
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

		CS_INFO("%s ���̷�Ż �޽� �ҷ����� �Ϸ�", meshFilePath.c_str());
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
		/*�θ������ �ڽĳ�� ���� �������鼭 �ִϸ��̼��� ���*/
		std::string nodeName = pNode->mName.C_Str();
		unsigned int animationIndex = 0;
		const aiAnimation* animation = m_AnimationScene->mAnimations[animationIndex]; // animationIndex��° animation
		DirectX::XMFLOAT4X4 nodeTransform = ToDirectXMathMatrix4x4(pNode->mTransformation);
		aiNodeAnim* nodeAnim = nullptr;
		/* Node�� �̸���, animationIndex�� ������ ��ȯ ������ ������ �ִ� ä���� Index�� �˻�*/
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
			/* Local x parentTransform(ToWorld) (�θ���� ���ʴ�� ����ϴ� ����.)*/
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