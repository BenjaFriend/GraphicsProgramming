#pragma once

#include "DebugSettings.h"

#include "DXCore.h"
#include "../Resources/SimpleShader.h"
#include <DirectXMath.h>
#include <vector>
#include "InputManager.h"
#include "../Lighting/Lights.h"
#include "../Entity/EntityManager.h"
#include "../Resources/ResourceManager.h"
#include "../Lighting/LightShaderDefs.h"

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
    void OnMouseDown( WPARAM buttonState, int x, int y );
    void OnMouseUp( WPARAM buttonState, int x, int y );
    void OnMouseMove( WPARAM buttonState, int x, int y );
    void OnMouseWheel( float wheelDelta, int x, int y );

private:

    // Initialization helper methods - feel free to customize, combine, etc.
    void LoadShaders();
    void CreateMatrices();
    void CreateBasicGeometry();
    void InitLights();

    // Wrappers for DirectX shaders to provide simplified functionality
    SimpleVertexShader* vertexShader;
    SimplePixelShader* pixelShader;

    // Flying camera for initial testing
    Camera* FlyingCamera = nullptr;

    // Lights
    std::vector<DirectionalLight> DirLights;

    // Keeps track of the old mouse position.  Useful for 
    // determining how far the mouse moved in a single frame.
    POINT prevMousePos;
};
