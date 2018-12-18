#pragma once

#include "../stdafx.h"

#include "../ECS/Component.h"
#include "PhysicsManager.h"

namespace Physics
{
    /// <summary>
    /// Box collider component for basic collisions
    /// </summary>
    class BoxCollider : public ECS::Component<BoxCollider>
    {
    public:

        /// <summary>
        /// A box collider component to handle simple box collision 
        /// </summary>
        /// <param name="aExtents">The extents of this object</param>
        BoxCollider( const VEC3 & aExtents = VEC3( 1.f, 1.f, 1.f ) );

        ~BoxCollider();

        virtual void DrawEditorGUI() override;

        virtual void SaveObject( nlohmann::json & aOutFile ) override;

        virtual const char* ComponentName() { return "BoxCollider"; }

        const bool Collides( const BoxCollider & aOther );

        ////////////////////////////////////////////////////
        // Accessors
        ////////////////////////////////////////////////////

        void SetCenterOffset( const VEC3 & aVal );

        const VEC3 & GetCenterOffset() const;

        /// <summary>
        /// Get the world position of this box collider with respect 
        /// to the owning object.
        /// </summary>
        /// <returns>VEC3 that is the center of this collider</returns>
        const VEC3 GetPosition() const;

        void SetExtents( const VEC3 & aVal );

        const VEC3 & GetExtents() const;

        const bool SetIsTrigger() const;

        void SetIsTrigger( bool aVal );

    private:

        VEC3 CenterOffset;

        VEC3 Extents;

        bool IsTrigger = false;

    };
}   // namespace Physics