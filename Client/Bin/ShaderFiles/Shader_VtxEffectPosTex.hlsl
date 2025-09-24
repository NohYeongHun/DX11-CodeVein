#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_DiffuseTextures[2];
texture2D g_OpacityTexture;
texture2D g_MaskTexture;


texture2D g_OtherTexture[5];



float g_fTime;
float g_fTimeRatio;
float g_fBloomIntensity = 2.0f;
float g_fScale; 

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

    float sparkShape = g_OpacityTexture.Sample(DefaultSampler, In.vTexcoord).r;

    float radialGlow = g_OtherTexture[2].Sample(DefaultSampler, In.vTexcoord).r;
    
    float finalShape = saturate(sparkShape + (radialGlow * 0.1f));

    float fadeAlpha = sin(saturate(g_fTimeRatio) * 3.14159f);
    
    float finalAlpha = finalShape * fadeAlpha;

    float3 goldColor = float3(2.5f, 1.8f, 0.5f) * g_fBloomIntensity;
    float3 finalColor = goldColor * finalShape;
    
    Out.vColor = float4(finalColor, finalAlpha);
    
    return Out;
}


//}

PS_OUT PS_LINESLASH_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 slashUV = In.vTexcoord;
    slashUV.y = (slashUV.y + 1.0f) / 4.0f; 
    float slashShape = g_OtherTexture[1].Sample(DefaultSampler, slashUV).r;

    slashShape = pow(slashShape, 2.0f);

    float2 noiseUV = In.vTexcoord * 2.0f + g_fTime * 0.5f;
    float noiseValue = g_OtherTexture[2].Sample(DefaultSampler, noiseUV).r;

    clip(noiseValue - pow(g_fTimeRatio, 2.2f));

    float3 goldColor = float3(2.5f, 1.8f, 0.5f);
    float3 finalColor = goldColor * slashShape * g_fBloomIntensity;
    
   
    float finalAlpha = slashShape * saturate((noiseValue - g_fTimeRatio) * 5.0f);

    Out.vColor = float4(finalColor, finalAlpha);
    
    return Out;
}




texture2D g_OtherTextures[3];
texture2D g_MaskTextures[2];
/* 심플 버전 - 명확한 클리핑 */
//PS_OUT PS_RENKETSU_SLASH_MAIN(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;
    
//    float2 uv = In.vTexcoord;
    
//    // 마스크들 샘플링
//    float4 vGlowMask = g_MaskTextures[0].Sample(DefaultSampler, uv);
//    float4 vSlashMask = g_MaskTextures[1].Sample(DefaultSampler, uv);
//    float4 vDiffuse = g_DiffuseTextures[1].Sample(DefaultSampler, uv);
    
//    // MaskTextures[0]: 흰 부분 사용, 검정 부분은 클리핑
//    float glow = vGlowMask.r;
    
//    // 검정 부분 완전 제거
//    clip(glow - 0.01f); // clip 함수로 더 간단하게
    
//    // MaskTextures[1]: 검은 부분 사용 (반전)
//    float slash = 1.0f - vSlashMask.r;
    
//    // 시간 애니메이션
//    float t = g_fTimeRatio;
    
//    // 슬래시 점진적 나타남
//    float slashReveal = smoothstep(0.3f - t, 0.5f - t * 0.5f, slash);
    
//    // 글로우 확산
//    float glowSpread = glow * (1.0f + t * 0.8f);
//    glowSpread = saturate(glowSpread);
    
//    // 색상 적용
//    float3 slashColor = vDiffuse.rgb * 2.0f;
//    float3 glowColor = float3(1.0f, 0.5f, 0.2f);
    
//    // 최종 색상
//    float3 finalColor = slashColor * slashReveal;
//    finalColor += glowColor * glowSpread * 0.7f;
    
//    // 알파 (글로우 마스크 영역 내에서만)
//    float alpha = saturate(slashReveal + glowSpread * 0.4f);
//    alpha *= (1.0f - smoothstep(0.8f, 1.0f, t));
//    alpha *= glow; // 글로우 마스크 경계 적용
    
//    Out.vColor = float4(finalColor, alpha);
    
//    return Out;
//}
PS_OUT PS_RENKETSU_SLASH_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 uv = In.vTexcoord;
    
    // 마스크들 샘플링
    float4 vGlowMask = g_MaskTextures[0].Sample(DefaultSampler, uv); // 흰색 글로우 마스크
    float4 vSlashMask = g_MaskTextures[1].Sample(DefaultSampler, uv); // 검은 슬래시 마스크
    
    // MaskTextures[0]: 흰 부분만 렌더링 (검정 부분 클리핑)
    float glowArea = vGlowMask.r;
    clip(glowArea - 0.01f);
    
    if (vGlowMask.r < 0.1f && vGlowMask.g < 0.1f && vGlowMask.b < 0.1f)
        discard;
    
    
    // MaskTextures[1]: 검은 부분이 슬래시 (반전)
    float slashCore = 1.0f - vSlashMask.r;
    
    // 1. 검은 검격 부분
    float3 slashColor = float3(0.1f, 0.05f, 0.05f); // 거의 검은색
    
    // 2. 주변 연한 빨간색 아우라
    // 슬래시에서 멀어질수록 연한 빨간색으로 변화
    float auraGradient = 1.0f - slashCore; // 슬래시에서 멀수록 1에 가까움
    float3 auraColor = float3(1.f, 0.01f, 0.01f); // 연한 빨간색
    
    // 아우라 범위 (슬래시 주변에만 생성)
    float auraRange = smoothstep(0.0f, 0.5f, auraGradient);
    
    // 3. 색상 블렌딩
    // 슬래시 중심은 검은색, 주변은 연한 빨간색
    float3 finalColor = slashColor;  //lerp(slashColor, auraColor, auraRange);
    
    // 슬래시 중심부를 더 어둡게 강조
    if (slashCore > 0.8f)
    {
        finalColor = slashColor * 0.5f; // 중심부는 더 검게
    }
    
    // 4. 알파값 설정
    // 슬래시는 불투명, 아우라는 반투명
    float slashAlpha = slashCore; // 슬래시 부분
    float auraAlpha = auraRange * 0.4f * (1.0f - slashCore); // 아우라는 반투명
    
    float alpha = saturate(slashAlpha + auraAlpha);
    alpha *= glowArea; // 글로우 마스크 경계 적용
    
    Out.vColor = float4(finalColor, 1.f);
    
    return Out;
} 

/* 렌케츠 슬래시 - 점점 벌어지는 검격 효과 */
/* 렌케츠 슬래시 - 마스크의 흰색 부분에만 효과 적용 */
/* 더 간단하고 명확한 버전 */
//PS_OUT PS_RENKETSU_SLASH_MAIN(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;
    
//    float2 uv = In.vTexcoord;
    
//    // 텍스처 샘플링
//    float4 vMask = g_MaskTexture.Sample(DefaultSampler, uv);
//    float4 vDiffuse = g_DiffuseTextures[1].Sample(DefaultSampler, uv);
    
//    // 마스크의 흰색 부분만 사용
//    float maskValue = vMask.r;
    
//    // 검은 부분은 discard
//    if (maskValue < 0.01f)
//    {
//        discard;
//    }
    
//    // 시간에 따른 확장
//    float expand = g_fTimeRatio;
    
//    // 점진적으로 나타나는 효과 (threshold animation)
//    float threshold = 1.0f - expand * 1.2f;
//    float slash = smoothstep(threshold, threshold + 0.2f, maskValue);
    
//    // 너비 확장 (선택적)
//    float widthBoost = 1.0f + expand * 0.5f;
//    slash = saturate(slash * widthBoost);
    
//    // 노이즈 텍스처의 색상을 마스크의 흰색 부분에 적용
//    float3 color = vDiffuse.rgb * 2.0f; // 빨간색 노이즈 색상 강화
    
//    // 중심부를 더 밝게
//    float brightness = pow(maskValue, 0.5f);
//    color *= (1.0f + brightness);
    
//    // 페이드 효과
//    float fadeOut = 1.0f - smoothstep(0.8f, 1.0f, expand);
    
//    // 최종 알파 (마스크 흰색 영역 * 애니메이션 * 페이드)
//    float finalAlpha = maskValue * slash * fadeOut;
    
//    Out.vColor = float4(color, finalAlpha);
    
//    return Out;
//}




technique11 DefaultTechnique
{
    pass DefaultPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN(); 
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass HitFlashPass // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HITFLASH_MAIN();
    }

    pass MonsterLineSlashPass // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LINESLASH_MAIN();
    }

    pass RenketsuLineSlashPass// 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RENKETSU_SLASH_MAIN();
    }

}
