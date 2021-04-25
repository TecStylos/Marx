#type vertex

cbuffer sceneData : register(b0)
{
	matrix c_viewProjection;
};

cbuffer modelData : register(b1)
{
	matrix c_modelTransform;
}

struct VS_INPUT
{
	float3 position : A_POSITION;
	float2 texCoord : A_TEXCOORD;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT inp)
{
	VS_OUTPUT op;

	op.position = mul(float4(inp.position, 1.0f), c_modelTransform);
	op.position = mul(op.position, c_viewProjection);
	op.texCoord = inp.texCoord;

	return op;
}

#type pixel

Texture2D g_tex : register(t0);
SamplerState g_sState : register(s0);

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

float4 main(VS_OUTPUT inp) : SV_TARGET
{
	return g_tex.Sample(g_sState, inp.texCoord);
}