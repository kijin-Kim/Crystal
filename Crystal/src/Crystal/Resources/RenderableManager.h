#pragma once
#include <unordered_map>
#include <memory>
#include "Renderable.h"

namespace Crystal {

	class RenderableManager
	{
	public:
		RenderableManager() = default;
		~RenderableManager() = default;

		
		template <class T, class... Args>
		Weak<Renderable> get(const std::string& fileName, Args ... args)
		{			
			auto it = m_Meshes.find(fileName);
			
			if (it != m_Meshes.end())
			{
				return it->second;
			}

			m_Meshes[fileName] = CreateShared<T>(fileName, std::forward<Args>(args)...);
			return m_Meshes[fileName];

		}

	

	private:
		std::unordered_map<std::string, Shared<Renderable>> m_Meshes;
	};
}


