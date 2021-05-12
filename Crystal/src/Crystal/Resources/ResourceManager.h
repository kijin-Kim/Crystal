#pragma once
#include "ShaderManager.h"
#include "RenderableManager.h"
#include "TextureManager.h"

namespace Crystal {

	/*
	 * 이 매니저 클래스는 파일로 부터 불러야하는 리소스를 캐싱하는 클래스입니다.
	 */

	class ResourceManager
	{
	public:
		static ResourceManager& Instance()
		{
			static ResourceManager instance;
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
		Shared<Renderable> GetRenderable(const std::string& fileName, Args ... args)
		{
			if (!m_RenderableManager)
			{
				return nullptr;
			}

			return m_RenderableManager->get<T>(fileName, std::forward<Args>(args)...);
		}

	private:
		ResourceManager()
		{
#if defined(CS_NM_STANDALONE) || defined(CS_NM_CLIENT)
			m_ShaderManager = CreateUnique<ShaderManager>();
			m_TextureManager = CreateUnique<TextureManager>();
			m_RenderableManager = CreateUnique<RenderableManager>();
#endif
		}

		~ResourceManager() = default;

	private:
		Unique<ShaderManager> m_ShaderManager = nullptr;
		Unique<TextureManager> m_TextureManager = nullptr;
		Unique<RenderableManager> m_RenderableManager = nullptr;
	};

}
