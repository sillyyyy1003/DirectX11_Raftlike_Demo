struct VS_IN
{
	float3 pos : POSITION;
};
struct VS_OUT
{
	float4 posH : SV_POSITION;
	float3 posL : POSITION;
};

cbuffer CameraBuffer : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};


VS_OUT main(VS_IN input)
{
	VS_OUT vOut;
	float4 posH = float4(input.pos, 1.f);
	posH = mul(posH, world);
	posH = mul(posH, view);
	posH = mul(posH, proj);
	vOut.posH = posH.xyww;
	vOut.posL = input.pos;

	return vOut;
}