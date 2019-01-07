#pragma once

#include "../stdafx.h"

#include "DXCore.h"
#include "../Resources/SimpleShader.h"
#include <DirectXMath.h>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>

#include "../Entity/EntityManager.h"
#include "../Resources/ResourceManager.h"
#include "../Lighting/LightShaderDefs.h"
#include "../Physics/Collisions.h"
#include "../Physics/PhysicsManager.h"
#include "../ECS/ComponentManager.h"
#include "../Resources/RenderSystem.h"

#include "json/json.hpp"

// Uncomment to not compile the editor
#define EDITOR_ON

#if defined(EDITOR_ON)

#include "../Editor/EditorCore.h"

#endif

/////////////////////////////////////////////////
// Forward Declarations
class Mesh;
class Entity;
class Camera;
class Material;

class Game
    : public DXCore
{

public:
    Game( HINSTANCE hInstance );
    ~Game();

    // Overridden setup and game loop methods, which
    // will be called automatically
    void Init();
    void OnResize();
    void Update( float deltaTime, float totalTime );
    void Draw( float deltaTime, float totalTime );

    // Overridden mouse input helper methods
    void OnLookDown();
    void OnLookUp();

    void OnMouseMove( WPARAM buttonState, int x, int y );
    void OnMouseWheel( float wheelDelta, int x, int y );

private:

    // Initialization helper methods - feel free to customize, combine, etc.
    void LoadShaders();
    void CreateMatrices();
    void CreateBasicGeometry();
    void InitLights();

    void DrawLightSources();
    void DrawColliders();

    /// <summary>
    /// Save all entities to a scene json file 
    /// </summary>
    void SaveScene();

    /// <summary>
    /// Loads entities from the scene json file 
    /// </summary>
    void LoadScene();

    /** The current scene file to save all entities to */
    char SceneFile[ 64 ] = "Scene_test.json";

    // Wrappers for DirectX shaders to provide simplified functionality
    SimpleVertexShader* vertexShader = nullptr;
    SimplePixelShader* pixelShader = nullptr;
    SimplePixelShader* UnlitPixelShader = nullptr;

    Sampler_ID SamplerID;
    ID3D11RasterizerState* WireFrame = nullptr;

    SRV_ID SkyboxSrvID = -1;
    Mesh_ID CubeMeshID = -1;

    // Skybox resources
    SimpleVertexShader* SkyBoxVS = nullptr;
    SimplePixelShader* SkyBoxPS = nullptr;
    ID3D11RasterizerState* skyRastState = nullptr;
    ID3D11DepthStencilState* skyDepthState = nullptr;



    float Gravity = -0.001f;

#ifdef ENABLE_UI

    ImVec4 BackgroundColor;

#endif // ENABLE_UI

    float LightMoveSpeed = 1.f;

    // Flying camera for initial testing
    Camera* FlyingCamera = nullptr;

    // managers
    ResourceManager* resourceMan = nullptr;
    EntityManager* entityMan = nullptr;

    Mesh_ID PointLightMesh_ID = 0;

    // Keeps track of the old mouse position.  Useful for 
    // determining how far the mouse moved in a single frame.
    POINT prevMousePos;

    // Editor things
    bool UseDirLights = true;
    bool UsePointLights = true;

    bool DrawLightGizmos = true;
    bool DrawSkyBox = true;
    bool DebugDrawColliders = true;

    Entity* SelectedEntity = nullptr;
    ECS::ComponentManager* ComponentMan = nullptr;
    Physics::PhysicsManager* PhysicsMan = nullptr;
    Scripting::ScriptManager* ScriptMan = nullptr;
    RenderSystem* RenderSys = nullptr;

#if defined(EDITOR_ON)

    Editor::EditorCore* editor = nullptr;

#endif
};