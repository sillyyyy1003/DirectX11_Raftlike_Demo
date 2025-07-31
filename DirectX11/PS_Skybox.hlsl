TextureCube skyboxTex : register(t0);
SamplerState mySampler : register(s0);

float4 main(float3 dir : TEXCOORD0) : SV_TARGET
{
	return skyboxTex.Sample(mySampler, dir);
}