#pragma once
#include "TransformComponent.h"
#include "Crystal/Resources/Material.h"
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
			m_Renderable->Update(deltaTime);
		}

		void AddMaterial(std::shared_ptr<Material> material) { m_Materials.push_back(std::move(material)); }
		const std::vector<std::shared_ptr<Material>>& GetMaterials() const { return m_Materials; }

		void SetRenderable(std::shared_ptr<Renderable> renderable) { m_Renderable = std::move(renderable); }
		const std::shared_ptr<Renderable>& GetRenderable() const { return m_Renderable; }

		bool CanBeRendered() const override { return true; }
		bool IsCollisionEnabled() const override { return false; }

		void AddForce(const DirectX::XMFLOAT3& force) { m_ForceAccum = Vector3::Add(m_ForceAccum, force); }

		STATIC_TYPE_IMPLE(PrimitiveComponent)
	protected:
		std::shared_ptr<Renderable> m_Renderable = nullptr;

		std::vector<std::shared_ptr<Material>> m_Materials;

		DirectX::XMFLOAT3 m_ForceAccum = Vector3::Zero;
	};
}