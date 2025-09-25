#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DistortionTexture;

// 변수
float g_fAlpha = 1.f;
float g_fTime = 1.f;
float g_fDistortionPower = 2.0f; // 왜곡 강도 조절용 변수 추가
float g_fNoiseScale = 1.5f;


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float fFade : TEXCOORD1; // 페이드값.
};

/* DrawIndexed함수를 호출하면. */
/* 인덱스버퍼에 담겨있는 인덱스번째의 정점을 VS_MAIN함수에 인자로 던진다. VS_IN에 저장된다. */
/* 일반적으로 TriangleList로 그릴경우, 정점 세개를 각각 VS_MAIN함수의 인자로 던진다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP, matVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    matVP = mul(g_ViewMatrix, g_ProjMatrix);

	/* 정점의 위치에 월드 뷰 투영행렬을 곱한다. 현재 정점은 ViewSpace에 존재한다. */
	/* 투영행렬까지 곱하면 정점위치의 w에 뷰스페이스 상의 z를 보관한다. == Out.vPosition이 반드시 float4이어야하는 이유. */
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.fFade = 1.f - In.vTexcoord.x;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float fFade : TEXCOORD1;
};



struct PS_OUT
{
    float4 vNormal : SV_TARGET0;
};



// 더 강한 디스토션을 위한 대체 픽셀 셰이더
//PS_OUT PS_MAIN(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;
    
//    // 극단적으로 강한 디스토션 효과
//    float2 baseUV = In.vTexcoord;
    
//    // 웨이브 패턴 추가
//    float wave = sin(baseUV.x * 10.0f + g_fTime * 5.0f) * 0.1f;
//    baseUV.y += wave;
    
//    // 3개의 레이어 사용
//    float2 uv1 = baseUV * 2.0f + float2(g_fTime * -3.0f, g_fTime * 0.5f);
//    float2 uv2 = baseUV * 3.5f + float2(g_fTime * 2.0f, g_fTime * -1.0f);
//    float2 uv3 = baseUV * 1.3f + float2(g_fTime * -1.5f, g_fTime * 2.0f);
    
//    float3 dist1 = g_DistortionTexture.Sample(DefaultSampler, uv1).xyz * 2.0f - 1.0f;
//    float3 dist2 = g_DistortionTexture.Sample(DefaultSampler, uv2).xyz * 2.0f - 1.0f;
//    float3 dist3 = g_DistortionTexture.Sample(DefaultSampler, uv3).xyz * 2.0f - 1.0f;
    
//    // 복잡한 블렌딩
//    float3 vTangentNormal = dist1 * 0.5f + dist2 * 0.3f + dist3 * 0.2f;
    
//    // 극단적인 증폭
//    vTangentNormal *= 3.0f;
    
//    // 중앙 강조
//    float centerBoost = 1.0f - pow(abs(In.vTexcoord.y - 0.5f) * 2.0f, 1.2f);
//    vTangentNormal *= centerBoost * 1.5f;
    
//    // 페이드
//    vTangentNormal *= pow(In.fFade, 0.5f);
    
//    // 클램핑
//    vTangentNormal = clamp(vTangentNormal, -1.0f, 1.0f);
    
//    Out.vNormal.xyz = vTangentNormal * 0.5f + 0.5f;
//    Out.vNormal.w = 1.0f;
    
//    return Out;
//}

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // <<< 1단계: 원의 속성 정의 >>>
    // 디스토션이 100% 적용될 원의 반지름 (0.0 ~ 0.5)
    float fRadius = 0.3f;
    // 원의 가장자리가 부드럽게 사라질 폭
    float fFadeWidth = 0.2f;

    // <<< 2단계: 중심으로부터의 거리 계산 >>>
    // UV 좌표의 중심은 (0.5, 0.5)
    float dist = distance(In.vTexcoord, float2(0.5, 0.5));

    // <<< 3단계: 거리를 이용해 원형 마스크 생성 >>>
    // smoothstep(min, max, val)은 val이 min->max로 변할 때 0->1로 부드럽게 변하는 값을 반환.
    // 1.0 - smoothstep(...) 을 통해 중심(거리 < fRadius)이 1, 가장자리(거리 > fRadius+fFadeWidth)가 0이 되도록 함.
    float circleMask = 1.0 - smoothstep(fRadius, fRadius + fFadeWidth, dist);


    // --- 기존 디스토션 계산 로직 (그대로 사용) ---
    float2 baseUV = In.vTexcoord;
    float wave = sin(baseUV.x * 10.0f + g_fTime * 5.0f) * 0.1f;
    baseUV.y += wave;
    
    float2 uv1 = baseUV * 2.0f + float2(g_fTime * -3.0f, g_fTime * 0.5f);
    float2 uv2 = baseUV * 3.5f + float2(g_fTime * 2.0f, g_fTime * -1.0f);
    float2 uv3 = baseUV * 1.3f + float2(g_fTime * -1.5f, g_fTime * 2.0f);
    
    float3 dist1 = g_DistortionTexture.Sample(DefaultSampler, uv1).xyz * 2.0f - 1.0f;
    float3 dist2 = g_DistortionTexture.Sample(DefaultSampler, uv2).xyz * 2.0f - 1.0f;
    float3 dist3 = g_DistortionTexture.Sample(DefaultSampler, uv3).xyz * 2.0f - 1.0f;
    
    float3 vTangentNormal = dist1 * 0.5f + dist2 * 0.3f + dist3 * 0.2f;
    vTangentNormal *= 3.0f;
    
    float centerBoost = 1.0f - pow(abs(In.vTexcoord.y - 0.5f) * 2.0f, 1.2f);
    vTangentNormal *= centerBoost * 1.5f;
    
    vTangentNormal *= pow(In.fFade, 0.5f);

    // --- 여기까지 기존 로직 ---

    // <<< 4단계: 최종 왜곡 값에 마스크 적용 >>>
    // circleMask가 0이면(원 바깥) vTangentNormal도 0이 되어 왜곡이 사라짐.
    vTangentNormal *= circleMask;

    // 클램핑 및 최종 출력
    vTangentNormal = clamp(vTangentNormal, -1.0f, 1.0f);
    
    Out.vNormal.xyz = vTangentNormal * 0.5f + 0.5f;
    Out.vNormal.w = 1.0f;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Trail
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AdditiveBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

   
}