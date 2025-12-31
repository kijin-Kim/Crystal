#include "cspch.h"
#include "Meshes.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/Vertex.h>
#include <assimp/postprocess.h>
#include "Buffer.h"

namespace Crystal {

	StaticSubMesh::StaticSubMesh(aiMesh* mesh)
	{
		StaticVertices.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			StaticVertex vertex = {};
			vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

			if (mesh->HasNormals())
			{
				vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
			}

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
				vertex.BiTangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
			}

			if (mesh->HasTextureCoords(0))
			{
				vertex.TexCoord = {mesh->mTextureCoords[0][i][0], mesh->mTextureCoords[0][i][1]};
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
			vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

			if (mesh->HasNormals())
			{
				vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
			}

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
				vertex.BiTangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
			}

			if (mesh->HasTextureCoords(0))
			{
				vertex.TexCoord = {mesh->mTextureCoords[0][i][0], mesh->mTextureCoords[0][i][1]};
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
		// CS_FATAL(!(!m_MeshScene || m_MeshScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_MeshScene->mRootNode), "%s ????? ?ех????? ????????????", filePath.c_str());
	}

	Mesh::~Mesh()
	{
		if (m_Importer)
			delete m_Importer;
	}

	StaticMesh::StaticMesh(const std::string& filePath) : Mesh(filePath)
	{
		// CS_INFO("%s ??? ??????? ?? ...", filePath.c_str());
		ProcessNode(m_MeshScene->mRootNode, m_MeshScene);
		for (const auto& submesh : m_Submeshes)
		{
			auto subMesh = (StaticSubMesh*)submesh.get();
			m_VertexBuffers.push_back(CreateShared<Buffer>(subMesh->StaticVertices.data(),
			                                               (UINT)sizeof(StaticVertex) * subMesh->StaticVertices.size(),
			                                               (UINT)subMesh->StaticVertices.size(), false, false));
			m_IndexBuffers.push_back(CreateShared<Buffer>(submesh->Indices.data(),
			                                              (UINT)(sizeof(UINT) * submesh->Indices.size()), (UINT)submesh->Indices.size(), false, false));
		}

		// CS_INFO("%s ??? ??????? ???", filePath.c_str());
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

	SkeletalMesh::SkeletalMesh(const std::string& meshFilePath) : Mesh(meshFilePath)
	{
		/*???? animation file?? path?? ???? ???? ??????*/
		// CS_INFO("%s ?????? ??? ??????? ?? ...", meshFilePath.c_str());
		ProcessNode(m_MeshScene->mRootNode, m_MeshScene);
		/*Skeletal Mesh?? ?ех?????.*/
		bool bMeshHasBones = m_MeshScene->HasMeshes() && m_MeshScene->mMeshes[0]->HasBones();
		
		// Bones

		/*m_InverseGlobalTransform = Matrix4x4::Inverse(ToDirectXMathMatrix4x4(m_MeshScene->mRootNode->mTransformation));*/

		if (bMeshHasBones)
		{
			m_BoneOffsets.resize(m_MeshScene->mMeshes[0]->mNumBones);
			for (unsigned int m = 0; m < m_MeshScene->mNumMeshes; m++)
			{
				aiMesh* mesh = m_MeshScene->mMeshes[m];
				for (unsigned int i = 0; i < mesh->mNumBones; i++)
				{
					aiBone* bone = mesh->mBones[i];
					std::string boneName = bone->mName.C_Str();
					int boneIndex = i;
					if (m_BoneMap.find(boneName) == m_BoneMap.end()) // ??? ?????? Bone???
					{
						m_BoneOffsets[boneIndex] = Matrix4x4::Transpose(*(DirectX::XMFLOAT4X4*)&bone->mOffsetMatrix);
						m_BoneMap[boneName] = boneIndex;
					}
					else // ?????? Bonemap?? ???????
					{
						boneIndex = m_BoneMap[boneName]; // ???? ?????? Bone???? ???
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
			auto subeMesh = (SkeletalSubMesh*)submesh.get();
			m_VertexBuffers.push_back(CreateShared<Buffer>(subeMesh->SkeletalVertices.data(),
			                                               (UINT)sizeof(SkeletalVertex) * subeMesh->SkeletalVertices.size(),
			                                               (UINT)subeMesh->SkeletalVertices.size(), false, false));
			m_IndexBuffers.push_back(CreateShared<Buffer>(submesh->Indices.data(),
			                                              (UINT)(sizeof(UINT) * submesh->Indices.size()), (UINT)submesh->Indices.size(), false, false));
		}




		// CS_INFO("%s ?????? ??? ??????? ???", meshFilePath.c_str());
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

}
