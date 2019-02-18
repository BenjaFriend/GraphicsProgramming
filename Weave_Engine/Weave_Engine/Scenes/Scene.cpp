#include "../stdafx.h"
#include "Scene.h"

#include "../Resources/Materials/Material.h"
#include "../Resources/Mesh.h"

using namespace SceneManagement;

Scene::Scene()
{
}


Scene::~Scene()
{
    UnloadAllEntities();
}

FORCE_INLINE Entity * Scene::AddEntity( std::string aName )
{
    Entity* tempEnt = new Entity( aName );
    EntityArray.push_back( tempEnt );
    return tempEnt;
}

FORCE_INLINE Entity * Scene::AddEntity( std::string aName, glm::vec3 aPos )
{
    Entity* tempEnt = new Entity( aName, aPos );

    EntityArray.push_back( tempEnt );
    return tempEnt;
}

FORCE_INLINE Entity * Scene::AddEntityFromfile( nlohmann::json const & aFile )
{
    Entity* tempEnt = new Entity( aFile );
    EntityArray.push_back( tempEnt );
    return tempEnt;
}

FORCE_INLINE void Scene::DeleteEntity( Entity * aEntity )
{
    if ( aEntity == nullptr ) return;
    // Remove this entity from the vector
    for ( size_t i = 0; i < EntityArray.size(); ++i )
    {
        if ( EntityArray [ i ] == aEntity )
        {
            EntityArray.erase( EntityArray.begin() + i );
        }
    }

    delete aEntity;
    aEntity = nullptr;
}

void Scene::UnloadAllEntities()
{
    // Delete each entity that has been added
    for ( auto it = EntityArray.begin(); it != EntityArray.end(); ++it )
    {
        if ( *it != nullptr )
            delete ( *it );
    }

    EntityArray.clear();
}