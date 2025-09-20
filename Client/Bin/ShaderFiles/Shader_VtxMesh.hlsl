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
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

    // -- 1. 높이 기반 소용돌이 강도 계산 --
    float fHeightRatio = saturate(1.0f - In.vTexcoord.y);
    float fVortexIntensity = g_fVortexStrength * fHeightRatio * g_fHeightGradient;
    
    // -- 2. 극좌표계 변환을 통한 소용돌이 효과 --
    float2 vCenterUV = float2(0.5f, 0.5f);
    float2 vToCenter = In.vTexcoord - vCenterUV;
    
    float fDistance = length(vToCenter);
    float fAngle = atan2(vToCenter.y, vToCenter.x);
    
    float fVortexRotation = fVortexIntensity / max(fDistance, 0.1f) * g_fTime * g_fVortexSpeed;
    fAngle += fVortexRotation;
    
    float2 vVortexUV = vCenterUV + fDistance * float2(cos(fAngle), sin(fAngle));
    
    // -- 3. 다층 노이즈 애니메이션 (점성 효과 추가) --
    float2 vNoiseUV1 = vVortexUV + float2(0.1f, g_fVerticalFlow * g_fViscosity) * g_fTime * 0.3f;
    float2 vNoiseUV2 = vVortexUV + float2(-0.15f, g_fVerticalFlow * 0.8f * g_fViscosity) * g_fTime * 0.5f;
    
    float2 vDetailUV1 = In.vTexcoord * 2.0f + float2(0.2f, g_fVerticalFlow * 1.5f) * g_fTime * 0.7f;
    float2 vDetailUV2 = In.vTexcoord * 1.5f + float2(-0.1f, g_fVerticalFlow * 1.2f) * g_fTime * 0.6f;
    
    // -- 4. 복합 왜곡 효과 --
    float2 vVortexDistortion = (g_OtherTextures[0].Sample(DefaultSampler, vNoiseUV1).rg * 2.f - 1.f) * g_fDistortionStrength;
    float2 vTurbulenceDistortion = (g_OtherTextures[0].Sample(DefaultSampler, vDetailUV1).rg * 2.f - 1.f) * g_fTurbulence * 0.02f;
    
    vVortexDistortion *= fHeightRatio * 1.5f;
    vTurbulenceDistortion *= (1.0f - fHeightRatio * 0.5f);
    
    float2 vFinalDistortedUV = In.vTexcoord + vVortexDistortion + vTurbulenceDistortion;
    
    // -- 5. 동적 침식 마스크 --
    float fErosionMask1 = g_OtherTextures[0].Sample(DefaultSampler, vNoiseUV2).r;
    float fErosionMask2 = g_OtherTextures[0].Sample(DefaultSampler, vDetailUV2).r;
    
    float fCombinedErosion = lerp(fErosionMask1, fErosionMask2, fHeightRatio);
    float fDynamicThreshold = g_fErosionThreshold + sin(g_fTime * 2.0f + fHeightRatio * 3.14159f) * 0.1f;
    
    clip(fCombinedErosion - fDynamicThreshold);
    
    // -- 6. 강화된 피 텍스처 샘플링 --
    // 메인 혈액 텍스처 (빨간색 텍스처 사용)
    float2 vBloodFlowUV = vFinalDistortedUV + float2(sin(fAngle) * 0.1f, g_fVerticalFlow * 0.4f) * g_fTime;
    vector vBloodColor = g_DiffuseTextures[4].Sample(DefaultSampler, vBloodFlowUV);
    
    // 디테일 레이어 (추가 깊이감)
    float2 vDetailFlowUV = vFinalDistortedUV * 0.5f + float2(cos(fAngle) * 0.05f, g_fVerticalFlow * 0.6f) * g_fTime;
    vector vDetailColor = g_DiffuseTextures[5].Sample(DefaultSampler, vDetailFlowUV);
    
    // -- 7. 피 색상 강화 처리 --
    // 베이스 컬러를 더 진하게 조정
    vector vFinalColor = vBloodColor;
    
    // 붉은색 채널 강화, 다른 채널 억제
    vFinalColor.r = pow(vFinalColor.r, 0.7f) * 1.2f; // 빨간색 강화
    vFinalColor.g *= 0.1f; // 녹색 크게 감소
    vFinalColor.b *= 0.15f; // 파란색 크게 감소
    
    // 디테일 색상 블렌딩 (Multiply 블렌딩으로 어둡게)
    vFinalColor.rgb = vFinalColor.rgb * (vDetailColor.rgb * 0.8f + 0.2f);
    
    // 채도 증가
    float fLuminance = dot(vFinalColor.rgb, float3(0.299f, 0.587f, 0.114f));
    vFinalColor.rgb = lerp(float3(fLuminance, fLuminance, fLuminance), vFinalColor.rgb, g_fBloodSaturation);
    
    // 대비 증가
    vFinalColor.rgb = saturate((vFinalColor.rgb - 0.5f) * g_fBloodContrast + 0.5f);
    
    // 전체적으로 어둡게 (피의 진한 느낌)
    vFinalColor.rgb *= g_fBloodDarkness;
    
    // -- 8. 노이즈 기반 혈흔 패턴 추가 --
    float fBloodPattern = g_OtherTextures[1].Sample(DefaultSampler, vFinalDistortedUV * 3.0f).r;
    float fBloodSpots = g_OtherTextures[0].Sample(DefaultSampler, vFinalDistortedUV * 5.0f).g;
    
    // 혈흔 패턴을 이용한 추가 어두운 영역
    float fDarkSpots = saturate(1.0f - fBloodPattern * fBloodSpots);
    vFinalColor.rgb *= (0.5f + fDarkSpots * 0.5f);
    
    // -- 9. 상승 에너지와 소용돌이 효과 --
    float2 vRisingMaskUV = In.vTexcoord + float2(sin(g_fTime + fAngle) * 0.1f, g_fTime * 2.0f);
    float fRisingMask = g_OtherTextures[1].Sample(DefaultSampler, vRisingMaskUV).r;
    
    float fVortexCore = saturate(1.0f - fDistance * 2.0f);
    fRisingMask = saturate(fRisingMask + fVortexCore * 0.5f);
    
    // 코어 부분은 더 진한 빨간색으로
    vFinalColor.r += fVortexCore * 0.3f;
    vFinalColor.rgb = saturate(vFinalColor.rgb);
    
    // -- 10. 강화된 프레넬 효과 (가장자리 빛) --
    vector vViewDir = normalize(g_vCamPosition - In.vWorldPos);
    float fFresnel = 1.f - saturate(dot(vViewDir.xyz, In.vNormal.xyz));
    fFresnel = pow(fFresnel, g_fFresnelPower);
    
    // 프레넬 색상도 더 진한 빨간색으로
    vector vDynamicFresnelColor = g_vFresnelColor * (0.5f + fVortexIntensity * 0.3f);
    vFinalColor.rgb += vDynamicFresnelColor.rgb * fFresnel * 0.3f; // 프레넬 효과 감소
    
    // -- 11. 점성 효과 추가 --
    // 시간에 따른 끈적한 흐름 효과
    float fViscosityFlow = sin(g_fTime * 0.5f + In.vTexcoord.y * 10.0f) * 0.05f;
    vFinalColor.rgb *= (1.0f + fViscosityFlow * fHeightRatio);
    
    // -- 12. 최종 알파 계산 --
    float fSoftEdge = saturate((fCombinedErosion - fDynamicThreshold) * 15.f);
    float fFinalAlpha = vBloodColor.a * fSoftEdge;
    
    // 진한 부분은 더 불투명하게
    fFinalAlpha = saturate(fFinalAlpha * (1.0f + fDarkSpots * 0.5f));
    fFinalAlpha *= (0.8f + fVortexIntensity * 0.2f);
    fFinalAlpha *= (1.f - g_fRatio);
    
    // -- 13. 최종 색상 클램핑 --
    vFinalColor.rgb = saturate(vFinalColor.rgb);
    
    // 최종 결과 출력
    Out.vDiffuse = float4(vFinalColor.rgb, fFinalAlpha);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_DEFFERED_BLOODPILLARB_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

    // -- 1. 높이 기반 소용돌이 강도 계산 --
    float fHeightRatio = saturate(1.0f - In.vTexcoord.y);
    float fVortexIntensity = g_fVortexStrength * fHeightRatio * g_fHeightGradient;
    
    // -- 2. 극좌표계 변환을 통한 소용돌이 효과 --
    float2 vCenterUV = float2(0.5f, 0.5f);
    float2 vToCenter = In.vTexcoord - vCenterUV;
    
    float fDistance = length(vToCenter);
    float fAngle = atan2(vToCenter.y, vToCenter.x);
    
    float fVortexRotation = fVortexIntensity / max(fDistance, 0.1f) * g_fTime * g_fVortexSpeed;
    fAngle += fVortexRotation;
    
    float2 vVortexUV = vCenterUV + fDistance * float2(cos(fAngle), sin(fAngle));
    
    // -- 3. 다층 노이즈 애니메이션 (점성 효과 추가) --
    float2 vNoiseUV1 = vVortexUV + float2(0.1f, g_fVerticalFlow * g_fViscosity) * g_fTime * 0.3f;
    float2 vNoiseUV2 = vVortexUV + float2(-0.15f, g_fVerticalFlow * 0.8f * g_fViscosity) * g_fTime * 0.5f;
    
    float2 vDetailUV1 = In.vTexcoord * 2.0f + float2(0.2f, g_fVerticalFlow * 1.5f) * g_fTime * 0.7f;
    float2 vDetailUV2 = In.vTexcoord * 1.5f + float2(-0.1f, g_fVerticalFlow * 1.2f) * g_fTime * 0.6f;
    
    // -- 4. 복합 왜곡 효과 --
    float2 vVortexDistortion = (g_OtherTextures[0].Sample(DefaultSampler, vNoiseUV1).rg * 2.f - 1.f) * g_fDistortionStrength;
    float2 vTurbulenceDistortion = (g_OtherTextures[0].Sample(DefaultSampler, vDetailUV1).rg * 2.f - 1.f) * g_fTurbulence * 0.02f;
    
    vVortexDistortion *= fHeightRatio * 1.5f;
    vTurbulenceDistortion *= (1.0f - fHeightRatio * 0.5f);
    
    float2 vFinalDistortedUV = In.vTexcoord + vVortexDistortion + vTurbulenceDistortion;
    
    // -- 5. 동적 침식 마스크 --
    float fErosionMask1 = g_OtherTextures[0].Sample(DefaultSampler, vNoiseUV2).r;
    float fErosionMask2 = g_OtherTextures[0].Sample(DefaultSampler, vDetailUV2).r;
    
    float fCombinedErosion = lerp(fErosionMask1, fErosionMask2, fHeightRatio);
    float fDynamicThreshold = g_fErosionThreshold + sin(g_fTime * 2.0f + fHeightRatio * 3.14159f) * 0.1f;
    
    clip(fCombinedErosion - fDynamicThreshold);
    
    // -- 6. 강화된 피 텍스처 샘플링 --
    // 메인 혈액 텍스처 (빨간색 텍스처 사용)
    float2 vBloodFlowUV = vFinalDistortedUV + float2(sin(fAngle) * 0.1f, g_fVerticalFlow * 0.4f) * g_fTime;
    vector vBloodColor = g_DiffuseTextures[4].Sample(DefaultSampler, vBloodFlowUV);
    
    // 디테일 레이어 (추가 깊이감)
    float2 vDetailFlowUV = vFinalDistortedUV * 0.5f + float2(cos(fAngle) * 0.05f, g_fVerticalFlow * 0.6f) * g_fTime;
    vector vDetailColor = g_DiffuseTextures[5].Sample(DefaultSampler, vDetailFlowUV);
    
    // -- 7. 피 색상 강화 처리 --
    // 베이스 컬러를 더 진하게 조정
    vector vFinalColor = vBloodColor;
    
    // 붉은색 채널 강화, 다른 채널 억제
    vFinalColor.r = pow(vFinalColor.r, 0.7f) * 1.2f; // 빨간색 강화
    vFinalColor.g *= 0.1f; // 녹색 크게 감소
    vFinalColor.b *= 0.15f; // 파란색 크게 감소
    
    // 디테일 색상 블렌딩 (Multiply 블렌딩으로 어둡게)
    vFinalColor.rgb = vFinalColor.rgb * (vDetailColor.rgb * 0.8f + 0.2f);
    
    // 채도 증가
    float fLuminance = dot(vFinalColor.rgb, float3(0.299f, 0.587f, 0.114f));
    vFinalColor.rgb = lerp(float3(fLuminance, fLuminance, fLuminance), vFinalColor.rgb, g_fBloodSaturation);
    
    // 대비 증가
    vFinalColor.rgb = saturate((vFinalColor.rgb - 0.5f) * g_fBloodContrast + 0.5f);
    
    // 전체적으로 어둡게 (피의 진한 느낌)
    vFinalColor.rgb *= g_fBloodDarkness;
    
    // -- 8. 노이즈 기반 혈흔 패턴 추가 --
    float fBloodPattern = g_OtherTextures[1].Sample(DefaultSampler, vFinalDistortedUV * 3.0f).r;
    float fBloodSpots = g_OtherTextures[0].Sample(DefaultSampler, vFinalDistortedUV * 5.0f).g;
    
    // 혈흔 패턴을 이용한 추가 어두운 영역
    float fDarkSpots = saturate(1.0f - fBloodPattern * fBloodSpots);
    vFinalColor.rgb *= (0.5f + fDarkSpots * 0.5f);
    
    // -- 9. 상승 에너지와 소용돌이 효과 --
    float2 vRisingMaskUV = In.vTexcoord + float2(sin(g_fTime + fAngle) * 0.1f, g_fTime * 2.0f);
    float fRisingMask = g_OtherTextures[1].Sample(DefaultSampler, vRisingMaskUV).r;
    
    float fVortexCore = saturate(1.0f - fDistance * 2.0f);
    fRisingMask = saturate(fRisingMask + fVortexCore * 0.5f);
    
    // 코어 부분은 더 진한 빨간색으로
    vFinalColor.r += fVortexCore * 0.3f;
    vFinalColor.rgb = saturate(vFinalColor.rgb);
    
    // -- 10. 강화된 프레넬 효과 (가장자리 빛) --
    vector vViewDir = normalize(g_vCamPosition - In.vWorldPos);
    float fFresnel = 1.f - saturate(dot(vViewDir.xyz, In.vNormal.xyz));
    fFresnel = pow(fFresnel, g_fFresnelPower);
    
    // 프레넬 색상도 더 진한 빨간색으로
    vector vDynamicFresnelColor = g_vFresnelColor * (0.5f + fVortexIntensity * 0.3f);
    vFinalColor.rgb += vDynamicFresnelColor.rgb * fFresnel * 0.3f; // 프레넬 효과 감소
    
    // -- 11. 점성 효과 추가 --
    // 시간에 따른 끈적한 흐름 효과
    float fViscosityFlow = sin(g_fTime * 0.5f + In.vTexcoord.y * 10.0f) * 0.05f;
    vFinalColor.rgb *= (1.0f + fViscosityFlow * fHeightRatio);
    
    // -- 12. 최종 알파 계산 --
    float fSoftEdge = saturate((fCombinedErosion - fDynamicThreshold) * 15.f);
    float fFinalAlpha = vBloodColor.a * fSoftEdge;
    
    // 진한 부분은 더 불투명하게
    fFinalAlpha = saturate(fFinalAlpha * (1.0f + fDarkSpots * 0.5f));
    fFinalAlpha *= (0.8f + fVortexIntensity * 0.2f);
    fFinalAlpha *= (1.f - g_fRatio);
    
    // -- 13. 최종 색상 클램핑 --
    vFinalColor.rgb = saturate(vFinalColor.rgb);
    
    // 최종 결과 출력
    Out.vDiffuse = float4(vFinalColor.rgb, fFinalAlpha);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_DEFFERED_BLOODPILLARC_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

    // -- 1. 높이 기반 소용돌이 강도 계산 --
    float fHeightRatio = saturate(1.0f - In.vTexcoord.y);
    float fVortexIntensity = g_fVortexStrength * fHeightRatio * g_fHeightGradient;
    
    // -- 2. 극좌표계 변환을 통한 소용돌이 효과 --
    float2 vCenterUV = float2(0.5f, 0.5f);
    float2 vToCenter = In.vTexcoord - vCenterUV;
    
    float fDistance = length(vToCenter);
    float fAngle = atan2(vToCenter.y, vToCenter.x);
    
    float fVortexRotation = fVortexIntensity / max(fDistance, 0.1f) * g_fTime * g_fVortexSpeed;
    fAngle += fVortexRotation;
    
    float2 vVortexUV = vCenterUV + fDistance * float2(cos(fAngle), sin(fAngle));
    
    // -- 3. 다층 노이즈 애니메이션 (점성 효과 추가) --
    float2 vNoiseUV1 = vVortexUV + float2(0.1f, g_fVerticalFlow * g_fViscosity) * g_fTime * 0.3f;
    float2 vNoiseUV2 = vVortexUV + float2(-0.15f, g_fVerticalFlow * 0.8f * g_fViscosity) * g_fTime * 0.5f;
    
    float2 vDetailUV1 = In.vTexcoord * 2.0f + float2(0.2f, g_fVerticalFlow * 1.5f) * g_fTime * 0.7f;
    float2 vDetailUV2 = In.vTexcoord * 1.5f + float2(-0.1f, g_fVerticalFlow * 1.2f) * g_fTime * 0.6f;
    
    // -- 4. 복합 왜곡 효과 --
    float2 vVortexDistortion = (g_OtherTextures[0].Sample(DefaultSampler, vNoiseUV1).rg * 2.f - 1.f) * g_fDistortionStrength;
    float2 vTurbulenceDistortion = (g_OtherTextures[0].Sample(DefaultSampler, vDetailUV1).rg * 2.f - 1.f) * g_fTurbulence * 0.02f;
    
    vVortexDistortion *= fHeightRatio * 1.5f;
    vTurbulenceDistortion *= (1.0f - fHeightRatio * 0.5f);
    
    float2 vFinalDistortedUV = In.vTexcoord + vVortexDistortion + vTurbulenceDistortion;
    
    // -- 5. 동적 침식 마스크 --
    float fErosionMask1 = g_OtherTextures[0].Sample(DefaultSampler, vNoiseUV2).r;
    float fErosionMask2 = g_OtherTextures[0].Sample(DefaultSampler, vDetailUV2).r;
    
    float fCombinedErosion = lerp(fErosionMask1, fErosionMask2, fHeightRatio);
    float fDynamicThreshold = g_fErosionThreshold + sin(g_fTime * 2.0f + fHeightRatio * 3.14159f) * 0.1f;
    
    clip(fCombinedErosion - fDynamicThreshold);
    
    // -- 6. 강화된 피 텍스처 샘플링 --
    // 메인 혈액 텍스처 (빨간색 텍스처 사용)
    float2 vBloodFlowUV = vFinalDistortedUV + float2(sin(fAngle) * 0.1f, g_fVerticalFlow * 0.4f) * g_fTime;
    vector vBloodColor = g_DiffuseTextures[4].Sample(DefaultSampler, vBloodFlowUV);
    
    // 디테일 레이어 (추가 깊이감)
    float2 vDetailFlowUV = vFinalDistortedUV * 0.5f + float2(cos(fAngle) * 0.05f, g_fVerticalFlow * 0.6f) * g_fTime;
    vector vDetailColor = g_DiffuseTextures[5].Sample(DefaultSampler, vDetailFlowUV);
    
    // -- 7. 피 색상 강화 처리 --
    // 베이스 컬러를 더 진하게 조정
    vector vFinalColor = vBloodColor;
    
    // 붉은색 채널 강화, 다른 채널 억제
    vFinalColor.r = pow(vFinalColor.r, 0.7f) * 1.2f; // 빨간색 강화
    vFinalColor.g *= 0.1f; // 녹색 크게 감소
    vFinalColor.b *= 0.15f; // 파란색 크게 감소
    
    // 디테일 색상 블렌딩 (Multiply 블렌딩으로 어둡게)
    vFinalColor.rgb = vFinalColor.rgb * (vDetailColor.rgb * 0.8f + 0.2f);
    
    // 채도 증가
    float fLuminance = dot(vFinalColor.rgb, float3(0.299f, 0.587f, 0.114f));
    vFinalColor.rgb = lerp(float3(fLuminance, fLuminance, fLuminance), vFinalColor.rgb, g_fBloodSaturation);
    
    // 대비 증가
    vFinalColor.rgb = saturate((vFinalColor.rgb - 0.5f) * g_fBloodContrast + 0.5f);
    
    // 전체적으로 어둡게 (피의 진한 느낌)
    vFinalColor.rgb *= g_fBloodDarkness;
    
    // -- 8. 노이즈 기반 혈흔 패턴 추가 --
    float fBloodPattern = g_OtherTextures[1].Sample(DefaultSampler, vFinalDistortedUV * 3.0f).r;
    float fBloodSpots = g_OtherTextures[0].Sample(DefaultSampler, vFinalDistortedUV * 5.0f).g;
    
    // 혈흔 패턴을 이용한 추가 어두운 영역
    float fDarkSpots = saturate(1.0f - fBloodPattern * fBloodSpots);
    vFinalColor.rgb *= (0.5f + fDarkSpots * 0.5f);
    
    // -- 9. 상승 에너지와 소용돌이 효과 --
    float2 vRisingMaskUV = In.vTexcoord + float2(sin(g_fTime + fAngle) * 0.1f, g_fTime * 2.0f);
    float fRisingMask = g_OtherTextures[1].Sample(DefaultSampler, vRisingMaskUV).r;
    
    float fVortexCore = saturate(1.0f - fDistance * 2.0f);
    fRisingMask = saturate(fRisingMask + fVortexCore * 0.5f);
    
    // 코어 부분은 더 진한 빨간색으로
    vFinalColor.r += fVortexCore * 0.3f;
    vFinalColor.rgb = saturate(vFinalColor.rgb);
    
    // -- 10. 강화된 프레넬 효과 (가장자리 빛) --
    vector vViewDir = normalize(g_vCamPosition - In.vWorldPos);
    float fFresnel = 1.f - saturate(dot(vViewDir.xyz, In.vNormal.xyz));
    fFresnel = pow(fFresnel, g_fFresnelPower);
    
    // 프레넬 색상도 더 진한 빨간색으로
    vector vDynamicFresnelColor = g_vFresnelColor * (0.5f + fVortexIntensity * 0.3f);
    vFinalColor.rgb += vDynamicFresnelColor.rgb * fFresnel * 0.3f; // 프레넬 효과 감소
    
    // -- 11. 점성 효과 추가 --
    // 시간에 따른 끈적한 흐름 효과
    float fViscosityFlow = sin(g_fTime * 0.5f + In.vTexcoord.y * 10.0f) * 0.05f;
    vFinalColor.rgb *= (1.0f + fViscosityFlow * fHeightRatio);
    
    // -- 12. 최종 알파 계산 --
    float fSoftEdge = saturate((fCombinedErosion - fDynamicThreshold) * 15.f);
    float fFinalAlpha = vBloodColor.a * fSoftEdge;
    
    // 진한 부분은 더 불투명하게
    fFinalAlpha = saturate(fFinalAlpha * (1.0f + fDarkSpots * 0.5f));
    fFinalAlpha *= (0.8f + fVortexIntensity * 0.2f);
    fFinalAlpha *= (1.f - g_fRatio);
    
    // -- 13. 최종 색상 클램핑 --
    vFinalColor.rgb = saturate(vFinalColor.rgb);
    
    // 최종 결과 출력
    Out.vDiffuse = float4(vFinalColor.rgb, fFinalAlpha);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}

//float BorderMask_Implementation(float2 InUV)
//{
//    // 1. 첫 번째 브랜치: TexCoord[0]의 G채널(Y값)을 가져옵니다.
//    float v_coord = InUV.y;

//    // 2. 두 번째 브랜치: Y값을 제곱하고 1에서 빼서 그라데이션을 만듭니다.
//    float gradient = 1.0f - pow(v_coord, 2);

//    // 3. Multiply: 두 결과를 곱합니다.
//    float final_result = v_coord * gradient;
    
//    return final_result;
//}



// T2 로직
float T2_Implementation(float2 InUV, const float g_fTime)
{
    float2 pannedUV = InUV + g_fScrollSpeed * g_fTime;
    float tex_val = g_SwirlTextures[4].Sample(DefaultSampler, pannedUV).r;
    return tex_val * 0.4f;
}

float T3_Implementation(float2 InUV, const float g_fTime)
{
    // [수정] 그래프에 맞게 파라미터와 0.5f 사용
    float2 speed = g_fScrollSpeed * 0.5f;
    float2 pannedUV = InUV + speed * g_fTime;
    
    float tex_val = g_SwirlTextures[5].Sample(DefaultSampler, pannedUV).r;
    
    float powered_result = pow(tex_val, 7);
    return saturate(powered_result);
}

float g_fTextureScale = 1.f;
float g_fGlobalAlpha = 0.3f;
float BorderMask_Implementation(float2 InUV)
{
    float v_coord = InUV.y;
    
    // 풍압 효과를 위해 더 날카로운 그라데이션 생성
    // 중앙에서 강하고 가장자리로 갈수록 빠르게 감소
    float sharpness = 3.0f; // 날카로움 조절
    float center_line = 1.0f - pow(abs(v_coord - 0.5f) * 2.0f, sharpness);
    
    // 진행 방향으로 늘어지는 효과
    float directional_fade = pow(v_coord, 0.7f);
    
    return center_line * directional_fade;
}


PS_OUT_BACKBUFFER PS_SWORDWIND_IMPROVED(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    // ===== 1. UV 좌표 기반 마스킹 =====
    // 검풍의 중심축과 가장자리 페이드 계산
    float2 uv = In.vTexcoord;
    
    // 중심선으로부터의 거리 계산 (수평 검풍 기준)
    float distFromCenter = abs(uv.y - 0.5f) * 2.0f;
    
    // 날카로운 가장자리 마스크 생성
    float edgeMask = 1.0f - pow(distFromCenter, 2.0f);
    edgeMask = saturate(edgeMask);
    
    // 진행 방향 페이드 (앞쪽이 더 강함)
    float directionalFade = pow(uv.x, 0.3f);
    
    // ===== 2. 동적 노이즈 패턴 생성 =====
    // 속도선용 스트릭 패턴 (Image 1 스타일 - 대각선 패턴)
    float2 streakUV = uv;
    streakUV.x *= 3.0f; // 가로로 늘려서 속도감 강화
    streakUV.x += g_fTime * 4.0f; // 빠른 스크롤
    float streakNoise = g_SwirlTextures[3].Sample(DefaultSampler, streakUV).r;
    
    // 왜곡용 물결 패턴 (Image 4 스타일)
    float2 waveUV = uv * 1.5f;
    waveUV.y += sin(uv.x * 5.0f + g_fTime * 3.0f) * 0.1f;
    float waveNoise = g_SwirlTextures[0].Sample(DefaultSampler, waveUV).r;
    
    // 부드러운 연기 패턴 (Image 10/11 스타일)
    float2 smokeUV = uv * 0.7f + float2(g_fTime * 0.5f, 0.0f);
    float smokeNoise = g_NoiseTextures[2].Sample(DefaultSampler, smokeUV).r;
    
    // 노이즈 합성 - 각 용도별로 가중치 조절
    float combinedNoise = streakNoise * 0.6f + waveNoise * 0.3f + smokeNoise * 0.1f;
    combinedNoise = pow(combinedNoise, 1.2f);
    
    // ===== 3. 왜곡 효과 (바람의 흐름) =====
    // 물결 패턴 기반 왜곡 (Image 4/5 스타일 활용)
    float2 distortion;
    distortion.x = waveNoise * 0.03f * sin(uv.y * 8.0f + g_fTime * 5.0f);
    distortion.y = waveNoise * 0.02f * cos(uv.x * 6.0f + g_fTime * 4.0f);
    
    float2 distortedUV = uv + distortion * edgeMask;
    
    // ===== 4. 속도선 효과 =====
    // 스트릭 패턴 활용 (Image 1/6 스타일)
    float speedLines = streakNoise;
    
    // 추가 디테일용 날카로운 선
    float sharpLines = sin((distortedUV.x * 40.0f) - (g_fTime * 15.0f));
    sharpLines = saturate(sharpLines * sharpLines); // 더 날카롭게
    
    // 속도선 합성
    speedLines = lerp(speedLines, sharpLines, 0.4f);
    speedLines = pow(speedLines, 2.0f);
    
    // ===== 5. 에너지 파동 =====
    float energyWave = sin(uv.x * 3.14159f + g_fTime * 4.0f) * 0.5f + 0.5f;
    energyWave *= sin(uv.y * 6.28318f + g_fTime * 3.0f) * 0.3f + 0.7f;
    
    // ===== 6. 최종 마스크 조합 =====
    float finalMask = edgeMask * directionalFade * combinedNoise;
    finalMask *= (speedLines * 0.7f + 0.3f);
    finalMask *= energyWave;
    
    // 임계값 처리 (날카로운 엣지)
    float threshold = 0.15f;
    if (finalMask < threshold)
        discard;
    
    // Soft edge 처리
    float softEdge = smoothstep(threshold, threshold + 0.1f, finalMask);
    
    // ===== 7. 색상 처리 =====
    // 검풍 색상 그라데이션
    float3 coreColor = float3(0.95f, 0.98f, 1.0f); // 중심 - 거의 흰색
    float3 midColor = float3(0.7f, 0.85f, 0.95f); // 중간 - 하늘색
    float3 edgeColor = float3(0.3f, 0.5f, 0.7f); // 가장자리 - 진한 청색
    
    // 강도에 따른 색상 보간
    float3 windColor;
    float intensity = pow(finalMask, 0.7f);
    
    if (intensity > 0.6f)
    {
        float t = (intensity - 0.6f) / 0.4f;
        windColor = lerp(midColor, coreColor, t);
    }
    else if (intensity > 0.3f)
    {
        float t = (intensity - 0.3f) / 0.3f;
        windColor = lerp(edgeColor, midColor, t);
    }
    else
    {
        windColor = edgeColor * (intensity / 0.3f);
    }
    
    // ===== 8. 프레넬 효과 (림 라이팅) =====
    float3 viewDir = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);
    float fresnel = 1.0f - saturate(dot(viewDir, In.vNormal.xyz));
    fresnel = pow(fresnel, 2.0f);
    
    // 프레넬 글로우 추가
    float3 fresnelGlow = float3(0.5f, 0.7f, 1.0f) * fresnel * 0.5f;
    windColor += fresnelGlow;
    
    // ===== 9. 펄스 애니메이션 =====
    float pulse = sin(g_fTime * 8.0f) * 0.1f + 0.9f;
    windColor *= pulse;
    
    // ===== 10. 스캐터링 효과 =====
    // 끝부분이 흩날리는 효과
    float scatter = 1.0f - pow(uv.x, 3.0f);
    float scatterNoise = g_NoiseTextures[2].Sample(DefaultSampler, uv * 3.0f + g_fTime).r;
    scatter *= (scatterNoise * 0.5f + 0.5f);
    
    // ===== 11. 최종 알파 계산 =====
    float finalAlpha = softEdge * intensity * scatter;
    
    // 중심부는 더 불투명하게
    if (intensity > 0.7f)
    {
        finalAlpha = lerp(finalAlpha, 1.0f, (intensity - 0.7f) / 0.3f);
    }
    
    // 시간에 따른 페이드 (g_fRatio 사용)
    finalAlpha *= (1.0f - g_fRatio);
    
    // ===== 12. 모션 블러 효과 시뮬레이션 =====
    // 이전 프레임과의 블렌딩을 시뮬레이션
    float motionBlur = 1.0f - (abs(uv.x - 0.5f) * 2.0f);
    windColor = lerp(windColor, windColor * 0.7f, motionBlur * 0.3f);
    
    // ===== 13. HDR 컬러 부스트 =====
    // 밝은 부분을 더 밝게
    float luminance = dot(windColor, float3(0.299f, 0.587f, 0.114f));
    if (luminance > 0.7f)
    {
        windColor *= 1.2f;
    }
    
    // ===== 최종 출력 =====
    //Out.vDiffuse = float4(saturate(windColor), saturate(finalAlpha));
    Out.vDiffuse = float4(saturate(windColor), saturate(1.f));
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}


PS_OUT_BACKBUFFER PS_VORTEXWIND_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

    // ===== 1. UV 좌표를 중심으로 이동 (-0.5 ~ 0.5 범위로) =====
    float2 centerUV = In.vTexcoord - 0.5f;

    // ===== 2. 극좌표계로 변환 (각도와 거리 계산) =====
    float distance = length(centerUV) * 2.0f; // 거리를 0~1 범위로 정규화
    float angle = atan2(centerUV.y, centerUV.x);

    // ===== 3. 애니메이션 적용 =====
    // 회전: 시간에 따라 각도를 변화시켜 회전 효과 생성
    float rotationSpeed = -3.0f; // 음수 = 시계방향, 양수 = 반시계방향
    angle += g_fTime * rotationSpeed;

    // 빨려 들어가는 효과: 시간에 따라 거리를 변화시켜 텍스처를 안쪽으로 이동
    // g_fTime이 증가할수록 distance를 더해서 샘플링할 UV를 바깥쪽으로 밀어냅니다.
    // 결과적으로 텍스처는 안쪽으로 흐르는 것처럼 보입니다.
    float inwardSpeed = 1.5f;
    distance += g_fTime * inwardSpeed;
    
    // ===== 4. 다시 직교좌표계 UV로 변환 =====
    float2 vortexUV;
    vortexUV.x = cos(angle) * distance * 0.5f + 0.5f;
    vortexUV.y = sin(angle) * distance * 0.5f + 0.5f;

    // ===== 5. 텍스처 샘플링 =====
    // 소용돌이(Swirl) 텍스처나 노이즈(Noise) 텍스처를 왜곡된 vortexUV로 샘플링합니다.
    float swirlNoise = g_SwirlTextures[0].Sample(DefaultSampler, vortexUV).r;
    float detailNoise = g_NoiseTextures[3].Sample(DefaultSampler, vortexUV * 2.0f + g_fTime).r; // 더 자글자글한 노이즈

    float combinedNoise = pow(swirlNoise * detailNoise, 1.5f);

    // ===== 6. 최종 마스크 및 알파 계산 =====
    // 중심에서 멀어질수록 투명해지는 마스크
    float distFromCenter = length(centerUV) * 2.0f;
    float edgeMask = 1.0f - saturate(pow(distFromCenter, 2.0f));

    // 최종 마스크
    float finalMask = combinedNoise * edgeMask;

    // 임계값 처리로 날카로운 경계 만들기
    float threshold = 0.2f;
    if (finalMask < threshold)
        discard;
    
    float softEdge = smoothstep(threshold, threshold + 0.2f, finalMask);
    
    // ===== 7. 색상 및 최종 알파 결정 =====
    float3 windColor = float3(0.7f, 0.85f, 0.95f); // 원하는 색상으로 변경 (예: 하늘색)
    windColor *= finalMask; // 마스크 강도에 따라 밝기 조절

    // 전체적인 페이드 아웃 (C++에서 g_fRatio 전달)
    float finalAlpha = softEdge * (1.0f - g_fRatio);

    // ===== 최종 출력 =====
    Out.vDiffuse = float4(saturate(windColor), saturate(finalAlpha));
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);

    return Out;
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

    // ===== 1. UV 스크롤링 (회전 대신 이동) =====
    // 평면 UV에서는 스크롤링이 더 자연스러움
    float2 scrolledUV = In.vTexcoord;
    scrolledUV.x += g_fTime * 5.f; // 가로 스크롤
    scrolledUV = frac(scrolledUV); // 0~1 범위로 래핑

    // ===== 2. 텍스처 샘플링 =====
    vector vDiffuseColor = g_DiffuseTexture.Sample(DefaultSampler, scrolledUV);
    vDiffuseColor.r *= 0.8f; // 빨강 유지
    vDiffuseColor.g *= 0.05f; // 약간의 녹색
    vDiffuseColor.b *= 0.0f; // 파랑 제거

    // ===== 3. UV 기반 외곽선 (위아래 경계) =====
    float edgeMask = 0.0f;
    float edgeThickness = 0.05f; // 외곽선 두께
    
    // 위쪽 가장자리 (V값이 1에 가까움)
    if (In.vTexcoord.y > 1.0f - edgeThickness)
    {
        edgeMask = (In.vTexcoord.y - (1.0f - edgeThickness)) / edgeThickness;
    }
    
    // 아래쪽 가장자리 (V값이 0에 가까움)
    if (In.vTexcoord.y < edgeThickness)
    {
        edgeMask = max(edgeMask, 1.0f - (In.vTexcoord.y / edgeThickness));
    }

    // ===== 4. 색상 처리 =====
    vector finalColor = vDiffuseColor;
    
    // 붉은색 강화
    finalColor.r = pow(finalColor.r, 0.7f) * 1.2f;
    finalColor.g *= 0.2f;
    finalColor.b *= 0.1f;
    
    // 외곽선을 검정색으로
    finalColor.rgb = lerp(finalColor.rgb, float3(0.0f, 0.0f, 0.0f), edgeMask);

    // ===== 5. 패턴 추가 (선택사항) =====
    // 세로 줄무늬 효과로 회전 느낌 연출
    float pattern = sin(In.vTexcoord.x * 20.0f - g_fTime * 5.0f) * 0.2f + 0.8f;
    finalColor.rgb *= pattern;

    // ===== 6. 알파 처리 =====
    float fadeAlpha = vDiffuseColor.a;
    fadeAlpha *= saturate(g_fGrowTime * 3.0f);
    fadeAlpha *= (1.0f - g_fDissolveTime);
    
    

    // ===== 출력 =====
    Out.vDiffuse = float4(saturate(finalColor.rgb), saturate(fadeAlpha));
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);

    return Out;
}


PS_OUT_BACKBUFFER PS_BLOOD_BODYAURA_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

    // ===== 1. 알파(Alpha) 계산: 오라의 '형태' 결정 =====

    // 1-1. 대각선 방향으로 스크롤되는 UV 생성
    float2 animatedUV = In.vTexcoord;
    animatedUV.x += g_fTime * 0.5f; // X축 이동 (회전)
    animatedUV.y -= g_fTime * 0.8f; // Y축 이동 (상승)

    // 1-2. 스크롤된 UV로 노이즈 텍스처를 샘플링하여 기본 모양 생성
    float noiseMask = g_NoiseTextures[5].Sample(DefaultSampler, frac(animatedUV)).r;
    noiseMask = smoothstep(0.4f, 0.6f, noiseMask); // 경계를 선명하게

    // 1-3. 물결 마스크(Wave Mask) 추가하여 사선으로 휘감는 느낌 강화
    float waveFrequency = 4.0f; // 물결의 갯수
    float waveSpeed = 3.0f;
    // UV의 y값(높이)과 시간에 따라 sin 값이 변하도록 하여 물결 생성
    float wave = sin(In.vTexcoord.y * waveFrequency + g_fTime * waveSpeed);
    // sin 결과(-1~1)를 0~1 범위로 변환
    float waveMask = wave * 0.5f + 0.5f;
    
    // 1-4. 높이 마스크 생성 (기존 로직 유지)
    float heightMask = 1.0f;
    heightMask *= smoothstep(0.0f, 0.2f, In.vTexcoord.y);
    heightMask *= (1.0f - smoothstep(0.8f, 1.0f, In.vTexcoord.y));

    // 최종 알파 = (기본 노이즈 모양) * (물결 모양) * (높이 마스크)
    float finalAlpha = noiseMask * waveMask * heightMask;
    finalAlpha *= (1.0f - g_fDissolveTime); // Dissolve 효과

    clip(finalAlpha - 0.01f);

    // ===== 2. 색상(Color) 계산 =====
    vector finalColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    // ===== 최종 출력 =====
    Out.vDiffuse = float4(saturate(finalColor.rgb), saturate(finalAlpha));
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);

    return Out;
}



//PS_OUT_BACKBUFFER PS_BLOOD_BODYAURA_MAIN(PS_BACKBUFFER_IN In)
//{
//    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

//     --- 1. UV 스크롤링으로 회전 느낌 주기 ---
//    float2 animatedUV = In.vTexcoord;
//    animatedUV.x += g_fTime * 0.5f;
//    animatedUV.y -= g_fTime * 0.8f;

//     --- 2. Noise 마스크 생성 및 '검은 부분 버리기' ---
//    float noiseValue = g_NoiseTextures[6].Sample(DefaultSampler, frac(animatedUV)).r;
//    float noiseMask = smoothstep(0.4f, 0.6f, noiseValue);

//     [핵심] clip 함수를 사용하여 noiseMask 값이 매우 낮은 (거의 검은색) 픽셀을 완전히 제거(버림)
//    clip(noiseMask - 0.01f);
    
//     --- 3. '흰색 부분에 Diffuse 색상 입히기' ---
//     clip을 통과한 픽셀들만 아래 코드를 실행하게 됩니다.
//    vector vDiffuseColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

//     최종 출력: 색상은 Diffuse, 투명도는 noiseMask 값을 그대로 사용하여 가장자리를 부드럽게 처리
//    Out.vDiffuse = float4(vDiffuseColor.rgb, 1.f);
    
//     나머지 G-Buffer 출력은 기본값으로 설정합니다.
//    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
//    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);

//    return Out;
//}

//PS_OUT_BACKBUFFER PS_BLOOD_BODYAURA_MAIN(PS_BACKBUFFER_IN In)
//{
//    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

//    // ===== 1. UV 왜곡(Distortion) 생성 =====
//    // 왜곡을 만들기 위한 노이즈 텍스처의 UV를 스크롤합니다.
//    float2 distortionUV = In.vTexcoord * 1.5f; // 왜곡 패턴의 크기 조절
//    distortionUV.y += g_fTime * 0.4f;

//    // 노이즈 텍스처를 샘플링하여 왜곡 방향과 강도를 얻습니다. (.rg 값을 -1~1 범위로 사용)
//    float2 distortionVector = (g_NoiseTextures[5].Sample(DefaultSampler, distortionUV).rg * 2.0f - 1.0f);
//    float distortionStrength = 0.2f; // 왜곡 강도

//    // 원래 UV 좌표에 왜곡 벡터를 더해 최종 UV를 만듭니다.
//    float2 distortedUV = In.vTexcoord + distortionVector * distortionStrength;


//    // ===== 2. 최종 알파(Alpha) 계산: 왜곡된 UV로 '형태' 결정 =====
    
//    // 2-1. 왜곡된 UV로 메인 패턴 텍스처를 샘플링합니다.
//    // (이 텍스처가 핏줄기의 주된 모양을 결정합니다. 다른 노이즈나 핏줄기 텍스처를 사용해도 좋습니다.)
//    float2 patternUV = distortedUV;
//    patternUV.y -= g_fTime * 0.5f; // 패턴 자체도 흐르도록 처리
//    float patternMask = g_NoiseTextures[6].Sample(DefaultSampler, patternUV).r;

//    // 2-2. 대비를 강화하여 핏줄기 형태를 날카롭게 만듭니다.
//    patternMask = pow(patternMask, 2.5f);
//    float sharpMask = smoothstep(0.4f, 0.6f, patternMask);
    
//    // 2-3. 높이 마스크 생성 (기존 로직 유지)
//    float heightMask = 1.0f;
//    heightMask *= smoothstep(0.0f, 0.2f, In.vTexcoord.y);
//    heightMask *= (1.0f - smoothstep(0.8f, 1.0f, In.vTexcoord.y));

//    // 최종 알파 계산
//    float finalAlpha = sharpMask * heightMask;
//    finalAlpha *= (1.0f - g_fDissolveTime);
    
//    clip(finalAlpha - 0.01f);

//    // ===== 3. 색상(Color) 계산 =====
//    // Diffuse 텍스처를 샘플링하여 그대로 색상으로 사용
//    vector finalColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

//    // ===== 최종 출력 =====
//    Out.vDiffuse = float4(saturate(finalColor.rgb), saturate(finalAlpha));
//    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
//    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
//    return Out;
//}

//PS_OUT_BACKBUFFER PS_BLOOD_BODYAURA_MAIN(PS_BACKBUFFER_IN In)
//{
//    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
   
    
    
//    // ===== 최종 출력 =====
//    Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
//    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
//    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
//    return Out;
//}







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

        //VertexShader = compile vs_5_0 VS_MAIN();
        //GeometryShader = NULL;
        //PixelShader = compile ps_5_0 PS_SWORDWIND_MAIN();
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //PixelShader = compile ps_5_0 PS_SWORDWIND_IMPROVED();
        PixelShader = compile ps_5_0 PS_VORTEXWIND_MAIN();
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

