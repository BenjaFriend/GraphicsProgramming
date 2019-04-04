#pragma once

#include "stdafx.h"
#include "MemoryBitStream.h"
#include "Transform.h"
#include "ECS/ComponentManager.h"

#include <memory>

/////////////////////////////////////////////////
// Forward Declarations
class Component;

class IEntity
{
public:

    enum EIEntityReplicationState
    {
        EIRS_POS        = 1 << 0,
        EIRS_ROT        = 1 << 1,
        EIRS_AllState = EIRS_POS | EIRS_ROT
    };

    IEntity();

    virtual ~IEntity();


    /** Get the current transform of this object */
    FORCE_INLINE Transform* GetTransform() const { return EntityTransform; }

    /// <summary>
    /// If an entity is destroyable on load, then it will be deleted during a 
    /// scene change. If not, then it will remain persistent throughout scenes
    /// </summary>
    /// <returns>True if destroyable</returns>
    FORCE_INLINE const bool GetIsDestroyableOnLoad() const { return IsDestroyableOnLoad; }

    FORCE_INLINE void SetIsDestroyableOnLoad( const bool aVal ) { IsDestroyableOnLoad = aVal; }

    FORCE_INLINE const bool GetIsValid() const { return IsValid; }

    /// <summary>
    /// Sets if this entity is active or not
    /// </summary>
    /// <param name="aStatus">True if active, false if in-active</param>
    FORCE_INLINE void SetIsActive( const bool aStatus ) { IsActive = aStatus; }

    /// <summary>
    /// Get if this entity is active or not
    /// </summary>
    /// <returns>True if active, false if in-active</returns>
    FORCE_INLINE const bool GetIsActive() const { return IsActive; }

    /// <summary>
    /// Get this entity's name
    /// </summary>
    /// <returns>Reference to the name of this entity</returns>
    FORCE_INLINE const std::string & GetName() const { return Name; }

    /// <summary>
    /// Set the name of this entity
    /// </summary>
    /// <param name="newName">The new name of this entity</param>
    FORCE_INLINE void SetName( std::string newName ) { Name = newName; }

    FORCE_INLINE void SetIsValid( const bool aValid ) { IsValid = aValid; }

    FORCE_INLINE const size_t GetID() const { return this->entID; }

    // Components ------------------------------

    /// <summary>
    /// Get a pointer to this component on this entity
    /// </summary>
    /// <returns>
    /// Pointer to the component if it exists, nullptr 
    /// if this entity does not have a component of this type
    /// </returns>
    template<typename T>
    T* GetComponent()
    {
        return this->componentManager->GetComponent<T>( this->entID );
    }

    /// <summary>
    /// Add a component of type T to this entity
    /// </summary>
    template<class T, class ...P>
    T* AddComponent( P&&... param )
    {
        return
            this->componentManager->AddComponent<T>(
                this,
                this->entID,
                std::forward<P>( param )...
                );
    }

    /// <summary>
    /// Remove the component of this type from this entity
    /// </summary>
    template<typename T>
    void RemoveComponent()
    {
        this->componentManager->RemoveComponent<T>( this->entID );
    }

    FORCE_INLINE const ECS::ComponentMap * GetAllComponents() const
    {
        return componentManager->GetAllComponents( this->entID );
    }

    FORCE_INLINE void RemoveAllComponents() { componentManager->RemoveAllEntityComponents( entID ); }

    /// <summary>
    /// Write this component to a replicated bit stream
    /// </summary>
    /// <param name="inOutputStream"></param>
    /// <param name="inDirtyState"></param>
    virtual void Write( OutputMemoryBitStream & inOutputStream ) const;

    /// <summary>
    /// Read this component from a bit stream
    /// </summary>
    /// <param name="inInputStream"></param>
    virtual void Read( InputMemoryBitStream & inInputStream );

    FORCE_INLINE const INT32  GetNetworkID() const { return NetworkID; }
    FORCE_INLINE void SetNetworkID( const INT32 aID ) { NetworkID = aID; }

    FORCE_INLINE void SetDirtyState( const UINT32 aVal ) { DirtyState = aVal; ReplicationAction = ERA_Update; }
    FORCE_INLINE UINT32 GetDirtyState() { return DirtyState; }
    FORCE_INLINE bool HasDirtyState() { return ( DirtyState != 0 ); }

    FORCE_INLINE const EReplicationAction GetReplicationAction() { return ReplicationAction; }
    FORCE_INLINE void SetReplicationAction( const EReplicationAction aAct ) { ReplicationAction = aAct; }

protected:

    virtual void WriteUpdateAction( OutputMemoryBitStream& inOutputStream, UINT32 inDirtyState ) const;

    virtual void ReadUpdateAction( InputMemoryBitStream& inInputStream );

    static size_t EntityCount;

    INT32 NetworkID = -1;

    /** handles the adding/removing of components for this entity */
    ECS::ComponentManager * componentManager = nullptr;

    /** The transform of the entity */
    Transform* EntityTransform = nullptr;

    /** The unique ID of this entity */
    size_t entID;

    /** If this entity is dirty of not */
    UINT32 DirtyState = EIEntityReplicationState::EIRS_AllState;

    /** The replication action for this entity to take */
    EReplicationAction ReplicationAction = EReplicationAction::ERA_Create;

    /** Flag for if this entity is active or not */
    UINT32 IsActive : 1;

    /** If true, then this entity will get destroyed when  */
    UINT32 IsDestroyableOnLoad : 1;

    /** If true, then this entity has been initialized and is valid in the memory pool */
    UINT32 IsValid : 1;

    /** The name of this object */
    std::string Name = "Default Entity";
};

// Use smart pointers for the client proxy to have safer exits
typedef std::shared_ptr< IEntity >	IEntityPtr;