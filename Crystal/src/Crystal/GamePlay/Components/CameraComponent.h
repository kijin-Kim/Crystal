#pragma once
#include "BaseComponents.h"
#include "Crystal/Math/Math.h"

namespace Crystal {
	enum class ECameraProjectionMode
	{
		CPM_Persepective,
		CPM_Orthographic
	};

	/* ���ӳ������� ī�޶� ��Ÿ���� Ŭ�����Դϴ� */
	class CameraComponent : public TransformComponent
	{
	public:
		CameraComponent(const std::string& name);
		virtual ~CameraComponent() = default;

		void Update(const float deltaTime) override
		{
			TransformComponent::Update(deltaTime);

			computeViewProjection();
		}

		void SetViewport(const D3D12_VIEWPORT& viewport) { m_Viewport = viewport;}
		void SetScissorRect(const D3D12_RECT& scissorRect) { m_ScissorRect = scissorRect;}
		void SetFieldOfView(float fieldOfView) { m_FieldOfView = fieldOfView; }
		void SetNearPlane(float nearPlane) { m_NearPlane = nearPlane;}
		void SetFarPlane(float farPlane) { m_FarPlane = farPlane;}
		void SetProjectionMode(ECameraProjectionMode mode) { m_ProjectionMode = mode;}

		const D3D12_VIEWPORT& GetViewport() const { return m_Viewport; }
		const D3D12_RECT& GetScissorRect() const { return m_ScissorRect; }

		/*VP�� InverseVP�� ���� �� �����Ӹ��� Update�� �Ҹ� ���� ��� �Ӽ��� �ٲ��� �ʾ��� ���� ����˴ϴ�.*/
		const DirectX::XMFLOAT4X4& GetViewProjection() const { return m_ViewProjection; }

	private:
		void computeViewProjection();
		
	private:
		DirectX::XMFLOAT4X4 m_ViewProjection = {};
		D3D12_VIEWPORT m_Viewport = {};
		D3D12_RECT m_ScissorRect = {};

		float m_FieldOfView = 60.0f;
		float m_NearPlane = 0.1f;
		float m_FarPlane = 1000.0f;

		ECameraProjectionMode m_ProjectionMode = ECameraProjectionMode::CPM_Persepective;
	};
}