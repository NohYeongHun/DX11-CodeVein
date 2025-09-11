#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* 총 6개 사용 */
texture2D g_DiffuseTexture;
texture2D g_OpacityTexture;
texture2D g_OtherTexture[4];

/* 사용할 변수들 */
float g_fTimeRatio;
float g_fBloomIntensity = 2.0f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;    
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;    
    
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);    
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    return Out;     
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;    
    float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};



PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;    
}


//PS_OUT PS_HITFLASH_MAIN(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;
    
//    // 1. 각 텍스쳐에서 정보 샘플링
//    // Opacity0.png: 별 모양의 형태를 결정하는 마스크
//    float shapeMask = g_OpacityTexture.Sample(DefaultSampler, In.vTexcoord).r;

//    // Other0.png: 불꽃의 역동적인 질감과 색상을 가져옴
//    // UV를 시간에 따라 살짝 움직여(panning) 텍스쳐가 살아있는 느낌을 줌
//    float2 panningUV = In.vTexcoord + float2(g_fTimeRatio * 0.1f, g_fTimeRatio * 0.1f);
//    float4 diffuseColor = g_DiffuseTexture.Sample(DefaultSampler, panningUV);

//    // Other2.png: 중앙에 집중되고 외곽으로 갈수록 부드럽게 사라지는 효과
//    float radialMask = g_OtherTexture[2].Sample(DefaultSampler, In.vTexcoord).r;

//    // 2. 시간(g_fTimeRatio)에 따른 애니메이션 처리
//    // 이펙트 전체 생명주기를 100%로 봤을 때,
//    // 0% -> 20%: 모이는 단계 (점점 밝아짐)
//    // 20% -> 100%: 퍼지는 단계 (점점 투명해짐)
//    float animationAlpha = 0.f;
//    if (g_fTimeRatio < 0.2f)
//    {
//        // 0.0 ~ 0.2 구간을 0.0 ~ 1.0으로 정규화하여 알파값으로 사용
//        animationAlpha = g_fTimeRatio / 0.2f;
//    }
//    else
//    {
//        // 0.2 ~ 1.0 구간을 1.0 ~ 0.0으로 정규화하여 알파값으로 사용
//        animationAlpha = 1.0f - (g_fTimeRatio - 0.2f) / 0.8f;
//    }

//    // 3. 최종 색상 조합 - Bloom 강도 적용
//    // 디버깅: 각 단계별 값 확인
    
//    // 먼저 diffuseColor가 제대로 샘플링되는지 확인
//    // Out.vColor = diffuseColor; return Out; // 테스트 1: diffuse 텍스처만
    
//    // shapeMask 확인
//    // Out.vColor = float4(shapeMask, shapeMask, shapeMask, 1.0f); return Out; // 테스트 2: 마스크만
    
//    // 강제로 밝은 금색 설정 (텍스처 무시)
//    float3 finalColor = float3(2.0f, 1.5f, 0.3f) * g_fBloomIntensity;

//    // 최종 알파 = 모양 마스크 * 중앙 집중 마스크 * 시간 애니메이션 알파  
//    float finalAlpha = shapeMask * radialMask * animationAlpha;
    
//    // 알파가 0이면 안 보이므로 최소값 설정

//    // 4. 결과 반환
//    Out.vColor = float4(finalColor, finalAlpha);
//    return Out;
//}

// UV를 회전시키는 헬퍼 함수
//PS_OUT PS_HITFLASH_MAIN(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;

//    // --- 1. 애니메이션 단계 설정 ---
//    float shockwave = 0.f; // 최종 충격파 마스크
//    float phaseTime = 0.f; // 각 단계별 진행 시간 (0.0 ~ 1.0)

//    // 전체 애니메이션 시간을 절반으로 나누어 처리
//    if (g_fTimeRatio < 0.5f)
//    {
//        // --- 2. 1단계: 수축 (Gathering) ---
//        // 0.0 ~ 0.5 구간을 0.0 ~ 1.0으로 변환
//        phaseTime = g_fTimeRatio / 0.5f;

//        // 원형 그라데이션 값을 가져옴 (중앙이 1, 가장자리가 0)
//        float radialValue = g_OtherTexture[2].Sample(DefaultSampler, In.vTexcoord).r;
        
//        // 링(Ring)의 위치를 가장자리(0)에서 중앙(1)으로 이동시킴
//        float ringPos = phaseTime;
//        float ringWidth = 0.15f; // 링의 두께
        
//        shockwave = smoothstep(ringPos - ringWidth, ringPos, radialValue)
//                    - smoothstep(ringPos, ringPos + ringWidth, radialValue);
//    }
//    else
//    {
//        // --- 3. 2단계: 폭발 (Burst) ---
//        // 0.5 ~ 1.0 구간을 0.0 ~ 1.0으로 변환
//        phaseTime = (g_fTimeRatio - 0.5f) / 0.5f;

//        // 원형 그라데이션 값을 가져옴
//        float radialValue = g_OtherTexture[2].Sample(DefaultSampler, In.vTexcoord).r;
        
//        // 링(Ring)의 위치를 중앙(1)에서 가장자리(0)로 이동시킴
//        float ringPos = 1.0f - phaseTime;
//        float ringWidth = 0.15f;
        
//        shockwave = smoothstep(ringPos - ringWidth, ringPos, radialValue)
//                    - smoothstep(ringPos, ringPos + ringWidth, radialValue);
//    }


//    // --- 4. 전체적인 밝기 및 최종 조합 ---
//    // 정적인 불꽃 마스크를 가져옵니다.
//    float sparkMask = g_OpacityTexture.Sample(DefaultSampler, In.vTexcoord).r;

//    // 전체 애니메이션이 중앙(g_fTimeRatio=0.5)에서 가장 밝고, 시작과 끝에서 사라지도록 밝기를 조절합니다.
//    // sin 함수를 이용해 부드러운 밝기 커브를 만듭니다.
//    float overallAlpha = sin(g_fTimeRatio * 3.14159f);

//    // 최종 형태 = (불꽃 마스크 * 움직이는 충격파 마스크)
//    float finalShape = sparkMask * shockwave;
    
//    // 금빛 색상을 정의합니다.
//    float3 goldColor = float3(2.5f, 1.8f, 0.5f) * g_fBloomIntensity;

//    // 최종 색상과 알파를 계산합니다.
//    float3 finalColor = goldColor * finalShape;
//    float finalAlpha = finalShape * overallAlpha;

//    // 5. 결과 반환
//    Out.vColor = float4(finalColor, finalAlpha);
    
//    return Out;
//}

PS_OUT PS_HITFLASH_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // --- 1. '선들이 길어지는' 효과를 위한 마스크 생성 ---
    // Radial 텍스처(Other[2])를 이용해 중앙에서부터 바깥으로 퍼지는 원형 마스크를 만듭니다.
    float radialValue = g_OtherTexture[2].Sample(DefaultSampler, In.vTexcoord).r;
    
    // 시간에 따라 마스크의 경계선이 1(중앙)에서 0(가장자리)으로 이동합니다.
    float threshold = 1.0f - g_fTimeRatio;
    
    // 경계선 안쪽만 보이도록 하여 '자라나는' 효과를 만듭니다.
    float revealMask = smoothstep(threshold - 0.1f, threshold + 0.1f, radialValue);

    // --- 2. 최종 조합 ---
    // Opacity 텍스처에서 원본 불꽃 모양을 가져옵니다.
    float sparkShape = g_OpacityTexture.Sample(DefaultSampler, In.vTexcoord).r;

    // 시간에 따라 전체적으로 옅어지며 사라지는 효과를 만듭니다.
    float fadeAlpha = pow(1.0f - g_fTimeRatio, 2.0f);

    // 최종 모양 = (원본 불꽃 모양 * 자라나는 마스크)
    float finalShape = sparkShape * revealMask;
    
    // 최종 알파 = (최종 모양 * 전체 Fade)
    float finalAlpha = finalShape * fadeAlpha;

    // 색상 적용
    float3 goldColor = float3(2.5f, 1.8f, 0.5f) * g_fBloomIntensity;
    float3 finalColor = goldColor * finalShape;
    
    // 최종 결과 반환
    Out.vColor = float4(finalColor, finalAlpha);
    
    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN(); 
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass HitFlashPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HITFLASH_MAIN();
    }

}
