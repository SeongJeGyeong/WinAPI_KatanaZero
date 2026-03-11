#include "pch.h"
#include "ComponentContainer.h"
#include "../Components/Component.h"

void ComponentContainer::UpdateComponents(float deltaTime)
{
	for (auto& [priority, ID] : _priorityGroups)
	{
		_componentMap[ID]->UpdateComponent(deltaTime);
	}
}

void ComponentContainer::RenderComponents(HDC hdc)
{
	for (auto& [priority, ID] : _priorityGroups)
	{
		_componentMap[ID]->RenderComponent(hdc);
	}
}

void ComponentContainer::ClearComponents()
{
	for (auto& component : _componentMap)
	{
		SAFE_DELETE(component.second);
	}
	_componentMap.clear();
	_priorityGroups.clear();
}
