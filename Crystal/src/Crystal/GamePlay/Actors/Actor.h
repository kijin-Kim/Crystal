#pragma once

namespace Crystal {
	class Component;
	class TransformComponent;
	class Level;
	class World;

	class Object
	{
	public:
		Object() = default;
		virtual ~Object() = default;

		virtual void Update(const float deltaTime) {}

		void SetParent(Object* parent) { m_Parent = parent; }
		Object* GetParent() const { return m_Parent; }

	private:
		Object* m_Parent = nullptr;
	};

	class Actor : public Object
	{
	public:
		Actor() = default;
		~Actor() override = default;

		virtual void Begin() {}
		virtual void End() {}
		void Update(const float deltaTime) override { Object::Update(deltaTime); }
		
		void UpdateComponents(float deltaTime);
		/*Component�� actor�� �����̳ʿ� �����ϰ� Owner�� ���� Actor�� �����մϴ�.*/
		void RegisterComponent(Component* component);
		/*Component�� actor�� �����̳ʿ��� �����ϰ� Owner�� nullptr�� �����մϴ�.*/
		void UnRegisterComponent(Component* component);

		/*Component�� TransformComponent���� Hierarchy�� �ű�ϴ�.*/
		void MoveToTransformComponentHierarchy(TransformComponent* component);

		TransformComponent* GetMainComponent() const { return m_MainComponent; }


		void SetPosition(const DirectX::XMFLOAT3& position);

		

	protected:
		/*Component�� �����ϰ� Register�մϴ�.*/
		template <class T>
		T* CreateComponent(const std::string& name)
		{
			/*TODO : Component�� �ް� üũ...*/
			T* newComponent = new T(name);
			newComponent->OnCreate();
			RegisterComponent(newComponent);
			return newComponent;
		}
	protected:
		/*MainComponent�� ����Ű�� Raw Pointer�Դϴ�. ���Ǹ� ���� Pointer�� ��, 
		LiftTime�� �����̳��� Unique Pointer �� ���� �����˴ϴ�.*/
		TransformComponent* m_MainComponent = nullptr;
	private:
		std::vector<std::unique_ptr<Component>> m_Components;
		/*�θ�, �ڽļ����� Transform �� ��ġ�Ǿ��ִ� �����̳� (MainComponent ����)*/
		std::vector<std::unique_ptr<TransformComponent>> m_TransformHierarchy;
	};
}
