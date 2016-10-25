//--------------------------------------------------------------------------------------
// File: Tutorial02.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
    float nan;
    float3 useless;
}

struct VS_OUTPUT
{
    float4 Pos  : SV_POSITION;
    float4 color : COlOR;
};


VS_OUTPUT VS( float4 Pos : POSITION )
{
    VS_OUTPUT output;
    output.Pos = Pos;

    output.color = isnan(nan)? float4(0.0f, 1.0f, 0.0f, 1.0f) : float4(1.0f, 0.0f, 0.0f, 1.0f);
    return output;
}


struct PS_INPUT
{
    float4 Pos  : SV_POSITION;
    float4 color : COLOR;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    return input.color;
}
