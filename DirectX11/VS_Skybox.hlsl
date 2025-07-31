struct VS_IN
{
	float3 pos : POSITION;
};
struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 dir : TEXCOORD0;
};

cbuffer CameraBuffer : register(b0)
{
	float4x4 view;
	float4x4 proj;
};


VS_OUT main(VS_IN input)
{
	VS_OUT output;

	float4x4 localView = view;
	localView[3] = float4(0, 0, 0, 1);

	output.pos = float4(input.pos, 1.0f);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, proj);

	output.dir = input.pos;
	return output;
}