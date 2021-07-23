#include "cspch.h"
#include "MeshComponents.h"

#include "assimp/anim.h"
#include "assimp/scene.h"
#include "Crystal/GamePlay/World/Level.h"

BOOST_CLASS_EXPORT(Crystal::MeshComponent)

BOOST_CLASS_EXPORT(Crystal::StaticMeshComponent)

BOOST_CLASS_EXPORT(Crystal::SkeletalMeshComponent)

namespace Crystal {


	void StaticMeshComponent::RegisterComponent()
	{
		Super::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto& scene = level->GetScene();

#ifndef CS_NM_DEDICATED
		scene->StaticMeshes.push_back(Cast<StaticMeshComponent>(shared_from_this()));
#endif
	}

	void SkeletalMeshComponent::RegisterComponent()
	{
		Super::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto& scene = level->GetScene();

#ifndef CS_NM_DEDICATED
		scene->SkeletalMeshes.push_back(Cast<SkeletalMeshComponent>(shared_from_this()));
#endif
	}

	void SkeletalMeshComponent::SetRenderable(Weak<Renderable> renderable)
	{
		MeshComponent::SetRenderable(renderable);

		auto skeletalMesh = Cast<SkeletalMesh>(renderable);
		if(skeletalMesh)
		{
			m_BoneTransforms.resize(skeletalMesh->GetBoneOffset().size());
		}
	}

	void SkeletalMeshComponent::SetAnimation(Weak<Animation> animation)
	{
		m_Animation = animation;
	}

	void SkeletalMeshComponent::Update(float deltaTime)
	{
		auto animation = m_Animation.lock();

		if (animation)
		{
			auto animScene = animation->GetAnimScene();
			float ticksPerSecond = animScene->mAnimations[0]->mTicksPerSecond != 0
				? (float)animScene->mAnimations[0]->mTicksPerSecond
				: 25.0f;
			m_AnimationTime += deltaTime * ticksPerSecond;
			m_AnimationTime = fmod(m_AnimationTime, (float)animScene->mAnimations[0]->mDuration);

			BoneTransform(m_AnimationTime);
		}
	}

	void SkeletalMeshComponent::BoneTransform(float deltaTime)
	{
		auto animation = m_Animation.lock();
		if(animation)
		{
			ReadNodeHierarchy(deltaTime, animation->GetAnimScene()->mRootNode, Matrix4x4::Identity());
		}
	}

	void SkeletalMeshComponent::ReadNodeHierarchy(float animationTime, const aiNode* pNode, const DirectX::XMFLOAT4X4& parentTransform)
	{
		// Reference : http://ogldev.atspace.co.uk/www/tutorial38/tutorial38.html
		/*부모노드부터 자식노드 까지 내려가면서 애니메이션을 계산*/
		std::string nodeName = pNode->mName.C_Str();
		unsigned int animationIndex = 0;

		auto animation = m_Animation.lock();
		if (!animation)
			return;

		
		aiAnimation* anim = animation->GetAnimScene()->mAnimations[animationIndex]; // animationIndex번째 animation
		DirectX::XMFLOAT4X4 nodeTransform = Matrix4x4::Transpose(*(DirectX::XMFLOAT4X4*)&pNode->mTransformation);
		aiNodeAnim* nodeAnim = nullptr;
		/* Node의 이름과, animationIndex를 가지고 변환 정보를 가지고 있는 채널의 Index를 검색*/
		int channelIndex = animation->FindNodeAnimIndex(animationIndex, nodeName);
		if (channelIndex != -1)
			nodeAnim = anim->mChannels[channelIndex];

		if (nodeAnim)
		{
			DirectX::XMFLOAT3 scale = interpolateScale(animationTime, nodeAnim);
			DirectX::XMFLOAT4X4 scaleMatrix = Matrix4x4::Scale(scale);

			DirectX::XMFLOAT4 rotation = interpolateRotation(animationTime, nodeAnim);
			DirectX::XMFLOAT4X4 rotationMatrix = Matrix4x4::RotationQuaternion(rotation);

			DirectX::XMFLOAT3 translation = interpolateTranslation(animationTime, nodeAnim);
			DirectX::XMFLOAT4X4 translationMatrix = Matrix4x4::Translation(translation);

			nodeTransform = Matrix4x4::Multiply(Matrix4x4::Multiply(scaleMatrix, rotationMatrix), translationMatrix);
		}

		DirectX::XMFLOAT4X4 transform = Matrix4x4::Multiply(nodeTransform, parentTransform);


		auto skeletalMesh = Cast<SkeletalMesh>(m_Renderable);
		auto& boneMap = skeletalMesh->GetBoneMap();
		auto& boneOffset = skeletalMesh->GetBoneOffset();
		
		if (boneMap.find(nodeName) != boneMap.end())
		{
			uint32_t boneIndex = boneMap[nodeName];
			/* Local x parentTransform(ToWorld) (부모부터 차례대로 계산하는 이유.)*/
			m_BoneTransforms[boneIndex] = Matrix4x4::Transpose(
				Matrix4x4::Multiply(Matrix4x4::Multiply(boneOffset[boneIndex], transform), m_InverseGlobalTransform));
		}

		for (uint32_t i = 0; i < pNode->mNumChildren; i++)
			ReadNodeHierarchy(animationTime, pNode->mChildren[i], transform);
	}


	DirectX::XMFLOAT3 SkeletalMeshComponent::interpolateScale(float animationTime, aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumScalingKeys == 1)
			return {
				nodeAnim->mScalingKeys[0].mValue.x, nodeAnim->mScalingKeys[0].mValue.y,
				nodeAnim->mScalingKeys[0].mValue.z
		};

		uint32_t index = findScale(animationTime, nodeAnim);
		uint32_t nextIndex = index + 1;
		float deltaTime = (float)(nodeAnim->mScalingKeys[nextIndex].mTime - nodeAnim->mScalingKeys[index].mTime);
		float factor = (float)(animationTime - nodeAnim->mScalingKeys[index].mTime) / (float)deltaTime;
		factor = max(factor, 0.0f);

		// interpolate
		auto startValue = nodeAnim->mScalingKeys[index].mValue;
		auto endValue = nodeAnim->mScalingKeys[nextIndex].mValue;
		auto delta = endValue - startValue;

		auto result = startValue + factor * delta;
		return { result.x, result.y, result.z };
	}

	DirectX::XMFLOAT4 SkeletalMeshComponent::interpolateRotation(float animationTime, aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumRotationKeys == 1)
			return {
				nodeAnim->mRotationKeys[0].mValue.x, nodeAnim->mRotationKeys[0].mValue.y,
				nodeAnim->mRotationKeys[0].mValue.z, nodeAnim->mRotationKeys[0].mValue.w
		};

		uint32_t index = findRotation(animationTime, nodeAnim);
		uint32_t nextIndex = index + 1;
		float deltaTime = (float)(nodeAnim->mRotationKeys[nextIndex].mTime - nodeAnim->mRotationKeys[index].mTime);
		float factor = (float)((animationTime - nodeAnim->mRotationKeys[index].mTime) / deltaTime);
		factor = max(factor, 0.0f);

		// Slerp
		auto startValue = nodeAnim->mRotationKeys[index].mValue;
		auto endValue = nodeAnim->mRotationKeys[nextIndex].mValue;

		aiQuaternion result;
		aiQuaternion::Interpolate(result, startValue, endValue, factor);
		result.Normalize();

		return { result.x, result.y, result.z, result.w };
	}

	DirectX::XMFLOAT3 SkeletalMeshComponent::interpolateTranslation(float animationTime, aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumPositionKeys == 1)
			return { nodeAnim->mPositionKeys[0].mValue.x, nodeAnim->mPositionKeys[0].mValue.y, nodeAnim->mPositionKeys[0].mValue.z };

		uint32_t index = findTranslation(animationTime, nodeAnim);
		uint32_t nextIndex = index + 1;
		float deltaTime = (float)(nodeAnim->mPositionKeys[nextIndex].mTime - nodeAnim->mPositionKeys[index].mTime);
		float factor = (float)((animationTime - nodeAnim->mPositionKeys[index].mTime) / deltaTime);
		factor = max(factor, 0.0f);

		// interpolate
		auto startValue = nodeAnim->mPositionKeys[index].mValue;
		auto endValue = nodeAnim->mPositionKeys[nextIndex].mValue;
		auto delta = endValue - startValue;

		auto result = startValue + factor * delta;
		return { result.x, result.y, result.z };
	}

	uint32_t SkeletalMeshComponent::findScale(float animationTime, aiNodeAnim* nodeAnim)
	{
		for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
		{
			if (animationTime < nodeAnim->mScalingKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}

	uint32_t SkeletalMeshComponent::findRotation(float animationTime, aiNodeAnim* nodeAnim)
	{
		for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
		{
			if (animationTime < nodeAnim->mRotationKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}

	uint32_t SkeletalMeshComponent::findTranslation(float animationTime, aiNodeAnim* nodeAnim)
	{
		for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
		{
			if (animationTime < nodeAnim->mPositionKeys[i + 1].mTime)
				return i;
		}
		return 0;
	}
}
