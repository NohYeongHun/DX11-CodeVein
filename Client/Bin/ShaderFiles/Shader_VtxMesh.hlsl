#include "Engine_Shader_Defines.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

/*재질*/
texture2D g_DiffuseTexture;
texture2D g_DiffuseTextures[6];

texture2D g_NormalTexture;
texture2D g_NoiseTexture;

/*재질*/
texture2D g_DissolveTexture;

vector g_vMtrlAmbient = 1.f;
vector g_vMtrlSpecular = 1.f;


// 시간.
float g_fDissolveTime;


float g_fTime;
float g_fRatio;
float g_fScrollSpeed = 2.f;


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

/* 정점쉐이더 : 정점 위치의 스페이스 변환(로컬 -> 월드 -> 뷰 -> 투영). */ 
/*          : 정점의 구성을 변경.(in:3개, out:2개 or 5개) */
/*          : 정점 단위(정점 하나당 VS_MAIN한번호출) */ 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    /* 정점의 로컬위치 * 월드 * 뷰 * 투영 */ 
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(mul(float4(In.vBinormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}


/* /W을 수행한다. 투영스페이스로 변환 */
/* 뷰포트로 변환하고.*/
/* 래스터라이즈 : 픽셀을 만든다. */

//struct PS_IN
//{
//    float4 vPosition : SV_POSITION;
//    float4 vNormal : NORMAL;
//    float2 vTexcoord : TEXCOORD0;
//    float4 vWorldPos : TEXCOORD1;

//};

struct PS_BACKBUFFER_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;

};

//struct PS_OUT
//{
//    float4 vColor : SV_TARGET0;
//};

struct PS_OUT_BACKBUFFER
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

/* 만든 픽셀 각각에 대해서 픽셀 쉐이더를 수행한다. */
/* 픽셀의 색을 결정한다. */


PS_OUT_BACKBUFFER PS_DEFFERED_OUT(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_DEFFERED_NORMALOUT(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
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

PS_OUT_BACKBUFFER PS_DEFFERED_DISSOLVE_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMtrlDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord);
    
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    // 안에 숫자가 0이되면 안그린다.
    
    clip(vMtrlDissolve.r - g_fDissolveTime);
    
    return Out;
}


/* */
float g_fScrollSpeedU = 2.f;
float g_fScrollSpeedV = 2.f;


PS_OUT_BACKBUFFER PS_DEFFERED_BLOODPILLARA_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    // [추가] 1. 시간에 따라 흐르는 UV 좌표 계산
    // Diffuse 텍스처용 UV. V(세로) 방향으로만 흐르도록 설정.
    float2 vDiffusePannedUV = In.vTexcoord + float2(0.f, g_fScrollSpeedV) * g_fTime;
    
    // Noise 텍스처용 UV. Diffuse와는 다른 속도로 흐르게 하여 깊이감을 줍니다.
    float2 vNoisePannedUV = In.vTexcoord + float2(g_fScrollSpeedU, g_fScrollSpeedV * 0.5f) * g_fTime;
    
    // [수정] 2. 계산된 UV로 텍스처 샘플링
    // 기본이 Noise
    vector vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vNoisePannedUV); // 수정: vNoisePannedUV 사용
    vector vMtrlDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord); // 디졸브는 고정
    
    vector vDarkColor = vector(0.5f, 0.f, 0.f, 1.f); // 어두운 피 색상
    vector vMtrlDiffuse = g_DiffuseTextures[5].Sample(DefaultSampler, vDiffusePannedUV); // 수정: vDiffusePannedUV 사용
    
    
    if (vMtrlNoise.a < 0.3f)
        discard;
    
   
    // 노이즈 값(vMtrlNoise.r)에 따라 두 색상을 보간합니다.
    //Out.vDiffuse.rgb = lerp(vDarkColor.rgb, vMtrlDiffuse.rgb, vMtrlNoise.r);
    Out.vDiffuse.rgb = vMtrlDiffuse.rgb;
    
    float fAlpha = saturate(1.f - g_fRatio);
    Out.vDiffuse.a = vMtrlDiffuse.a * fAlpha; // 시간에 따라 알파값 감소.
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    // 알파 값을 시간에 따라 감소?
    
    // 안에 숫자가 0이되면 안그린다.
    clip(vMtrlDissolve.r - g_fDissolveTime);
    
    return Out;
}




PS_OUT_BACKBUFFER PS_DEFFERED_BLOODPILLARB_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    // [추가] 1. 시간에 따라 흐르는 UV 좌표 계산
    // Diffuse 텍스처용 UV. V(세로) 방향으로만 흐르도록 설정.
    float2 vDiffusePannedUV = In.vTexcoord + float2(0.f, g_fScrollSpeedV) * g_fTime;
    
    // Noise 텍스처용 UV. Diffuse와는 다른 속도로 흐르게 하여 깊이감을 줍니다.
    float2 vNoisePannedUV = In.vTexcoord + float2(g_fScrollSpeedU, g_fScrollSpeedV * 0.5f) * g_fTime;
    
    // [수정] 2. 계산된 UV로 텍스처 샘플링
    // 기본이 Noise
    vector vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vNoisePannedUV); // 수정: vNoisePannedUV 사용
    vector vMtrlDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord); // 디졸브는 고정
    
    vector vDarkColor = vector(0.5f, 0.f, 0.f, 1.f); // 어두운 피 색상
    vector vMtrlDiffuse = g_DiffuseTextures[5].Sample(DefaultSampler, vDiffusePannedUV); // 수정: vDiffusePannedUV 사용
    
    
    if (vMtrlNoise.a < 0.3f)
        discard;
    
    
    // 노이즈 값(vMtrlNoise.r)에 따라 두 색상을 보간합니다.
    //Out.vDiffuse.rgb = lerp(vDarkColor.rgb, vMtrlDiffuse.rgb, vMtrlNoise.r);
    Out.vDiffuse.rgb = vMtrlDiffuse.rgb;
    
    float fAlpha = saturate(1.f - g_fRatio);
    Out.vDiffuse.a = vMtrlDiffuse.a * fAlpha; // 시간에 따라 알파값 감소.
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    // 안에 숫자가 0이되면 안그린다.
    clip(vMtrlDissolve.r - g_fDissolveTime);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_DEFFERED_BLOODPILLARC_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    // [추가] 1. 시간에 따라 흐르는 UV 좌표 계산
    // Diffuse 텍스처용 UV. V(세로) 방향으로만 흐르도록 설정.
    float2 vDiffusePannedUV = In.vTexcoord + float2(0.f, g_fScrollSpeedV) * g_fTime;
    
    // Noise 텍스처용 UV. Diffuse와는 다른 속도로 흐르게 하여 깊이감을 줍니다.
    float2 vNoisePannedUV = In.vTexcoord + float2(g_fScrollSpeedU, g_fScrollSpeedV * 0.5f) * g_fTime;
    
    // [수정] 2. 계산된 UV로 텍스처 샘플링
    // 기본이 Noise
    vector vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vNoisePannedUV); // 수정: vNoisePannedUV 사용
    vector vMtrlDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord); // 디졸브는 고정
    
    vector vDarkColor = vector(0.5f, 0.f, 0.f, 1.f); // 어두운 피 색상
    vector vMtrlDiffuse = g_DiffuseTextures[5].Sample(DefaultSampler, vDiffusePannedUV); // 수정: vDiffusePannedUV 사용
    
    
    if (vMtrlNoise.a < 0.3f)
        discard;
    
    
    // 노이즈 값(vMtrlNoise.r)에 따라 두 색상을 보간합니다.
    //Out.vDiffuse.rgb = lerp(vDarkColor.rgb, vMtrlDiffuse.rgb, vMtrlNoise.r);
    Out.vDiffuse.rgb = vMtrlDiffuse.rgb;
    
    float fAlpha = saturate(1.f - g_fRatio);
    Out.vDiffuse.a = vMtrlDiffuse.a * fAlpha; // 시간에 따라 알파값 감소.
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    // 안에 숫자가 0이되면 안그린다.
    clip(vMtrlDissolve.r - g_fDissolveTime);
    
    return Out;
}

technique11 DefaultTechnique
{
    /* 특정 패스를 이용해서 점정을 그려냈다. */
    /* 하나의 모델을 그려냈다. */ 
    /* 모델의 상황에 따라 다른 쉐이딩 기법 세트(명암 + 림라이트 + 스펙큘러 + 노멀맵 + ssao )를 먹여주기위해서 */
    pass DefaultPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //PixelShader = compile ps_5_0 PS_MAIN();
        PixelShader = compile ps_5_0 PS_DEFFERED_OUT();
    }

    pass NormalPass // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //PixelShader = compile ps_5_0 PS_MAIN();
        PixelShader = compile ps_5_0 PS_DEFFERED_NORMALOUT();
    }
    
    pass DessolvePass // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //PixelShader = compile ps_5_0 PS_DISSOLVE_MAIN();
        PixelShader = compile ps_5_0 PS_DEFFERED_DISSOLVE_MAIN();

    }

    pass BloodPillarAPass // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DEFFERED_BLOODPILLARA_MAIN();

    }

    pass BloodPillarBPass // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //PixelShader = compile ps_5_0 PS_DISSOLVE_MAIN();
        PixelShader = compile ps_5_0 PS_DEFFERED_BLOODPILLARB_MAIN();

    }

    pass BloodPillarCPass // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DEFFERED_BLOODPILLARC_MAIN();

    }


}
