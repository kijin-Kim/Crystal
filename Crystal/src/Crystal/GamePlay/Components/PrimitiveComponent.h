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
			ar & m_ForceAccum;
		}

	public:
		PrimitiveComponent()
		{
		}

		~PrimitiveComponent() override = default;

		void RegisterComponent() override;

		void Update(const float deltaTime) override
		{
			TransformComponent::Update(deltaTime);

			if (m_Renderable)
				m_Renderable->Update(deltaTime);
		}

		void AddMaterial(Shared<Material> material);
		const std::vector<Shared<Material>>& GetMaterials() const;
		Material* GetMaterial(uint32_t index) const;
		


		void SetRenderable(Shared<Renderable> renderable)
		{
			m_Renderable = renderable;
		}

		const Shared<Renderable>& GetRenderable() const { return m_Renderable; }


		bool CanBeRendered() const override { return true; }
		bool IsCollisionEnabled() const override { return false; }

		void AddForce(const DirectX::XMFLOAT3& force) { m_ForceAccum = Vector3::Add(m_ForceAccum, force); }

		STATIC_TYPE_IMPLE(PrimitiveComponent)
	protected:
		Shared<Renderable> m_Renderable = nullptr;

		std::vector<Shared<Material>> m_Materials;

		DirectX::XMFLOAT3 m_ForceAccum = Vector3::Zero;

	};
}
