cbuffer modelData : register(b0)
{
	float4 c_modelColor;
}

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

float4 main() : SV_TARGET
{
	return c_modelColor;
}