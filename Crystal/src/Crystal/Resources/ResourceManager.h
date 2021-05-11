#pragma once
#include "ShaderManager.h"
#include "RenderableManager.h"
#include "TextureManager.h"

namespace Crystal {

	class ResourceManager
	{
	public:
		static ResourceManager& Instance()
		{
			static ResourceManager instance;
			return instance;
		}

		std::weak_ptr<Shader> CreateShaderFromFile(const std::string& fileName, const std::string& name)
		{
#ifndef CS_NM_DEDICATED
			return m_ShaderManager.createFromFile(fileName, name);
#else
			return {};
#endif
		}

		void DestroyShader(const std::string& name)
		{
#ifndef CS_NM_DEDICATED
			m_ShaderManager.destroy(name);
#endif
		}

		std::weak_ptr<Shader> GetShader(const std::string& name)
		{
#ifndef CS_NM_DEDICATED
			return m_ShaderManager.get(name);
#else
			return {};
#endif
		}

		std::weak_ptr<Texture> CreateTextureFromFile(const std::string& fileName, const std::string& name,
		                                             D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE)
		{
#ifndef CS_NM_DEDICATED
			return m_TextureManager.createFromFile(fileName, name, resourceFlags);
#else
			return {};
#endif
		}

		std::weak_ptr<Texture> CreateTexture(int width, int height, int depth, int mipLevels, DXGI_FORMAT format,
		                                     D3D12_RESOURCE_FLAGS resourceFlags, D3D12_RESOURCE_STATES initialStates,
		                                     const std::string& name)
		{
#ifndef CS_NM_DEDICATED
			return m_TextureManager.create(width, height, depth, mipLevels, format,
			                               resourceFlags, initialStates, name);
#else
			return {};
#endif
		}

		std::weak_ptr<Texture> CreateTextureByResource(ID3D12Resource* resource, const std::string& name,
		                                               D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE)
		{
#ifndef CS_NM_DEDICATED
			return m_TextureManager.createByResource(resource, name, resourceFlags);
#else
			return {};
#endif
		}


		void DestroyTexture(const std::string& name)
		{
#ifndef CS_NM_DEDICATED
			m_TextureManager.destroy(name);
#endif
		}

		std::weak_ptr<Texture> GetTexture(const std::string& name)
		{
#ifndef CS_NM_DEDICATED
			return m_TextureManager.get(name);
#else
			return {};
#endif
		}

		template <class T>
		void CreateRenderable(const std::string& name)
		{
			m_RenderableManager.create<T>(name);
		}

		template <class T, class... Args>
		void CreateRenderableFromFile(const std::string& alias, Args ... args)
		{
			m_RenderableManager.createFromFile<T>(alias, std::forward<Args>(args)...);
		}

		void DestroyRenderable(const std::string& name)
		{
			m_RenderableManager.destroy(name);
		}

		std::weak_ptr<Renderable> GetRenderable(const std::string& name)
		{
#ifndef CS_NM_DEDICATED
			return m_RenderableManager.get(name);
#else
			return {};
#endif
		}

	private:
		ResourceManager() = default;
		~ResourceManager() = default;

	private:
		ShaderManager m_ShaderManager;
		TextureManager m_TextureManager;
		RenderableManager m_RenderableManager;
	};


	/*
	 * 이 매니저 클래스는 파일로 부터 불러야하는 리소스를 캐싱하는 클래스입니다.
	 */

	class NewResourceManager
	{
	public:
		static NewResourceManager& Instance()
		{
			static NewResourceManager instance;
			return instance;
		}

		Shared<Shader> GetShader(const std::string& fileName)
		{
			if (!m_ShaderManager)
			{
				return nullptr;
			}

			return m_ShaderManager->get(fileName);
		}

		Shared<Texture> GetTexture(const std::string& fileName)
		{
			if (!m_TextureManager)
			{
				return nullptr;
			}

			return m_TextureManager->get(fileName);
		}


		template <class T, class... Args>
		Shared<Renderable> GetRenderable(Args... args)
		{
			if (!m_RenderableManager)
			{
				return nullptr;
			}

			return m_RenderableManager->get<T>(std::forward<Args>(args)...);
		}

	private:
		NewResourceManager()
		{
#if defined(CS_NM_STANDALONE) || defined(CS_NM_CLIENT)
			m_ShaderManager = CreateUnique<NewShaderManager>();
			m_TextureManager = CreateUnique<NewTextureManager>();
			m_RenderableManager = CreateUnique<NewRenderableManager>();
#endif
		}

		~NewResourceManager() = default;

	private:
		Unique<NewShaderManager> m_ShaderManager = nullptr;
		Unique<NewTextureManager> m_TextureManager = nullptr;
		Unique<NewRenderableManager> m_RenderableManager = nullptr;
	};

}
