// GLSL
//
// #version 300 es
// in highp vec2 position;
// out highp vec2 v_coord;
// void main(void)
// {
// 	gl_Position = vec4(position, 0, 1);
//    vec2 texCoord = (position * 0.5) + 0.5;
// 	v_coord = texCoord;
// }
// 

float4 vec4(float2 x0, int x1, int x2)
{
    return float4(x0, x1, x2);
}

// Attributes
static float2 _position = {0, 0};

static float4 gl_Position = float4(0, 0, 0, 0);

// Varyings
static  float2 _v_coord = {0, 0};

void gl_main()
{
(gl_Position = vec4(_position, 0, 1));
float2 _texCoord = ((_position * 0.5) + 0.5);
(_v_coord = _texCoord);
}

struct VS_INPUT
{
    float2 _position : TEXCOORD0;
};

void initAttributes(VS_INPUT input)
{
    _position = input._position;
}

struct VS_OUTPUT
{
    float4 dx_Position : SV_Position;
    float2 v0 : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
    initAttributes(input);

    gl_main();

    VS_OUTPUT output;
    output.dx_Position.x = gl_Position.x;
    output.dx_Position.y = -gl_Position.y;
    output.dx_Position.z = (gl_Position.z + gl_Position.w) * 0.5;
    output.dx_Position.w = gl_Position.w;
    output.v0 = _v_coord;

    return output;
}
