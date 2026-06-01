// triangle.hlsl — 가장 단순한 셰이더 쌍 (DX11, 셰이더 모델 5)
//   정점 셰이더: 위치를 그대로 통과 + 색을 다음 단계로 전달
//   픽셀 셰이더: 보간된 색을 그대로 출력

struct VSInput {
    float3 pos : POSITION;   // 정점 위치 (이미 클립공간 -1~1 이라고 가정)
    float3 col : COLOR;      // 정점 색
};

struct VSOutput {
    float4 pos : SV_POSITION;   // 시스템이 요구하는 최종 위치(클립공간)
    float3 col : COLOR;         // 래스터화 단계에서 픽셀마다 자동 보간됨
};

VSOutput VSMain(VSInput i) {
    VSOutput o;
    o.pos = float4(i.pos, 1.0f);   // 아직 변환 없음 — 다음 레슨(3.3)에서 행렬 곱 추가
    o.col = i.col;
    return o;
}

float4 PSMain(VSOutput i) : SV_TARGET {
    return float4(i.col, 1.0f);    // 보간된 색 → 최종 픽셀 색
}
