#pragma once
#include "TransformComponent.h"
#include "Crystal/Math/Math.h"

namespace Crystal {
	enum class ECameraProjectionMode : uint32_t
	{
		CPM_Persepective,
		CPM_Orthographic
	};

	/* 게임내에서의 카메라를 나타내는 클래스입니다 */
	class CameraComponent : public TransformComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<TransformComponent>(*this);
			ar & m_View;
			ar & m_Projection;
			ar & m_ViewProjection;
			ar & m_Viewport;
			ar & m_ScissorRect;

			ar & m_FieldOfView;
			ar & m_NearPlane;
			ar & m_FarPlane;

			ECameraProjectionMode m_ProjectionMode = ECameraProjectionMode::CPM_Persepective;
		}

	public:
		CameraComponent();
		virtual ~CameraComponent() = default;

		void RegisterComponent() override;

		void Update(const float deltaTime) override;

		void SetViewport(const D3D12_VIEWPORT& viewport) { m_Viewport = viewport; }
		void SetScissorRect(const D3D12_RECT& scissorRect) { m_ScissorRect = scissorRect; }
		void SetFieldOfView(float fieldOfView) { m_FieldOfView = fieldOfView; }
		void SetNearPlane(float nearPlane) { m_NearPlane = nearPlane; }
		void SetFarPlane(float farPlane) { m_FarPlane = farPlane; }
		void SetProjectionMode(ECameraProjectionMode mode) { m_ProjectionMode = mode; }

		const D3D12_VIEWPORT& GetViewport() const { return m_Viewport; }
		const D3D12_RECT& GetScissorRect() const { return m_ScissorRect; }

		/*VP와 InverseVP의 값은 매 프레임마다 Update가 불린 직후 어떠한 속성도 바뀌지 않았을 때만 보장됩니다.*/
		const DirectX::XMFLOAT4X4& GetViewProjection() const { return m_ViewProjection; }
		const DirectX::XMFLOAT4X4& GetView() const { return m_View; }
		const DirectX::XMFLOAT4X4& GetProjection() const { return m_Projection; }

		STATIC_TYPE_IMPLE(CameraComponent)

	private:
		void computeViewProjection();

	private:
		DirectX::XMFLOAT4X4 m_View = {};
		DirectX::XMFLOAT4X4 m_Projection = {};
		DirectX::XMFLOAT4X4 m_ViewProjection = {};
		D3D12_VIEWPORT m_Viewport = {};
		D3D12_RECT m_ScissorRect = {};

		float m_FieldOfView = 60.0f;
		float m_NearPlane = 0.1f;
		float m_FarPlane = 1000.0f;

		ECameraProjectionMode m_ProjectionMode = ECameraProjectionMode::CPM_Persepective;
	};
}
