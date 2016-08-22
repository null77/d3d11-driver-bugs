// GLSL
//
// #version 300 es
// in highp vec2 v_coord;
// out highp vec4 color;
// uniform sampler2D tex;
// void main()
// {
//    color = texture(tex, v_coord);
// }
// 

// Uniforms

uniform SamplerState sampler__tex : register(s0);
uniform Texture2D texture__tex : register(t0);

// Varyings
static  float2 _v_coord = {0, 0};

static float4 out_color = {0, 0, 0, 0};

float4 gl_texture2D(Texture2D x, SamplerState s, float2 t)
{
    return x.Sample(s, float2(t.x, t.y));
}

void gl_main()
{
    out_color = gl_texture2D(texture__tex, sampler__tex, _v_coord);
}

struct PS_INPUT
{
    float4 dx_Position : SV_Position;
    float2 v0 : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 out_color : SV_TARGET0;
};

PS_OUTPUT generateOutput()
{
    PS_OUTPUT output;
    output.out_color = out_color;
    return output;
}

PS_OUTPUT main(PS_INPUT input)
{
    _v_coord = input.v0.xy;

    gl_main();

    return generateOutput();
}
