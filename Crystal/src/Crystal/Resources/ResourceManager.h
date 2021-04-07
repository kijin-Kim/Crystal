#pragma once
#include "ShaderManager.h"
#include "RenderableManager.h"
#include "TextureManager.h"

namespace Crystal {
	class ResourceManager
	{
	public:
		static ResourceManager& Instance() { static ResourceManager instance; return instance; }

		std::weak_ptr<Shader> CreateShaderFromFile(const std::string& fileName, const std::string& name)
		{
			return m_ShaderManager.createFromFile(fileName, name);
		}

		void DestroyShader(const std::string& name)
		{
			m_ShaderManager.destroy(name);
		}

		std::weak_ptr<Shader> GetShader(const std::string& name)
		{
			return m_ShaderManager.get(name);
		}

		std::weak_ptr<Texture> CreateTextureFromFile(const std::string& fileName, const std::string& name, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE)
		{
			return m_TextureManager.createFromFile(fileName, name, resourceFlags);
		}

		std::weak_ptr<Texture> CreateTexture(int width, int height, int depth, int mipLevels, DXGI_FORMAT format,
			D3D12_RESOURCE_FLAGS resourceFlags, D3D12_RESOURCE_STATES initialStates, const std::string& name)
		{
			return m_TextureManager.create(width, height, depth, mipLevels, format,
				resourceFlags, initialStates, name);
		}

		std::weak_ptr<Texture> CreateTextureByResource(ID3D12Resource* resource, const std::string& name, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE)
		{
			return m_TextureManager.createByResource(resource, name, resourceFlags);
		}

		

		void DestroyTexture(const std::string& name)
		{
			m_TextureManager.destroy(name);
		}

		std::weak_ptr<Texture> GetTexture(const std::string& name)
		{
			return m_TextureManager.get(name);
		}

		template<class T>
		void CreateRenderable(const std::string& name)
		{
			m_RenderableManager.create<T>(name);
		}

		template<class T>
		void CreateRenderableFromFile(const std::string& filePath, const std::string& alias = "")
		{
			m_RenderableManager.createFromFile<T>(filePath, alias);
		}

		void DestroyRenderable(const std::string& name)
		{
			m_RenderableManager.destroy(name);
		}

		const std::shared_ptr<Renderable>& GetRenderable(const std::string& name)
		{
			return m_RenderableManager.get(name);
		}

	private:
		ResourceManager() = default;
		~ResourceManager() = default;

	private:
		ShaderManager m_ShaderManager;
		TextureManager m_TextureManager;
		RenderableManager m_RenderableManager;
	};
}
