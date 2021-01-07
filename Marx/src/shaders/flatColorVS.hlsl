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
	float4 pos : POSITION;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT inp)
{
	VS_OUTPUT op;

	op.pos = mul(inp.pos, c_modelTransform);
	op.pos = mul(op.pos, c_viewProjection);

	return op;
}