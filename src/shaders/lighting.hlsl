// lighting.hlsl — 디퍼드의 "라이팅 패스" (DX11 / SM5)
//   화면을 덮는 삼각형 하나로, G-buffer를 픽셀마다 샘플해 조명을 계산한다.
//   (7.1: 방향광만. 7.2에서 점광원 추가)

cbuffer LightCB : register(b0)
{
    float4 gEyePos;     // xyz = 카메라 위치 (지금은 미사용, 향후 스페큘러용)
    float4 gDirL;       // xyz = "빛으로 향하는" 방향(L)
    float4 gDirColor;   // rgb = 방향광 색
};

Texture2D    gAlbedo   : register(t0);
Texture2D    gNormal   : register(t1);
Texture2D    gPosition : register(t2);
SamplerState gSampler  : register(s0);

struct VSOut { float4 pos : SV_POSITION; float2 uv : TEXCOORD; };

// ★ 정점 버퍼 없이 SV_VertexID 만으로 화면 전체를 덮는 큰 삼각형을 만든다.
//   id=0→(0,0), 1→(2,0), 2→(0,2) 의 UV. 화면(-1~1)을 완전히 덮음.
VSOut VSMain(uint id : SV_VertexID)
{
    VSOut o;
    o.uv  = float2((id << 1) & 2, id & 2);
    o.pos = float4(o.uv * float2(2, -2) + float2(-1, 1), 0, 1);
    return o;
}

float4 PSMain(VSOut i) : SV_TARGET
{
    float4 alb = gAlbedo.Sample(gSampler, i.uv);
    if (alb.a < 0.5f)
        return float4(0.10f, 0.12f, 0.18f, 1.0f);   // 배경(물체 없음)

    float3 albedo = alb.rgb;
    float3 N = normalize(gNormal.Sample(gSampler, i.uv).rgb * 2.0f - 1.0f);  // [0,1]→[-1,1]

    float3 color = albedo * 0.15f;                                   // 앰비언트
    color += albedo * gDirColor.rgb * saturate(dot(N, gDirL.xyz));   // 방향광 (N·L)
    return float4(color, 1.0f);
}
