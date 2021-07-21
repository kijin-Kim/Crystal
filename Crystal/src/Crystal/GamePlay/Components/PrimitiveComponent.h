#pragma once
#include "TransformComponent.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include <array>


namespace Crystal {
	/*�������� ��ġ�� ���� ���̴� ������ ������ ������Ʈ�� ������ �ִ� ������Ʈ���� ���̽� Ŭ����*/

	class PrimitiveComponent : public TransformComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<TransformComponent>(*this);
			ar & m_bHideInGame;
		}

	public:
		PrimitiveComponent() = default;

		~PrimitiveComponent() override = default;


		void Update(const float deltaTime) override
		{
			TransformComponent::Update(deltaTime);

			if (auto renderable = m_Renderable.lock())
			{
				renderable->Update(deltaTime);
			}
		}

		void AddMaterial(Shared<Material> material);
		const std::vector<Shared<Material>>& GetMaterials() const;
		Material* GetMaterial(uint32_t index) const;


		void SetRenderable(Weak<Renderable> renderable)
		{
			m_Renderable = renderable;
		}

		Weak<Renderable> GetRenderable() const { return m_Renderable; }


		bool CanBeRendered() const override { return true; }
		bool IsCollisionEnabled() const override { return false; }

		void SetHideInGame(bool bHide) { m_bHideInGame = bHide; }
		bool GetHideInGame() const { return m_bHideInGame; }

		STATIC_TYPE_IMPLE(PrimitiveComponent)
	protected:
		Weak<Renderable> m_Renderable = {};

		std::vector<Shared<Material>> m_Materials;

		bool m_bHideInGame = false;
	};
}
