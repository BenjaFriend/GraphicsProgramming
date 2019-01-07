#pragma once

/*************************************************************/
/* System includes                                           */
/*************************************************************/

#include <iostream>
#include <cstdlib>          // wcstombs_s
#include <stdlib.h>  
#include <stdio.h>
#include <crtdbg.h>  
#include <assert.h>
#include <string>
#include <fstream>
#include <tuple>

#include "Core/Logger.h"    // Logging definitions to only happen in debug


/**************************************************************/
/* Platform specifications                                    */
/**************************************************************/
#if defined( _WIN32 ) || defined ( _WIN64 )

#include <DirectXMath.h>	// XMFLOAT3, XMFLOAT4X4
#include <d3d11.h>          // Any directX functionality
#include <Windows.h>        // Windows callbacks

typedef DirectX::XMFLOAT2           VEC2;
typedef DirectX::XMFLOAT3           VEC3;
typedef DirectX::XMFLOAT4           VEC4;
typedef DirectX::XMFLOAT4X4         VEC4x4;

// Windows uses wide chars for their file names
typedef std::wstring                FileName;

#else 

// Using other another graphics library
typedef std::string                FileName;

// GLM

#endif



#define  ENABLE_UI

#if defined( ENABLE_UI )
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#endif


// Replace the new keyword with the debug new 

/**************************************************************/
/* Debug Macros                                               */
/**************************************************************/
#if defined( _DEBUG ) || defined ( DEBUG )

#define _CRTDBG_MAP_ALLOC   // memory leak detection
#include <crtdbg.h> 

// Debug new macro for trackinge xactly where the leaks are
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#define new DEBUG_NEW // This breaks IMGui

#else 

// Define DEBUG_NEW as just new
#define DEBUG_NEW new

#endif


/**************************************************************/
/* Util Macros                                                */
/**************************************************************/
#define SAFE_DELETE( obj ) if( obj != nullptr ){ delete obj; obj = nullptr; } 