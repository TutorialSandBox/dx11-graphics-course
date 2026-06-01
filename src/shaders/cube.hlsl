// cube.hlsl — 3D 큐브 (Forward 렌더링), DX11 / 셰이더 모델 5
//   (4.3: 텍스처 + 램버트 방향광 조명)

cbuffer Transform : register(b0)
{
    row_major float4x4 gMVP;     // Model * View * Projection (위치 변환)
    row_major float4x4 gWorld;   // Model (법선을 월드로 옮길 때)
    float4 gLightDir;            // xyz = "빛으로 향하는" 방향(L), 단위벡터
};

Texture2D    gTexture : register(t0);
SamplerState gSampler : register(s0);

struct VSInput  { float3 pos : POSITION; float3 nrm : NORMAL;  float2 uv : TEXCOORD; };
struct VSOutput { float4 pos : SV_POSITION; float3 nrmW : NORMAL; float2 uv : TEXCOORD; };

VSOutput VSMain(VSInput i)
{
    VSOutput o;
    o.pos  = mul(float4(i.pos, 1.0f), gMVP);
    o.nrmW = mul(i.nrm, (float3x3)gWorld);   // 법선을 월드 공간으로 (회전 적용)
    o.uv   = i.uv;
    return o;
}

float4 PSMain(VSOutput i) : SV_TARGET
{
    float3 albedo = gTexture.Sample(gSampler, i.uv).rgb;

    // 램버트(Lambert) 확산 조명: 빛이 면을 얼마나 정면으로 때리나 = N·L
    float3 N   = normalize(i.nrmW);
    float3 L   = normalize(gLightDir.xyz);
    float  ndl = saturate(dot(N, L));        // 0~1 (뒤를 향하면 0)

    // 앰비언트(주변광) 0.35 + 확산광 0.8*ndl → 그늘진 면도 완전히 깜깜하진 않게
    float3 color = albedo * (0.35f + 0.8f * ndl);
    return float4(color, 1.0f);
}
