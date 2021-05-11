#pragma once
#include "TransformComponent.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include <array>


namespace Crystal {
	/*물리적인 위치와 눈에 보이는 렌더링 가능한 오브젝트를 가지고 있는 컴포넌트들의 베이스 클래스*/

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

			for (const auto& renderable : m_Renderables)
			{
				if (!renderable)
				{
					continue;
				}

				renderable->Update(deltaTime);
			}
		}

		void AddMaterial(Unique<NewMaterial> material);
		const std::vector<Unique<NewMaterial>>& GetMaterials() const;
		NewMaterial* GetMaterial(uint32_t index) const;


		void SetRenderable(Shared<Renderable> renderable, uint32_t index = 0)
		{
			m_Renderables[index] = renderable;
		}

		const Shared<Renderable>& GetRenderable(uint32_t index = 0) const { return m_Renderables[0]; }


		bool CanBeRendered() const override { return true; }
		bool IsCollisionEnabled() const override { return false; }

		void AddForce(const DirectX::XMFLOAT3& force) { m_ForceAccum = Vector3::Add(m_ForceAccum, force); }

		STATIC_TYPE_IMPLE(PrimitiveComponent)
	protected:
		std::vector<Shared<Renderable>> m_Renderables;

		std::vector<Unique<NewMaterial>> m_Materials;

		DirectX::XMFLOAT3 m_ForceAccum = Vector3::Zero;
	};
}
