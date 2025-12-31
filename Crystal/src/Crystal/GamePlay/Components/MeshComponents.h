#pragma once
#include <functional>

#include "Component.h"
#include "PrimitiveComponent.h"
#include "Crystal/Core/Timer.h"

#include "Crystal/Resources/Animation.h"

#define CS_ANIMATION_FN(fn) std::bind(&fn, this)

struct aiNodeAnim;
struct aiNode;

namespace Crystal {
	class MeshComponent : public PrimitiveComponent
	{
		using Super = PrimitiveComponent;



	public:
		MeshComponent() = default;
		~MeshComponent() override = default;

		void SetCastShadow(bool bCastShadow) { m_bCastShadow = bCastShadow; }
		bool GetCastShadow() const { return m_bCastShadow; }

		STATIC_TYPE_IMPLE(MeshComponent)

	private:
		bool m_bCastShadow = true;
	};

	class StaticMeshComponent : public MeshComponent
	{
		using Super = MeshComponent;

	public:
		StaticMeshComponent() = default;
		~StaticMeshComponent() override = default;

		void RegisterComponent() override;

		STATIC_TYPE_IMPLE(StaticMeshComponent)


	private:
	};

	class SkeletalMeshComponent : public MeshComponent
	{
		using Super = MeshComponent;

	public:
		SkeletalMeshComponent() = default;
		~SkeletalMeshComponent() override = default;

		void RegisterComponent() override;

		void SetRenderable(Weak<Renderable> renderable) override;

		void PlayAnimation(Weak<Animation> animation, bool bShouldLoop);
		void PlayAnimationWithEvent(Weak<Animation> animation, bool bShouldLoop, const std::function<void()>& event, float eventInterval);
		void PlayAnimationWithEndEvent(Weak<Animation> animation, bool bShouldLoop, const std::function<void()>& event);
		

		void Update(float deltaTime) override;


		void BoneTransform(float deltaTime);
		void ReadNodeHierarchy(float animationTime, const aiNode* pNode, const DirectX::XMFLOAT4X4& parentTransform);


		const std::vector <DirectX::XMFLOAT4X4>& GetBoneTransforms() const { return m_BoneTransforms; }


		STATIC_TYPE_IMPLE(SkeletalMeshComponent)

	private:
		DirectX::XMFLOAT3 InterpolateScale(float animationTime, aiNodeAnim* nodeAnim);
		DirectX::XMFLOAT4 InterpolateRotation(float animationTime, aiNodeAnim* nodeAnim);
		DirectX::XMFLOAT3 InterpolateTranslation(float animationTime, aiNodeAnim* nodeAnim);

		uint32_t FindScale(float animationTime, aiNodeAnim* nodeAnim);
		uint32_t FindRotation(float animationTime, aiNodeAnim* nodeAnim);
		uint32_t FindTranslation(float animationTime, aiNodeAnim* nodeAnim);


	private:
		Weak<Animation> m_Animation;
		
		std::vector <DirectX::XMFLOAT4X4> m_BoneTransforms; // ���������� ��ȯ�� Bone���� �� (World Space)
		
		DirectX::XMFLOAT4X4 m_InverseGlobalTransform = Matrix4x4::Identity();		
		
		float m_AnimationTime = 0.0f;

		bool m_bShouldLoopAnimation = false;
		

		std::function<void()> m_AnimationEvent = nullptr;
		float m_AnimationEventInterval = 0.0f;

		Timer m_AnimationEventTimer = {};

		bool m_bIsPlayingAnimation = false;
	};
}


