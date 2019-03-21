#pragma once
/// <summary>
/// Provide the math definitions that we will need in the future
/// for cross platform development
/// </summary>
/// <author>Ben Hoffman</author>

// GLM Manual: https://github.com/g-truc/glm/blob/master/manual.md
#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS

// #TODO Experiment with the performance of different precisions
#define GLM_FORCE_PRECISION_LOWP_FLOAT
#define GLM_FORCE_PRECISION_LOWP_DOUBLE
#define GLM_FORCE_PRECISION_LOWP_INT
#define GLM_FORCE_PRECISION_LOWP_UINT
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/quaternion.hpp>

#if defined( _WIN32 ) || defined ( _WIN64 )

//#include <DirectXMath.h>	// XMFLOAT3, XMFLOAT4X4
#include <d3d11.h>          // Any directX functionality
#include <Windows.h>        // Windows callbacks
#include <string>

// Windows uses wide chars for their file names
typedef std::wstring                FileName;

#else 

// Using other another graphics library
typedef std::string                FileName;

// #TODO 
// Glm defintions 

#endif

typedef unsigned __int64        UINT64;
typedef unsigned __int32        UINT32;
typedef unsigned __int8         UINT8;
typedef signed __int64          INT64;
typedef signed __int32          INT32;
typedef signed __int8           INT8;