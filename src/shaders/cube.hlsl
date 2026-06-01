// cube.hlsl — 3D 큐브 (Forward 렌더링), DX11 / 셰이더 모델 5
//   (4.1: 법선을 색으로 시각화 → 3D 입체감 확인. 텍스처/조명은 4.2~4.3에서 추가)

cbuffer Transform : register(b0)
{
    row_major float4x4 gMVP;   // Model * View * Projection
};

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
    // 법선(-1~1)을 색(0~1)으로 변환해 면마다 다른 색 → 입체가 보이는지 확인용.
    return float4(normalize(i.nrm) * 0.5f + 0.5f, 1.0f);
}
