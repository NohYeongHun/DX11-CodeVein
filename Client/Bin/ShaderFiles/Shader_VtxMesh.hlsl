#include "Engine_Shader_Defines.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;


vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

/*재질*/
texture2D g_DiffuseTexture;
texture2D g_DiffuseTextures[7];
texture2D g_OtherTextures[13];
texture2D g_NoiseTextures[8];

texture2D g_SwirlTextures[6];

texture2D g_NormalTexture;
texture2D g_NoiseTexture;

/*재질*/
texture2D g_DissolveTexture;

vector g_vMtrlAmbient = 1.f;
vector g_vMtrlSpecular = 1.f;


// 시간.
float g_fDissolveTime;
float g_fGrowTime;
float g_fMoveTime;


float g_fTime;
float g_fRatio;
float g_fScrollSpeed = 2.f;

float g_fDissolveThresold; // Dissolve 값.


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

float g_fSlashDirection = 1.f; // 베기 방향
float g_fSlashIntensity = 1.f; // 베기 강도
float g_fWaveAmplitude = 0.3f; // 파동 진폭

VS_OUT VS_SWORDWIND_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    // 정점 위치 변형 - 검풍의 날카로운 형태 만들기
    float3 localPos = In.vPosition;
    
    // UV 기반으로 정점 변형 (가로로 늘어나는 효과)
    float stretchFactor = 1.0f + (abs(In.vTexcoord.x - 0.5f) * 2.0f) * g_fSlashIntensity;
    localPos.x *= stretchFactor * 2.5f; // X축으로 늘리기
    
    // 파동 효과 추가 (시간에 따른 웨이브)
    float wave = sin(In.vTexcoord.x * 6.28f + g_fTime * 10.0f) * g_fWaveAmplitude;
    localPos.y += wave * (1.0f - abs(In.vTexcoord.x - 0.5f) * 2.0f);
    
    // 끝부분을 날카롭게 (테이퍼링)
    float taperFactor = 1.0f - pow(abs(In.vTexcoord.x - 0.5f) * 2.0f, 2.0f);
    localPos.y *= taperFactor;
    localPos.z *= taperFactor;
    
    // 나머지 변환 적용
    float4x4 matWV = mul(g_WorldMatrix, g_ViewMatrix);
    float4x4 matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(localPos, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(mul(float4(In.vBinormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}


struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;
      /* 정점의 로컬위치 * 월드 * 뷰 * 투영 */ 
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vProjPos = Out.vPosition;
    
    return Out;

}


/* 만든 픽셀 각각에 대해서 픽셀 쉐이더를 수행한다. */
/* 픽셀의 색을 결정한다. */
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

struct PS_OUT_BACKBUFFER
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

PS_OUT_BACKBUFFER MakeGBufferOut(PS_BACKBUFFER_IN In, float4 color)
{
    PS_OUT_BACKBUFFER Out;
    Out.vDiffuse = color;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    return Out;
}

float3 GetBloodColor(float2 uv, int texIdx)
{
    float4 tex = g_DiffuseTextures[texIdx].Sample(DefaultSampler, uv);

    float3 color = tex.rgb;
    color.r *= 1.2f;
    color.g *= 0.15f;
    color.b *= 0.15f;

    return saturate(color);
}

float GetNoiseMask(float2 uv, float thresholdMin, float thresholdMax)
{
    float n = g_OtherTextures[0].Sample(DefaultSampler, uv).r;
    return smoothstep(thresholdMin, thresholdMax, n);
}


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

// 강화된 피 색상 파라미터
float g_fBloodDarkness = 0.3f; // 피의 어두운 정도 (0-1, 낮을수록 진함)
float g_fBloodSaturation = 1.8f; // 채도 강도
float g_fBloodContrast = 1.5f; // 대비 강도
float g_fViscosity = 0.85f; // 점성도 (끈적임 효과)

// 기존 파라미터 조정값
float g_fErosionThreshold = 0.4f;
float g_fDistortionStrength = 0.03f;
float g_fFresnelPower = 5.0f;
vector g_vFresnelColor = float4(0.6f, 0.0f, 0.0f, 1.f); // 더 진한 붉은색
float g_fCrackIntensity = 2.5f;
float g_fCrackSpeed = -4.0f;

float g_fVortexStrength = 2.0f;
float g_fVortexSpeed = 1.5f;
float g_fHeightGradient = 1.0f;
float g_fTurbulence = 0.5f;
float g_fVerticalFlow = 2.0f;

PS_OUT_BACKBUFFER PS_DEFFERED_BLOODPILLARA_MAIN(PS_BACKBUFFER_IN In)
{
    float2 uv = In.vTexcoord;
    uv.y -= g_fTime * 1.2f;

    float3 color = GetBloodColor(uv, 4);

    float noiseMask = GetNoiseMask(uv * 2.0f, 0.25f, 0.75f);
    float heightFade = 1.0f - In.vTexcoord.y;
    float lifeFade = 1.0f - g_fRatio;

    float alpha = noiseMask * heightFade * lifeFade;
    clip(alpha - 0.05f);

    return MakeGBufferOut(In, float4(color, alpha));
}


PS_OUT_BACKBUFFER PS_DEFFERED_BLOODPILLARB_MAIN(PS_BACKBUFFER_IN In)
{
    float2 uv = In.vTexcoord;
    uv.x += g_fTime * 0.3f;

    float3 color = GetBloodColor(uv, 4);

    float2 center = In.vTexcoord - float2(0.5f, 0.5f);
    float radial = 1.0f - saturate(length(center) * 2.0f);

    float noiseMask = GetNoiseMask(uv * 1.5f, 0.2f, 0.8f);
    float lifeFade = 1.0f - g_fRatio;

    float alpha = radial * noiseMask * lifeFade;
    clip(alpha - 0.05f);

    return MakeGBufferOut(In, float4(color, alpha));
}


PS_OUT_BACKBUFFER PS_DEFFERED_BLOODPILLARC_MAIN(PS_BACKBUFFER_IN In)
{
    float2 uv = In.vTexcoord;
    uv.y -= g_fTime * 1.8f;
    uv.x += sin(In.vTexcoord.y * 8.0f + g_fTime * 3.0f) * 0.03f;

    float3 color = GetBloodColor(uv, 5);

    float noiseMask = GetNoiseMask(uv * 2.5f, 0.35f, 0.8f);
    float edgeFade = 1.0f - abs(In.vTexcoord.x - 0.5f) * 2.0f;
    float lifeFade = 1.0f - g_fRatio;

    float alpha = noiseMask * edgeFade * lifeFade;
    clip(alpha - 0.05f);

    return MakeGBufferOut(In, float4(color, alpha));
}

PS_OUT_BACKBUFFER PS_SWORDWIND_MAIN(PS_BACKBUFFER_IN In)
{
    float2 uv = In.vTexcoord;
    
    // 1. 띠를 따라 흐르는 스크롤
    uv.y -= g_fTime * 1.5f;
    
    // 2. 기본 색상
    float3 color = g_DiffuseTextures[6].Sample(DefaultSampler, In.vTexcoord).rgb;
    
    // 3. 약한 노이즈 마스크
    float noise = g_NoiseTextures[4].Sample(DefaultSampler, uv).r;
    float flowMask = smoothstep(0.25f, 0.8f, noise);
    
    // 4. 띠 중앙은 진하고 양끝은 약하게
    float edgeFade = 1.0f - abs(In.vTexcoord.x - 0.5f) * 2.0f;
    
    // 5. 수명 페이드
    float lifeFade = 1.0f;
    if (g_fRatio < 0.2f)
        lifeFade = smoothstep(0.0f, 0.2f, g_fRatio);
    else if (g_fRatio > 0.7f)
        lifeFade = 1.0f - smoothstep(0.7f, 1.0f, g_fRatio);
    
    float alpha = flowMask * edgeFade * lifeFade;
    clip(alpha - 0.05f);
    
    return MakeGBufferOut(In, float4(color, alpha));
}

struct PS_IN_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

struct PS_OUT_SHADOW
{
    float4 vLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out;
    
    Out.vLightDepth = float4(In.vProjPos.w / 1000.0f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_BLOOD_CIRCLE_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

    float2 uv = In.vTexcoord;
    uv.x += g_fTime * 0.8f;
    uv = frac(uv);

    float4 tex = g_DiffuseTexture.Sample(DefaultSampler, uv);

    float3 color = tex.rgb;
    color.r *= 0.8f;
    color.g *= 0.15f;
    color.b *= 0.10f;

    float alpha = tex.a;
    alpha *= (1.0f - g_fDissolveTime);

    Out.vDiffuse = float4(saturate(color), saturate(alpha));
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);

    return Out;
}

PS_OUT_BACKBUFFER PS_BLOOD_BODYAURA_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

    float2 uv = frac(In.vTexcoord + float2(g_fTime * 0.4f, -g_fTime * 1.0f));
    float noise = g_NoiseTextures[5].Sample(DefaultSampler, uv).r;

    float noiseMask = smoothstep(0.3f, 0.75f, noise);
    float widthMask = 1.0f - abs(In.vTexcoord.x - 0.5f) * 2.0f;
    float heightMask = smoothstep(0.0f, 0.15f, In.vTexcoord.y) * (1.0f - smoothstep(0.75f, 1.0f, In.vTexcoord.y));

    float alpha = noiseMask * widthMask * heightMask * (1.0f - g_fDissolveTime);
    clip(alpha - 0.05f);

    float3 color = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vDiffuse = float4(color, alpha);
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);

    return Out;
}


float g_fEffectDuration;



float GenerateGradientMask(float2 uv)
{
    // Y축 기준 그라데이션
    float gradient = 1.0f - abs(uv.y - 0.5f) * 2.0f;
    return pow(gradient, 2.0f); // Power로 Falloff 조절
}

// 원형 검기에 더 적합한 방사형 그라데이션
float GenerateRadialGradientMask(float2 uv, float innerRadius, float outerRadius)
{
    float2 center = float2(0.5f, 0.5f);
    float dist = distance(uv, center);
    
    // 내부 반경과 외부 반경 사이에서 그라데이션
    float mask = 1.0f - saturate((dist - innerRadius) / (outerRadius - innerRadius));
    return pow(mask, 1.5f); // 더 날카로운 엣지
}

// 회전하는 슬래시 형태의 그라데이션
float GenerateRotatingSlashMask(float2 uv, float time, float bladeCount)
{
    float2 center = float2(0.5f, 0.5f);
    float2 dir = uv - center;
    float angle = atan2(dir.y, dir.x) + time; // 시간에 따라 회전
    
    // 여러 개의 날(blade) 생성
    float blade = sin(angle * bladeCount) * 0.5f + 0.5f;
    
    // 중심에서의 거리에 따른 감쇠
    float dist = length(dir);
    float radialFade = 1.0f - saturate(dist * 2.0f);
    
    return blade * radialFade;
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

    pass SwordWind // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //PixelShader = compile ps_5_0 PS_SWORDWIND_IMPROVED();
        PixelShader = compile ps_5_0 PS_SWORDWIND_MAIN();
    }


    pass ShadowPass // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        //SetDepthStencilState(DSS_SwordWind, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        //PixelShader = compile ps_5_0 PS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

    pass BloodFloorAuraPass // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //PixelShader = compile ps_5_0 PS_SWORDWIND_IMPROVED();
        PixelShader = compile ps_5_0 PS_BLOOD_CIRCLE_MAIN();
    }


    pass BloodBodyAuraPass // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //PixelShader = compile ps_5_0 PS_SWORDWIND_IMPROVED();
        PixelShader = compile ps_5_0 PS_BLOOD_BODYAURA_MAIN();
    }

}

