#include "cspch.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/Vertex.h>
#include <assimp/postprocess.h>
#include "CommandList.h"


namespace Crystal {

	DirectX::XMFLOAT4X4 ToDirectXMathMatrix4x4(const aiMatrix4x4& matrix)
	{
		return Matrix4x4::Transpose(*(DirectX::XMFLOAT4X4*) & matrix);
	}

	SubMesh::SubMesh(aiMesh* mesh, const aiScene* scene)
	{
		if (!scene->HasAnimations())
		{
			StaticVertices.reserve(mesh->mNumVertices);
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				StaticVertex vertex = {};
				vertex.Position = *(DirectX::XMFLOAT3*) & mesh->mVertices[i];

				if (mesh->HasNormals())
				{
					vertex.Normal = *(DirectX::XMFLOAT3*) & mesh->mNormals[i];
				}

				if (mesh->HasTangentsAndBitangents())
				{
					vertex.Tangent = *(DirectX::XMFLOAT3*) & mesh->mTangents[i];
					vertex.BiTangent = *(DirectX::XMFLOAT3*) & mesh->mBitangents[i];
				}

				if (mesh->HasTextureCoords(0))
				{
					vertex.TexCoord = *(DirectX::XMFLOAT2*) & mesh->mTextureCoords[0][i];
				}
				StaticVertices.push_back(vertex);
			}
		}
		else
		{
			AnimatedVertices.reserve(mesh->mNumVertices);
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				AnimatedVertex vertex = {};
				vertex.Position = *(DirectX::XMFLOAT3*) & mesh->mVertices[i];

				if (mesh->HasNormals())
				{
					vertex.Normal = *(DirectX::XMFLOAT3*) & mesh->mNormals[i];
				}

				if (mesh->HasTangentsAndBitangents())
				{
					vertex.Tangent = *(DirectX::XMFLOAT3*) & mesh->mTangents[i];
					vertex.BiTangent = *(DirectX::XMFLOAT3*) & mesh->mBitangents[i];
				}

				if (mesh->HasTextureCoords(0))
				{
					vertex.TexCoord = *(DirectX::XMFLOAT2*) & mesh->mTextureCoords[0][i];
				}
				AnimatedVertices.push_back(vertex);
			}
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
		CS_INFO("%s 메쉬 불러오는 중 ...", filePath.c_str());

		m_Importer = new Assimp::Importer();
		m_Scene = m_Importer->ReadFile(filePath, 
			aiProcess_ConvertToLeftHanded |
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords |
			aiProcess_OptimizeMeshes |
			aiProcess_ValidateDataStructure);
		CS_FATAL(!(!m_Scene || m_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode), "메쉬를 로드하는데 실패하였습니다");
		m_bIsAnimated = m_Scene->HasAnimations();
		processNode(m_Scene->mRootNode, m_Scene);


		// Bones

		m_InverseGlobalTransform = Matrix4x4::Inverse(ToDirectXMathMatrix4x4(m_Scene->mRootNode->mTransformation));
		
		if (m_bIsAnimated)
		{
			m_BoneTransforms.resize(m_Scene->mMeshes[0]->mNumBones);
			m_BoneOffsets.resize(m_Scene->mMeshes[0]->mNumBones);
			for (unsigned int m = 0; m < m_Scene->mNumMeshes; m++)
			{
				aiMesh* mesh = m_Scene->mMeshes[m];
				for (unsigned int i = 0; i < mesh->mNumBones; i++)
				{
					aiBone* bone = mesh->mBones[i];
					std::string boneName = bone->mName.C_Str();
					int boneIndex = i;
					if (m_BoneMap.find(boneName) == m_BoneMap.end())
					{
						m_BoneOffsets[boneIndex] = ToDirectXMathMatrix4x4(bone->mOffsetMatrix);
						m_BoneMap[boneName] = boneIndex;
					}
					else
					{
						boneIndex = m_BoneMap[boneName];
					}

					for (unsigned int j = 0; j < bone->mNumWeights; j++)
					{
						int vertexID = bone->mWeights[j].mVertexId;
						float weight = bone->mWeights[j].mWeight;
						m_Submeshes[m]->AnimatedVertices[vertexID].AddBoneData(boneIndex, weight);
					}
				}
			}
		}

		if (m_bIsAnimated)
		{
			for (const auto& submesh : m_Submeshes)
			{
				m_VertexBuffers.push_back(std::make_unique<VertexBuffer>(submesh->AnimatedVertices.data(), (UINT)sizeof(AnimatedVertex), (UINT)submesh->AnimatedVertices.size()));
				m_IndexBuffers.push_back(std::make_unique<IndexBuffer>(submesh->Indices.data(), (UINT)(sizeof(UINT) * submesh->Indices.size()), (UINT)submesh->Indices.size()));
			}
		}
		else
		{
			for (const auto& submesh : m_Submeshes)
			{
				m_VertexBuffers.push_back(std::make_unique<VertexBuffer>(submesh->StaticVertices.data(), (UINT)sizeof(StaticVertex), (UINT)submesh->StaticVertices.size()));
				m_IndexBuffers.push_back(std::make_unique<IndexBuffer>(submesh->Indices.data(), (UINT)(sizeof(UINT) * submesh->Indices.size()), (UINT)submesh->Indices.size()));
			}
		}

		if (m_bIsAnimated)
		{
			m_AnimationChannelMaps.resize(m_Scene->mNumAnimations);
			for (unsigned int i = 0; i < m_Scene->mNumAnimations; i++)
			{
				auto animation = m_Scene->mAnimations[i];
				for (unsigned int j = 0; j < animation->mNumChannels; j++)
				{
					auto animationChannel = animation->mChannels[j];
					m_AnimationChannelMaps[i][animationChannel->mNodeName.C_Str()] = j;
				}
			}
		}

		CS_INFO("%s 메쉬 불러오기 완료", filePath.c_str());
	}

	Mesh::~Mesh()
	{
		delete m_Importer;
	}

	void Mesh::Update(float deltaTime)
	{
		if (m_bIsAnimated)
		{
			float ticksPerSecond = m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? (float)m_Scene->mAnimations[0]->mTicksPerSecond : 25.0f;
			m_AnimationTime += deltaTime * ticksPerSecond;
			m_AnimationTime = fmod(m_AnimationTime, (float)m_Scene->mAnimations[0]->mDuration);
			boneTransform(m_AnimationTime);
		}
	}

	void Mesh::Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		for(int i = 0; i < m_VertexBuffers.size(); i++)
		{
			commandList->IASetVertexBuffers(0, 1, &m_VertexBuffers[i]->GetView());
			commandList->IASetIndexBuffer(&m_IndexBuffers[i]->GetView());
			commandList->DrawIndexedInstanced(m_IndexBuffers[i]->GetCount(), 1, 0, 0, 0);
		}
	}

	void Mesh::processNode(aiNode* rootNode, const aiScene* scene)
	{
		for (unsigned int i = 0; i < rootNode->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[rootNode->mMeshes[i]];
			m_Submeshes.push_back(std::make_unique<SubMesh>(mesh, scene));
		}

		for (unsigned int i = 0; i < rootNode->mNumChildren; i++)
		{
			processNode(rootNode->mChildren[i], scene);
		}
	}

	void Mesh::boneTransform(float deltaTime)
	{
		readNodeHierarchy(deltaTime, m_Scene->mRootNode, Matrix4x4::Identity());
	}

	int Mesh::findNodeAnimIndex(unsigned int animationIndex, const std::string& nodeName)
	{
		auto& animationChannelMap = m_AnimationChannelMaps[animationIndex];
		if (animationChannelMap.count(nodeName))
		{
			return animationChannelMap[nodeName];
		}
		
		return -1;
	}

	DirectX::XMFLOAT3 Mesh::interpolateScale(float animationTime, aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumScalingKeys == 1)
			return { nodeAnim->mScalingKeys[0].mValue.x, nodeAnim->mScalingKeys[0].mValue.y, nodeAnim->mScalingKeys[0].mValue.z };

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

	DirectX::XMFLOAT4 Mesh::interpolateRotation(float animationTime, aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumRotationKeys == 1)
			return { nodeAnim->mRotationKeys[0].mValue.x, nodeAnim->mRotationKeys[0].mValue.y, nodeAnim->mRotationKeys[0].mValue.z,  nodeAnim->mRotationKeys[0].mValue.w };

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

	DirectX::XMFLOAT3 Mesh::interpolateTranslation(float animationTime, aiNodeAnim* nodeAnim)
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

	uint32_t Mesh::findScale(float animationTime, aiNodeAnim* nodeAnim)
	{
		for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
		{
			if (animationTime < nodeAnim->mScalingKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}

	uint32_t Mesh::findRotation(float animationTime, aiNodeAnim* nodeAnim)
	{
		for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
		{
			if (animationTime < nodeAnim->mRotationKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}

	uint32_t Mesh::findTranslation(float animationTime, aiNodeAnim* nodeAnim)
	{
		for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
		{
			if (animationTime < nodeAnim->mPositionKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}

	void Mesh::readNodeHierarchy(float animationTime, const aiNode* pNode, const DirectX::XMFLOAT4X4& parentTransform)
	{
		std::string nodeName = pNode->mName.C_Str();
		unsigned int animationIndex = 0;
		const aiAnimation* animation = m_Scene->mAnimations[animationIndex];
		DirectX::XMFLOAT4X4 nodeTransform = ToDirectXMathMatrix4x4(pNode->mTransformation);
		aiNodeAnim* nodeAnim = nullptr;
		int channelIndex = findNodeAnimIndex(animationIndex, nodeName);
		if(channelIndex != -1)
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
			m_BoneTransforms[boneIndex] = Matrix4x4::Transpose(Matrix4x4::Multiply(Matrix4x4::Multiply(m_BoneOffsets[boneIndex], transform), m_InverseGlobalTransform));
			//m_BoneTransforms[boneIndex] = Matrix4x4::Transpose(Matrix4x4::Multiply(m_BoneOffsets[boneIndex], transform));
		}


		for (uint32_t i = 0; i < pNode->mNumChildren; i++)
			readNodeHierarchy(animationTime, pNode->mChildren[i], transform);
	}



}