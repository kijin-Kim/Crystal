#pragma once
#include <DirectXMath.h>

#include "Crystal/GamePlay/Objects/Object.h"
#include "Crystal/Math/Math.h"


namespace Crystal {
	class Component;
	class TransformComponent;
	class Level;
	class World;


	
	// Spawn-able object
	class Actor : public Object
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<Object>(*this);
			ar & *m_MainComponent;
			for(auto& c : m_Components)
			{
				ar& c;
			}

			for(auto& c : m_TransformHierarchy)
			{
				ar& c;
			}
		}
		
	public:
		struct ActorSpawnParams
		{
			std::string Name;
			DirectX::XMFLOAT3 Position = Vector3::Zero;
			DirectX::XMFLOAT4 Rotation = Vector4::Zero;
			float Scale = 0.0f;
			Weak<Actor> Instigator;
			Level* Level = nullptr;
		};

	public:
		Actor() = default;
		~Actor() override = default;

		void Initialize() override;

		void OnCreate() override;

		void Begin() override;
		
		void UpdateComponents(float deltaTime);
		/*Component�� actor�� �����̳ʿ� �����ϰ� Owner�� ���� Actor�� �����մϴ�.*/
		void AddComponent(const std::shared_ptr<Component>& component);
		void RegisterComponents();

		/*Component�� TransformComponent���� Hierarchy�� �ű�ϴ�.*/
		void MoveToTransformComponentHierarchy(const std::shared_ptr<TransformComponent>& component);

		std::weak_ptr<TransformComponent> GetMainComponent() const { return m_MainComponent; }

		void SetUnitScale(float scale);
		const DirectX::XMFLOAT3& GetScale() const;

		void SetPosition(const DirectX::XMFLOAT3& position);
		DirectX::XMFLOAT3 GetPosition() const;

		void SetRotationQuat(const DirectX::XMFLOAT4& rotation);
		DirectX::XMFLOAT4 GetRotationQuat() const;

		
		DirectX::XMFLOAT3 GetForwardVector() const;

		const DirectX::XMFLOAT4X4& GetWorldTransform() const;


		void RotatePitch(float angle);
		void RotateYaw(float angle);
		void RotateRoll(float angle);

		void AddForce(const DirectX::XMFLOAT3& force);


		std::weak_ptr<Component> GetComponentByName(const std::string& name);
		std::weak_ptr<Component> GetComponentByClass(const std::string& classType);


		void SetIsDead(bool bIsDead) { m_bIsDead = bIsDead; }
		bool GetIsDead() const { return m_bIsDead; }
		virtual void Destroy() { m_bIsDead = true; }

		virtual void OnTakeDamage(float damage, Weak<Actor> damageCauser) {}

		Weak<Level> GetLevel() const;
		Weak<World> GetWorld() const;

		float GetDeltaTime() const;

		void SetInstigator(Weak<Actor> instigator) { m_Instigator = instigator; }
		Weak<Actor> GetInstigator() const { return m_Instigator; }


		STATIC_TYPE_IMPLE(Actor)
	protected:
		/*Component�� �����ϰ� Register�մϴ�.*/
		template <class T>
		std::shared_ptr<T> CreateComponent(const std::string& name = "")
		{
			/*TODO : Component�� �ް� üũ...*/
			std::shared_ptr<T> newComponent = CreateObject<T>();
			newComponent->SetObjectName(name);
			newComponent->SetOuter(weak_from_this());
			AddComponent(newComponent);
			return newComponent;
		}


	protected:
		std::shared_ptr<TransformComponent> m_MainComponent = nullptr;
		std::vector<std::shared_ptr<Component>> m_Components;
		/*�θ�, �ڽļ����� Transform �� ��ġ�Ǿ��ִ� �����̳� (MainComponent ����)*/
		std::vector<std::shared_ptr<TransformComponent>> m_TransformHierarchy;

		bool m_bIsDead = false;

		Weak<Actor> m_Instigator = {};
	};


}
