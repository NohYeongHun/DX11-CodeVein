#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_SceneTexture;
Texture2D g_DistortionMap;
float g_fDistortionStrength;
float2 g_vScreenSize; // Screen Size

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

    // 1. [추가] 픽셀의 화면 좌표(SV_POSITION)를 이용해 Screen UV를 계산합니다.
    float2 vScreenUV = In.vPosition.xy / g_vScreenSize.xy;
    
    // 2. [유지] Distortion 맵을 샘플링하는 부분은 올바릅니다.
    //    단, 왜곡 패턴을 위한 UV는 모델의 UV(In.vTexcoord)를 그대로 사용합니다.
    float2 distortion = g_DistortionMap.Sample(DefaultSampler, In.vTexcoord).rg;
    distortion = (distortion * 2.0f - 1.0f) * g_fDistortionStrength;

    // 3. [수정] 모델의 UV가 아닌, 1번에서 계산한 Screen UV를 비틉니다.
    float2 vDistortedUV = vScreenUV + distortion;

    // 4. [수정] 비틀어진 Screen UV로 배경 텍스처를 샘플링합니다.
    float4 sceneColor = g_SceneTexture.Sample(DefaultSampler, vDistortedUV);

    Out.vColor = sceneColor;
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
