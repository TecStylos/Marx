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
	float3 worldPos : V_WORLDPOS;
	float2 texCoord : V_TEXCOORD;
	float3 normal : V_NORMAL;
};

VS_OUTPUT main(VS_INPUT inp)
{
	VS_OUTPUT op;

	op.worldPos = (float3)mul(float4(inp.position, 1.0f), c_modelTransform);
	op.position = mul(float4(op.worldPos, 1.0f), c_viewProjection);
	op.texCoord = inp.texCoord;
	op.normal = mul(inp.normal, (float3x3)c_modelTransform);

	return op;
}

#type pixel

@cbuff 2 float3 c_ambientColor;
@cbuff 2 float1 c_diffuseIntensity;
@cbuff 2 float3 c_lightPos;
@cbuff 2 float1 c_attConst;
@cbuff 2 float3 c_diffuseColor;
@cbuff 2 float1 c_attLin;
@cbuff 2 float1 c_attQuad;
@cbuff 2 float1 c_specularIntensity;
@cbuff 2 float1 c_specularPower;
@cbuff 2 float1 c_padding;


Texture2D g_tex : register(t0);
SamplerState g_sState : register(s0);

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 worldPos : V_WORLDPOS;
	float2 texCoord : V_TEXCOORD;
	float3 normal : V_NORMAL;
};

float4 main(VS_OUTPUT inp) : SV_TARGET
{
	const float3 vToL = c_lightPos - inp.worldPos;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;

	const float att = 1.0f / (c_attConst + c_attLin * distToL + c_attQuad * (distToL * distToL));

	const float3 diffuse = c_diffuseColor * c_diffuseIntensity * att * max(0.0f, dot(dirToL, inp.normal));

	const float3 w = inp.normal * dot(vToL, inp.normal);
	const float3 r = w * 2.0f - vToL;

	const float3 specular = att * (c_diffuseColor * c_diffuseIntensity) * c_specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(inp.worldPos))), c_specularPower);

	const float4 materialColor = g_tex.Sample(g_sState, inp.texCoord);

	const float4 color = float4(saturate((diffuse + c_ambientColor + specular) * materialColor), 1.0f);

	return color;
}