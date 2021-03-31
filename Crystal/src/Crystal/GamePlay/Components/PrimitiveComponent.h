#pragma once
#include "TransformComponent.h"
#include "Crystal/Resources/Material.h"
#include <array>
namespace Crystal {
	/*물리적인 위치와 눈에 보이는 렌더링 가능한 오브젝트를 가지고 있는 컴포넌트들의 베이스 클래스*/
	class PrimitiveComponent : public TransformComponent
	{
	public:
		enum class EPrimitiveComponentType
		{
			None,

			Mesh,
			StaticMesh,
			SkeletalMesh,

			Collision,
			Ray,
			BoundingBox,
			BoundingOrientedBox,
			BoundingSphere,
		};

	public:
		PrimitiveComponent()
		{
			m_PrimitiveComponentType = EPrimitiveComponentType::None;
		}
		~PrimitiveComponent() override = default;

		void OnCreate() override;

		void Update(const float deltaTime) override
		{
			TransformComponent::Update(deltaTime);
			m_Renderable->Update(deltaTime);
		}

		void SetMaterial(std::shared_ptr<Material> material, int index = 0) { m_Materials[index] = std::move(material); }
		const std::array<std::shared_ptr<Material>, 5>& GetMaterials() const { return m_Materials; }

		void SetPrimitiveComponentType(EPrimitiveComponentType type) { m_PrimitiveComponentType = type; }
		void SetRenderable(std::shared_ptr<Renderable> renderable) { m_Renderable = std::move(renderable); }
		const std::shared_ptr<Renderable>& GetRenderable() const { return m_Renderable; }

		EPrimitiveComponentType GetPrimitiveComponentType() const { return m_PrimitiveComponentType; }

		STATIC_TYPE_IMPLE(PrimitiveComponent)
	protected:
		std::shared_ptr<Renderable> m_Renderable = nullptr;
		EPrimitiveComponentType m_PrimitiveComponentType;

		std::array<std::shared_ptr<Material>, 5> m_Materials;
	};
}