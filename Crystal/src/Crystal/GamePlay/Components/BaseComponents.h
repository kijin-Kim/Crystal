#pragma once
#include <DirectXMath.h>
#include "Crystal/Math/Math.h"

namespace Crystal {
	class Component
	{
	public:
		Component() = default;
		virtual ~Component() = default;

		virtual void Update(float deltaTime) {}
	};


	/*물리적인 위치를 나타내는 특성을 가지고 있는 컴포넌트들의 베이스 클래스
	MainComponent가 될 수 있고, 최대 하나의 부모를 가지고 부모의 상대적 Transform을 지님*/
	class TransformComponent : public Component
	{
	public:
		TransformComponent()
		{
			DirectX::XMStoreFloat4x4(&m_Transform, DirectX::XMMatrixIdentity());
		}
		virtual ~TransformComponent()
		{
		}

		virtual void Update(float deltaTime) override
		{
			Component::Update(deltaTime);
		}

		void AttachToComponent(TransformComponent* parentComponent) { m_Parent = parentComponent; }
		TransformComponent* GetParent() const { return m_Parent; }

		const DirectX::XMFLOAT4X4& GetTransform() { return m_Transform; }

	private:
		TransformComponent* m_Parent = nullptr;
		DirectX::XMFLOAT4X4 m_Transform = Matrix4x4::Identity();
	};


	/*물리적인 위치와 눈에 보이는 렌더링 가능한 오브젝트를 가지고 있는 컴포넌트들의 베이스 클래스*/
	class RenderComponent : public TransformComponent
	{
	public:
		RenderComponent() = default;
		virtual ~RenderComponent() = default;

		virtual void Update(float deltaTime) override
		{
			TransformComponent::Update(deltaTime);
		}
	};
}
