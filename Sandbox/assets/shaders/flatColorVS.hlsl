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
	float4 position : A_POSITION;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT inp)
{
	VS_OUTPUT op;

	op.position = mul(inp.position, c_modelTransform);
	op.position = mul(op.position, c_viewProjection);

	return op;
}