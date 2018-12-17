#include "../stdafx.h"
#include "BoxCollider.h"
#include "../Entity/EntityManager.h"

using namespace Physics;

BoxCollider::BoxCollider( const VEC3 & aExtents )
    : Extents( aExtents )
{
    CenterOffset = VEC3( 0.f, 0.f, 0.f );
    PhysicsManager::GetInstance()->AddBoxCollider( this );
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::DrawEditorGUI()
{
    ImGui::InputFloat3( "Center Offset", ( float* ) &CenterOffset );
    ImGui::InputFloat3( "Extents", ( float* ) &Extents );
}

void BoxCollider::SaveObject( nlohmann::json & aOutFile )
{
}

const bool BoxCollider::Collides( const BoxCollider & aOther )
{
    VEC3 ownerPos = EntityManager::GetInstance()->GetEntity( owner )->GetPosition();
    VEC3 otherPos = EntityManager::GetInstance()->GetEntity( aOther.GetOwner() )->GetPosition();

    return 
        ownerPos.x - this->Extents.x <= otherPos.x + aOther.Extents.x &&
        ownerPos.x + this->Extents.x >= otherPos.x + aOther.Extents.x &&
        ownerPos.y - this->Extents.y <= otherPos.y + aOther.Extents.y &&
        ownerPos.y + this->Extents.y >= otherPos.y + aOther.Extents.y &&
        ownerPos.z - this->Extents.z <= otherPos.z + aOther.Extents.z && 
        ownerPos.z + this->Extents.z >= otherPos.z + aOther.Extents.z;
}

void BoxCollider::SetCenterOffset( const VEC3 & aVal )
{
    CenterOffset = aVal;
}

const VEC3 & BoxCollider::GetCenterOffset() const
{
    return CenterOffset;
}

const VEC3 Physics::BoxCollider::GetPosition() const
{
    VEC3 worldPos = EntityManager::GetInstance()->GetEntity( owner )->GetPosition();
    worldPos.x += CenterOffset.x;
    worldPos.y += CenterOffset.y;
    worldPos.z += CenterOffset.z;
    return worldPos;
}

void BoxCollider::SetExtents( const VEC3 & aVal )
{
    Extents = aVal;
}

const VEC3 & BoxCollider::GetExtents() const
{
    return Extents;
}

const bool BoxCollider::SetIsTrigger() const
{
    return IsTrigger;
}

void BoxCollider::SetIsTrigger( bool aVal )
{
    IsTrigger = aVal;
}