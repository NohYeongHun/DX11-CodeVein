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






//PS_OUT_BACKBUFFER PS_SWORDWIND_MAIN(PS_BACKBUFFER_IN In)
//{
//    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

//     // 1. Base Color 텍스처 샘플링 (블렌더의 T_FX_GEZWhiteColor01)
//    float4 vMtrlDiffuse = g_DiffuseTextures[6].Sample(DefaultSampler, In.vTexcoord);
    
//    // 2. Alpha에 사용할 노이즈 텍스처 샘플링 (블렌더의 T_FX_UE4TilingNoise03)
//    float4 vMtrlNoise = g_NoiseTextures[4].Sample(DefaultSampler, In.vTexcoord);
    
//     // 3. Color Ramp 구현
//    // 노이즈 텍스처의 R 채널 값을 팩터로 사용합니다. (0.0 ~ 1.0 사이의 값)
//    float noiseFactor = vMtrlNoise.r;

//    float rampT = saturate(noiseFactor / 1.f);
    
//    float finalAlpha = lerp(0.f, 1.f, rampT);

//    // 원본 Diffuse 색상에 강도 값을 곱해 최종 색상을 계산합니다.
//    float3 finalColor = vMtrlDiffuse.rgb;
    
//    float fAlpha = vMtrlNoise.r * saturate(1.f - g_fRatio * 2.f);
    
//   // ===== 최종 출력 =====
//    // vMtrlDiffuse의 RGB 색상과 위에서 계산한 finalAlpha 값을 조합합니다.
//    //Out.vDiffuse = float4(finalColor, vMtrlNoise.r);
//    Out.vDiffuse = float4(finalColor, fAlpha);
//    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
//    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);

//    return Out;
//}

PS_OUT_BACKBUFFER PS_SWORDWIND_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    // UV 스크롤링 적용
    float2 scrolledUV = In.vTexcoord;
    scrolledUV.y += g_fRatio * 0.5f; // Y축 스크롤 (위로)
    scrolledUV.x += sin(g_fRatio * 3.14159f) * 0.2f; // X축 약간의 사인파 움직임
    
    // 1. Base Color 텍스처 샘플링
    float4 vMtrlDiffuse = g_DiffuseTextures[6].Sample(DefaultSampler, In.vTexcoord);
    
    // 2. 노이즈 텍스처 샘플링 (스크롤된 UV 사용)
    float4 vMtrlNoise = g_NoiseTextures[4].Sample(DefaultSampler, scrolledUV);
    
    // 3. 추가 노이즈 레이어 (다른 속도로 스크롤)
    float2 scrolledUV2 = In.vTexcoord;
    scrolledUV2.y -= g_fRatio * 0.3f; // 반대 방향
    float4 vMtrlNoise2 = g_NoiseTextures[4].Sample(DefaultSampler, scrolledUV2);
    
    // 두 노이즈 텍스처 블렌딩
    float noiseBlend = (vMtrlNoise.r + vMtrlNoise2.r) * 0.5f;
    
    // 4. 시간에 따른 알파 감쇠 (자연스러운 페이드 아웃)
    float timeFade = 1.0f;
    
    // 초반 20%는 페이드 인
    if (g_fRatio < 0.2f)
    {
        timeFade = smoothstep(0.0f, 0.2f, g_fRatio);
    }
    // 후반 30%에서 페이드 아웃 시작
    else if (g_fRatio > 0.7f)
    {
        timeFade = 1.0f - smoothstep(0.7f, 1.0f, g_fRatio);
    }
    
    // 5. 최종 알파 계산
    float finalAlpha = noiseBlend * timeFade;
    
    // 엣지를 더 선명하게 (선택사항)
    finalAlpha = saturate(finalAlpha * 1.5f - 0.2f);
    
    // 6. 색상 계산
    float3 finalColor = vMtrlDiffuse.rgb;
    
    // 시간에 따른 색상 변화 (선택사항)
    float3 colorTint = lerp(float3(1.0f, 1.0f, 1.0f),
                            float3(0.8f, 0.9f, 1.0f),
                            g_fRatio);
    finalColor *= colorTint;
    
    // 최종 출력
    Out.vDiffuse = float4(finalColor, finalAlpha);
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
    
    // 1-1. 대각선 방향으로 스크롤되는 UV 생성 (회전 속도 증가)
    float2 animatedUV = In.vTexcoord;
    animatedUV.x += g_fTime * 0.7f; // X축 이동 (회전) - 속도 증가
    animatedUV.y -= g_fTime * 1.2f; // Y축 이동 (상승) - 속도 증가
    
    // 1-2. 다층 노이즈로 더 선명한 디테일 생성
    float noiseMask1 = g_NoiseTextures[5].Sample(DefaultSampler, frac(animatedUV)).r;
    float noiseMask2 = g_NoiseTextures[5].Sample(DefaultSampler, frac(animatedUV * 2.0f)).r;
    
    // 두 노이즈를 곱해서 더 날카로운 엣지 생성
    float noiseMask = noiseMask1 * noiseMask2;
    noiseMask = pow(noiseMask, 0.5f); // 감마 보정으로 중간값 강화
    noiseMask = smoothstep(0.2f, 0.4f, noiseMask); // 더 타이트한 경계
    
    // 1-3. 나선형 물결 마스크 (토네이도의 회전 강조)
    float waveFrequency = 6.0f; // 물결 갯수 증가
    float waveSpeed = 4.0f; // 회전 속도 증가
    float spiralOffset = In.vTexcoord.x * 3.14159f; // X 좌표에 따른 위상 차이
    
    // 나선형 패턴 생성
    float wave = sin(In.vTexcoord.y * waveFrequency + g_fTime * waveSpeed + spiralOffset);
    // 더 선명한 스트라이프를 위해 pow 사용
    float waveMask = saturate(pow(abs(wave), 0.3f));
    
    // 1-4. 높이 마스크 개선 (토네이도 형태 강화)
    float heightMask = 1.0f;
    
    // 아래쪽은 넓게, 위쪽은 좁게 (토네이도 형태)
    float widthScale = lerp(1.2f, 0.3f, In.vTexcoord.y);
    float centerDist = abs(In.vTexcoord.x - 0.5f) * 2.0f;
    float radialMask = 1.0f - smoothstep(0.0f, widthScale, centerDist);
    
    // 상하 페이드
    heightMask *= smoothstep(0.0f, 0.15f, In.vTexcoord.y);
    heightMask *= (1.0f - smoothstep(0.85f, 1.0f, In.vTexcoord.y));
    
    // 1-5. 엣지 강조를 위한 추가 마스크
    float edgeEnhance = 1.0f - smoothstep(0.6f, 0.9f, noiseMask);
    edgeEnhance = saturate(edgeEnhance + 0.3f); // 최소값 보장
    
    // 최종 알파 계산 (각 요소의 기여도 조정)
    float finalAlpha = noiseMask * waveMask * heightMask * radialMask * edgeEnhance;
    
    // 알파 값 강화 (더 진하게)
    finalAlpha = pow(finalAlpha, 0.7f); // 감마 보정으로 전체적으로 밝게
    finalAlpha = saturate(finalAlpha * 1.5f); // 강도 증폭
    
    finalAlpha *= (1.0f - g_fDissolveTime); // Dissolve 효과
    
    // 더 선명한 컷오프
    clip(finalAlpha - 0.05f);
    
    // ===== 2. 색상(Color) 계산 =====
    vector baseColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // 엣지 부분 색상 강조 (선택적)
    float edgeGlow = 1.0f + (1.0f - noiseMask) * 0.5f;
    vector finalColor = baseColor * edgeGlow;
    
    // ===== 최종 출력 =====
    Out.vDiffuse = float4(saturate(finalColor.rgb), saturate(finalAlpha));
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}


// =============================================================
// 전역 변수 (Global Variables)
// =============================================================

float g_fEffectDuration;

// 이 셰이더는 g_OtherTextures[5]에 'Black -> White -> Black' 그라디언트 텍스처를 사용해야 합니다.


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

// 메인 픽셀 셰이더
PS_OUT_BACKBUFFER PS_SWORDWIND_CIRCLE_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    // 전역 변수
    float fTimeRatio = g_fRatio;
    float fTime = g_fTime;
    float fScrollSpeed = g_fScrollSpeed;
    
    // 1. UV 스크롤링 (원형 효과를 위한 회전)
    float2 vScrolledUV = In.vTexcoord;
    
    // 회전 스크롤 (원형 검기에 적합)
    float2 center = float2(0.5f, 0.5f);
    float2 offset = vScrolledUV - center;
    float scrollAngle = fTime * fScrollSpeed;
    float2 rotatedUV;
    rotatedUV.x = offset.x * cos(scrollAngle) - offset.y * sin(scrollAngle);
    rotatedUV.y = offset.x * sin(scrollAngle) + offset.y * cos(scrollAngle);
    vScrolledUV = rotatedUV + center;
    
    // 2. 베이스 텍스처 샘플링
    float4 vBaseColor = g_DiffuseTextures[6].Sample(DefaultSampler, In.vTexcoord);
    
    // 3. 동적 그라데이션 마스크 생성 (텍스처 대신)
    float fGradientMask = GenerateGradientMask(vScrolledUV);
    
    // 추가: 방사형 마스크와 조합 (원형 검기 효과 강화)
    float fRadialMask = GenerateRadialGradientMask(In.vTexcoord, 0.1f, 0.5f);
    
    // 추가: 회전 슬래시 효과 (선택적)
    float fSlashMask = GenerateRotatingSlashMask(In.vTexcoord, fTime * 2.0f, 4.0f);
    
    // 마스크 조합 (필요에 따라 선택)
    float fCombinedMask = fGradientMask * fRadialMask; // 두 마스크 곱하기
    // float fCombinedMask = max(fGradientMask, fSlashMask * 0.5f);  // 또는 블렌딩
    
    // 4. 시간에 따른 페이드 아웃
    float fTimeFade = 1.0f - fTimeRatio;
    
    // S-커브로 더 부드러운 페이드
    fTimeFade = smoothstep(0.0f, 1.0f, fTimeFade);
    
    // 5. 펄스 효과 (선택적 - 생동감 추가)
    float fPulse = 1.0f + sin(fTime * 8.0f) * 0.15f * fTimeFade;
    
    // 6. 엣지 글로우 효과
    float fEdgeGlow = pow(1.0f - fRadialMask, 3.0f) * fTimeFade;
    float3 vGlowColor = float3(0.3f, 0.6f, 1.0f); // 파란빛 글로우
    
    // 7. 최종 알파 계산
    float fFinalAlpha = vBaseColor.a * fCombinedMask * fTimeFade * fPulse;
    
    // 알파 최소값 설정 (완전히 안 보이는 것 방지)
    fFinalAlpha = max(fFinalAlpha, 0.05f * fTimeFade);
    
    // 8. 최종 색상 계산
    float3 vFinalRGB = vBaseColor.rgb;
    
    // 엣지 글로우 추가
    vFinalRGB += vGlowColor * fEdgeGlow * 0.5f;
    
    // HDR 부스트 (시작 시 더 밝게)
    vFinalRGB *= (1.0f + fTimeFade * 0.5f);
    
    // 9. 최종 출력
    Out.vDiffuse = float4(vFinalRGB, fFinalAlpha);
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
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

    pass SwordWindCircle // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //PixelShader = compile ps_5_0 PS_SWORDWIND_IMPROVED();
        PixelShader = compile ps_5_0 PS_SWORDWIND_CIRCLE_MAIN();
    }



}

