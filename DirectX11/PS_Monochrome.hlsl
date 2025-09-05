struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

cbuffer monoColor : register(b0)
{
	float4 monoColor;
}

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(PS_IN pin) : SV_TARGET
{
/*
	float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //ポストプロセスで画像に効果を加えるために
    //RenderTargetに書き込まれた色情報を取得
	color = tex.Sample(samp, pin.uv);
    //monochrome
	float mono = 0.0f;

#if 0
    mono+=color.r;
    mono+=color.g;
    monot+=color.b;
    mono/=3.f;
#else
    //人間の視覚特性を考慮したモノクロ
    //青色の変化を感じずらい
    //緑色の変化を感じやすい
	mono += color.r * (monoColor.r * monoColor.a);//0.3
	mono += color.g * (monoColor.g * monoColor.a); //0.6
	mono += color.b * (monoColor.b * monoColor.a); //0.1

#endif

	color.rgb = mono;
	return color;
*/
	float4 color = tex.Sample(samp, pin.uv);
	float mono = dot(color.rgb, monoColor.rgb);

    // Lerp between mono & original color
	float3 finalColor = lerp(color.rgb, mono.xxx, monoColor.a);

	return float4(finalColor, color.a);

}