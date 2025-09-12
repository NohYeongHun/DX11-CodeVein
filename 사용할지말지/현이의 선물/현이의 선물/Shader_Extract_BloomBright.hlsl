#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_DiffuseTexture;
Texture2D g_BlurTexture;
float2 g_vTexelSize = float2(1.0f / (1920.f / 4.f), 1.0f / (1080.f / 4.f));
float threshold = 0.5f; // 조정 가능 (예: 0.8 ~ 0.95)
float soft = 0.20f; // 페이드 범위

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;

    float4x4 matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

// PS_OUT PS_DOWNSAMPLE_2X2(PS_IN In)
// {
//     PS_OUT Out = (PS_OUT)0;
//     float4 color = float4(0, 0, 0, 0);
//
//     // 주변 4픽셀 샘플링 (2x2)
//     color += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
//     color += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord + float2(g_vTexelSize.x, 0));
//     color += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord + float2(0, g_vTexelSize.y));
//     color += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord + g_vTexelSize);
//
//     color *= 0.25f; // 평균값
//     Out.vColor = color;
//     return Out;
// }

// PS_OUT PS_DOWNSAMPLE_4X4(PS_IN In)
// {
//     PS_OUT Out = (PS_OUT)0;
//     float4 color = float4(0, 0, 0, 0);
//
//     // 4x4 샘플링
//     for (int y = 0; y < 4; ++y)
//     {
//         for (int x = 0; x < 4; ++x)
//         {
//             float2 offset = float2(x * g_vTexelSize.x, y * g_vTexelSize.y);
//             color += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord + offset);
//         }
//     }
//
//     color *= 1.0 / 16.0; // 16픽셀 평균
//     Out.vColor = color;
//     return Out;
// }

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    float4 BrightColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 originalColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    float brightness = dot(originalColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));

    float factor = smoothstep(threshold, threshold + soft, brightness);
    BrightColor = originalColor * factor;
    Out.vColor = BrightColor;
    return Out;
}

PS_OUT PS_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    float4 vColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float weights[5] = {
        0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216
    }; // 가우시안 블러 가중치
    vColor += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord) * weights[0];
    // 수평 방향 블러
    for (int i = 1; i < 5; ++i)
    {
        float2 offset = float2(g_vTexelSize.x * i, 0.0f);
        vColor += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord + offset) * weights[i];
        vColor += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord - offset) * weights[i];
    }
    Out.vColor = vColor;
    return Out;
}

PS_OUT PS_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    float4 vColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float weights[5] = {
        0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216
    }; // 가수이안 블러 가중치
    vColor += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord) * weights[0];
    // 수평 방향 블러
    for (int i = 1; i < 5; ++i)
    {
        float2 offset = float2(0.0f, g_vTexelSize.y * i);
        vColor += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord + offset) * weights[i];
        vColor += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord - offset) * weights[i];
    }
    Out.vColor = vColor;
    return Out;
}

PS_OUT PS_SUM_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    float4 vColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    vColor += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vColor += g_BlurTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor = vColor;
    return Out;
}

technique11 DefaultTechnique
{
    pass BloomExtractBrightPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    }

    pass GaussianBlurXPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_X();
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    }

    pass GaussianBlurYPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
                
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_Y();
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    }
    pass SumBlurYPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
                
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SUM_BLUR();
    
    
    
    }

}
