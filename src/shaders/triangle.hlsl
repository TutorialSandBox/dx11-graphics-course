// triangle.hlsl — 가장 단순한 셰이더 쌍 (DX11, 셰이더 모델 5)
//   정점 셰이더: 위치를 그대로 통과 + 색을 다음 단계로 전달
//   픽셀 셰이더: 보간된 색을 그대로 출력

// 상수 버퍼(constant buffer): CPU가 매 프레임 셰이더로 넘기는 값들의 묶음.
//   row_major + mul(v, M) 규약(Part 1) → CPU의 행렬을 전치 없이 그대로 사용.
cbuffer Transform : register(b0)
{
    row_major float4x4 gTransform;
};

struct VSInput {
    float3 pos : POSITION;   // 정점 위치
    float3 col : COLOR;      // 정점 색
};

struct VSOutput {
    float4 pos : SV_POSITION;   // 시스템이 요구하는 최종 위치(클립공간)
    float3 col : COLOR;         // 래스터화 단계에서 픽셀마다 자동 보간됨
};

VSOutput VSMain(VSInput i) {
    VSOutput o;
    o.pos = mul(float4(i.pos, 1.0f), gTransform);   // ★ Part 1의 v * M 이 여기서 실행됨
    o.col = i.col;
    return o;
}

float4 PSMain(VSOutput i) : SV_TARGET {
    return float4(i.col, 1.0f);    // 보간된 색 → 최종 픽셀 색
}
