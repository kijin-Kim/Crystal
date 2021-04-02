#pragma once
#include "TransformComponent.h"
#include "Crystal/Resources/Material.h"
#include <array>
namespace Crystal {
	/*�������� ��ġ�� ���� ���̴� ������ ������ ������Ʈ�� ������ �ִ� ������Ʈ���� ���̽� Ŭ����*/
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

		void AddMaterial(std::shared_ptr<Material> material) { m_Materials.push_back(std::move(material)); }
		const std::vector<std::shared_ptr<Material>>& GetMaterials() const { return m_Materials; }

		void SetPrimitiveComponentType(EPrimitiveComponentType type) { m_PrimitiveComponentType = type; }
		void SetRenderable(std::shared_ptr<Renderable> renderable) { m_Renderable = std::move(renderable); }
		const std::shared_ptr<Renderable>& GetRenderable() const { return m_Renderable; }

		EPrimitiveComponentType GetPrimitiveComponentType() const { return m_PrimitiveComponentType; }

		bool CanBeRendered()const override { return true; }

		STATIC_TYPE_IMPLE(PrimitiveComponent)
	protected:
		std::shared_ptr<Renderable> m_Renderable = nullptr;
		EPrimitiveComponentType m_PrimitiveComponentType;

		std::vector<std::shared_ptr<Material>> m_Materials;
	};
}