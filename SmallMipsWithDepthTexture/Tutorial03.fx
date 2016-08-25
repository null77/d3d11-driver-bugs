//--------------------------------------------------------------------------------------
// File: Tutorial02.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

Texture2D<float> tex : register(t0);
SamplerState samp : register(s0);

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void VS(float4 PosIn : POSITION, out float4 PosOut : SV_Position, out float2 TexCoord : TEXCOORD0)
{
    PosOut = PosIn;
    TexCoord = PosIn.xy * 0.5f + 0.5f;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
void PS(float4 Pos : SV_POSITION, float2 TexCoord : TEXCOORD0, out float4 Color : SV_Target)
{
    Color = tex.Sample(samp, TexCoord);
}
