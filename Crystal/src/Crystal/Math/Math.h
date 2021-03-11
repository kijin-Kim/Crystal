#pragma once
#include <DirectXMath.h>

namespace Crystal {

	namespace Vector3
	{
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

		inline DirectX::XMFLOAT3 Divide(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVectorDivide(XMLoadFloat3(&v1), XMLoadFloat3(&v2));
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

		inline float LengthSquared(const DirectX::XMFLOAT3& v1)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMVECTOR newVector = DirectX::XMVector3LengthSq(XMLoadFloat3(&v1));
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
		inline DirectX::XMFLOAT4 QuternionRotationAxis(const DirectX::XMFLOAT3& axis, float angle)
		{
			DirectX::XMFLOAT4 result;
			DirectX::XMVECTOR newVector = DirectX::XMQuaternionRotationAxis(XMLoadFloat3(&axis), DirectX::XMConvertToRadians(angle));
			XMStoreFloat4(&result, newVector);
			return result;
		}

		inline DirectX::XMFLOAT4 QuaternionRollPitchYaw(const DirectX::XMFLOAT3& rollPitchYaw)
		{
			DirectX::XMFLOAT4 result;
			DirectX::XMVECTOR newVector = DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(rollPitchYaw.y),
				DirectX::XMConvertToRadians(rollPitchYaw.z), DirectX::XMConvertToRadians(rollPitchYaw.x));
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
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(r1.x), DirectX::XMConvertToRadians(r1.y), DirectX::XMConvertToRadians(r1.z));
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
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fovY), aspect, nearPlane, farPlane);
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
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(angle));
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 RotationY(float angle)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(angle));
			XMStoreFloat4x4(&result, newMatrix);
			return result;
		}

		inline DirectX::XMFLOAT4X4 RotationZ(float angle)
		{
			DirectX::XMFLOAT4X4 result;
			DirectX::XMMATRIX newMatrix = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(angle));
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
