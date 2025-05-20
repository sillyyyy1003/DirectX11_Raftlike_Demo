#include "Triangle.hlsli"

VS_OUT vs_main(VS_IN vIn)
{
	VS_OUT vOut;
	vOut.posH = float4(vIn.pos, 1.0f);
	vOut.color = vIn.color;
	return vOut;
}