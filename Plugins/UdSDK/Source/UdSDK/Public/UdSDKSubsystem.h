// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
// #include "Modules/ModuleManager.h"

// Simple engine singleton to marshal data around varying classes
// Not sure if this is the best way to adresss this, but either way
class UUdSDKSubsystem : public UEngineSubsystem  
{
    public:
    
    // Storage location for the current width/height of the buffers for UD renders
    int32 DepthBufferWidth = 2;
    int32 DepthBufferHeight = 2;

    /*
    uint32 ColorBufferWidth = 2;
    uint32 ColorBufferHeight = 2;
    */

    // Save a copy of the width/height of the unreal extents for the depth buffer
    void SetDepthExtents(int32 width, int32 height)
    {
        DepthBufferWidth = width;
        DepthBufferHeight = height;
    }

    int32 Width()
    {
        return DepthBufferWidth;
    }

    int32 Height()
    {
        return DepthBufferHeight;
    }

    /*
    // Save a copy of the width/height of the unreal extents for the color buffer
    void SetColorExtents(int32 width, int32 height)
    {
        ColorBufferWidth = width;
        ColorBufferHeight = height;
    }*/
};
