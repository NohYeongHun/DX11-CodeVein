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
texture2D g_NoiseTextures[5];
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


//PS_OUT_BACKBUFFER PS_SWORDWIND_IMPROVED(PS_BACKBUFFER_IN In)
//{
//    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
//    // 0 . T1 패스 구현
//    // 1. 외부에서 받아온 크기 값을 곱해줍니다.
//    float2 vTexcoord = In.vTexcoord;
//    vTexcoord *= g_fTextureScale;
    
//    // 2. 처음 텍스쳐 두개의 RGB를 빼줍니다.
//    float4 vSwirl1 = g_SwirlTextures[0].Sample(DefaultSampler, vTexcoord);
//    float4 vSwirl2 = g_SwirlTextures[1].Sample(DefaultSampler, vTexcoord);
//    float3 vSubStract0 = vSwirl1.rgb - vSwirl2.rgb;
    
//    // 3. 두 텍스쳐를 빼줍니다.
//    float4 vSwirl3 = g_SwirlTextures[2].Sample(DefaultSampler, vTexcoord);
//    float4 vSwirl4 = g_SwirlTextures[3].Sample(DefaultSampler, vTexcoord);
    
//    float3 vSubStract1 = vSwirl3.rgb - vSwirl4.rgb;
    
//    // 4. MaskOrient 스위치로 두 Subtract 결과 중 하나를 선택합니다.
//    // 원래는 bool값으로 결정하지만 일단 임의로 0결정.
//    //float3 vMaskResult = g_bMaskOrient ? vSubStract0 : vSubStract1;
//    float3 vMaskResult = vSubStract0;
//    vMaskResult *= 1.3f;
    
//    // 5. Clamp 노드로 최종 결과값을 0과 1 사이로 제한합니다.
//    float3 vFinalResult_T1 = saturate(vMaskResult);
    
//    // 6. T0 패스 구현.
//    float vFinalResult_T0 = BorderMask_Implementation(In.vTexcoord);
    
//    // 7. T2 패스 구현 진행.
//    float result_T2 = T2_Implementation(In.vTexcoord, g_fTime);
//    float result_T3 = T3_Implementation(In.vTexcoord, g_fTime);
    
//    // 8. T0 * T2
//    //float vFinalResult_T2_T3 = result_T3;
//    float vFinalResult_T2_T3 = result_T2;
    
//    // 기본 Color
//    float4 vBaseColor = float4(1.f, 1.f, 1.f, 1.f);
//    //float final_mask = vFinalResult_T0 * vFinalResult_T2_T3;
//    //float3 final_color = vFinalResult_T1 * final_mask * vBaseColor.rgb;
    
//    //Out.vDiffuse = float4(final_color, 0.8f);
    
//    float final_mask = vFinalResult_T0 * vFinalResult_T2_T3;
//    float3 final_color = vFinalResult_T1 * final_mask * vBaseColor.rgb;
    
//    // 색상의 밝기를 계산 (더 정확한 luminance 공식 사용)
//    float luminance = dot(final_color, float3(0.299f, 0.587f, 0.114f));
    
//    // 밝기가 임계값 이하면 완전 투명하게 처리
//    float threshold = 0.1f; // 이 값을 조절해서 검은색 제거 정도 조절
//    float alpha_from_brightness = smoothstep(0.0f, threshold, luminance);
    
//    // 최종 알파값 계산
//    float final_alpha = alpha_from_brightness * 0.8f * g_fGlobalAlpha;
    
//    // 검은색에 가까울수록 discard
//    if (final_alpha < 0.01f)
//        discard;
    
//    Out.vDiffuse = float4(final_color, 1.f);
    
    
//    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
//    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
//    return Out;
//}

//PS_OUT_BACKBUFFER PS_SWORDWIND_IMPROVED(PS_BACKBUFFER_IN In)
//{
//    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
//    // 0 . T1 패스 구현
//    // 1. 외부에서 받아온 크기 값을 곱해줍니다.
//    float2 vTexcoord = In.vTexcoord;
//    vTexcoord *= g_fTextureScale;
    
//    // 2. 처음 텍스쳐 두개의 RGB를 빼줍니다.
//    float4 vSwirl1 = g_SwirlTextures[0].Sample(DefaultSampler, vTexcoord);
//    float4 vSwirl2 = g_SwirlTextures[1].Sample(DefaultSampler, vTexcoord);
//    float3 vSubStract0 = vSwirl1.rgb - vSwirl2.rgb;
    
//    // 3. 두 텍스쳐를 빼줍니다.
//    float4 vSwirl3 = g_SwirlTextures[2].Sample(DefaultSampler, vTexcoord);
//    float4 vSwirl4 = g_SwirlTextures[3].Sample(DefaultSampler, vTexcoord);
    
//    float3 vSubStract1 = vSwirl3.rgb - vSwirl4.rgb;
    
//    // 4. MaskOrient 스위치로 두 Subtract 결과 중 하나를 선택합니다.
//    // 원래는 bool값으로 결정하지만 일단 임의로 0결정.
//    //float3 vMaskResult = g_bMaskOrient ? vSubStract0 : vSubStract1;
//    float3 vMaskResult = vSubStract0;
//    vMaskResult *= 1.3f;
    
//    // 5. Clamp 노드로 최종 결과값을 0과 1 사이로 제한합니다.
//    float3 vFinalResult_T1 = saturate(vMaskResult);
    
//    // 6. T0 패스 구현.
//    float vFinalResult_T0 = BorderMask_Implementation(In.vTexcoord);
    
//    // 7. T2 패스 구현 진행.
//    float result_T2 = T2_Implementation(In.vTexcoord, g_fTime);
//    float result_T3 = T3_Implementation(In.vTexcoord, g_fTime);
    
//    // 8. T0 * T2
//    //float vFinalResult_T2_T3 = result_T3;
//    float vFinalResult_T2_T3 = result_T2;
    
//    // 기본 Color
//    float4 vBaseColor = float4(1.f, 1.f, 1.f, 1.f);
//    //float final_mask = vFinalResult_T0 * vFinalResult_T2_T3;
//    //float3 final_color = vFinalResult_T1 * final_mask * vBaseColor.rgb;
    
//    //Out.vDiffuse = float4(final_color, 0.8f);
    
//    float final_mask = vFinalResult_T0 * vFinalResult_T2_T3;
//    float3 final_color = vFinalResult_T1 * final_mask * vBaseColor.rgb;
    
//    // 마스크 값이 너무 낮으면 픽셀을 버림
//    float mask_threshold = 0.05f;
//    if (final_mask < mask_threshold)
//        discard;
    
//    // 마스크를 알파에도 적용
//    float final_alpha = saturate(final_mask * 2.0f) * 0.8f;
    
//    Out.vDiffuse = float4(final_color, final_alpha);
//    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
//    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
//    return Out;
//}




//PS_OUT_BACKBUFFER PS_SWORDWIND_IMPROVED(PS_BACKBUFFER_IN In)
//{
//    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
//    // 0 . T1 패스 구현
//    // 1. 외부에서 받아온 크기 값을 곱해줍니다.
//    float2 vTexcoord = In.vTexcoord;
//    vTexcoord *= g_fTextureScale;
    
//    // 2. 처음 텍스쳐 두개의 RGB를 빼줍니다.
//    float4 vSwirl1 = g_SwirlTextures[0].Sample(DefaultSampler, vTexcoord);
//    float4 vSwirl2 = g_SwirlTextures[1].Sample(DefaultSampler, vTexcoord);
//    float3 vSubStract0 = vSwirl1.rgb - vSwirl2.rgb;
    
//    // 3. 두 텍스쳐를 빼줍니다.
//    float4 vSwirl3 = g_SwirlTextures[2].Sample(DefaultSampler, vTexcoord);
//    float4 vSwirl4 = g_SwirlTextures[3].Sample(DefaultSampler, vTexcoord);
    
//    float3 vSubStract1 = vSwirl3.rgb - vSwirl4.rgb;
    
//    // 4. MaskOrient 스위치로 두 Subtract 결과 중 하나를 선택합니다.
//    float3 vMaskResult = vSubStract0;
//    vMaskResult *= 1.3f;
    
//    // 5. Clamp 노드로 최종 결과값을 0과 1 사이로 제한합니다.
//    float3 vFinalResult_T1 = saturate(vMaskResult);
    
//    // 6. T0 패스 구현.
//    float vFinalResult_T0 = BorderMask_Implementation(In.vTexcoord);
    
//    // 7. T2 패스 구현 진행.
//    float result_T2 = T2_Implementation(In.vTexcoord, g_fTime);
//    float result_T3 = T3_Implementation(In.vTexcoord, g_fTime);
    
//    // 8. T0 * T2
//    float vFinalResult_T2_T3 = result_T2;
    
//    // 마스크 계산
//    float final_mask = vFinalResult_T0 * vFinalResult_T2_T3;
    
//    // 마스크 값이 너무 낮으면 픽셀을 버림 (검은색 제거)
//    float mask_threshold = 0.05f;
//    if (final_mask < mask_threshold)
//        discard;
    
//    // ===== 연기 색상 처리 =====
//    // 1. 기본 연기 색상 설정 (회색톤)
//    //float3 smokeBaseColor = float3(0.7f, 0.7f, 0.8f); // 약간 푸른빛이 도는 회색
    
//    // 2. 마스크를 사용해 색상 변화 추가
//    float colorVariation = pow(final_mask, 0.5f); // 마스크를 부드럽게 만듦
    
//    //// 3. 연기 색상 계산 - 가장자리는 더 밝고 투명하게, 중심은 더 진하게
//    //float3 smokeColor = lerp(
//    //    float3(0.9f, 0.9f, 0.95f), // 밝은 연기 (가장자리)
//    //    float3(0.5f, 0.5f, 0.6f), // 진한 연기 (중심)
//    //    colorVariation
//    //);
    
//    // 색상 부분만 교체
//    float3 smokeBaseColor = float3(0.85f, 0.85f, 0.9f);
//    float3 smokeColor = smokeBaseColor * (0.7f + colorVariation * 0.3f);
//    float3 final_color = smokeColor; // 텍스처 영향 제거
    
//    // 4. 원본 텍스처 패턴을 약하게 적용
//    float3 textureInfluence = vFinalResult_T1 * 0.3f; // 텍스처 영향을 줄임
    
//    // 5. 최종 색상 계산
//    //float3 final_color = smokeColor + textureInfluence;
    
//    // ===== 알파값 처리 =====
//    // 1. 마스크 기반 알파 계산
//    float base_alpha = saturate(final_mask * 1.5f); // 마스크를 조금 더 강하게
    
//    // 2. 가장자리 페이드 효과
//    float edge_fade = smoothstep(mask_threshold, mask_threshold + 0.1f, final_mask);
    
//    // 3. 전체적인 투명도 조절
//    float final_alpha = base_alpha * edge_fade * 0.6f; // 전체적으로 반투명하게
    
//    // 4. 중심부는 조금 더 불투명하게
//    float center_boost = smoothstep(0.2f, 0.7f, final_mask);
//    final_alpha = lerp(final_alpha, final_alpha * 1.3f, center_boost);
//    final_alpha = saturate(final_alpha);
    
//    Out.vDiffuse = float4(final_color, final_alpha);
//    Out.vDiffuse = float4(final_color, 1.f);
//    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
//    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
//    return Out;
//}

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
    
    // 텍스처 좌표 설정
    float2 vTexcoord = In.vTexcoord;
    vTexcoord *= g_fTextureScale;
    
    // 왜곡 효과 추가 (풍압의 움직임)
    float2 distortion = float2(sin(g_fTime * 3.0f) * 0.02f, cos(g_fTime * 2.0f) * 0.01f);
    vTexcoord += distortion;
    
    // 텍스처 샘플링
    float4 vSwirl1 = g_SwirlTextures[0].Sample(DefaultSampler, vTexcoord);
    float4 vSwirl2 = g_SwirlTextures[1].Sample(DefaultSampler, vTexcoord);
    float3 vSubstract0 = vSwirl1.rgb - vSwirl2.rgb;
    
    float4 vSwirl3 = g_SwirlTextures[2].Sample(DefaultSampler, vTexcoord);
    float4 vSwirl4 = g_SwirlTextures[3].Sample(DefaultSampler, vTexcoord);
    float3 vSubStract1 = vSwirl3.rgb - vSwirl4.rgb;
    
    // 더 강한 대비를 위해 값 증폭
    float3 vMaskResult = vSubstract0;
    vMaskResult *= 2.0f; // 1.3f에서 2.0f로 증가
    
    float3 vFinalResult_T1 = saturate(vMaskResult);
    
    // 개선된 BorderMask 적용
    float vFinalResult_T0 = BorderMask_Implementation(In.vTexcoord);
    
    // T2, T3 결과
    float result_T2 = T2_Implementation(In.vTexcoord, g_fTime);
    float result_T3 = T3_Implementation(In.vTexcoord, g_fTime);
    
    // T3를 섞어서 더 날카로운 효과
    float vFinalResult_T2_T3 = lerp(result_T2, result_T3, 0.3f);
    
    // 마스크 계산
    float final_mask = vFinalResult_T0 * vFinalResult_T2_T3;
    
    // 더 높은 threshold로 날카로운 경계
    float mask_threshold = 0.1f; // 0.05f에서 0.1f로 증가
    if (final_mask < mask_threshold)
        discard;
    
    // ===== 칼바람 색상 처리 =====
    // 풍압 강도 계산
    float windPressure = pow(final_mask, 0.7f);
    
    // 칼바람 색상 팔레트 (차갑고 날카로운 느낌)
    float3 coreColor = float3(0.9f, 0.95f, 1.0f); // 중심부 - 거의 흰색
    float3 midColor = float3(0.6f, 0.7f, 0.85f); // 중간 - 차가운 청백색
    float3 edgeColor = float3(0.3f, 0.4f, 0.6f); // 가장자리 - 진한 청회색
    
    // 3단계 그라데이션으로 날카로운 층 생성
    float3 windColor;
    if (windPressure > 0.7f)
    {
        // 핵심부 - 가장 밝고 강렬
        windColor = lerp(midColor, coreColor, (windPressure - 0.7f) / 0.3f);
    }
    else if (windPressure > 0.3f)
    {
        // 중간층
        windColor = lerp(edgeColor, midColor, (windPressure - 0.3f) / 0.4f);
    }
    else
    {
        // 가장자리 - 흩어지는 바람
        windColor = edgeColor * windPressure / 0.3f;
    }
    
    // 속도감을 위한 스트릭(줄무늬) 효과 추가
    float streak = sin(In.vTexcoord.x * 50.0f + g_fTime * 5.0f) * 0.1f + 0.9f;
    windColor *= streak;
    
    // 텍스처 디테일을 약간 추가 (너무 많으면 흐려짐)
    float3 textureDetail = vFinalResult_T1 * 0.2f;
    float3 final_color = windColor + textureDetail;
    
    // 에너지 효과 (선택적 - 더 강렬한 느낌)
    float energyPulse = sin(g_fTime * 4.0f) * 0.1f + 0.9f;
    final_color *= energyPulse;
    
    // ===== 알파값 처리 =====
    // 중심은 불투명, 가장자리는 투명
    float core_alpha = saturate(pow(final_mask, 0.5f));
    
    // 날카로운 경계를 위한 스텝 함수
    float sharp_edge = smoothstep(mask_threshold, mask_threshold + 0.15f, final_mask);
    
    // 풍압 끝부분 흩날림 효과
    float scatter = 1.0f - pow(In.vTexcoord.y, 3.0f);
    
    // 최종 알파
    float final_alpha = core_alpha * sharp_edge * scatter;
    final_alpha = saturate(final_alpha * 1.2f); // 전체적으로 더 선명하게
    
    // 중심부 강조
    if (windPressure > 0.6f)
    {
        final_alpha = lerp(final_alpha, 1.0f, (windPressure - 0.6f) / 0.4f);
    }
    
    //Out.vDiffuse = float4(final_color, final_alpha);
    Out.vDiffuse = float4(final_color, 1.f);
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
        //GeometryShader = NULL;
        //PixelShader = compile ps_5_0 PS_SWORDWIND_MAIN();
        //VertexShader = compile vs_5_0 VS_SWORDWIND_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SWORDWIND_IMPROVED();
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


}

