#include "Triangle.hlsli"

float4 ps_main(VS_OUT pIn) : SV_Target
{
	return pIn.color;
}