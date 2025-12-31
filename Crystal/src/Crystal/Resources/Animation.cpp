#include "cspch.h"
#include "Animation.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace Crystal {

	Animation::Animation(const std::string& filePath)
	{
		m_AnimationFileImporter = new Assimp::Importer();

		m_AnimationScene = m_AnimationFileImporter->ReadFile(filePath, aiProcess_ConvertToLeftHanded);

		CS_FATAL(!(!m_AnimationScene || !m_AnimationScene->mRootNode), "%s 애니메이션 데이터를 로드하는데 실패하였습니다", filePath.c_str());

		m_AnimationChannelMaps.resize(m_AnimationScene->mNumAnimations);
		for (unsigned int i = 0; i < m_AnimationScene->mNumAnimations; i++)
		{
			auto animation = m_AnimationScene->mAnimations[i];
			for (unsigned int j = 0; j < animation->mNumChannels; j++)
			{
				auto animationChannel = animation->mChannels[j];
				m_AnimationChannelMaps[i][animationChannel->mNodeName.C_Str()] = j;
			}
		}

		CS_DEBUG_INFO("%s 애니메이션 데이터를 로드하였습니다.", filePath.c_str());

	}

	Animation::~Animation()
	{
		delete m_AnimationFileImporter;
	}

	int Animation::FindNodeAnimIndex(unsigned animationIndex, const std::string& nodeName)
	{
		auto& animationChannelMap = m_AnimationChannelMaps[animationIndex];
		if (animationChannelMap.count(nodeName))
		{
			return animationChannelMap[nodeName];
		}

		return -1;
	}

}


