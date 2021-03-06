#pragma once
#include <DirectXMath.h>

namespace Crystal {
	namespace Vector3
	{
		const DirectX::XMFLOAT3 UnitX = { 1.0f, 0.0f, 0.0f };
		const DirectX::XMFLOAT3 UnitY = { 0.0f, 1.0f, 0.0f };
		const DirectX::XMFLOAT3 UnitZ = { 0.0f, 0.0f, 1.0f };
		const DirectX::XMFLOAT3 Zero = { 0.0f, 0.0f, 0.0f };

		const DirectX::XMFLOAT3 White = { 1.0f, 1.0f, 1.0f };
		const DirectX::XMFLOAT3 Red = { 1.0f, 0.0f, 0.0f };
		const DirectX::XMFLOAT3 Green = { 0.0f, 1.0f, 0.0f };
		const DirectX::XMFLOAT3 Blue = { 0.0f, 0.0f, 1.0f };

		const DirectX::XMFLOAT3 Cyan = { 0.0f, 1.0f, 1.0f };
		const DirectX::XMFLOAT3 Magenta = { 1.0f, 0.0f, 1.0f };
		const DirectX::XMFLOAT3 Yellow = { 1.0f, 1.0f, 0.0f };

		inline DirectX::XMFLOAT3 Add(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVectorAdd(XMLoadFloat3(&v1), XMLoadFloat3(&v2));
			XMStoreFloat3(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT3 Subtract(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVectorSubtract(XMLoadFloat3(&v1), XMLoadFloat3(&v2));
			XMStoreFloat3(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT3 Multiply(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVectorMultiply(XMLoadFloat3(&v1), XMLoadFloat3(&v2));
			XMStoreFloat3(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT3 Multiply(const DirectX::XMFLOAT3& v1, float f1)
		{
			DirectX::XMFLOAT3 result;

			DirectX::XMVECTOR newVector = DirectX::XMVectorMultiply(XMLoadFloat3(&v1), DirectX::XMVectorSet(f1, f1, f1, 0.0f));
			XMStoreFloat3(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT3 Divide(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVectorDivide(XMLoadFloat3(&v1), XMLoadFloat3(&v2));
			XMStoreFloat3(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT3 Divide(const DirectX::XMFLOAT3& v1, float f1)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVectorDivide(XMLoadFloat3(&v1), DirectX::XMVectorSet(f1, f1, f1, 0.0f));
			XMStoreFloat3(&result, newVector);
			return result;
		}

		inline bool Equal(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
		{
			return DirectX::XMVector3Equal(XMLoadFloat3(&v1), XMLoadFloat3(&v2));
		}

		inline float Greater(const DirectX::XMFLOAT3& v1)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVector3LengthSq(XMLoadFloat3(&v1));
			XMStoreFloat3(&result, newVector);
			return result.x;
		}

		inline float Length(const DirectX::XMFLOAT3& v1)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVector3Length(XMLoadFloat3(&v1));
			XMStoreFloat3(&result, newVector);
			return result.x;
		}

		inline float LengthSquared(const DirectX::XMFLOAT3& v1)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVector3LengthSq(XMLoadFloat3(&v1));
			XMStoreFloat3(&result, newVector);
			return result.x;
		}

		inline float Dot(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVector3Dot(XMLoadFloat3(&v1), XMLoadFloat3(&v2));
			XMStoreFloat3(&result, newVector);
			return result.x;
		}

		inline DirectX::XMFLOAT3 Cross(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVector3Cross(XMLoadFloat3(&v1), XMLoadFloat3(&v2));
			XMStoreFloat3(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& v1)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVector3Normalize(XMLoadFloat3(&v1));
			XMStoreFloat3(&result, newVector);
			return result;
		}

		inline bool IsZero(const DirectX::XMFLOAT3& v1)
		{
			bool isZero = DirectX::XMVector3Equal(XMLoadFloat3(&v1), DirectX::XMVectorZero());
			return isZero;
		}

		inline DirectX::XMFLOAT3 Clamp(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2, const DirectX::XMFLOAT3& v3)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVectorClamp(XMLoadFloat3(&v1), XMLoadFloat3(&v2), XMLoadFloat3(&v3));
			XMStoreFloat3(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT3 RotateQuaternion(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT4& quaternion)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVector3Rotate(XMLoadFloat3(&v1), XMLoadFloat4(&quaternion));
			XMStoreFloat3(&result, newVector);
			return result;
		}
	}

	namespace Vector4
	{
		const DirectX::XMFLOAT4 Zero = { 0.0f, 0.0f, 0.0f, 0.0f };


		namespace Quaternion {
			const DirectX::XMFLOAT4 Identity = { 0.0f, 0.0f, 0.0f, 1.0f };
		}

		inline DirectX::XMFLOAT4 Multiply(const DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
		{
			DirectX::XMFLOAT4 result;
			DirectX::XMVECTOR newVector = DirectX::XMVectorMultiply(XMLoadFloat4(&v1), XMLoadFloat4(&v2));
			XMStoreFloat4(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT4 Multiply(const DirectX::XMFLOAT4& v1, float f1)
		{
			DirectX::XMFLOAT4 result;

			DirectX::XMVECTOR newVector = DirectX::XMVectorMultiply(XMLoadFloat4(&v1), DirectX::XMVectorSet(f1, f1, f1, 0.0f));
			XMStoreFloat4(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT4 Divide(const DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
		{
			DirectX::XMFLOAT4 result;
			DirectX::XMVECTOR newVector = DirectX::XMVectorDivide(XMLoadFloat4(&v1), XMLoadFloat4(&v2));
			XMStoreFloat4(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT4 Divide(const DirectX::XMFLOAT4& v1, float f1)
		{
			DirectX::XMFLOAT4 result;
			DirectX::XMVECTOR newVector = DirectX::XMVectorDivide(XMLoadFloat4(&v1), DirectX::XMVectorSet(f1, f1, f1, 0.0f));
			XMStoreFloat4(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT4 QuaternionMultiply(const DirectX::XMFLOAT4& q1, const DirectX::XMFLOAT4& q2)
		{
			DirectX::XMFLOAT4 result;
			DirectX::XMVECTOR newVector = DirectX::XMQuaternionMultiply(XMLoadFloat4(&q1), XMLoadFloat4(&q2));
			XMStoreFloat4(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT4 QuaternionInverse(const DirectX::XMFLOAT4& q1)
		{
			DirectX::XMFLOAT4 result;
			DirectX::XMVECTOR newVector = DirectX::XMQuaternionInverse(XMLoadFloat4(&q1));
			XMStoreFloat4(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT4 QuaternionRotationAxis(const DirectX::XMFLOAT3& axis, float angle)
		{
			DirectX::XMFLOAT4 result;
			DirectX::XMVECTOR newVector = DirectX::XMQuaternionRotationAxis(XMLoadFloat3(&axis), (angle));
			XMStoreFloat4(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT4 QuaternionRollPitchYaw(const DirectX::XMFLOAT3& rollPitchYaw)
		{
			DirectX::XMFLOAT4 result;
			DirectX::XMVECTOR newVector = DirectX::XMQuaternionRotationRollPitchYaw((rollPitchYaw.y),
				(rollPitchYaw.z), (rollPitchYaw.x));
			XMStoreFloat4(&result, newVector);
			return result;
		}
	}

	namespace Matrix4x4
	{
		inline DirectX::XMFLOAT4X4 Identity()
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixIdentity();
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 Translation(const DirectX::XMFLOAT3& translation)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&translation));
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 Scale(const DirectX::XMFLOAT3& scale)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&scale));
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 Scale(const float scale)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMFLOAT3 scaleVector = { scale , scale , scale };
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&scaleVector));
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 Transpose(const DirectX::XMFLOAT4X4& m1)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixTranspose(XMLoadFloat4x4(&m1));
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 Inverse(const DirectX::XMFLOAT4X4& m1)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(XMLoadFloat4x4(&m1)), XMLoadFloat4x4(&m1));
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 Multiply(const DirectX::XMFLOAT4X4& m1, const DirectX::XMFLOAT4X4& m2)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixMultiply(XMLoadFloat4x4(&m1), XMLoadFloat4x4(&m2));
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 RotationQuaternion(const DirectX::XMFLOAT4& quat)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&quat));
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 RotationRollPitchYaw(const DirectX::XMFLOAT3& r1)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixRotationRollPitchYaw((r1.x), (r1.y), (r1.z));
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 LookAt(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& lookAt, const DirectX::XMFLOAT3& up)
		{
			DirectX::XMFLOAT4X4 result;

			DirectX::XMVECTOR vPosition = XMLoadFloat3(&position);
			DirectX::XMVECTOR vLookAt = XMLoadFloat3(&lookAt);
			DirectX::XMVECTOR vUp = XMLoadFloat3(&up);

			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixLookAtLH(vPosition, vLookAt, vUp);
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 LookTo(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& lookTo, const DirectX::XMFLOAT3& up)
		{
			DirectX::XMFLOAT4X4 result;

			DirectX::XMVECTOR vPosition = XMLoadFloat3(&position);
			DirectX::XMVECTOR vLookAt = XMLoadFloat3(&lookTo);
			DirectX::XMVECTOR vUp = XMLoadFloat3(&up);

			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixLookToLH(vPosition, vLookAt, vUp);
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 Perspective(float fovY, float aspect, float nearPlane, float farPlane)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixPerspectiveFovLH((fovY), aspect, nearPlane, farPlane);
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 OrthoGraphic(float viewWidth, float viewHeight, float nearPlane, float farPlane)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearPlane, farPlane);
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 RotationX(float angle)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixRotationX((angle));
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 RotationY(float angle)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixRotationY((angle));
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 RotationZ(float angle)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixRotationZ((angle));
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}
	}

	namespace Matrix3x3
	{
		inline DirectX::XMFLOAT3X3 Transpose(const DirectX::XMFLOAT3X3& m1)
		{
			DirectX::XMFLOAT3X3 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixTranspose(XMLoadFloat3x3(&m1));
			XMStoreFloat3x3(&result, newMatrix);
			return result;
		}
	}
}
