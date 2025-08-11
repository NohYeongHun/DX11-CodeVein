#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;


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

/* 정점쉐이더 : 정점 위치의 스페이스 변환(로컬 -> 월드 -> 뷰 -> 투영). */ 
/*          : 정점의 구성을 변경.(in:3개, out:2개 or 5개) */
/*          : 정점 단위(정점 하나당 VS_MAIN한번호출) */ 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;    
    
    /* 정점의 로컬위치 * 월드 * 뷰 * 투영 */ 
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);    
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    return Out;     
}

/* /W을 수행한다. 투영스페이스로 변환 */
/* 뷰포트로 변환하고.*/
/* 래스터라이즈 : 픽셀을 만든다. */

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

/* 만든 픽셀 각각에 대해서 픽셀 쉐이더를 수행한다. */
/* 픽셀의 색을 결정한다. */



PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    //Out.vColor.rgb = In.vTexcoord.y;
    
    return Out;    
}


// Texture Index에 따라서 다른 픽셀 쉐이더를 제공하기.
PS_OUT PS_MAIN2(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 uv = In.vTexcoord;
    
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv); // 원본 텍스처 색
    float4 fillerColor = float4(1, 1, 1, 1); // 다이아몬드 안에 채워질 색
    
    // 다이아몬드 중심
    float2 center = float2(0.5f, 0.5f);
    float2 delta = abs(uv - center);
    bool bIsInDiamond = (delta.x + delta.y) < 0.5f;
    
    if (bIsInDiamond)
    {
        Out.vColor = lerp(baseColor, fillerColor, 0.8f); // 부드럽게 섞기
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
    float4 fillerColor = float4(0, 0, 0, 1); // 다이아몬드 안에 채워질 색
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv); // 원본 텍스처 색

    
    // 다이아몬드 중심
    float2 center = float2(0.5f, 0.5f); 
    float2 delta = abs(uv - center);
    bool bIsInDiamond = (delta.x + delta.y) < 0.51f;

    // 아래서 위로 채우기
    bool bIsFillRegion = uv.y > (1.0 - g_fFillRatio); 

    if (bIsInDiamond && bIsFillRegion)
    {
        // 다이아몬드 내부이면서, 채워질 영역이면
        Out.vColor = lerp(baseColor, fillerColor, 0.8f); // 부드럽게 섞기
    }
    else
    {
        // 그 외는 원본 텍스처 유지
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
    
    // 핵심 한 줄: 원본→검정으로 선형 보간
    Out.vColor = baseColor;
    

    return Out;
}

float g_fAlpha;

// Logo Alpha Light 전용
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

// HP Bar Progress 용도.
PS_OUT PS_MAIN6(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 uv = In.vTexcoord;
    float4 fillerColor = float4(0.5, 0.5, 0.5, 1); // 줄어든 체력에 채워질 회색.
    float4 fillerBlack = float4(0, 0, 0, 1); // 줄어든 체력에 채워질 회색.
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv); // 원본 텍스처 색

    bool bIsFillGray = uv.x < g_fRightRatio && uv.x > g_fLeftRatio;
    bool bIsFill; 
    
    if (g_bIncrease)
        bIsFill = uv.x > g_fLeftRatio; // uv.x가 g_fLeftRatio보다 크다면?
    else
        bIsFill = uv.x > (1.0 - g_fFillRatio); // 이거 때문임. => 미리 증가해버려서 안보임.
       
    
    
    if (bIsFill)
    {
        // 감소해야한다면?
        if (bIsFillGray)
        {
            Out.vColor = lerp(baseColor, fillerColor, 0.8f); // 부드럽게 섞기  
        }
        else
            Out.vColor = fillerBlack; // 검정 설정.
    }
    else
    {
        // 그 외는 원본 텍스처 유지
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
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv); // 원본 텍스처 색
    
    Out.vColor = baseColor;
    
    //Out.vColor.rgb = In.vTexcoord.y;
    
    return Out;
}


technique11 DefaultTechnique
{
    /* 특정 패스를 이용해서 점정을 그려냈다. */
    /* 하나의 모델을 그려냈다. */ 
    /* 모델의 상황에 따라 다른 쉐이딩 기법 세트(명암 + 림라이트 + 스펙큘러 + 노멀맵 + ssao )를 먹여주기위해서 */
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();   
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass LoadingSlotPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN2();
    }

    pass SkillSlotPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN3();
    }

    pass FadeOutPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN4();
    }

    pass TitleBackGroundPass // Alpha Blend용 (Alpha 값만 섞음)
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN5();
    }

    pass HPProgressBarPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN6();
    }

    pass BlackColorPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN7();
    }

    pass LockOnPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0, 0, 0, 0), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN8();
    }
    


    ///* 모델의 상황에 따라 다른 쉐이딩 기법 세트(블렌딩 + 디스토션  )를 먹여주기위해서 */
    //pass DefaultPass1
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN1();

    //}

    ///* 정점의 정보에 따라 쉐이더 파일을 작성한다. */
    ///* 정점의 정보가 같지만 완전히 다른 취급을 하느 ㄴ객체나 모델을 그리는 방식 -> 렌더링방식에 차이가 생길 수 있다. */ 
    //pass DefaultPass1
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN1();

    //}

}
