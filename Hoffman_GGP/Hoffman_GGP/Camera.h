#pragma once

#include <DirectXMath.h>	// XMFLOAT3, XMFLOAT4X4
#include "Windows.h"			// GetAsyncKeyState
#include <stdio.h>

/////////////////////////////////////////////////
// Forward Declarations


/**
* Camera class capable of rendering entities, 
* determining the view and projection matrices,
* as well as some player input.
* 
* @author Ben Hoffman
*/
class Camera
{
public:

	/** Constructor; initalize matricies */
	Camera();
	
	/** Destructor for camera class */
	~Camera();

	/**
	* Update camera matricies appropriately. 
	* 
	* @param DeltaTime		Delta Time of this frame
	*/
	void Update(const float aDeltaTime);

	/**
	* Updates the projection matrix of the camera
	* 
	* @param aWidth		The aspect ration width
	* @param aHeight	The aspect ration height
	*/
	void UpdateProjectionMatrix(const unsigned int aWidth, const unsigned int aHeight);

	/**
	* Updates the camera's roation based on the mouse movement
	* of the player scaled by this cameras input speed
	*
	* @param aDeltaMouseX		The delta mouse input in X axis
	* @param aDeltaMouseY		The delta mouse input in Y axis
	*/
	void UpdateMouseInput(const float aDeltaMouseX, const float aDeltaMouseY);

	////////////////////////////////////////////////////
	// Accessors
	////////////////////////////////////////////////////

	/** Returns camera's current position */
	const DirectX::XMFLOAT3 GetPosition() const;

	/** Returns cameras current direction */
	const DirectX::XMFLOAT3 GetForwardDirection() const;

	/** Returns camera's current Rotation */
	const float GetXAxisRotation() const;
	
	/** Returns the cameras current rotation in the Y Axis */
	const float GetYAxisRotation() const;

	/** Returns the cameras current view matrix */
	const DirectX::XMFLOAT4X4 GetViewMatrix() const;

	/** Returns the current projection matrix */
	const DirectX::XMFLOAT4X4 GetProjectMatrix() const;

	/** Returns the current horizontal rotation speed */
	const float GetHorizontalRotSpeed() const;

	/** Returns the current vertical rotation speed */
	const float GetVerticalRotSpeed() const;

private:

	/** Update the view matrix of the camera */
	void UpdateViewMatrix(const float aDeltaTime);

	/** Current position of the camera */
	DirectX::XMFLOAT3 Position;
	
	/** Current direction of the camera */
	DirectX::XMFLOAT3 ForwardDirection;

	/** Rotation in the X axis of this camera */
	float RotationXAxis = 0.f;

	/** Rotation in the Y axis of this camera */
	float RotationYAxis = 0.f;
	
	/** Current View matrix of this camera. Defines the viewer of the scene. */
	DirectX::XMFLOAT4X4 ViewMatrix;

	/** Current projection matrix of the camera. Defines how the 3D scene is mapped onto 2d Screen */
	DirectX::XMFLOAT4X4 ProjectionMatrix;

	/** The horizontal rotation speed of the camera */
	const float HorizontalRotSpeed = 0.005f;

	/** The vertical rotation speed of the camera */
	const float VerticalRotSpeed = 0.005f;

	/** Wheter or not the user is using southpaw rotation */
	bool UseSouthpawRotation = false;

	/** Keyboard input from the player */
	DirectX::XMFLOAT3 RelativeInput;

};