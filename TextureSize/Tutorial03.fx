//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

// texSizeCase = {lod, baseLevel, expectedWith, expectedHeight}
uniform int4 texSizeCase : register(b0);

uniform Texture2D textures2D : register(t0);
SamplerState simpleSampler : register(s0);

int2 gl_texture2DSize(int baseLevel, int lod)
{
    uint width; uint height; uint numberOfLevels;
    // NVIDIA drivers can query properties of the whole texture,
    // while the reference rasterizer returns default values when lod >= the SRV's MipLevels.
    textures2D.GetDimensions(lod, width, height, numberOfLevels);
    // Seems that NVIDIA will ignore the base level which is set in SRV.
    // Here if we add the baseLevel to lod, it will pass in NVIDIA.
    // textures2D.GetDimensions(baseLevel + lod, width, height, numberOfLevels);
    return int2(width, height);
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

PS_INPUT VS(float4 Pos : POSITION)
{
    float4 s1 = { 0, 0, 0, 0 };
    int lod = texSizeCase.x;
    int baseLevel = texSizeCase.y;
    int2 texSize = { texSizeCase.z, texSizeCase.w };
    if (all(gl_texture2DSize(baseLevel, lod) == texSize))
    {
        (s1 = float4(1.0, 1.0, 1.0, 1.0));
    }
    else
    {
        (s1 = float4(1.0, 0.0, 0.0, 1.0));
    }

    PS_INPUT output = (PS_INPUT)0;
    output.Pos = Pos;
    output.Color = s1;
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    return input.Color;
}
