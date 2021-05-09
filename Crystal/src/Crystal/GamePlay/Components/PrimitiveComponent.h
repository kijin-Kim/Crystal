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
			m_Renderables.resize(10);
		}

		~PrimitiveComponent() override = default;

		void RegisterComponent() override;

		void Update(const float deltaTime) override
		{
			TransformComponent::Update(deltaTime);

			for (const auto& weak : m_Renderables)
			{
				auto renderable = weak.lock();
				if (!renderable)
					continue;

				renderable->Update(deltaTime);
			}	
		}

		void AddMaterial(std::unique_ptr<NewMaterial> material);
		const std::vector<std::unique_ptr<NewMaterial>>& GetMaterials() const;
		NewMaterial* GetMaterial(uint32_t index) const;


		void SetRenderable(std::weak_ptr<Renderable> renderable, uint32_t index = 0)
		{
			m_Renderables[index] = renderable;
		}

		const std::weak_ptr<Renderable>& GetRenderable(uint32_t index = 0) const { return m_Renderables[0]; }


		bool CanBeRendered() const override { return true; }
		bool IsCollisionEnabled() const override { return false; }

		void AddForce(const DirectX::XMFLOAT3& force) { m_ForceAccum = Vector3::Add(m_ForceAccum, force); }

		STATIC_TYPE_IMPLE(PrimitiveComponent)
	protected:
		std::vector<std::weak_ptr<Renderable>> m_Renderables;

		std::vector<std::unique_ptr<NewMaterial>> m_Materials;

		DirectX::XMFLOAT3 m_ForceAccum = Vector3::Zero;
	};
}
