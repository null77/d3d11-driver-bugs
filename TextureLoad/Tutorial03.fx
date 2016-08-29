//--------------------------------------------------------------------------------------
// File: Tutorial03.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = input;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    // 'Correct' means getting same images with either usage of 'Load' in one group.
    // 'Incorrect' means the opposite.

    // 1. Correct: All coordinates are non-negative.
    //return txDiffuse.Load(int3(input.Tex.x, input.Tex.y, 0) + int3(7, 7, 0));
    //return txDiffuse.Load(int3(input.Tex.x, input.Tex.y, 0), int2(7, 7));

    // 2. Correct: all coordinates are non-negative.
    //return txDiffuse.Load(int3(input.Tex.x, input.Tex.y, 0) + int3(-7, -7, 0));
    //return txDiffuse.Load(int3(input.Tex.x, input.Tex.y, 0), int2(-7, -7));

    // 3. Incorrect: two x- and y-coordinates are negative.
    //return txDiffuse.Load(int3(input.Tex.x, input.Tex.y, 0) + int3(7, 7, 0));
    return txDiffuse.Load(int3(input.Tex.x, input.Tex.y, 0), int2(7, 7));

    // 4. Incorrect: two x-coordinates are negative.
    //return txDiffuse.Load(int3(input.Tex.x, input.Tex.y, 0) + int3(7, -7, 0));
    //return txDiffuse.Load(int3(input.Tex.x, input.Tex.y, 0), int2(7, -7));

    // 5. Incorrect: two y-coordinates are negative.
    //return txDiffuse.Load(int3(input.Tex.x, input.Tex.y, 0) + int3(-7, 7, 0));
    //return txDiffuse.Load(int3(input.Tex.x, input.Tex.y, 0), int2(-7, 7));
}
