#type vertex

@cbuff 0 float4x4 c_viewProjection;
@cbuff 1 float4x4 c_modelTransform;

@cbuff 2 float1 c_conButton_A;
@cbuff 2 float1 c_conButton_B;
@cbuff 2 float1 c_conButton_X;
@cbuff 2 float1 c_conButton_Y;
@cbuff 2 float1 c_conButton_Start;
@cbuff 2 float1 c_conButton_Back;
@cbuff 2 float1 c_conButton_DPAD_Up;
@cbuff 2 float1 c_conButton_DPAD_Down;
@cbuff 2 float1 c_conButton_DPAD_Left;
@cbuff 2 float1 c_conButton_DPAD_Right;
@cbuff 2 float1 c_conButton_Thumb_Left;
@cbuff 2 float1 c_conButton_Thumb_Right;
@cbuff 2 float1 c_conButton_Shoulder_Left;
@cbuff 2 float1 c_conButton_Shoulder_Right;

@cbuff 2 float2 c_conStick_Left;
@cbuff 2 float2 c_conStick_Right;

@cbuff 2 float1 c_conTrigger_Left;
@cbuff 2 float1 c_conTrigger_Right;

struct VS_INPUT
{
	float2 position : A_POSITION;
	float1 brightness : A_BRIGHTNESS;
	int1 partID : A_PARTID;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float1 brightness : V_BRIGHTNESS;
	float1 value : V_VALUE;
};

VS_OUTPUT main(VS_INPUT inp)
{
	VS_OUTPUT op;

	op.brightness = inp.brightness;

	switch (inp.partID)
	{
	case 0: // Button A
		op.value = c_conButton_A; break;
	case 1: // Button B
		op.value = c_conButton_B; break;
	case 2: // Button X
		op.value = c_conButton_X; break;
	case 3: // Button Y
		op.value = c_conButton_Y; break;
	case 4: // Button Start
		op.value = c_conButton_Start; break;
	case 5: // Button Back
		op.value = c_conButton_Back; break;
	case 6: // DPAD Up
		op.value = c_conButton_DPAD_Up; break;
	case 7: // DPAD Down
		op.value = c_conButton_DPAD_Down; break;
	case 8: // DPAD_Left
		op.value = c_conButton_DPAD_Left; break;
	case 9: // DPAD Right
		op.value = c_conButton_DPAD_Right; break;
	case 10: // Thumb Left
		op.value = c_conButton_Thumb_Left;
		inp.position += c_conStick_Left * 0.1f;
		break;
	case 11: // Thumb Right
		op.value = c_conButton_Thumb_Right;
		inp.position += c_conStick_Right * 0.1f;
		break;
	case 12: // Shoulder Left
		op.value = c_conButton_Shoulder_Left; break;
	case 13: // Shoulder Right
		op.value = c_conButton_Shoulder_Right; break;
	case 14: // Trigger Left
		op.value = c_conTrigger_Left; break;
	case 15: // Trigger Right
		op.value = c_conTrigger_Right; break;
	}

	float4 worldPos = mul(float4(inp.position, 0.0f, 1.0f), c_modelTransform);
	op.position = mul(worldPos, c_viewProjection);

	return op;
}

#type pixel

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float1 brightness : V_BRIGHTNESS;
	float1 value : V_VALUE;
};

float4 main(VS_OUTPUT inp) : SV_TARGET
{
	float val = 0.05f;
	if (inp.value >= inp.brightness)
	{
		val = 1.0f;
	}
	return float4(float3(1.0f, 1.0f, 1.0f) * val, 1.0f);
}