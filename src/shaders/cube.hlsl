// cube.hlsl — 3D 큐브 (Forward 렌더링), DX11 / 셰이더 모델 5
//   (4.2: 텍스처를 입힘. 조명은 4.3에서 추가)

cbuffer Transform : register(b0)
{
    row_major float4x4 gMVP;   // Model * View * Projection
};

Texture2D    gTexture : register(t0);   // 입힐 이미지 (SRV 슬롯 0)
SamplerState gSampler : register(s0);   // 샘플링 방법 (필터/주소 모드)

struct VSInput  { float3 pos : POSITION; float3 nrm : NORMAL; float2 uv : TEXCOORD; };
struct VSOutput { float4 pos : SV_POSITION; float3 nrm : NORMAL; float2 uv : TEXCOORD; };

VSOutput VSMain(VSInput i)
{
    VSOutput o;
    o.pos = mul(float4(i.pos, 1.0f), gMVP);   // 3D 위치 → 클립 공간
    o.nrm = i.nrm;
    o.uv  = i.uv;
    return o;
}

float4 PSMain(VSOutput i) : SV_TARGET
{
    // UV 좌표로 텍스처에서 색을 읽어옴(샘플링).
    float3 albedo = gTexture.Sample(gSampler, i.uv).rgb;
    return float4(albedo, 1.0f);
}
