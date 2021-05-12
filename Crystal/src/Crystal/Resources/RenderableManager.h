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
		Shared<Renderable> get(const std::string& fileName, Args ... args)
		{
			auto it = m_Meshes.find(fileName);

			Shared<Renderable> returnValue = nullptr;
			
			if (it == m_Meshes.end() || it->second.expired())
			{
				returnValue = CreateShared<T>(fileName, std::forward<Args>(args)...);
				m_Meshes[fileName] = returnValue;
			}
			else
			{
				returnValue = m_Meshes[fileName].lock();
			}

			return returnValue;
		}

		void RemoveGarbage()
		{
			for (auto it = m_Meshes.begin(); it != m_Meshes.end();)
			{
				if (it->second.expired())
				{
					it = m_Meshes.erase(it);
				}
				else
				{
					++it;
				}

			}
		}
		

	private:
		std::unordered_map<std::string, Weak<Renderable>> m_Meshes;
	};
}
