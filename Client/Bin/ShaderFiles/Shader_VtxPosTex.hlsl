#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_MaskTexture;
texture2D g_NoiseTexture;

texture2D g_DiffuseTextures[2];


float g_fFillRatio;
float g_fNoiseTime;

float g_fRightRatio;
float g_fLeftRatio;
float g_fScrollSpeed;
bool g_bIncrease;


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
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    //Out.vColor.rgb = In.vTexcoord.y;
    
    return Out;    
}


PS_OUT PS_LOADINGSLOT_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 uv = In.vTexcoord;
    
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv);
    float4 fillerColor = float4(1, 1, 1, 1);
    
    float2 center = float2(0.5f, 0.5f);
    float2 delta = abs(uv - center);
    bool bIsInDiamond = (delta.x + delta.y) < 0.5f;
    
    if (bIsInDiamond)
    {
        Out.vColor = lerp(baseColor, fillerColor, 0.8f);
    }
    else
    {
        discard;
        //Out.vColor = baseColor;
    }
    
    return Out;
}

PS_OUT PS_SKILLSLOT_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 uv = In.vTexcoord;
    float4 fillerColor = float4(0, 0, 0, 1); 
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv); 

    
    float2 center = float2(0.5f, 0.5f); 
    float2 delta = abs(uv - center);
    bool bIsInDiamond = (delta.x + delta.y) < 0.51f;

    bool bIsFillRegion = uv.y > (1.0 - g_fFillRatio); 

    if (bIsInDiamond && bIsFillRegion)
    {
        Out.vColor = lerp(baseColor, fillerColor, 0.8f); 
    }
    else
    {
        Out.vColor = baseColor;
    }

    return Out;
}

float g_fFade;

// Fade Out Shader
PS_OUT PS_FADEOUT_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 uv = In.vTexcoord;
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv);
    float4 fillerColor = float4(0, 0, 0, 1);
   
    // Alpha Blend
    baseColor.rgb = lerp(baseColor.rgb, float3(0.0, 0.0, 0.0), saturate(g_fFade));
    
    Out.vColor = baseColor;
    

    return Out;
}

float g_fAlpha;


PS_OUT PS_TITLE_BACKGROUND_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor.a = g_fAlpha;
    
    return Out;
}


// HP Bar Progress
PS_OUT PS_HP_PROGRESSBAR_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 uv = In.vTexcoord;
    float4 fillerColor = float4(0.5, 0.5, 0.5, 1);
    float4 fillerBlack = float4(0, 0, 0, 1);
    
    // 시간 변수 및 스크롤 속도 (외부에서 전달되어야 함)

    // 스크롤된 uv를 사용하여 baseColor를 샘플링
    float2 scrolledUv = uv;
    scrolledUv.x = frac(uv.x - g_fNoiseTime * g_fScrollSpeed);

    // 왜곡 효과 적용 (선택 사항)
    float4 distortion = g_NoiseTexture.Sample(PointSampler, scrolledUv);
    float distortionStrength = 0.05;
    float2 distortedUv = scrolledUv + (distortion.rg - 0.5) * distortionStrength;

    // 흐르는 효과가 적용된 baseColor
    float4 baseColor = g_Texture.Sample(PointSampler, distortedUv);
    
    // bIsFill과 bIsFillGray는 원래의, 즉 스크롤되지 않은 uv를 사용합니다.
    bool bIsFillGray = uv.x < g_fRightRatio && uv.x > g_fLeftRatio;
    bool bIsFill;
    
    if (g_bIncrease)
        bIsFill = uv.x > g_fLeftRatio;
    else
        bIsFill = uv.x > (1.0 - g_fFillRatio);
    
    if (bIsFill)
    {
        // 흐르는 효과가 적용된 baseColor를 사용해 lerp
        if (bIsFillGray)
        {
            Out.vColor = lerp(baseColor, fillerColor, 0.8f);
        }
        else
            Out.vColor = fillerBlack;
    }
    else
    {
        Out.vColor = baseColor;
    }

    return Out;
}

PS_OUT PS_BLACK_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = float4(0.f, 0.f, 0.f, 0.9f);
    //Out.vColor.rgb = In.vTexcoord.y;
    
    return Out;
}

PS_OUT PS_LOCKON_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 uv = In.vTexcoord;
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv); 
    
    Out.vColor = baseColor;
    
    //Out.vColor.rgb = In.vTexcoord.y;
    
    return Out;
}

PS_OUT PS_FADEIN_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 텍스처 없이 검정색 오버레이만 생성
    float fadeAmount = saturate(g_fFade);
    
    // 검정색 오버레이 (알파 값으로 점진적 페이드 제어)
    Out.vColor = float4(0.0, 0.0, 0.0, fadeAmount);
    

    return Out;
}

float g_fTimeRatio; // 0.0 ~ 1.0 (시간 진행도)
float g_fScale; // 스케일 팩터

PS_OUT PS_LINESLASH_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // UV 좌표를 중심에서 스케일링
    float2 center = float2(0.5f, 0.5f);
    float2 scaledUV = center + (In.vTexcoord - center) / g_fScale;
    
    // 스케일된 UV가 범위를 벗어나면 discard
    if (scaledUV.x < 0.0f || scaledUV.x > 1.0f || scaledUV.y < 0.0f || scaledUV.y > 1.0f)
        discard;

    // 텍스처 없이 검정색 오버레이만 생성
    vector vSourDiffuse = g_Texture.Sample(DefaultSampler, scaledUV);
    vector vDestDiffuse = g_MaskTexture.Sample(DefaultSampler, scaledUV);
    
    // 소스 텍스처가 검정색이면 discard
    if (vDestDiffuse.r < 0.1f && vDestDiffuse.g < 0.1f && vDestDiffuse.b < 0.1f)
        discard;
    
    
    vector vMask = g_MaskTexture.Sample(DefaultSampler, scaledUV);
    vector vMtrlDiffuse = vDestDiffuse * (1.f - vMask) + vSourDiffuse * (vMask);
    
    // 시간에 따른 알파 페이드아웃
    float fadeAlpha = 1.0f - g_fTimeRatio;
    
    
    vMtrlDiffuse.a *= fadeAlpha;
    
    Out.vColor = vMtrlDiffuse;
    

    
    

    return Out;
}

PS_OUT PS_HITFLASH_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // UV 좌표를 중심에서 스케일링
    float2 center = float2(0.5f, 0.5f);
    float2 scaledUV = center + (In.vTexcoord - center) / g_fScale;
    
    // 스케일된 UV가 범위를 벗어나면 discard
    if (scaledUV.x < 0.0f || scaledUV.x > 1.0f || scaledUV.y < 0.0f || scaledUV.y > 1.0f)
        discard;

    // 텍스처 없이 검정색 오버레이만 생성
    vector vSourDiffuse = g_Texture.Sample(DefaultSampler, scaledUV);
    vector vDestDiffuse = g_MaskTexture.Sample(DefaultSampler, scaledUV);
    
    // 소스 텍스처가 검정색이면 discard
    if (vDestDiffuse.r < 0.1f && vDestDiffuse.g < 0.1f && vDestDiffuse.b < 0.1f)
        discard;
    
    
    vector vMask = g_MaskTexture.Sample(DefaultSampler, scaledUV);
    vector vMtrlDiffuse = vDestDiffuse * (1.f - vMask) + vSourDiffuse * (vMask);
    
    // 시간에 따른 알파 페이드아웃
    float fadeAlpha = 1.0f - g_fTimeRatio;
    vMtrlDiffuse.a *= fadeAlpha;
    
    Out.vColor = vMtrlDiffuse;

    return Out;
}


PS_OUT PS_MONSTERHP_PROGRESSBAR_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    
    float2 uv = In.vTexcoord;
    float4 baseColor = g_Texture.Sample(PointSampler, uv);
    
    float4 fillerColor = float4(0.4, 0.4, 0.4, 1);
    float4 fillerBlack = float4(0, 0, 0, 1);
    
    // 침범할 범위를 아주 작은 값으로 설정 (예: 0.005)
    float invasionMargin = 0.005f;
    
    // 붉은색 픽셀인지 판단하는 조건
    bool isReddish = (baseColor.r > 0.7f) && (baseColor.g < 0.5f) && (baseColor.b < 0.5f);
    
    
    
    bool bIsFillGray = uv.x < g_fRightRatio && uv.x > g_fLeftRatio;
    
    bool bIsFill;
    
    if (isReddish)
    {
        if (g_bIncrease)
            bIsFill = uv.x > g_fLeftRatio;
        else 
            bIsFill = uv.x > (1.0 - g_fFillRatio);
        
        if (bIsFill)
        {
            if (bIsFillGray)
                Out.vColor = lerp(baseColor, fillerColor, 0.8f);
            else
                Out.vColor = fillerBlack;
        }
        else
        {
            Out.vColor = baseColor;
        }
    }
    else
    {
        // 붉은색이 아니면 (외곽선), 항상 렌더링
        Out.vColor = baseColor;
    }
    
    return Out;
}

PS_OUT PS_QUEENKNIGHT_ENCOUNTER_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 baseColor = g_DiffuseTextures[0].Sample(DefaultSampler, In.vTexcoord);
    

    // g_fFade에 따라 점차 투명해지기 (0.0 = 완전히 보임, 1.0 = 완전히 투명)
    float fadeAlpha = 1.0f - saturate(g_fFade);
    baseColor.a *= fadeAlpha;

    Out.vColor = baseColor;

    return Out;
}

// Image 1: 메인 UI 텍스처 (Queen's Knight 글자 및 문양)
// Image 2: 마스크 텍스처 (UI 주변을 어둡게 할 영역을 정의 - 밝은 부분이 어둡게 됨)
// Image 3: 이펙트 (현재 쉐이더에 사용되지 않음)







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

    pass LoadingSlotPass // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LOADINGSLOT_MAIN();
    }

    pass SkillSlotPass // 2
    { 
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SKILLSLOT_MAIN();
    }

    pass FadeOutPass // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FADEOUT_MAIN();
    }

    pass TitleBackGroundPass // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TITLE_BACKGROUND_MAIN();
    }

    pass HPProgressBarPass // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HP_PROGRESSBAR_MAIN();
    }

    pass BlackColorPass // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLACK_MAIN();
    }

    pass LockOnPass // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0, 0, 0, 0), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LOCKON_MAIN();
    }


    pass FadeInPass // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FADEIN_MAIN();
    }
    
    pass MonsterLineSlashPass // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LINESLASH_MAIN();
    }

    pass MonsterHitFlashPass // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HITFLASH_MAIN();
    }

    pass MonsterHpProgressBarPass // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MONSTERHP_PROGRESSBAR_MAIN();
    }

    pass QueenKnightEncounterUIPass // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_QUEENKNIGHT_ENCOUNTER_MAIN();
    }

  

}
