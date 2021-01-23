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


	/*�������� ��ġ�� ��Ÿ���� Ư���� ������ �ִ� ������Ʈ���� ���̽� Ŭ����
	MainComponent�� �� �� �ְ�, �ִ� �ϳ��� �θ� ������ �θ��� ����� Transform�� ����*/
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


	/*�������� ��ġ�� ���� ���̴� ������ ������ ������Ʈ�� ������ �ִ� ������Ʈ���� ���̽� Ŭ����*/
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
