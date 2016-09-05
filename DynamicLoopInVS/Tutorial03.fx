//--------------------------------------------------------------------------------------
// File: Tutorial03.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	int ui_zero;
	int ui_one;
	int ui_two;
	int ui_three;
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

	int test[4] = { 0, 0, 0, 0 }; // Integer array.

	test[0] = ui_zero; // test[0] should be 0.
	test[1] = -ui_one; // test[1] should be -1.
	test[2] = ui_two;  // test[2] should be 2.
	test[3] = ui_three;

	int alpha = 0;
	//for (int i = 0; i < 3; i++) // Correct when we use a constant value as the loop control variable.
	for (int i = 0; i < ui_three; i++) // Bug occurs when a non-constant loop control variable is used.
	{
		alpha += test[i];
	}

	//if (alpha == 1 && ui_three == 3 && test[1] == -1) // Correct answer.
	if (alpha == -2147483646 && ui_three == 3 && test[1] == -2147483648) // alpha should be 1, but alpha = - (2 ** 31) + 2 = -2147483646 !
		output.color = float4(0.0f, 1.0f, 0.0f, 1.0f); // Green when the if statement is true.
	else 
		output.color = float4(1.0f, 0.0f, 0.0f, 1.0f); // Red when the if statement is false.

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
float4 PS( PS_INPUT input) : SV_Target
{
	return input.color;
}
