#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_DiffuseTexture;
Texture2D g_BloomTexture;
//float2 g_vTexelSize = float2(1.0f / (1920.f / 4.f), 1.0f / (1080.f / 4.f));
float2 g_vTexelSize = float2(1.0f / (1920.f / 8.f), 1.0f / (1080.f / 8.f));

float threshold = 0.95f; // 조정 가능 (예: 0.8 ~ 0.95)
float soft = 0.1f; // 페이드 범위
float g_fBloomIntensity = 2.f;

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

PS_OUT PS_BLOOMBRIGHT_MAIN(PS_IN In)
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
    //vColor += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord) * weights[0];
    vColor += g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord) * weights[0];
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
    //vColor += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord) * weights[0];
    vColor += g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord) * weights[0];
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

//PS_OUT PS_SUM_BLUR(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT)0;
//    float4 vColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
//    vColor += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
//    vColor += g_BloomTexture.Sample(DefaultSampler, In.vTexcoord);
//    Out.vColor = vColor;
//    return Out;
//}

PS_OUT PS_SUM_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vSceneColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vBloomColor = g_BloomTexture.Sample(DefaultSampler, In.vTexcoord);

    // 금빛 느낌을 강조하기 위해 약간의 오렌지색 틴트(Tint)를 곱해줍니다.
    float3 vBloomTint = float3(1.0f, 0.7f, 0.3f);
    vBloomColor.rgb *= vBloomTint;

    // C++에서 받아온 g_fBloomIntensity와 같은 변수로 강도 조절
    Out.vColor = vSceneColor + vBloomColor * g_fBloomIntensity;
    
    // (선택) 톤 매핑으로 자연스러운 마무리
    Out.vColor.rgb = Out.vColor.rgb / (Out.vColor.rgb + 1.0f);
    
    return Out;
}


technique11 DefaultTechnique
{
    pass BloomExtractBrightPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOOMBRIGHT_MAIN();
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    }

    pass GaussianBlurXPass // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_X();
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    }

    pass GaussianBlurYPass // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
                
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_Y();
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    }
    pass SumBlurPass // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
                
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SUM_BLUR();
    
    
    
    }

}
