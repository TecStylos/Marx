#type vertex

@cbuff 0 float4x4 c_viewProjection;
@cbuff 1 float4x4 c_modelTransform;

struct VS_INPUT
{
	float3 position : A_POSITION;
	float2 texCoord : A_TEXCOORD;
	float3 normal : A_NORMAL;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : V_TEXCOORD;
	float3 normal : V_NORMAL;
};

VS_OUTPUT main(VS_INPUT inp)
{
	VS_OUTPUT op;

	float4 pos = mul(float4(inp.position, 1.0f), c_modelTransform);
	op.position = mul(pos, c_viewProjection);
	op.texCoord = inp.texCoord;
	op.normal = mul(inp.normal, (float3x3)c_modelTransform);

	return op;
}

#type pixel

Texture2D g_tex : register(t0);
SamplerState g_sState : register(s0);

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : V_TEXCOORD;
	float3 normal : V_NORMAL;
};

float4 main(VS_OUTPUT inp) : SV_TARGET
{
	float3 n = normalize(inp.normal);
	float brightness = dot(n, float3(0.5f, 0.5f, 1.0f));
	brightness = max(brightness, 0.1f);
	float4 color = g_tex.Sample(g_sState, inp.texCoord);

	color.xyz *= brightness;

	return color;
}