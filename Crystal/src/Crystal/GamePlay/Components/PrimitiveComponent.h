#pragma once
#include "TransformComponent.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include <array>



namespace Crystal {
	/*�������� ��ġ�� ���� ���̴� ������ ������ ������Ʈ�� ������ �ִ� ������Ʈ���� ���̽� Ŭ����*/

	class PrimitiveComponent : public TransformComponent
	{
	public:
		PrimitiveComponent() = default;
		~PrimitiveComponent() override = default;

		void OnCreate() override;

		void RegisterComponent() override;

		void Update(const float deltaTime) override
		{
			TransformComponent::Update(deltaTime);
			auto renderable = m_Renderable.lock();
			if(renderable)
				renderable->Update(deltaTime);
		}

		void AddMaterialOld(std::shared_ptr<Material> material) { m_MaterialsOld.push_back(std::move(material)); }
		const std::vector<std::shared_ptr<Material>>& GetMaterialsOld() const { return m_MaterialsOld; }


		void AddMaterial(std::unique_ptr<NewMaterial> material);
		const std::vector<std::unique_ptr<NewMaterial>>& GetMaterials() const;
		NewMaterial* GetMaterial(uint32_t index) const;

		void SetRenderable(std::weak_ptr<Renderable> renderable) { m_Renderable = renderable; }
		const std::weak_ptr<Renderable>& GetRenderable() const { return m_Renderable; }

		bool CanBeRendered() const override { return true; }
		bool IsCollisionEnabled() const override { return false; }

		void AddForce(const DirectX::XMFLOAT3& force) { m_ForceAccum = Vector3::Add(m_ForceAccum, force); }

		STATIC_TYPE_IMPLE(PrimitiveComponent)
	protected:
		std::weak_ptr<Renderable> m_Renderable;

		std::vector<std::shared_ptr<Material>> m_MaterialsOld;
		std::vector<std::unique_ptr<NewMaterial>> m_Materials;

		DirectX::XMFLOAT3 m_ForceAccum = Vector3::Zero;
	};
}