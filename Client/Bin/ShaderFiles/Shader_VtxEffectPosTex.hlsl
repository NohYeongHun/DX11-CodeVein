#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* 총 6개 사용 */
texture2D g_DiffuseTexture;
texture2D g_OpacityTexture;
texture2D g_MaskTexture;
texture2D g_OtherTexture[5];

/* 사용할 변수들 */
float g_fTimeRatio;
float g_fBloomIntensity = 2.0f;
float g_fScale; // 스케일 팩터

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



PS_OUT PS_HITFLASH_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 1. 원본 불꽃 모양을 가져옵니다.
    float sparkShape = g_OpacityTexture.Sample(DefaultSampler, In.vTexcoord).r;

    // 부드러운 빛무리 효과를 위해 Radial 텍스처를 더해줍니다.
    float radialGlow = g_OtherTexture[2].Sample(DefaultSampler, In.vTexcoord).r;
    
    // radialGlow 관련 두 줄을 삭제하고 아래와 같이 수정합니다.
    //float finalShape = sparkShape;
    float finalShape = saturate(sparkShape + (radialGlow * 0.1f));

    // 2. 시간에 따라 나타났다가 사라지는 알파값을 계산합니다.
    float fadeAlpha = sin(saturate(g_fTimeRatio) * 3.14159f);
    
    // 3. 최종 알파 = (최종 모양 * 전체 Fade)
    float finalAlpha = finalShape * fadeAlpha;

    // 색상 적용
    float3 goldColor = float3(2.5f, 1.8f, 0.5f) * g_fBloomIntensity;
    float3 finalColor = goldColor * finalShape;
    
    Out.vColor = float4(finalColor, finalAlpha);
    
    return Out;
}


PS_OUT PS_LINESLASH_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 1. 시간에 따라 사라지는 듯한 '축소' 효과를 위한 UV 계산 (기존과 동일)
    float2 center = float2(0.5f, 0.5f);
    float2 scaledUV = center + (In.vTexcoord - center) / g_fScale;
    if (scaledUV.x < 0.0f || scaledUV.x > 1.0f || scaledUV.y < 0.0f || scaledUV.y > 1.0f)
        discard;

    // ▼▼▼ [핵심 수정] 일렁이는 효과를 만드는 UV 왜곡 로직 전체 삭제 ▼▼▼
    
    // 2. 왜곡되지 않은 UV로 검격(Slash) 텍스처 샘플링
    // 검격 텍스처(Other[1])는 4줄짜리 SubUV이므로, 그 중 한 줄을 선택하여 사용
    float2 slashUV = scaledUV;
    slashUV.y = (slashUV.y + 1.0f) / 4.0f; // 두 번째 줄 선택

    float slashShape = g_OtherTexture[1].Sample(DefaultSampler, slashUV).r;
    
    // 3. 시간에 따른 알파 애니메이션 (기존과 동일)
    float fadeAlpha = 1.0f;
    if (g_fTimeRatio > 0.2f)
    {
        fadeAlpha = 1.0f - (g_fTimeRatio - 0.2f) / 0.8f;
    }

    // 4. 최종 색상 및 블룸 적용 (기존과 동일)
    float3 goldColor = float3(2.5f, 1.8f, 0.5f);
    float3 finalColor = goldColor * slashShape * g_fBloomIntensity;
    float finalAlpha = slashShape * fadeAlpha;

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

    pass MonsterLineSlashPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LINESLASH_MAIN();
    }

}
