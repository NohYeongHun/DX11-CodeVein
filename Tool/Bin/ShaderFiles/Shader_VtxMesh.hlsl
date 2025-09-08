#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

/*    */
texture2D g_DiffuseTexture;
texture2D g_NoiseTexture;
vector g_vMtrlAmbient = 1.f;
vector g_vMtrlSpecular = 1.f;




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
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

/*        ̴  :        ġ        ̽    ȯ(     ->      ->    ->     ). */ 
/*          :                   .(in:3  , out:2   or 5  ) */
/*          :          (      ϳ    VS_MAIN ѹ ȣ  ) */ 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    /*              ġ *      *    *      */ 
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    return Out;
}


VS_OUT VS_MAIN2(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    

        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
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
    
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);
    
    /*     ̵   ̾߱  ߴ */
    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_vCamPosition;
    
    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.0f);
    
    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
                    (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
    
    return Out;
}


PS_OUT PS_MAIN2(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
      
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

      //    İ  0     쿡    ֺ   ȼ          
    if (vMtrlDiffuse.a < 0.01f)
    {
        float2 texelSize = float2(1.0f / 1024.0f, 1.0f / 1024.0f); //  ؽ ó  ػ󵵿   °      

          // 4      Ǵ  8   ⿡      İ   ִ  ù   °  ȼ  ã  
        vector neighbors[4] =
        {
            g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord + float2(0, -texelSize.y)), //   
              g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord + float2(texelSize.x, 0)), //       
              g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord + float2(0, texelSize.y)), //  Ʒ 
              g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord + float2(-texelSize.x, 0)) //     
        };

          //   ȿ    ֺ   ȼ     ù   °       
        for (int i = 0; i < 4; i++)
        {
            if (neighbors[i].a > 0.01f)
            {
                vMtrlDiffuse = float4(neighbors[i].rgb, 1.0f);
                break;
            }
        }
    }

    float fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_vCamPosition;

    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.0f);

    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient)) +
                      (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    Out.vColor.a = 1.0f;

    return Out;
}


// ... 기존 변수들 아래에 추가
float g_fTime = 0.f;
float g_fSpiralStrength = 5.0f; // 나선 강도
float g_fRotationSpeed = 2.0f; // 회전 속도
vector g_vTintColor = vector(1.f, 1.f, 1.f, 1.f); // 기본값은 흰색

// ... 기존 변수들 아래에 추가 ...
float g_fEffectLifetime = 4.f; // 이펙트의 총 수명 (초)
float g_fEffectStartTime = 0.f; // 이펙트가 생성된 시간
float g_fWipeInTime = 1.5f; // 나타나는 데 걸리는 시간 (초)
float g_fWipeOutTime = 1.f; // 사라지는 데 걸리는 시간 (초)
float g_fWipeSoftness = 0.2f; // 와이프 경계선의 부드러운 정도

//PS_OUT PS_EFFECT_WIND(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;
    
//     // --- 절차적 UV 계산 시작 ---
//    // 1. UV 좌표를 (-0.5 ~ 0.5) 범위로 중앙 정렬합니다.
//    float2 vCenterUV = In.vTexcoord - 0.5f;

//    // 2. 중심으로부터의 거리와 각도를 계산합니다 (극좌표계 변환).
//    float fDistance = length(vCenterUV);
//    float fAngle = atan2(vCenterUV.y, vCenterUV.x);

//    // 3. 거리에 비례해서 각도를 왜곡하고, 시간에 따라 회전시킵니다.
//    fAngle += fDistance * g_fSpiralStrength;
//    fAngle += g_fTime * g_fRotationSpeed;

//    // 4. 새로운 각도와 기존 거리를 이용해 새로운 UV 좌표를 만듭니다.
//    float2 vSpiralUV;
//    vSpiralUV.x = cos(fAngle) * fDistance;
//    vSpiralUV.y = sin(fAngle) * fDistance;

//    // 5. UV 좌표를 다시 (0 ~ 1) 범위로 복원합니다.
//    vSpiralUV += 0.5f;
//    // --- 절차적 UV 계산 끝 ---


//    // 왜곡된 vSpiralUV를 사용해 텍스처를 샘플링합니다.
//    float4 vTextureColor = g_DiffuseTexture.Sample(DefaultSampler, vSpiralUV);

//    Out.vColor = vTextureColor * g_vTintColor;
//    Out.vColor.a = vTextureColor.r * g_vTintColor.a;

//    float fBaseAlpha = Out.vColor.a;

//    // 1. 이펙트 생성 후 경과 시간을 계산합니다.
//    float fElapsedTime = g_fTime - g_fEffectStartTime;

//    // 2. Wipe-In 진행률 (0에서 1로 증가)
//    float fWipeInRatio = saturate(fElapsedTime / g_fWipeInTime);

//    // 3. Wipe-Out 시작 시간을 계산하고, 진행률 (0에서 1로 증가)을 구합니다.
//    float fWipeOutStartTime = g_fEffectLifetime - g_fWipeOutTime;
//    float fWipeOutRatio = saturate((fElapsedTime - fWipeOutStartTime) / g_fWipeOutTime);

//    // 4. 현재 픽셀의 가로 UV 좌표를 이용해 와이프 마스크를 만듭니다.
//    //    오른쪽에서 왼쪽으로 나타나는 마스크 (Wipe-In)
//    //    In.vTexcoord.x가 1.0 (오른쪽)에 가까울수록 먼저 나타나기 시작합니다.
//    float fWipeInMask = smoothstep(1.0 - fWipeInRatio - g_fWipeSoftness, 1.0 - fWipeInRatio, In.vTexcoord.x);
    
//    //    오른쪽에서 왼쪽으로 사라지는 마스크 (Wipe-Out)
//    //    여기서 In.vTexcoord.x 대신 (1.0f - In.vTexcoord.x)를 사용하여 사라지는 방향을 반전시킵니다.
//    //    (1.0f - In.vTexcoord.x)는 UV의 오른쪽(1)에서 0으로, 왼쪽(0)에서 1로 증가하는 값을 가집니다.
//    //    smoothstep의 첫 번째 인자는 시작점, 두 번째 인자는 끝점입니다.
//    //    이 값들은 WipeOutRatio가 진행될수록 이동하며 영역을 만듭니다.
//    float fWipeOutMask = 1.0 - smoothstep(0.0 - g_fWipeSoftness, 0.0, (1.0f - In.vTexcoord.x) - fWipeOutRatio);

//    // 5. 최종 알파 값은 [기존 알파] x [Wipe-In 마스크] x [Wipe-Out 마스크] 입니다.
//    Out.vColor.a = fBaseAlpha * fWipeInMask * fWipeOutMask;
    
//    return Out;
//}



PS_OUT PS_EFFECT_WIND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
     // --- 기존의 절차적 UV 계산 및 텍스처 샘플링 (변동 없음) ---
    float2 vCenterUV = In.vTexcoord - 0.5f;
    float fDistance = length(vCenterUV); // 중심으로부터의 거리
    float fAngle = atan2(vCenterUV.y, vCenterUV.x);
    fAngle += fDistance * g_fSpiralStrength;
    fAngle += g_fTime * g_fRotationSpeed;
    float2 vSpiralUV;
    vSpiralUV.x = cos(fAngle) * fDistance;
    vSpiralUV.y = sin(fAngle) * fDistance;
    vSpiralUV += 0.5f;

    float4 vTextureColor = g_DiffuseTexture.Sample(DefaultSampler, vSpiralUV);
    // 연한 회색 기본색을 정의합니다. (RGB)
    float3 vBaseGreyColor = float3(0.7f, 0.7f, 0.7f); // 연한 회색
    
    Out.vColor.rgb = vBaseGreyColor * vTextureColor.rgb;
    //Out.vColor = vTextureColor * g_vTintColor;
    float fBaseAlpha = vTextureColor.r * g_vTintColor.a;


    
    float fElapsedTime = g_fTime - g_fEffectStartTime;

    float2 vNoiseUV = In.vTexcoord;
    vNoiseUV += fDistance * 0.5f;
    vNoiseUV += g_fTime * 0.1f;
    
    float fNoiseValue = g_NoiseTexture.Sample(DefaultSampler, vNoiseUV).r; // 노이즈 값 (0~1)

    float fAppearProgress = saturate(fElapsedTime / g_fWipeInTime);
    float fAppearMask = smoothstep(fAppearProgress - g_fWipeSoftness, fAppearProgress, fNoiseValue);

    float fDisappearProgress = saturate((fElapsedTime - (g_fEffectLifetime - g_fWipeOutTime)) / g_fWipeOutTime);
    float fDisappearMask = 1.0 - smoothstep(fDisappearProgress - g_fWipeSoftness, fDisappearProgress, fNoiseValue);
    
    Out.vColor.a = fBaseAlpha * fAppearMask * fDisappearMask;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();

        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass SkyPass // 하늘 전용 패스
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN2();
    }

    pass EffectWindPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();

        PixelShader = compile ps_5_0 PS_EFFECT_WIND();
    }

}
