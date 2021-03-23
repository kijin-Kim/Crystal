#pragma once
#include <string>

namespace Crystal {

	struct aiScene;

	class Animation final
	{
	public:
		Animation(const std::string& filePath) {}
		~Animation() = default;



	private:
		const aiScene* m_Scene = nullptr;
	
	};

}
