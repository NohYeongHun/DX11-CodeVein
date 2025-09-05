#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_MaskTexture;


float g_fFillRatio;

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


PS_OUT PS_MAIN2(PS_IN In)
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
        Out.vColor = baseColor;
    }
    
    return Out;
}

PS_OUT PS_MAIN3(PS_IN In)
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
        Out.vColor = lerp(baseColor, fillerColor, 0.8f); // �ε巴�� ����
    }
    else
    {
        Out.vColor = baseColor;
    }

    return Out;
}

float g_fFade;

// Fade Out Shader
PS_OUT PS_MAIN4(PS_IN In)
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


PS_OUT PS_MAIN5(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor.a = g_fAlpha;
    
    return Out;
}

float g_fRightRatio; 
float g_fLeftRatio;
bool g_bIncrease;

// HP Bar Progress
PS_OUT PS_MAIN6(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 uv = In.vTexcoord;
    float4 fillerColor = float4(0.5, 0.5, 0.5, 1); 
    float4 fillerBlack = float4(0, 0, 0, 1);
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv);

    bool bIsFillGray = uv.x < g_fRightRatio && uv.x > g_fLeftRatio;
    bool bIsFill; 
    
    if (g_bIncrease)
        bIsFill = uv.x > g_fLeftRatio; 
    else
        bIsFill = uv.x > (1.0 - g_fFillRatio); 
       
    
    
    if (bIsFill)
    {
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

PS_OUT PS_MAIN7(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = float4(0.f, 0.f, 0.f, 0.9f);
    //Out.vColor.rgb = In.vTexcoord.y;
    
    return Out;
}

PS_OUT PS_MAIN8(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 uv = In.vTexcoord;
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv); 
    
    Out.vColor = baseColor;
    
    //Out.vColor.rgb = In.vTexcoord.y;
    
    return Out;
}

PS_OUT PS_MAIN9(PS_IN In)
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
    
    //if (vDestDiffuse.r > 0.9f && vDestDiffuse.g > 0.9f && vDestDiffuse.b > 0.9f)
    //    discard;
    
    
    //vector vMask = g_MaskTexture.Sample(DefaultSampler, scaledUV);
    //vector vMtrlDiffuse = vDestDiffuse * (vMask) + vSourDiffuse * (1 - vMask);
    
    // 시간에 따른 알파 페이드아웃
    float fadeAlpha = 1.0f - g_fTimeRatio;
    vMtrlDiffuse.a *= fadeAlpha;
    
    Out.vColor = vMtrlDiffuse;
    //float2 uv = In.vTexcoord;
    //float4 baseColor = g_MaskTexture.Sample(DefaultSampler, uv);
    //float4 baseColor = g_Texture.Sample(DefaultSampler, uv);
    
    //Out.vColor = baseColor;
    
    

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

    pass LoadingSlotPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN2();
    }

    pass SkillSlotPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN3();
    }

    pass FadeOutPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN4();
    }

    pass TitleBackGroundPass // Alpha Blend
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN5();
    }

    pass HPProgressBarPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN6();
    }

    pass BlackColorPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN7();
    }

    pass LockOnPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0, 0, 0, 0), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN8();
    }


    pass FadeInPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN9();
    }
    
    pass MonsterLineSlashPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LINESLASH_MAIN();
    }

    pass MonsterHitFlashPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HITFLASH_MAIN();
    }


}
