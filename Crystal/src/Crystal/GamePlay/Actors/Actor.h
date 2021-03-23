#pragma once

namespace Crystal {
	class Component;
	class TransformComponent;

	class Actor
	{
	public:
		Actor() = default;
		virtual ~Actor() = default;

		virtual void Begin() {}
		virtual void End() {}
		virtual void Update(const float deltaTime) {}
		void UpdateComponents(float deltaTime);

		/*Component를 actor의 컨테이너에 저장하고 Owner를 현재 Actor로 지정합니다.*/
		void RegisterComponent(Component* component);
		/*Component를 actor의 컨테이너에서 삭제하고 Owner를 nullptr로 지정합니다.*/
		void UnRegisterComponent(Component* component);

		/*Component를 TransformComponent들의 Hierarchy에 옮깁니다.*/
		void MoveToTransformComponentHierarchy(TransformComponent* component);

		TransformComponent* GetMainComponent() const { return m_MainComponent; }

	protected:
		/*Component를 생성하고 Register합니다.*/
		template <class T>
		T* CreateComponent(const std::string& name)
		{
			/*TODO : Component만 받게 체크...*/
			T* newComponent = new T(name);
			newComponent->OnCreate();
			RegisterComponent(newComponent);
			return newComponent;
		}
	protected:
		/*MainComponent를 가르키는 Raw Pointer입니다. 편의를 위한 Pointer일 뿐, 
		LiftTime은 컨테이너의 Unique Pointer 에 의해 관리됩니다.*/
		TransformComponent* m_MainComponent = nullptr;
	private:
		std::vector<std::unique_ptr<Component>> m_Components;
		/*부모, 자식순으로 Transform 가 배치되어있는 컨테이너 (MainComponent 제외)*/
		std::vector<std::unique_ptr<TransformComponent>> m_TransformHierarchy;
	};
}
