#pragma once

class Component;

class ComponentContainer
{
private:
	map<size_t, Component*> _componentMap;
	map<uint32, size_t> _priorityGroups;

public:
	template<typename T>
	bool AddComponent()
	{
		auto component = new T();
		if (_componentMap.contains(T::StaticClass())) return false;
		_componentMap.emplace(T::StaticClass(), component);
		_componentMap.insert(make_pair(T::StaticClass(), component));
		_priorityGroups[component->GetPriority()] = T::StaticClass();
	}

	template<typename T>
	T* GetComponent()
	{
		auto it = _componentMap.find(T::StaticClass());
		if (it != _componentMap.end())
		{
			return static_cast<T*>(it->second);
		}

		return nullptr;
	}

	void UpdateComponents(float deltaTime);
	void RenderComponents(HDC hdc);

	void ClearComponents();
};

