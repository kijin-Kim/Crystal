#pragma once

#include "BaseComponents.h"
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
		explicit PrimitiveComponent(const std::string& name);
		~PrimitiveComponent() override = default;

		void OnCreate() override;

		void Update(const float deltaTime) override
		{
			TransformComponent::Update(deltaTime);

			m_Renderable->Update(deltaTime);
		}

		EPrimitiveComponentType GetPrimitiveComponentType() const { return m_PrimitiveComponentType; }
		void SetPrimitiveComponentType(EPrimitiveComponentType type) { m_PrimitiveComponentType = type; }

		void SetRenderable(Renderable* renderable) { m_Renderable = renderable; }
		Renderable* GetRenderable() const { return m_Renderable; }

	protected:
		Renderable* m_Renderable = nullptr;
		EPrimitiveComponentType m_PrimitiveComponentType;
	};

}