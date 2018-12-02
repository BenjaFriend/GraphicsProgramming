#ifndef LIGHT_SHADER_DEFS
#define LIGHT_SHADER_DEFS

#define MAX_DIR_LIGHTS 32
#define MAX_POINT_LIGHTS 64

#define DRAW_LIGHTS

#if defined( __cplusplus )

#define FLOAT4 DirectX::XMFLOAT4
#define FLOAT3 DirectX::XMFLOAT3

#else 

#define FLOAT4 float4
#define FLOAT3 float3

#endif

struct DirectionalLight
{
    FLOAT4 AmbientColor;
    FLOAT4 DiffuseColor;
    FLOAT3 Direction;
    float Intensity;      // This has to be here because of the way that
                          // Simple Shader works
};


struct PointLightData
{
    FLOAT3 Color;        // 12 bytes
    float Range;                    // 16 bytes

    FLOAT3 Position;     // 28 bytes
    float Intensity;                // 32 bytes
};

#endif // LIGHT_SHADER_DEFS