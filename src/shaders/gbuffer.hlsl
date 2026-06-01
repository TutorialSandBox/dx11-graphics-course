// gbuffer.hlsl — 디퍼드의 "지오메트리 패스" (DX11 / SM5)
//   큐브를 그리되 화면에 색을 내지 않고, G-buffer 3개 타깃에 정보를 "기록"한다.

cbuffer Transform : register(b0)
{
    row_major float4x4 gMVP;     // 위치 변환
    row_major float4x4 gWorld;   // 법선/월드좌표 변환
};

Texture2D    gTexture : register(t0);
SamplerState gSampler : register(s0);

struct VSInput  { float3 pos : POSITION; float3 nrm : NORMAL; float2 uv : TEXCOORD; };
struct VSOutput {
    float4 pos  : SV_POSITION;   // 클립 공간(래스터화용)
    float3 nrmW : NORMAL;        // 월드 법선
    float2 uv   : TEXCOORD;
    float3 posW : POSITION;      // 월드 좌표
};

VSOutput VSMain(VSInput i)
{
    VSOutput o;
    o.pos  = mul(float4(i.pos, 1.0f), gMVP);
    o.posW = mul(float4(i.pos, 1.0f), gWorld).xyz;
    o.nrmW = mul(i.nrm, (float3x3)gWorld);
    o.uv   = i.uv;
    return o;
}

// ★ 픽셀 셰이더가 3개의 타깃에 동시에 출력 (MRT = Multiple Render Targets)
struct PSOutput
{
    float4 albedo   : SV_Target0;   // 표면 색
    float4 normal   : SV_Target1;   // 법선
    float4 position : SV_Target2;   // 월드 좌표
};

PSOutput PSMain(VSOutput i)
{
    PSOutput o;
    float3 albedo = gTexture.Sample(gSampler, i.uv).rgb;
    o.albedo   = float4(albedo, 1.0f);                            // a=1: "여기 물체 있음" 마스크
    o.normal   = float4(normalize(i.nrmW) * 0.5f + 0.5f, 1.0f);   // [-1,1]→[0,1] (8비트 저장용)
    o.position = float4(i.posW, 1.0f);
    return o;
}
