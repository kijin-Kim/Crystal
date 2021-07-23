#pragma once


struct aiScene;

namespace Assimp {
	class Importer;
}

namespace Crystal {

	class Animation
	{
	public:
		Animation(const std::string& filePath);
		~Animation();

		int FindNodeAnimIndex(unsigned int animationIndex, const std::string& nodeName);


		const aiScene* GetAnimScene() const { return m_AnimationScene; }
	
	private:
		const aiScene* m_AnimationScene = nullptr;
		class Assimp::Importer* m_AnimationFileImporter;

		using AnimationChannelMap = std::map<std::string, uint32_t>;
		std::vector <AnimationChannelMap> m_AnimationChannelMaps;

	};
	
}
