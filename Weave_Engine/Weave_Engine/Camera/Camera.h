#pragma once

#include <stdio.h>
#include "ECS/Component.h"
#include "../Input/InputManager.h"   // Input
#include "UI_OPTIONS.h"

/////////////////////////////////////////////////
// Forward Declarations
class CameraManager;

/// <summary>
/// Camera class capable of rendering entities, 
/// determining the view and projection matrices,
/// as well as some player input.
/// </summary>
/// <author>Ben Hoffman</author>
class Camera : public ECS::Component<Camera>
{
    friend class CameraManager;

public:

    COMPONENT( Camera );

    /** Constructor; initalize matricies */
    Camera();

    Camera( nlohmann::json const & aInitData );

    /// <summary>
    /// Updates the projection matrix of the camera
    /// </summary>
    /// <param name="aWidth">The aspect ration width</param>
    /// <param name="aHeight">The aspect ration height</param>
    void UpdateProjectionMatrix( const float aWidth, const float aHeight );

    /// <summary>
    /// Updates the camera's rotation based on the mouse movement
    /// of the player scaled by this cameras input speed
    /// </summary>
    /// <param name="aDeltaMouseX">The delta mouse input in X axis</param>
    /// <param name="aDeltaMouseY">The delta mouse input in Y axis</param>
    void UpdateMouseInput( const long aDeltaMouseX, const long aDeltaMouseY );

    ////////////////////////////////////////////////////
    // Accessors
    ////////////////////////////////////////////////////

    /** Returns camera's current position */
    const glm::vec3 GetPosition() const;

    /** Returns the cameras current view matrix */
    const glm::mat4 GetViewMatrix() const;

    /** Returns the current projection matrix */
    const glm::mat4 GetProjectMatrix() const;

    /// <summary>
    /// Set if the camera should be rotating
    /// </summary>
    /// <param name="aDoRot">Should rotate</param>
    void SetDoRotation( bool aDoRot );

    /** Returns if the camera is doing rotation */
    const bool GetDoRotation() const;

    /** Returns the camera's current movements speed */
    const float GetMovementSpeed() const;

    void SetPosition( glm::vec3 );

    /** Set the movement speed of this camera. Default is 4 */
    void SetMovementSpeed( float aNewVal );

    void SetSouthPaw( const bool val ) { SouthPaw = val; }
    const bool GetSouthPaw() const { return SouthPaw; }

    /// <summary>
    /// The ID of the current camera
    /// </summary>
    /// <returns></returns>
    const size_t GetCameraID() const { return CameraID;  }

    /// <summary>
    /// Set this camera as the one to be used for rendering right now
    /// </summary>
    void SetAsActiveCamera();

    bool const GetDoMovement() const { return DoMovement; }
    void SetDoMovement( const bool aDoMove ) { DoMovement = aDoMove; }

protected:

    static size_t CameraCount;

    size_t CameraID = 0;

    /** Destructor for camera class */
    ~Camera();

    virtual void SaveComponentData( nlohmann::json & aCompData ) override;

private:

    const glm::vec4 DEFAULT_UP = glm::vec4( 0.f, 1.f, 0.f, 0.f );
    const glm::vec4 DEFAULT_FORWARD = glm::vec4( 0.f, 0.f, -1.f, 0.f );
    const glm::vec4 DEFAULT_RIGHT = glm::vec4( 1.f, 1.f, 0.f, 0.f );

    const float MAX_PITCH = glm::pi<float>() / 2.0f;
    const float SENSITIVITY = 0.01f;

    float PitchAngle;
    float YawAngle;

    /** Option for south paw controls */
    UINT32 SouthPaw : 1;

    bool DoMovement = true;

    /** Current position of the camera */
    glm::vec3 Pos;

    glm::vec3 Forward;

    glm::vec3 Right;

    glm::vec3 Up;

    glm::mat4 View;

    glm::mat4 Projection;

    float FOV = 60.f * glm::pi<float>() / 180.0f;

    float NearZ = 0.01f;

    float FarZ = 100.0f;

    /** Movement speed of the camera */
    float MovementSpeed = 4.0f;

    /** Whether or not the user is using southpaw rotation */
    bool UseSouthpawRotation = false;

    /** Keyboard input from the player */
    glm::vec3 RelativeInput;

    /** If true than the camera will rotate */
    bool DoRotation = false;

    /** Pointer to input manager */
    Input::InputManager* inputManager = nullptr;

    // Inherited via Component
    virtual void DrawEditorGUI() override;
};
