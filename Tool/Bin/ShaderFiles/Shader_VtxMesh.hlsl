#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vCamPosition;

/*    */
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;

float g_fTime;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(mul(float4(In.vBinormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    
    
    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;

};



struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
     /* 노멀 벡터 하나를 정의하기위한 독립적인 로컬스페이스를 만들고 그 공간안에서의 방향벡터를 정의 */
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vDiffuse = vMtrlDiffuse;
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}



float g_fFadeOutStartTime;
float g_fFadeOutDuration;
float g_fEmissiveIntensity;
float4 g_vBaseColor;

// 픽셀 셰이더 메인 함수
PS_OUT PS_WIND_EFFECT_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // === 1. UV 좌표 가져오기 ===
    float2 original_uv = In.vTexcoord;

    // === 2. UV 변환 (회전만) - 행렬 방식 ===
    float2 center = float2(0.5f, 0.5f);
    
    // 1. 변환의 중심을 원점(0,0)으로 이동
    float2 p = original_uv - center;

    // 2. 확대(Zoom-in) 효과 제거
    /*
    float growthSpeed = 0.5f;
    float scale = 1.0f + g_fTime * growthSpeed;
    p /= scale;
    */

    // 3. 회전
    float rotateSpeed = 0.5f; // 회전 속도
    float angle = g_fTime * rotateSpeed;
    float s = sin(angle);
    float c = cos(angle);

    // 2D 회전 행렬을 p에 적용
    float2 rotated_p;
    rotated_p.x = p.x * c - p.y * s;
    rotated_p.y = p.x * s + p.y * c;
    p = rotated_p;
    
    // 4. 다시 원래 중심으로 이동
    float2 transformed_uv = p + center;

    // 5. 초기 UV 스케일링 적용
    float2 uv = transformed_uv;

    // === 3. 텍스처 샘플링 ===
    float4 texColor = g_DiffuseTexture.Sample(DefaultSampler, uv);

    float alpha = texColor.r;
    if (alpha < 0.3f)
        discard;

    // === 4. 페이드 아웃 ===
    float fadeFactor = 1.0f;
    if (g_fTime > g_fFadeOutStartTime)
    {
        float fadeT = saturate((g_fTime - g_fFadeOutStartTime) / g_fFadeOutDuration);
        fadeFactor = 1.0f - fadeT;
    }

    // === 5. 최종 색상 ===
    float4 finalColor = float4(g_vBaseColor.rgb * texColor.r * g_fEmissiveIntensity, alpha);
    finalColor.a *= fadeFactor;

    // === 6. 출력 ===
    Out.vDiffuse = finalColor;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}

//PS_OUT PS_WIND_EFFECT_MAIN(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;

//    // === 1. UV 좌표 가져오기 ===
//    float2 uv = In.vTexcoord * float2(2.0f, 1.5f);

//    // === 2. UV 회전 (Spiral 느낌) ===
//    float2 center = float2(0.5f, 0.5f);
//    float2 p = uv - center;
//    float r = length(p);
//    float theta = atan2(p.y, p.x);

//    // 회전 + 꼬임
//    float rotateSpeed = 0.5f;
//    float spiralStrength = 0.5f;
//    //theta += rotateSpeed * g_fTime + spiralStrength * r;
//    theta += rotateSpeed * g_fTime; // r 비례 꼬임 제거

//    // 극좌표 → UV 복원
//    uv = float2(cos(theta), sin(theta)) * r + center;

//    // === 3. 텍스처 샘플링 ===
//    float4 texColor = g_DiffuseTexture.Sample(DefaultSampler, uv);

//    float alpha = texColor.r;
//    if (alpha < 0.1f)
//        discard;

//    // === 4. 페이드 아웃 ===
//    float fadeFactor = 1.0f;
//    if (g_fTime > g_fFadeOutStartTime)
//    {
//        float fadeT = saturate((g_fTime - g_fFadeOutStartTime) / g_fFadeOutDuration);
//        fadeFactor = 1.0f - fadeT;
//    }

//    // === 5. 최종 색상 ===
//    float4 finalColor = float4(g_vBaseColor.rgb * texColor.r * g_fEmissiveIntensity, alpha);
//    finalColor.a *= fadeFactor;

//    // === 6. 출력 ===
//    Out.vDiffuse = finalColor;
//    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
//    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);

//    return Out;
    
//}
    

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    //pass SkyPass // 하늘 전용 패스
    //{
    //    SetRasterizerState(RS_Cull_CW);
    //    SetDepthStencilState(DSS_None, 0);
    //    SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

    //    VertexShader = compile vs_5_0 VS_MAIN();
    //    GeometryShader = NULL;
    //    PixelShader = compile ps_5_0 PS_MAIN2();
    //}

    pass WindEffectPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WIND_EFFECT_MAIN();
    }

   
}
