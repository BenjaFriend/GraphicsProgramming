#include "Game.h"
#include "../Resources/Vertex.h"
#include "../Resources/Mesh.h"
#include "../Entity/Entity.h"
#include "../Entity/Camera.h"
#include "../Resources/Materials/Material.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game( HINSTANCE hInstance )
    : DXCore(
        hInstance,		   // The application's handle
        "DirectX Game",	   // Text for the window's title bar
        1280,			   // Width of the window's client area
        720,			   // Height of the window's client area
        true )			   // Show extra stats (fps) in title bar?
{

    vertexShader = 0;
    pixelShader = 0;
    //EntityCount = 0;
    FlyingCamera = new Camera();

#if defined(DEBUG) || defined(_DEBUG)
    // Do we want a console window?  Probably only in debug mode
    CreateConsoleWindow( 500, 120, 32, 120 );
    printf( "Console window created successfully.  Feel free to printf() here." );
#endif

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
    delete vertexShader;
    delete pixelShader;
    delete UnlitPixelShader;
    delete SkyBoxVS;
    delete SkyBoxPS;

    skyRastState->Release();
    skyDepthState->Release();

    EntityManager::ReleaseInstance();

    ResourceManager::ReleaseInstance();

    delete FlyingCamera;

    InputManager::Release();

}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
    EntityManager::GetInstance();

    ResourceManager::Initalize( device );

    // Rasterizer state for drawing the inside of my sky box geometry
    D3D11_RASTERIZER_DESC rs = {};
    rs.FillMode = D3D11_FILL_SOLID;
    rs.CullMode = D3D11_CULL_FRONT;
    rs.DepthClipEnable = true;
    device->CreateRasterizerState( &rs, &skyRastState );

    D3D11_DEPTH_STENCIL_DESC ds = {};
    ds.DepthEnable = true;
    ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    device->CreateDepthStencilState( &ds, &skyDepthState );

    // Helper methods for loading shaders, creating some basic
    // geometry to draw and some simple camera matrices.
    //  - You'll be expanding and/or replacing these later
    LoadShaders();
    CreateMatrices();
    CreateBasicGeometry();
    InitLights();


    // Tell the input assembler stage of the pipeline what kind of
    // geometric primitives (points, lines or triangles) we want to draw.  
    // Essentially: "What kind of shape should the GPU draw with our data?"
    context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
    vertexShader = new SimpleVertexShader( device, context );
    vertexShader->LoadShaderFile( L"VertexShader.cso" );

    pixelShader = new SimplePixelShader( device, context );
    pixelShader->LoadShaderFile( L"PixelShader.cso" );

    UnlitPixelShader = new SimplePixelShader( device, context );
    UnlitPixelShader->LoadShaderFile( L"PixelShader_Unlit.cso" );

    SkyBoxVS = new SimpleVertexShader( device, context );
    SkyBoxVS->LoadShaderFile( L"SkyVS.cso" );

    SkyBoxPS = new SimplePixelShader( device, context );
    SkyBoxPS->LoadShaderFile( L"SkyPS.cso" );

}

void Game::InitLights()
{
    DirectionalLight Light1 = {};
    Light1.AmbientColor = XMFLOAT4( 1.f, 1.f, 1.f, 1.0f ); // Ambient color is the color when we are in shadow
    Light1.DiffuseColor = XMFLOAT4( 1.f, 1.f, 1.f, 1.0f );
    Light1.Direction = XMFLOAT3( 1.0f, 0.0f, 0.0f );
    Light1.Intensity = 0.1f;
    DirLights.emplace_back( Light1 );

    /*DirectionalLight Light2 = {};
    Light2.AmbientColor = XMFLOAT4( 1.f, 1.f, 1.f, 1.0f );
    Light2.DiffuseColor = XMFLOAT4( 0.5f, 1.f, 1.f, 0.1f );
    Light2.Direction = XMFLOAT3( -1.0f, 0.0f, 0.5f );
    Light2.Intensity = 1.0f;
    DirLights.emplace_back( Light2 );*/

    XMFLOAT3 Red = XMFLOAT3( 1.0f, 0.0f, 0.0f );
    XMFLOAT3 Blue = XMFLOAT3( 0.0f, 0.0f, 1.0f );
    XMFLOAT3 White = XMFLOAT3( 1.0f, 1.0f, 1.0f );

    PointLight pLight1 = {};
    pLight1.Color = Red;
    pLight1.Position = XMFLOAT3( 0.f, 2.0f, 0.0f );
    pLight1.Intensity = 2.f;
    pLight1.Range = 5.f;
    PointLights.emplace_back( pLight1 );

    PointLight pLight2 = {};
    pLight2.Color = Blue;
    pLight2.Position = XMFLOAT3( 0.f, -1.0f, 0.0f );
    pLight2.Intensity = 5.f;
    pLight2.Range = 2.f;
    PointLights.emplace_back( pLight2 );

}

// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{
    FlyingCamera->UpdateProjectionMatrix( width, height );
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
    // Load in the meshes
    ResourceManager* resources = ResourceManager::GetInstance();
    EntityManager* enMan = EntityManager::GetInstance();
    UINT meshID = resources->LoadMesh( "Assets/Models/sphere.obj" );
    PointLightMesh_ID = meshID;

    UINT floorMeshID = resources->LoadMesh( "Assets/Models/cube.obj" );


    UINT diffSRV = resources->LoadSRV( context, L"Assets/Textures/cobblestone_albedo.png" );
    UINT normSRV = resources->LoadSRV( context, L"Assets/Textures/cobblestone_normals.png" );
    UINT roughnessMap = resources->LoadSRV( context, L"Assets/Textures/cobblestone_roughness.png" );
    UINT metalMap = resources->LoadSRV( context, L"Assets/Textures/cobblestone_metal.png" );

    D3D11_SAMPLER_DESC samplerDesc = {}; // Zero out the struct memory
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    SamplerID = resources->AddSampler( samplerDesc );

    UINT matID = resources->LoadMaterial( vertexShader, pixelShader, diffSRV, normSRV, roughnessMap, metalMap, SamplerID );

    enMan->AddEntity(
        resources->GetMesh( meshID ), resources->GetMaterial( matID ), XMFLOAT3( 1.f, 0.f, 0.f ) );


    UINT woodDif = resources->LoadSRV( context, L"Assets/Textures/wood_albedo.png" );
    UINT woodNormSRV = resources->LoadSRV( context, L"Assets/Textures/wood_normals.png" );
    UINT woodRoughnessMap = resources->LoadSRV( context, L"Assets/Textures/wood_roughness.png" );
    UINT woodMetalMap = resources->LoadSRV( context, L"Assets/Textures/wood_metal.png" );
    UINT woodMatID = resources->LoadMaterial( vertexShader, pixelShader, woodDif, woodNormSRV, woodRoughnessMap, woodMetalMap, SamplerID );

    XMFLOAT3 newPos = XMFLOAT3( -1.f, 0.f, 0.f );
    UINT woodEntID = enMan->AddEntity(
        resources->GetMesh( meshID ), resources->GetMaterial( woodMatID ), newPos );
    
    enMan->GetEntity( woodEntID )->SetMass( 0.5f );

    UINT floorDif = resources->LoadSRV( context, L"Assets/Textures/floor_albedo.png" );
    UINT floorNormSRV = resources->LoadSRV( context, L"Assets/Textures/floor_normals.png" );
    UINT floorRoughnessMap = resources->LoadSRV( context, L"Assets/Textures/floor_roughness.png" );
    UINT floorMetalMap = resources->LoadSRV( context, L"Assets/Textures/floor_metal.png" );
    UINT floorMatID = resources->LoadMaterial( vertexShader, pixelShader, floorDif, floorNormSRV, floorRoughnessMap, floorMetalMap, SamplerID );

    XMFLOAT3 floorPos = XMFLOAT3( 0.f, -5.f, 0.f );
    UINT floorID = enMan->AddEntity(
        resources->GetMesh( floorMeshID ), resources->GetMaterial( floorMatID ), floorPos );

    enMan->GetEntity( floorID )->SetScale( XMFLOAT3( 5.f, 5.f, 5.f ) );
    enMan->GetEntity( floorID )->SetPhysicsLayer( EPhysicsLayer::STATIC );

    UINT bronzeDif = resources->LoadSRV( context, L"Assets/Textures/bronze_albedo.png" );
    UINT bronzeNormSRV = resources->LoadSRV( context, L"Assets/Textures/bronze_normals.png" );
    UINT bronzeRoughnessMap = resources->LoadSRV( context, L"Assets/Textures/bronze_roughness.png" );
    UINT bronzeMetalMap = resources->LoadSRV( context, L"Assets/Textures/bronze_metal.png" );
    UINT bronzeMatID = resources->LoadMaterial( vertexShader, pixelShader, bronzeDif, bronzeNormSRV, bronzeRoughnessMap, bronzeMetalMap, SamplerID );

    UINT bronzeEntID = enMan->AddEntity(
        resources->GetMesh( meshID ), resources->GetMaterial( bronzeMatID ), XMFLOAT3( -2.f, 0.f, 0.f ) );
    enMan->GetEntity( bronzeEntID )->SetMass( 10.f );

    // Load in the skybox SRV
    SkyboxSrvID = resources->LoadSRV_DDS(context, L"Assets/Textures/SunnyCubeMap.dds" );

    resources = nullptr;
    enMan = nullptr;
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
    // Handle base-level DX resize stuff
    DXCore::OnResize();

    FlyingCamera->UpdateProjectionMatrix( width, height );
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update( float deltaTime, float totalTime )
{
    // Quit if the escape key is pressed
    if ( GetAsyncKeyState( VK_ESCAPE ) )
        Quit();

    if ( InputManager::GetInstance()->IsAsyncKeyDown( 'L' ) )
    {
        UseDirLights = !UseDirLights;
    }

    // Update the camera
    FlyingCamera->Update( deltaTime );

    static float speed = 1.f;
    static float amountMoved = 0.f;
    const float target = 10.0f;

    for ( size_t i = 0; i < PointLights.size(); ++i )
    {
        XMFLOAT3 newPos = PointLights[ i ].Position;
        XMFLOAT3 acceleration = {};


        if ( i % 2 == 0 )
        {
            newPos.x += speed * deltaTime;
        }
        else
        {
            newPos.x -= speed * deltaTime;
        }

        amountMoved += speed * deltaTime;

        if ( amountMoved > 5.f || amountMoved < -5.f )
        {
            amountMoved = 0.f;
            speed *= -1.f;
        }

        PointLights[ i ].Position = newPos;
    }

    EntityManager* manager = EntityManager::GetInstance();
    Entity* entityA = nullptr;
    Entity* entityB = nullptr;

    for ( size_t i = 0; i < manager->GetEntityCount(); ++i )
    {
        entityA = manager->GetEntity( i );

        if ( entityA->GetPhysicsLayer() == EPhysicsLayer::STATIC ) continue;

        XMFLOAT3 forceToApply = XMFLOAT3( 0.f, 0.f, 0.f );

        for ( size_t j = 0; j < manager->GetEntityCount(); ++j )
        {
            entityB = manager->GetEntity( j );

            if ( Physics::Collisions::Intersects( entityA->GetCollider(), entityB->GetCollider() ) )
            {
                //DEBUG_PRINT(" There is an entity collision! ");
            }
        }


        // This introduces branching, remove it in the future and have a better solution
        // probably with just putting the different entities in different "buckets" at 
        // some point

        forceToApply.y += Gravity;

        forceToApply.x *= deltaTime;
        forceToApply.y *= deltaTime;
        forceToApply.z *= deltaTime;

        entityA->ApplyForce( forceToApply );
        
        if ( entityA->GetPosition().y < -2.f )
        {
            XMFLOAT3 opposingVel = entityA->GetVelocity();
            opposingVel.x *= -1.f;
            opposingVel.y *= -1.f;
            opposingVel.z *= -1.f;
            entityA->ApplyForce( opposingVel );
        }
        
        
        
        entityA->ApplyAcceleration();
    }

}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw( float deltaTime, float totalTime )
{
    // Background color (Cornflower Blue in this case) for clearing

    //const float color[ 4 ] = { 0.4f, 0.6f, 0.75f, 0.0f };
    const float color[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };

    // Clear the render target and depth buffer (erases what's on the screen)
    //  - Do this ONCE PER FRAME
    //  - At the beginning of Draw (before drawing *anything*)
    context->ClearRenderTargetView( backBufferRTV, color );
    context->ClearDepthStencilView(
        depthStencilView,
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0 );

    // Set buffers in the input assembler
    //  - Do this ONCE PER OBJECT you're drawing, since each object might
    //    have different geometry.
    UINT stride = sizeof( Vertex );
    UINT offset = 0;

    Mesh* EnMesh = nullptr;
    ID3D11Buffer* VertBuff = nullptr;

    Entity* CurrentEntity = EntityManager::GetInstance()->GetEntity( 0 );

    EntityManager* manager = EntityManager::GetInstance();

    for ( size_t i = 0; i < manager->GetEntityCount(); ++i )
    {
        CurrentEntity = manager->GetEntity( i );

        if ( !CurrentEntity->GetIsActive() ) continue;

        // Send lighting info ---------------------------------------------------------
        if ( DirLights.size() > 0 )
        {
            pixelShader->SetData( "DirLights", (void*) ( &DirLights[ 0 ] ), sizeof( DirectionalLight ) * MAX_DIR_LIGHTS );
        }
        pixelShader->SetInt( "DirLightCount", ( UseDirLights ? static_cast<int>( DirLights.size() ) : 0 ) );

        if ( PointLights.size() > 0 )
        {
            pixelShader->SetData( "PointLights", (void*) ( &PointLights[ 0 ] ), sizeof( PointLight ) * MAX_POINT_LIGHTS );
        }
        pixelShader->SetInt( "PointLightCount", static_cast<int>( PointLights.size() ) );


        // Send camera info ---------------------------------------------------------
        pixelShader->SetFloat3( "CameraPosition", FlyingCamera->GetPosition() );
        CurrentEntity->PrepareMaterial( FlyingCamera->GetViewMatrix(), FlyingCamera->GetProjectMatrix() );

        // Draw the entity ---------------------------------------------------------
        EnMesh = CurrentEntity->GetEntityMesh();
        VertBuff = EnMesh->GetVertexBuffer();

        context->IASetVertexBuffers( 0, 1, &VertBuff, &stride, &offset );
        context->IASetIndexBuffer( EnMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0 );

        context->DrawIndexed(
            EnMesh->GetIndexCount(),
            0,
            0 );

        // Draw the Sky box -------------------------------------

        // Set up sky render states
        context->RSSetState( skyRastState );
        context->OMSetDepthStencilState( skyDepthState, 0 );

        // After drawing all of our regular (solid) objects, draw the sky!
        ID3D11Buffer* skyVB = CurrentEntity->GetEntityMesh()->GetVertexBuffer();
        ID3D11Buffer* skyIB = CurrentEntity->GetEntityMesh()->GetIndexBuffer();

        // Set the buffers
        context->IASetVertexBuffers( 0, 1, &skyVB, &stride, &offset );
        context->IASetIndexBuffer( skyIB, DXGI_FORMAT_R32_UINT, 0 );

        SkyBoxVS->SetMatrix4x4( "view", FlyingCamera->GetViewMatrix() );
        SkyBoxVS->SetMatrix4x4( "projection", FlyingCamera->GetProjectMatrix() );

        SkyBoxVS->CopyAllBufferData();
        SkyBoxVS->SetShader();

        // Send texture-related stuff
        SkyBoxPS->SetShaderResourceView( "SkyTexture", ResourceManager::GetInstance()->GetSRV( SkyboxSrvID ) );
        SkyBoxPS->SetSamplerState( "BasicSampler", ResourceManager::GetInstance()->GetSampler( SamplerID ) );

        SkyBoxPS->CopyAllBufferData(); // Remember to copy to the GPU!!!!
        SkyBoxPS->SetShader();

        // Finally do the actual drawing
        context->DrawIndexed( CurrentEntity->GetEntityMesh()->GetIndexCount(), 0, 0 );

        // Reset any changed render states!
        context->RSSetState( 0 );
        context->OMSetDepthStencilState( 0, 0 );


    }

    manager = nullptr;

#if defined( _DEBUG ) ||  defined( DRAW_LIGHTS )

    DrawLightSources();

#endif // _DEBUG

    // Present the back buffer to the user
    //  - Puts the final frame we're drawing into the window so the user can see it
    //  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
    swapChain->Present( 0, 0 );
}


void Game::DrawLightSources()
{
    Mesh* lightMesh = ResourceManager::GetInstance()->GetMesh( PointLightMesh_ID );
    ID3D11Buffer * vb = lightMesh->GetVertexBuffer();
    ID3D11Buffer * ib = lightMesh->GetIndexBuffer();
    unsigned int indexCount = lightMesh->GetIndexCount();

    // Turn on these shaders
    vertexShader->SetShader();
    UnlitPixelShader->SetShader();

    // Set up vertex shader
    vertexShader->SetMatrix4x4( "view", FlyingCamera->GetViewMatrix() );
    vertexShader->SetMatrix4x4( "projection", FlyingCamera->GetProjectMatrix() );

    for ( size_t i = 0; i < PointLights.size(); ++i )
    {
        PointLight light = PointLights[ i ];
        // Set buffers in the input assembler
        UINT stride = sizeof( Vertex );
        UINT offset = 0;
        context->IASetVertexBuffers( 0, 1, &vb, &stride, &offset );
        context->IASetIndexBuffer( ib, DXGI_FORMAT_R32_UINT, 0 );

        float scale = light.Range / 13.0f;

        XMMATRIX rotMat = XMMatrixIdentity();
        XMMATRIX scaleMat = XMMatrixScaling( scale, scale, scale );
        XMMATRIX transMat = XMMatrixTranslation( light.Position.x, light.Position.y, light.Position.z );

        // Make the transform for this light
        XMFLOAT4X4 world;
        XMStoreFloat4x4( &world, XMMatrixTranspose( scaleMat * rotMat * transMat ) );

        // Set up the world matrix for this light
        vertexShader->SetMatrix4x4( "world", world );

        // Set up the pixel shader data
        XMFLOAT3 finalColor = light.Color;
        finalColor.x *= light.Intensity;
        finalColor.y *= light.Intensity;
        finalColor.z *= light.Intensity;
        UnlitPixelShader->SetFloat3( "Color", finalColor );

        // Copy data
        vertexShader->CopyAllBufferData();
        UnlitPixelShader->CopyAllBufferData();

        // Draw
        context->DrawIndexed( indexCount, 0, 0 );

    }

}



#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown( WPARAM buttonState, int x, int y )
{
    // Add any custom code here...
    FlyingCamera->SetDoRotation( true );

    // Save the previous mouse position, so we have it for the future
    prevMousePos.x = x;
    prevMousePos.y = y;

    // Caputure the mouse so we keep getting mouse move
    // events even if the mouse leaves the window.  we'll be
    // releasing the capture once a mouse button is released
    SetCapture( hWnd );
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp( WPARAM buttonState, int x, int y )
{
    // Add any custom code here...
    // Reverse the camera direction
    FlyingCamera->SetDoRotation( false );

    // We don't care about the tracking the cursor outside
    // the window anymore (we're not dragging if the mouse is up)
    ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove( WPARAM buttonState, int x, int y )
{
    FlyingCamera->UpdateMouseInput( prevMousePos.x - x, prevMousePos.y - y );

    // Save the previous mouse position, so we have it for the future
    prevMousePos.x = x;
    prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel( float wheelDelta, int x, int y )
{
    // Add any custom code here...
}
#pragma endregion