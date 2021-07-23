#pragma once
#include "Component.h"
#include "PrimitiveComponent.h"

#include "Crystal/Resources/Animation.h"

struct aiNodeAnim;
struct aiNode;

namespace Crystal {
	class MeshComponent : public PrimitiveComponent
	{
		using Super = PrimitiveComponent;

		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<PrimitiveComponent>(*this);
			ar & m_bCastShadow;
		}

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

		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<MeshComponent>(*this);
		}

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

		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<MeshComponent>(*this);
		}

	public:
		SkeletalMeshComponent() = default;
		~SkeletalMeshComponent() override = default;

		void RegisterComponent() override;

		void SetRenderable(Weak<Renderable> renderable) override;


		void SetAnimation(Weak<Animation> animation);

		void Update(float deltaTime) override;


		void BoneTransform(float deltaTime);
		void ReadNodeHierarchy(float animationTime, const aiNode* pNode, const DirectX::XMFLOAT4X4& parentTransform);

		const std::vector <DirectX::XMFLOAT4X4>& GetBoneTransforms() const { return m_BoneTransforms; }
		
		
		STATIC_TYPE_IMPLE(SkeletalMeshComponent)

	private:
		DirectX::XMFLOAT3 interpolateScale(float animationTime, aiNodeAnim* nodeAnim);
		DirectX::XMFLOAT4 interpolateRotation(float animationTime, aiNodeAnim* nodeAnim);
		DirectX::XMFLOAT3 interpolateTranslation(float animationTime, aiNodeAnim* nodeAnim);

		uint32_t findScale(float animationTime, aiNodeAnim* nodeAnim);
		uint32_t findRotation(float animationTime, aiNodeAnim* nodeAnim);
		uint32_t findTranslation(float animationTime, aiNodeAnim* nodeAnim);

	private:
		Weak<Animation> m_Animation;
		
		std::vector <DirectX::XMFLOAT4X4> m_BoneTransforms; // 최종적으로 변환된 Bone들의 값 (World Space)
		
		DirectX::XMFLOAT4X4 m_InverseGlobalTransform = Matrix4x4::Identity();		
		
		float m_AnimationTime = 0.0f;
	};
}
