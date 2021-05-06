#pragma once
#include <DirectXMath.h>

#include "Crystal/GamePlay/Objects/Object.h"

namespace Crystal {
	class Component;
	class TransformComponent;
	class Level;
	class World;


	// Spawn-able object
	class Actor : public Object
	{
	public:
		struct ActorSpawnParams
		{
			std::string Name;
			Level* Level = nullptr;
		};
	public:
		Actor() = default;
		~Actor() override = default;

		void Initialize() override;

		void OnCreate() override;

		virtual void Begin();

		virtual void End()
		{
		}

		void UpdateComponents(float deltaTime);
		/*Component�� actor�� �����̳ʿ� �����ϰ� Owner�� ���� Actor�� �����մϴ�.*/
		void AddComponent(const std::shared_ptr<Component>& component);
		void RegisterComponents();

		/*Component�� TransformComponent���� Hierarchy�� �ű�ϴ�.*/
		void MoveToTransformComponentHierarchy(const std::shared_ptr<TransformComponent>& component);

		std::weak_ptr<TransformComponent> GetMainComponent() const { return m_MainComponent; }

		void SetScale(float scale);
		float GetScale() const;
		
		void SetPosition(const DirectX::XMFLOAT3& position);
		DirectX::XMFLOAT3 GetPosition() const;

		void SetRotation(const DirectX::XMFLOAT4& rotation);
		DirectX::XMFLOAT4 GetRotation() const;
		
		

		std::weak_ptr<Component> GetComponentByName(const std::string& name);
		std::weak_ptr<Component> GetComponentByClass(const std::string& classType);

		void SetIsDead(bool bIsDead) { m_bIsDead = bIsDead; }
		bool GetIsDead() const { return m_bIsDead; }
		void Destroy() { m_bIsDead = true; }
		

		STATIC_TYPE_IMPLE(Actor)
	protected:
		/*Component�� �����ϰ� Register�մϴ�.*/
		template <class T>
		std::shared_ptr<T> CreateComponent(const std::string& name = "")
		{
			/*TODO : Component�� �ް� üũ...*/
			std::shared_ptr<T> newComponent = CreateObject<T>(name, weak_from_this());
			AddComponent(newComponent);
			return newComponent;
		}


	protected:
		std::shared_ptr<TransformComponent> m_MainComponent = nullptr;
		std::vector<std::shared_ptr<Component>> m_Components;
		/*�θ�, �ڽļ����� Transform �� ��ġ�Ǿ��ִ� �����̳� (MainComponent ����)*/
		std::vector<std::shared_ptr<TransformComponent>> m_TransformHierarchy;

		bool m_bIsDead = false;

	
	};

}
