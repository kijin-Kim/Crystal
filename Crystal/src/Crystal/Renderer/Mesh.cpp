#include "cspch.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/Vertex.h>
#include <assimp/postprocess.h>
#include "CommandList.h"

namespace Crystal {
	SubMesh::SubMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		vertices.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex = {};
			vertex.Position = *(DirectX::XMFLOAT3*) & mesh->mVertices[i];
			vertex.Normal = *(DirectX::XMFLOAT3*) & mesh->mNormals[i];

			/*if(HasBitangent)
			{
			}*/

			if (mesh->HasTextureCoords(0))
			{
				vertex.TexCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			}

			vertices.push_back(vertex);
		}

		m_VertexBuffer = std::make_unique<VertexBuffer>(vertices.data(), (UINT)(sizeof(float) * 8), (UINT)vertices.size());

		std::vector<UINT> indices;
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			indices.reserve(mesh->mFaces[i].mNumIndices);
			for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			{
				indices.emplace_back(mesh->mFaces[i].mIndices[j]);
			}
		}

		m_IndexBuffer = std::make_unique<IndexBuffer>(indices.data(), (UINT)(indices.size() * sizeof(UINT)), (UINT)indices.size());
	}

	void SubMesh::Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		commandList->IASetVertexBuffers(0, 1, &m_VertexBuffer->GetView());
		commandList->IASetIndexBuffer(&m_IndexBuffer->GetView());
		commandList->DrawIndexedInstanced(m_IndexBuffer->GetCount(), 1, 0, 0, 0);
	}

	Mesh::Mesh(const std::string& filePath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
		CS_ASSERT(!(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode), "모델을 로드하는데 실패하였습니다");
		processNode(scene->mRootNode, scene);
	}

	void Mesh::Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		for (const auto& mesh : m_Meshes)
		{
			mesh->Render(commandList);
		}
	}

	void Mesh::processNode(aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_Meshes.push_back(std::make_unique<SubMesh>(mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}
}