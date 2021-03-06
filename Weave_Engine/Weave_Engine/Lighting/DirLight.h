#pragma once

#include "stdafx.h"

#include "LightShaderDefs.h"
#include "ECS/Component.h"
#include "UI_OPTIONS.h"

class LightSystem;

/// <summary>
/// Component for controlling a directional light
/// </summary>
/// <author>Ben Hoffman</author>
class DirLight : public ECS::Component<DirLight>
{
public:

    COMPONENT( DirLight );

    DirLight();

    DirLight( DirectionalLightData aLightData );

    DirLight( nlohmann::json const & aInitData );

    ~DirLight();

    /// <summary>
    /// Get the light information about this 
    /// </summary>
    /// <returns></returns>
    const DirectionalLightData& GetLightData() const;

    /// <summary>
    /// Set this directional lights information
    /// </summary>
    /// <param name="aLightData">The lighting data for this dir light</param>
    void SetLightData( DirectionalLightData aLightData );

    virtual void SaveComponentData( nlohmann::json & aOutFile ) override;

    virtual void DrawEditorGUI() override;

private:

    /** The lighting information that is needed */
    DirectionalLightData LightingData;

};