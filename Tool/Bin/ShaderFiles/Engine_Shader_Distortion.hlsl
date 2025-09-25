#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float g_fDistortionStrength = 0.2f; // 기본값을 0이 아닌 값으로 설정해두면 테스트 시 편리합니다.
float2 g_vScreenSize;

Texture2D g_CombineTexture; // 원본 씬 텍스처
Texture2D g_DistortionTexture; // 왜곡 정보 텍스처

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
    VS_OUT Out;
    float4x4 matWVP = mul(mul(g_WorldMatrix, g_ViewMatrix), g_ProjMatrix);
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
    float4 vColor : SV_TARGET;
};

PS_OUT PS_DISTORTION_MAIN(PS_IN In)
{
    PS_OUT Out;

    // 1. 현재 픽셀의 화면 UV를 계산합니다. (0.0 ~ 1.0 범위)
    float2 vScreenUV = In.vPosition.xy / g_vScreenSize.xy;

    // 2. [수정] '화면 UV'를 사용해서 '왜곡 텍스처'를 샘플링합니다.
    //    .rg 채널에 왜곡 방향 정보(보통 노멀 벡터의 x, y)가 저장되어 있다고 가정합니다.
    float2 vDistortionVector = g_DistortionTexture.Sample(DefaultSampler, vScreenUV).rg;

    // 3. [설명] 샘플링한 값을 -1.0 ~ 1.0 범위로 변환하여 방향으로 사용하고, 강도를 곱해줍니다.
    //    텍스처에 저장된 값이 0~1 범위이므로 2를 곱하고 1을 빼서 -1~1 범위로 만듭니다.
    vDistortionVector = (vDistortionVector * 2.0f - 1.0f) * g_fDistortionStrength;

    // 4. [설명] 원래 샘플링해야 할 화면 UV에 왜곡 벡터를 더해 최종 UV를 계산합니다.
    float2 vDistortedUV = vScreenUV + vDistortionVector;
    
    // 5. [수정] 계산된 최종 UV를 이용해 '원본 씬 텍스처'를 샘플링합니다.
    float4 vSceneColor = g_CombineTexture.Sample(DefaultSampler, vDistortedUV);

    Out.vColor = vSceneColor;
    return Out;
}
technique11 DefaultTechnique
{
    pass DistortionPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION_MAIN();
    }
}
