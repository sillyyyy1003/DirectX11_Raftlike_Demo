struct VS_IN
{
	float3 pos : POSITION0;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};

struct VS_OUT
{
	float4 pos : SV_POSITION0;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
	float4 worldPos : POSITION0;
};

cbuffer WVP : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 proj;
}


VS_OUT main(VS_IN vin)
{
	VS_OUT vOut;

	vOut.pos = float4(vin.pos, 1.0f);
	vOut.pos = mul(vOut.pos, world);
	vOut.worldPos = vOut.pos;
	vOut.pos = mul(vOut.pos, view);
	vOut.pos = mul(vOut.pos, proj);
	vOut.normal = mul(vin.normal, (float3x3) world);
	vOut.tex = vin.tex;
	
	return vOut;
}