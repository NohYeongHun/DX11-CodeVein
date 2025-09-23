#include "Engine_Shader_Defines.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;


vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

/*����*/
texture2D g_DiffuseTexture;
texture2D g_DiffuseTextures[7];
texture2D g_OtherTextures[13];
texture2D g_NoiseTextures[8];

texture2D g_SwirlTextures[6];

texture2D g_NormalTexture;
texture2D g_NoiseTexture;

/*����*/
texture2D g_DissolveTexture;

vector g_vMtrlAmbient = 1.f;
vector g_vMtrlSpecular = 1.f;


// �ð�.
float g_fDissolveTime;
float g_fGrowTime;
float g_fMoveTime;


float g_fTime;
float g_fRatio;
float g_fScrollSpeed = 2.f;

float g_fDissolveThresold; // Dissolve ��.


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

/* �������̴� : ���� ��ġ�� �����̽� ��ȯ(���� -> ���� -> �� -> ����). */ 
/*          : ������ ������ ����.(in:3��, out:2�� or 5��) */
/*          : ���� ����(���� �ϳ��� VS_MAIN�ѹ�ȣ��) */ 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    /* ������ ������ġ * ���� * �� * ���� */ 
        
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

float g_fSlashDirection = 1.f; // ���� ����
float g_fSlashIntensity = 1.f; // ���� ����
float g_fWaveAmplitude = 0.3f; // �ĵ� ����

VS_OUT VS_SWORDWIND_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    // ���� ��ġ ���� - ��ǳ�� ��ī�ο� ���� �����
    float3 localPos = In.vPosition;
    
    // UV ������� ���� ���� (���η� �þ�� ȿ��)
    float stretchFactor = 1.0f + (abs(In.vTexcoord.x - 0.5f) * 2.0f) * g_fSlashIntensity;
    localPos.x *= stretchFactor * 2.5f; // X������ �ø���
    
    // �ĵ� ȿ�� �߰� (�ð��� ���� ���̺�)
    float wave = sin(In.vTexcoord.x * 6.28f + g_fTime * 10.0f) * g_fWaveAmplitude;
    localPos.y += wave * (1.0f - abs(In.vTexcoord.x - 0.5f) * 2.0f);
    
    // ���κ��� ��ī�Ӱ� (�����۸�)
    float taperFactor = 1.0f - pow(abs(In.vTexcoord.x - 0.5f) * 2.0f, 2.0f);
    localPos.y *= taperFactor;
    localPos.z *= taperFactor;
    
    // ������ ��ȯ ����
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
      /* ������ ������ġ * ���� * �� * ���� */ 
    
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

/* ���� �ȼ� ������ ���ؼ� �ȼ� ���̴��� �����Ѵ�. */
/* �ȼ��� ���� �����Ѵ�. */


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
    
     /* ��� ���� �ϳ��� �����ϱ����� �������� ���ý����̽��� ����� �� �����ȿ����� ���⺤�͸� ���� */
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
    
    // �ȿ� ���ڰ� 0�̵Ǹ� �ȱ׸���.
    
    clip(vMtrlDissolve.r - g_fDissolveTime);
    
    return Out;
}


/* */
float g_fScrollSpeedU = 2.f;
float g_fScrollSpeedV = 2.f;

// ��ȭ�� �� ���� �Ķ����
float g_fBloodDarkness = 0.3f; // ���� ��ο� ���� (0-1, �������� ����)
float g_fBloodSaturation = 1.8f; // ä�� ����
float g_fBloodContrast = 1.5f; // ��� ����
float g_fViscosity = 0.85f; // ������ (������ ȿ��)

// ���� �Ķ���� ������
float g_fErosionThreshold = 0.4f;
float g_fDistortionStrength = 0.03f;
float g_fFresnelPower = 5.0f;
vector g_vFresnelColor = float4(0.6f, 0.0f, 0.0f, 1.f); // �� ���� ������
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

    // -- 1. ���� ��� �ҿ뵹�� ���� ��� --
    float fHeightRatio = saturate(1.0f - In.vTexcoord.y);
    float fVortexIntensity = g_fVortexStrength * fHeightRatio * g_fHeightGradient;
    
    // -- 2. ����ǥ�� ��ȯ�� ���� �ҿ뵹�� ȿ�� --
    float2 vCenterUV = float2(0.5f, 0.5f);
    float2 vToCenter = In.vTexcoord - vCenterUV;
    
    float fDistance = length(vToCenter);
    float fAngle = atan2(vToCenter.y, vToCenter.x);
    
    float fVortexRotation = fVortexIntensity / max(fDistance, 0.1f) * g_fTime * g_fVortexSpeed;
    fAngle += fVortexRotation;
    
    float2 vVortexUV = vCenterUV + fDistance * float2(cos(fAngle), sin(fAngle));
    
    // -- 3. ���� ������ �ִϸ��̼� (���� ȿ�� �߰�) --
    float2 vNoiseUV1 = vVortexUV + float2(0.1f, g_fVerticalFlow * g_fViscosity) * g_fTime * 0.3f;
    float2 vNoiseUV2 = vVortexUV + float2(-0.15f, g_fVerticalFlow * 0.8f * g_fViscosity) * g_fTime * 0.5f;
    
    float2 vDetailUV1 = In.vTexcoord * 2.0f + float2(0.2f, g_fVerticalFlow * 1.5f) * g_fTime * 0.7f;
    float2 vDetailUV2 = In.vTexcoord * 1.5f + float2(-0.1f, g_fVerticalFlow * 1.2f) * g_fTime * 0.6f;
    
    // -- 4. ���� �ְ� ȿ�� --
    float2 vVortexDistortion = (g_OtherTextures[0].Sample(DefaultSampler, vNoiseUV1).rg * 2.f - 1.f) * g_fDistortionStrength;
    float2 vTurbulenceDistortion = (g_OtherTextures[0].Sample(DefaultSampler, vDetailUV1).rg * 2.f - 1.f) * g_fTurbulence * 0.02f;
    
    vVortexDistortion *= fHeightRatio * 1.5f;
    vTurbulenceDistortion *= (1.0f - fHeightRatio * 0.5f);
    
    float2 vFinalDistortedUV = In.vTexcoord + vVortexDistortion + vTurbulenceDistortion;
    
    // -- 5. ���� ħ�� ����ũ --
    float fErosionMask1 = g_OtherTextures[0].Sample(DefaultSampler, vNoiseUV2).r;
    float fErosionMask2 = g_OtherTextures[0].Sample(DefaultSampler, vDetailUV2).r;
    
    float fCombinedErosion = lerp(fErosionMask1, fErosionMask2, fHeightRatio);
    float fDynamicThreshold = g_fErosionThreshold + sin(g_fTime * 2.0f + fHeightRatio * 3.14159f) * 0.1f;
    
    clip(fCombinedErosion - fDynamicThreshold);
    
    // -- 6. ��ȭ�� �� �ؽ�ó ���ø� --
    // ���� ���� �ؽ�ó (������ �ؽ�ó ���)
    float2 vBloodFlowUV = vFinalDistortedUV + float2(sin(fAngle) * 0.1f, g_fVerticalFlow * 0.4f) * g_fTime;
    vector vBloodColor = g_DiffuseTextures[4].Sample(DefaultSampler, vBloodFlowUV);
    
    // ������ ���̾� (�߰� ���̰�)
    float2 vDetailFlowUV = vFinalDistortedUV * 0.5f + float2(cos(fAngle) * 0.05f, g_fVerticalFlow * 0.6f) * g_fTime;
    vector vDetailColor = g_DiffuseTextures[5].Sample(DefaultSampler, vDetailFlowUV);
    
    // -- 7. �� ���� ��ȭ ó�� --
    // ���̽� �÷��� �� ���ϰ� ����
    vector vFinalColor = vBloodColor;
    
    // ������ ä�� ��ȭ, �ٸ� ä�� ����
    vFinalColor.r = pow(vFinalColor.r, 0.7f) * 1.2f; // ������ ��ȭ
    vFinalColor.g *= 0.1f; // ��� ũ�� ����
    vFinalColor.b *= 0.15f; // �Ķ��� ũ�� ����
    
    // ������ ���� ���� (Multiply �������� ��Ӱ�)
    vFinalColor.rgb = vFinalColor.rgb * (vDetailColor.rgb * 0.8f + 0.2f);
    
    // ä�� ����
    float fLuminance = dot(vFinalColor.rgb, float3(0.299f, 0.587f, 0.114f));
    vFinalColor.rgb = lerp(float3(fLuminance, fLuminance, fLuminance), vFinalColor.rgb, g_fBloodSaturation);
    
    // ��� ����
    vFinalColor.rgb = saturate((vFinalColor.rgb - 0.5f) * g_fBloodContrast + 0.5f);
    
    // ��ü������ ��Ӱ� (���� ���� ����)
    vFinalColor.rgb *= g_fBloodDarkness;
    
    // -- 8. ������ ��� ���� ���� �߰� --
    float fBloodPattern = g_OtherTextures[1].Sample(DefaultSampler, vFinalDistortedUV * 3.0f).r;
    float fBloodSpots = g_OtherTextures[0].Sample(DefaultSampler, vFinalDistortedUV * 5.0f).g;
    
    // ���� ������ �̿��� �߰� ��ο� ����
    float fDarkSpots = saturate(1.0f - fBloodPattern * fBloodSpots);
    vFinalColor.rgb *= (0.5f + fDarkSpots * 0.5f);
    
    // -- 9. ��� �������� �ҿ뵹�� ȿ�� --
    float2 vRisingMaskUV = In.vTexcoord + float2(sin(g_fTime + fAngle) * 0.1f, g_fTime * 2.0f);
    float fRisingMask = g_OtherTextures[1].Sample(DefaultSampler, vRisingMaskUV).r;
    
    float fVortexCore = saturate(1.0f - fDistance * 2.0f);
    fRisingMask = saturate(fRisingMask + fVortexCore * 0.5f);
    
    // �ھ� �κ��� �� ���� ����������
    vFinalColor.r += fVortexCore * 0.3f;
    vFinalColor.rgb = saturate(vFinalColor.rgb);
    
    // -- 10. ��ȭ�� ������ ȿ�� (�����ڸ� ��) --
    vector vViewDir = normalize(g_vCamPosition - In.vWorldPos);
    float fFresnel = 1.f - saturate(dot(vViewDir.xyz, In.vNormal.xyz));
    fFresnel = pow(fFresnel, g_fFresnelPower);
    
    // ������ ���� �� ���� ����������
    vector vDynamicFresnelColor = g_vFresnelColor * (0.5f + fVortexIntensity * 0.3f);
    vFinalColor.rgb += vDynamicFresnelColor.rgb * fFresnel * 0.3f; // ������ ȿ�� ����
    
    // -- 11. ���� ȿ�� �߰� --
    // �ð��� ���� ������ �帧 ȿ��
    float fViscosityFlow = sin(g_fTime * 0.5f + In.vTexcoord.y * 10.0f) * 0.05f;
    vFinalColor.rgb *= (1.0f + fViscosityFlow * fHeightRatio);
    
    // -- 12. ���� ���� ��� --
    float fSoftEdge = saturate((fCombinedErosion - fDynamicThreshold) * 15.f);
    float fFinalAlpha = vBloodColor.a * fSoftEdge;
    
    // ���� �κ��� �� �������ϰ�
    fFinalAlpha = saturate(fFinalAlpha * (1.0f + fDarkSpots * 0.5f));
    fFinalAlpha *= (0.8f + fVortexIntensity * 0.2f);
    fFinalAlpha *= (1.f - g_fRatio);
    
    // -- 13. ���� ���� Ŭ���� --
    vFinalColor.rgb = saturate(vFinalColor.rgb);
    
    // ���� ��� ���
    Out.vDiffuse = float4(vFinalColor.rgb, fFinalAlpha);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_DEFFERED_BLOODPILLARB_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

    // -- 1. ���� ��� �ҿ뵹�� ���� ��� --
    float fHeightRatio = saturate(1.0f - In.vTexcoord.y);
    float fVortexIntensity = g_fVortexStrength * fHeightRatio * g_fHeightGradient;
    
    // -- 2. ����ǥ�� ��ȯ�� ���� �ҿ뵹�� ȿ�� --
    float2 vCenterUV = float2(0.5f, 0.5f);
    float2 vToCenter = In.vTexcoord - vCenterUV;
    
    float fDistance = length(vToCenter);
    float fAngle = atan2(vToCenter.y, vToCenter.x);
    
    float fVortexRotation = fVortexIntensity / max(fDistance, 0.1f) * g_fTime * g_fVortexSpeed;
    fAngle += fVortexRotation;
    
    float2 vVortexUV = vCenterUV + fDistance * float2(cos(fAngle), sin(fAngle));
    
    // -- 3. ���� ������ �ִϸ��̼� (���� ȿ�� �߰�) --
    float2 vNoiseUV1 = vVortexUV + float2(0.1f, g_fVerticalFlow * g_fViscosity) * g_fTime * 0.3f;
    float2 vNoiseUV2 = vVortexUV + float2(-0.15f, g_fVerticalFlow * 0.8f * g_fViscosity) * g_fTime * 0.5f;
    
    float2 vDetailUV1 = In.vTexcoord * 2.0f + float2(0.2f, g_fVerticalFlow * 1.5f) * g_fTime * 0.7f;
    float2 vDetailUV2 = In.vTexcoord * 1.5f + float2(-0.1f, g_fVerticalFlow * 1.2f) * g_fTime * 0.6f;
    
    // -- 4. ���� �ְ� ȿ�� --
    float2 vVortexDistortion = (g_OtherTextures[0].Sample(DefaultSampler, vNoiseUV1).rg * 2.f - 1.f) * g_fDistortionStrength;
    float2 vTurbulenceDistortion = (g_OtherTextures[0].Sample(DefaultSampler, vDetailUV1).rg * 2.f - 1.f) * g_fTurbulence * 0.02f;
    
    vVortexDistortion *= fHeightRatio * 1.5f;
    vTurbulenceDistortion *= (1.0f - fHeightRatio * 0.5f);
    
    float2 vFinalDistortedUV = In.vTexcoord + vVortexDistortion + vTurbulenceDistortion;
    
    // -- 5. ���� ħ�� ����ũ --
    float fErosionMask1 = g_OtherTextures[0].Sample(DefaultSampler, vNoiseUV2).r;
    float fErosionMask2 = g_OtherTextures[0].Sample(DefaultSampler, vDetailUV2).r;
    
    float fCombinedErosion = lerp(fErosionMask1, fErosionMask2, fHeightRatio);
    float fDynamicThreshold = g_fErosionThreshold + sin(g_fTime * 2.0f + fHeightRatio * 3.14159f) * 0.1f;
    
    clip(fCombinedErosion - fDynamicThreshold);
    
    // -- 6. ��ȭ�� �� �ؽ�ó ���ø� --
    // ���� ���� �ؽ�ó (������ �ؽ�ó ���)
    float2 vBloodFlowUV = vFinalDistortedUV + float2(sin(fAngle) * 0.1f, g_fVerticalFlow * 0.4f) * g_fTime;
    vector vBloodColor = g_DiffuseTextures[4].Sample(DefaultSampler, vBloodFlowUV);
    
    // ������ ���̾� (�߰� ���̰�)
    float2 vDetailFlowUV = vFinalDistortedUV * 0.5f + float2(cos(fAngle) * 0.05f, g_fVerticalFlow * 0.6f) * g_fTime;
    vector vDetailColor = g_DiffuseTextures[5].Sample(DefaultSampler, vDetailFlowUV);
    
    // -- 7. �� ���� ��ȭ ó�� --
    // ���̽� �÷��� �� ���ϰ� ����
    vector vFinalColor = vBloodColor;
    
    // ������ ä�� ��ȭ, �ٸ� ä�� ����
    vFinalColor.r = pow(vFinalColor.r, 0.7f) * 1.2f; // ������ ��ȭ
    vFinalColor.g *= 0.1f; // ��� ũ�� ����
    vFinalColor.b *= 0.15f; // �Ķ��� ũ�� ����
    
    // ������ ���� ���� (Multiply �������� ��Ӱ�)
    vFinalColor.rgb = vFinalColor.rgb * (vDetailColor.rgb * 0.8f + 0.2f);
    
    // ä�� ����
    float fLuminance = dot(vFinalColor.rgb, float3(0.299f, 0.587f, 0.114f));
    vFinalColor.rgb = lerp(float3(fLuminance, fLuminance, fLuminance), vFinalColor.rgb, g_fBloodSaturation);
    
    // ��� ����
    vFinalColor.rgb = saturate((vFinalColor.rgb - 0.5f) * g_fBloodContrast + 0.5f);
    
    // ��ü������ ��Ӱ� (���� ���� ����)
    vFinalColor.rgb *= g_fBloodDarkness;
    
    // -- 8. ������ ��� ���� ���� �߰� --
    float fBloodPattern = g_OtherTextures[1].Sample(DefaultSampler, vFinalDistortedUV * 3.0f).r;
    float fBloodSpots = g_OtherTextures[0].Sample(DefaultSampler, vFinalDistortedUV * 5.0f).g;
    
    // ���� ������ �̿��� �߰� ��ο� ����
    float fDarkSpots = saturate(1.0f - fBloodPattern * fBloodSpots);
    vFinalColor.rgb *= (0.5f + fDarkSpots * 0.5f);
    
    // -- 9. ��� �������� �ҿ뵹�� ȿ�� --
    float2 vRisingMaskUV = In.vTexcoord + float2(sin(g_fTime + fAngle) * 0.1f, g_fTime * 2.0f);
    float fRisingMask = g_OtherTextures[1].Sample(DefaultSampler, vRisingMaskUV).r;
    
    float fVortexCore = saturate(1.0f - fDistance * 2.0f);
    fRisingMask = saturate(fRisingMask + fVortexCore * 0.5f);
    
    // �ھ� �κ��� �� ���� ����������
    vFinalColor.r += fVortexCore * 0.3f;
    vFinalColor.rgb = saturate(vFinalColor.rgb);
    
    // -- 10. ��ȭ�� ������ ȿ�� (�����ڸ� ��) --
    vector vViewDir = normalize(g_vCamPosition - In.vWorldPos);
    float fFresnel = 1.f - saturate(dot(vViewDir.xyz, In.vNormal.xyz));
    fFresnel = pow(fFresnel, g_fFresnelPower);
    
    // ������ ���� �� ���� ����������
    vector vDynamicFresnelColor = g_vFresnelColor * (0.5f + fVortexIntensity * 0.3f);
    vFinalColor.rgb += vDynamicFresnelColor.rgb * fFresnel * 0.3f; // ������ ȿ�� ����
    
    // -- 11. ���� ȿ�� �߰� --
    // �ð��� ���� ������ �帧 ȿ��
    float fViscosityFlow = sin(g_fTime * 0.5f + In.vTexcoord.y * 10.0f) * 0.05f;
    vFinalColor.rgb *= (1.0f + fViscosityFlow * fHeightRatio);
    
    // -- 12. ���� ���� ��� --
    float fSoftEdge = saturate((fCombinedErosion - fDynamicThreshold) * 15.f);
    float fFinalAlpha = vBloodColor.a * fSoftEdge;
    
    // ���� �κ��� �� �������ϰ�
    fFinalAlpha = saturate(fFinalAlpha * (1.0f + fDarkSpots * 0.5f));
    fFinalAlpha *= (0.8f + fVortexIntensity * 0.2f);
    fFinalAlpha *= (1.f - g_fRatio);
    
    // -- 13. ���� ���� Ŭ���� --
    vFinalColor.rgb = saturate(vFinalColor.rgb);
    
    // ���� ��� ���
    Out.vDiffuse = float4(vFinalColor.rgb, fFinalAlpha);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_DEFFERED_BLOODPILLARC_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

    // -- 1. ���� ��� �ҿ뵹�� ���� ��� --
    float fHeightRatio = saturate(1.0f - In.vTexcoord.y);
    float fVortexIntensity = g_fVortexStrength * fHeightRatio * g_fHeightGradient;
    
    // -- 2. ����ǥ�� ��ȯ�� ���� �ҿ뵹�� ȿ�� --
    float2 vCenterUV = float2(0.5f, 0.5f);
    float2 vToCenter = In.vTexcoord - vCenterUV;
    
    float fDistance = length(vToCenter);
    float fAngle = atan2(vToCenter.y, vToCenter.x);
    
    float fVortexRotation = fVortexIntensity / max(fDistance, 0.1f) * g_fTime * g_fVortexSpeed;
    fAngle += fVortexRotation;
    
    float2 vVortexUV = vCenterUV + fDistance * float2(cos(fAngle), sin(fAngle));
    
    // -- 3. ���� ������ �ִϸ��̼� (���� ȿ�� �߰�) --
    float2 vNoiseUV1 = vVortexUV + float2(0.1f, g_fVerticalFlow * g_fViscosity) * g_fTime * 0.3f;
    float2 vNoiseUV2 = vVortexUV + float2(-0.15f, g_fVerticalFlow * 0.8f * g_fViscosity) * g_fTime * 0.5f;
    
    float2 vDetailUV1 = In.vTexcoord * 2.0f + float2(0.2f, g_fVerticalFlow * 1.5f) * g_fTime * 0.7f;
    float2 vDetailUV2 = In.vTexcoord * 1.5f + float2(-0.1f, g_fVerticalFlow * 1.2f) * g_fTime * 0.6f;
    
    // -- 4. ���� �ְ� ȿ�� --
    float2 vVortexDistortion = (g_OtherTextures[0].Sample(DefaultSampler, vNoiseUV1).rg * 2.f - 1.f) * g_fDistortionStrength;
    float2 vTurbulenceDistortion = (g_OtherTextures[0].Sample(DefaultSampler, vDetailUV1).rg * 2.f - 1.f) * g_fTurbulence * 0.02f;
    
    vVortexDistortion *= fHeightRatio * 1.5f;
    vTurbulenceDistortion *= (1.0f - fHeightRatio * 0.5f);
    
    float2 vFinalDistortedUV = In.vTexcoord + vVortexDistortion + vTurbulenceDistortion;
    
    // -- 5. ���� ħ�� ����ũ --
    float fErosionMask1 = g_OtherTextures[0].Sample(DefaultSampler, vNoiseUV2).r;
    float fErosionMask2 = g_OtherTextures[0].Sample(DefaultSampler, vDetailUV2).r;
    
    float fCombinedErosion = lerp(fErosionMask1, fErosionMask2, fHeightRatio);
    float fDynamicThreshold = g_fErosionThreshold + sin(g_fTime * 2.0f + fHeightRatio * 3.14159f) * 0.1f;
    
    clip(fCombinedErosion - fDynamicThreshold);
    
    // -- 6. ��ȭ�� �� �ؽ�ó ���ø� --
    // ���� ���� �ؽ�ó (������ �ؽ�ó ���)
    float2 vBloodFlowUV = vFinalDistortedUV + float2(sin(fAngle) * 0.1f, g_fVerticalFlow * 0.4f) * g_fTime;
    vector vBloodColor = g_DiffuseTextures[4].Sample(DefaultSampler, vBloodFlowUV);
    
    // ������ ���̾� (�߰� ���̰�)
    float2 vDetailFlowUV = vFinalDistortedUV * 0.5f + float2(cos(fAngle) * 0.05f, g_fVerticalFlow * 0.6f) * g_fTime;
    vector vDetailColor = g_DiffuseTextures[5].Sample(DefaultSampler, vDetailFlowUV);
    
    // -- 7. �� ���� ��ȭ ó�� --
    // ���̽� �÷��� �� ���ϰ� ����
    vector vFinalColor = vBloodColor;
    
    // ������ ä�� ��ȭ, �ٸ� ä�� ����
    vFinalColor.r = pow(vFinalColor.r, 0.7f) * 1.2f; // ������ ��ȭ
    vFinalColor.g *= 0.1f; // ��� ũ�� ����
    vFinalColor.b *= 0.15f; // �Ķ��� ũ�� ����
    
    // ������ ���� ���� (Multiply �������� ��Ӱ�)
    vFinalColor.rgb = vFinalColor.rgb * (vDetailColor.rgb * 0.8f + 0.2f);
    
    // ä�� ����
    float fLuminance = dot(vFinalColor.rgb, float3(0.299f, 0.587f, 0.114f));
    vFinalColor.rgb = lerp(float3(fLuminance, fLuminance, fLuminance), vFinalColor.rgb, g_fBloodSaturation);
    
    // ��� ����
    vFinalColor.rgb = saturate((vFinalColor.rgb - 0.5f) * g_fBloodContrast + 0.5f);
    
    // ��ü������ ��Ӱ� (���� ���� ����)
    vFinalColor.rgb *= g_fBloodDarkness;
    
    // -- 8. ������ ��� ���� ���� �߰� --
    float fBloodPattern = g_OtherTextures[1].Sample(DefaultSampler, vFinalDistortedUV * 3.0f).r;
    float fBloodSpots = g_OtherTextures[0].Sample(DefaultSampler, vFinalDistortedUV * 5.0f).g;
    
    // ���� ������ �̿��� �߰� ��ο� ����
    float fDarkSpots = saturate(1.0f - fBloodPattern * fBloodSpots);
    vFinalColor.rgb *= (0.5f + fDarkSpots * 0.5f);
    
    // -- 9. ��� �������� �ҿ뵹�� ȿ�� --
    float2 vRisingMaskUV = In.vTexcoord + float2(sin(g_fTime + fAngle) * 0.1f, g_fTime * 2.0f);
    float fRisingMask = g_OtherTextures[1].Sample(DefaultSampler, vRisingMaskUV).r;
    
    float fVortexCore = saturate(1.0f - fDistance * 2.0f);
    fRisingMask = saturate(fRisingMask + fVortexCore * 0.5f);
    
    // �ھ� �κ��� �� ���� ����������
    vFinalColor.r += fVortexCore * 0.3f;
    vFinalColor.rgb = saturate(vFinalColor.rgb);
    
    // -- 10. ��ȭ�� ������ ȿ�� (�����ڸ� ��) --
    vector vViewDir = normalize(g_vCamPosition - In.vWorldPos);
    float fFresnel = 1.f - saturate(dot(vViewDir.xyz, In.vNormal.xyz));
    fFresnel = pow(fFresnel, g_fFresnelPower);
    
    // ������ ���� �� ���� ����������
    vector vDynamicFresnelColor = g_vFresnelColor * (0.5f + fVortexIntensity * 0.3f);
    vFinalColor.rgb += vDynamicFresnelColor.rgb * fFresnel * 0.3f; // ������ ȿ�� ����
    
    // -- 11. ���� ȿ�� �߰� --
    // �ð��� ���� ������ �帧 ȿ��
    float fViscosityFlow = sin(g_fTime * 0.5f + In.vTexcoord.y * 10.0f) * 0.05f;
    vFinalColor.rgb *= (1.0f + fViscosityFlow * fHeightRatio);
    
    // -- 12. ���� ���� ��� --
    float fSoftEdge = saturate((fCombinedErosion - fDynamicThreshold) * 15.f);
    float fFinalAlpha = vBloodColor.a * fSoftEdge;
    
    // ���� �κ��� �� �������ϰ�
    fFinalAlpha = saturate(fFinalAlpha * (1.0f + fDarkSpots * 0.5f));
    fFinalAlpha *= (0.8f + fVortexIntensity * 0.2f);
    fFinalAlpha *= (1.f - g_fRatio);
    
    // -- 13. ���� ���� Ŭ���� --
    vFinalColor.rgb = saturate(vFinalColor.rgb);
    
    // ���� ��� ���
    Out.vDiffuse = float4(vFinalColor.rgb, fFinalAlpha);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}






PS_OUT_BACKBUFFER PS_SWORDWIND_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

    float4 vMtrlDiffuse = g_DiffuseTextures[6].Sample(DefaultSampler, In.vTexcoord);
    float4 vMtrlNoise = g_NoiseTextures[4].Sample(DefaultSampler, In.vTexcoord);

    // ===== ���� ��� =====
    //Out.vDiffuse = vMtrlDiffuse;
    Out.vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
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

    // ===== 1. UV ��ũ�Ѹ� (ȸ�� ��� �̵�) =====
    // ��� UV������ ��ũ�Ѹ��� �� �ڿ�������
    float2 scrolledUV = In.vTexcoord;
    scrolledUV.x += g_fTime * 5.f; // ���� ��ũ��
    scrolledUV = frac(scrolledUV); // 0~1 ������ ����

    // ===== 2. �ؽ�ó ���ø� =====
    vector vDiffuseColor = g_DiffuseTexture.Sample(DefaultSampler, scrolledUV);
    vDiffuseColor.r *= 0.8f; // ���� ����
    vDiffuseColor.g *= 0.05f; // �ణ�� ���
    vDiffuseColor.b *= 0.0f; // �Ķ� ����

    // ===== 3. UV ��� �ܰ��� (���Ʒ� ���) =====
    float edgeMask = 0.0f;
    float edgeThickness = 0.05f; // �ܰ��� �β�
    
    // ���� �����ڸ� (V���� 1�� �����)
    if (In.vTexcoord.y > 1.0f - edgeThickness)
    {
        edgeMask = (In.vTexcoord.y - (1.0f - edgeThickness)) / edgeThickness;
    }
    
    // �Ʒ��� �����ڸ� (V���� 0�� �����)
    if (In.vTexcoord.y < edgeThickness)
    {
        edgeMask = max(edgeMask, 1.0f - (In.vTexcoord.y / edgeThickness));
    }

    // ===== 4. ���� ó�� =====
    vector finalColor = vDiffuseColor;
    
    // ������ ��ȭ
    finalColor.r = pow(finalColor.r, 0.7f) * 1.2f;
    finalColor.g *= 0.2f;
    finalColor.b *= 0.1f;
    
    // �ܰ����� ����������
    finalColor.rgb = lerp(finalColor.rgb, float3(0.0f, 0.0f, 0.0f), edgeMask);

    // ===== 5. ���� �߰� (���û���) =====
    // ���� �ٹ��� ȿ���� ȸ�� ���� ����
    float pattern = sin(In.vTexcoord.x * 20.0f - g_fTime * 5.0f) * 0.2f + 0.8f;
    finalColor.rgb *= pattern;

    // ===== 6. ���� ó�� =====
    float fadeAlpha = vDiffuseColor.a;
    fadeAlpha *= saturate(g_fGrowTime * 3.0f);
    fadeAlpha *= (1.0f - g_fDissolveTime);
    
    

    // ===== ��� =====
    Out.vDiffuse = float4(saturate(finalColor.rgb), saturate(fadeAlpha));
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);

    return Out;
}




PS_OUT_BACKBUFFER PS_BLOOD_BODYAURA_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    // ===== 1. ����(Alpha) ���: ������ '����' ���� =====
    
    // 1-1. �밢�� �������� ��ũ�ѵǴ� UV ���� (ȸ�� �ӵ� ����)
    float2 animatedUV = In.vTexcoord;
    animatedUV.x += g_fTime * 0.7f; // X�� �̵� (ȸ��) - �ӵ� ����
    animatedUV.y -= g_fTime * 1.2f; // Y�� �̵� (���) - �ӵ� ����
    
    // 1-2. ���� ������� �� ������ ������ ����
    float noiseMask1 = g_NoiseTextures[5].Sample(DefaultSampler, frac(animatedUV)).r;
    float noiseMask2 = g_NoiseTextures[5].Sample(DefaultSampler, frac(animatedUV * 2.0f)).r;
    
    // �� ����� ���ؼ� �� ��ī�ο� ���� ����
    float noiseMask = noiseMask1 * noiseMask2;
    noiseMask = pow(noiseMask, 0.5f); // ���� �������� �߰��� ��ȭ
    noiseMask = smoothstep(0.2f, 0.4f, noiseMask); // �� Ÿ��Ʈ�� ���
    
    // 1-3. ������ ���� ����ũ (����̵��� ȸ�� ����)
    float waveFrequency = 6.0f; // ���� ���� ����
    float waveSpeed = 4.0f; // ȸ�� �ӵ� ����
    float spiralOffset = In.vTexcoord.x * 3.14159f; // X ��ǥ�� ���� ���� ����
    
    // ������ ���� ����
    float wave = sin(In.vTexcoord.y * waveFrequency + g_fTime * waveSpeed + spiralOffset);
    // �� ������ ��Ʈ�������� ���� pow ���
    float waveMask = saturate(pow(abs(wave), 0.3f));
    
    // 1-4. ���� ����ũ ���� (����̵� ���� ��ȭ)
    float heightMask = 1.0f;
    
    // �Ʒ����� �а�, ������ ���� (����̵� ����)
    float widthScale = lerp(1.2f, 0.3f, In.vTexcoord.y);
    float centerDist = abs(In.vTexcoord.x - 0.5f) * 2.0f;
    float radialMask = 1.0f - smoothstep(0.0f, widthScale, centerDist);
    
    // ���� ���̵�
    heightMask *= smoothstep(0.0f, 0.15f, In.vTexcoord.y);
    heightMask *= (1.0f - smoothstep(0.85f, 1.0f, In.vTexcoord.y));
    
    // 1-5. ���� ������ ���� �߰� ����ũ
    float edgeEnhance = 1.0f - smoothstep(0.6f, 0.9f, noiseMask);
    edgeEnhance = saturate(edgeEnhance + 0.3f); // �ּҰ� ����
    
    // ���� ���� ��� (�� ����� �⿩�� ����)
    float finalAlpha = noiseMask * waveMask * heightMask * radialMask * edgeEnhance;
    
    // ���� �� ��ȭ (�� ���ϰ�)
    finalAlpha = pow(finalAlpha, 0.7f); // ���� �������� ��ü������ ���
    finalAlpha = saturate(finalAlpha * 1.5f); // ���� ����
    
    finalAlpha *= (1.0f - g_fDissolveTime); // Dissolve ȿ��
    
    // �� ������ �ƿ���
    clip(finalAlpha - 0.05f);
    
    // ===== 2. ����(Color) ��� =====
    vector baseColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // ���� �κ� ���� ���� (������)
    float edgeGlow = 1.0f + (1.0f - noiseMask) * 0.5f;
    vector finalColor = baseColor * edgeGlow;
    
    // ===== ���� ��� =====
    Out.vDiffuse = float4(saturate(finalColor.rgb), saturate(finalAlpha));
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    return Out;
}


technique11 DefaultTechnique
{
    /* Ư�� �н��� �̿��ؼ� ������ �׷��´�. */
    /* �ϳ��� ���� �׷��´�. */ 
    /* ���� ��Ȳ�� ���� �ٸ� ���̵� ��� ��Ʈ(��� + ������Ʈ + ����ŧ�� + ��ָ� + ssao )�� �Կ��ֱ����ؼ� */
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

