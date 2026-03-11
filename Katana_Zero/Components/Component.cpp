#include "pch.h"
#include "Component.h"
#include "../Objects/Actors/Actor.h"

Vector2 Component::GetPos() const
{
    if(_owner) return _owner->GetPos();

    return { 0, 0 };
}
