#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_DiffuseTexture;
Texture2D g_BloomTexture;
float2 g_vTexelSize = float2(1.0f / (1920.f / 8.f), 1.0f / (1080.f / 8.f));


float threshold = 0.95f; 
float soft = 0.1f; 
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

float3 ACESFilm(float3 x)
{
    const float a = 2.51f; const float b = 0.03f; const float c = 2.43f; const float d = 0.59f; const float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

// 빛이 번질 만큼 충분히 밝은 부분만 골라내기 위함.
PS_OUT PS_BLOOMBRIGHT_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    float4 BrightColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 originalColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord); // 원본 색상
    float brightness = dot(originalColor.rgb, float3(0.2126f, 0.7152f, 0.0722f)); // 상대 휘도 계산. (인간의 눈으로 본 밝기 계싼)

    float factor = smoothstep(threshold, threshold + soft, brightness); // 경계 필터링을 위함 => 어디서부터 빛나게 할것인지를 결정.
    BrightColor = originalColor * factor; // 밝은 부분만 추출 => 어두운 부분은 factor가 0에 가까워져서 검게되고 밝은 부분만 원래 색상을 유지하며 살아남음.
    Out.vColor = BrightColor;
    return Out;
}

// 가우시안 블러 => 추출된 밝은 영역을 사방으로 퍼뜨려 Glow 효과를 만드는 단계
// BLUR_X니까 가로 방향으로 블러 처리 => 블러처리를 가로 -> 세로 방향으로 수행하면 N * N 연산보다 훨씬 효율적. => 이중 포문을 1중 포문 두개로 분리하면 연산량이 줄어듦
PS_OUT PS_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    float4 vColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float weights[5] = {
        0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216
    }; 
    vColor += g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord) * weights[0];
    for (int i = 1; i < 5; ++i)
    {
        float2 offset = float2(g_vTexelSize.x * i, 0.0f);
        vColor += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord + offset) * weights[i];
        vColor += g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord - offset) * weights[i];
    }
    Out.vColor = vColor;
    return Out;
}

// BLUR_Y니까 세로 방향으로 블러 처리
PS_OUT PS_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;
    float4 vColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float weights[5] = {
        0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216
    }; 
    vColor += g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord) * weights[0];
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
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vSceneColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord); // Combine Texture
    float4 vBloomColor = g_BloomTexture.Sample(DefaultSampler, In.vTexcoord); // Bloom Texture

    float3 finalColor = vSceneColor.rgb + vBloomColor.rgb * g_fBloomIntensity;
    finalColor = ACESFilm(finalColor);
    
    Out.vColor = float4(finalColor, vSceneColor.a);
    //Out.vColor.rgb = Out.vColor.rgb / (Out.vColor.rgb + 1.0f);
    
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
