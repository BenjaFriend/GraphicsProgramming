#include "stdafx.h"
#include "ServerScene.h"

ServerScene::ServerScene()
{
    LOG_TRACE( "Create a server scene!" );
}

ServerScene::~ServerScene()
{
    EntityArray.clear();
    NetworkIdToEntityMap.clear();
    LOG_TRACE( "DELETE a server scene!" );
}

void ServerScene::Write( OutputMemoryBitStream & inOutputStream, uint32_t inDirtyState ) const
{
    inOutputStream.Write( static_cast< UINT32 > ( NetworkIdToEntityMap.size() ) );

    // #TODO Have a replication manager to only write dirty states out
    // and what actions to take on the given object

    for ( const auto & entity : NetworkIdToEntityMap )
    {
        // #TODO Read in the class ID for this object
        entity.second->Write( inOutputStream );
        // Reset our replication action
        entity.second->SetReplicationAction( EReplicationAction::ERA_Update );
    }
}

IEntityPtr ServerScene::AddEntity( const std::string & aName, UINT32 aID, const EReplicatedClassType aClassType)
{
    IEntityPtr newEnt = std::make_shared<IEntity>();
    newEnt->SetName( aName );
    newEnt->SetNetworkID( aID );
    EntityArray.push_back( newEnt );
    // Setup replication actions
    newEnt->SetReplicationAction( EReplicationAction::ERA_Create );
    newEnt->SetDirtyState( IEntity::EIEntityReplicationState::EIRS_AllState );
    newEnt->SetReplicationClassType( aClassType ); 
    // Add this object to the replication map
    AddReplicatedObject( newEnt.get() );

    return newEnt;
}
