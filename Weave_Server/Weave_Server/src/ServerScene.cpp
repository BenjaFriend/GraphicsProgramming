#include "stdafx.h"
#include "ServerScene.h"

ServerScene::ServerScene()
{
    LOG_TRACE( "Create a server scene!" );
    EntityPool = new ObjectPool< Entity >( MAX_ENTITY_COUNT );

    Entity* EntAray_Raw = EntityPool->GetRaw();

    for ( size_t i = 0; i < MAX_ENTITY_COUNT; ++i )
    {
        EntAray_Raw[ i ].SetIsInUse( false );
    }

}

ServerScene::~ServerScene()
{
    NetworkIdToEntityMap.clear();

    SAFE_DELETE( EntityPool );
    LOG_TRACE( "DELETE a server scene!" );
}

void ServerScene::Write( OutputMemoryBitStream & inOutputStream, uint32_t inDirtyState ) const
{
    inOutputStream.Write( static_cast< UINT32 > ( NetworkIdToEntityMap.size() ) );

    // #TODO Have a replication manager to only write dirty states out
    // and what actions to take on the given object
    for ( const auto & entity : NetworkIdToEntityMap )
    {
        if ( !entity.second->GetIsInUse() )
        {
            continue;
        }
        // #TODO Read in the class ID for this object
        entity.second->Write( inOutputStream );
        // Reset our replication action
        entity.second->SetReplicationAction( EReplicationAction::ERA_Update );
    }
}

void ServerScene::Update( float deltaTime, float totalTime )
{
    Entity* EntityArray_Raw = EntityPool->GetRaw();

    for ( size_t i = 0; i < MAX_ENTITY_COUNT; ++i )
    {
        if ( EntityArray_Raw[ i ].GetIsInUse() && EntityArray_Raw[ i ].GetIsActive() )
        {
            EntityArray_Raw[ i ].Update( deltaTime );
        }
    }
}

Entity* ServerScene::AddEntity( const std::string & aName, UINT32 aID, const EReplicatedClassType aClassType)
{
    Entity* newEnt = EntityPool->GetResource();

    assert( newEnt != nullptr );

    newEnt->SetName( aName );
    newEnt->SetNetworkID( aID );
    newEnt->SetIsActive( true );
    newEnt->SetIsInUse( true );

    // Setup replication actions
    newEnt->SetReplicationAction( EReplicationAction::ERA_Create );
    newEnt->SetDirtyState( Entity::EIEntityReplicationState::EIRS_AllState );
    newEnt->SetReplicationClassType( aClassType ); 
    
    // Add this object to the replication map
    AddReplicatedObject( newEnt );

    return newEnt;
}
