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
	//return float4(inp.texCoord, 0.0f, 1.0f);
}