#include "Engine_Shader_Defines.hlsli"
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

/*����*/
texture2D g_DiffuseTexture;
texture2D g_DiffuseTextures[6];
texture2D g_OtherTextures[12];

texture2D g_NormalTexture;
texture2D g_NoiseTexture;

/*����*/
texture2D g_DissolveTexture;

vector g_vMtrlAmbient = 1.f;
vector g_vMtrlSpecular = 1.f;


// �ð�.
float g_fDissolveTime;
float g_fGrowTime;


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


//float g_fErosionThreshold = 0.4f;
//float g_fDistortionStrength = 0.03f;
//float g_fFresnelPower = 5.0f;
//vector g_vFresnelColor = float4(1.f, 0.2f, 0.2f, 1.f);
//float g_fCrackIntensity = 2.5f;
//float g_fCrackSpeed = -4.0f;

//float g_fVortexStrength = 2.0f; // �ҿ뵹�� ����
//float g_fVortexSpeed = 1.5f; // �ҿ뵹�� ȸ�� �ӵ�
//float g_fHeightGradient = 1.0f; // ���̺� �ҿ뵹�� ���� ����
//float g_fTurbulence = 0.5f; // ���� ȿ�� ����
//float g_fVerticalFlow = 2.0f; // ���� �帧 �ӵ�

//PS_OUT_BACKBUFFER PS_DEFFERED_BLOODPILLARA_MAIN(PS_BACKBUFFER_IN In)
//{
//    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;

//    // -- 1. ���� ��� �ҿ뵹�� ���� ��� --
//    // Y ��ǥ ������� �ҿ뵹�� ������ ���� (�Ʒ����� �� ����)
//    float fHeightRatio = saturate(1.0f - In.vTexcoord.y); // �Ʒ����� ���� ������ ������
//    float fVortexIntensity = g_fVortexStrength * fHeightRatio * g_fHeightGradient;
    
//    // -- 2. ����ǥ�� ��ȯ�� ���� �ҿ뵹�� ȿ�� --
//    float2 vCenterUV = float2(0.5f, 0.5f);
//    float2 vToCenter = In.vTexcoord - vCenterUV;
    
//    // �Ÿ��� ���� ���
//    float fDistance = length(vToCenter);
//    float fAngle = atan2(vToCenter.y, vToCenter.x);
    
//    // �ҿ뵹�� ȸ�� ȿ�� (�Ÿ��� �������� �� ������ ȸ��)
//    float fVortexRotation = fVortexIntensity / max(fDistance, 0.1f) * g_fTime * g_fVortexSpeed;
//    fAngle += fVortexRotation;
    
//    // ����ǥ���� ������ǥ�� �ٽ� ��ȯ
//    float2 vVortexUV = vCenterUV + fDistance * float2(cos(fAngle), sin(fAngle));
    
//    // -- 3. ���� ������ �ִϸ��̼� --
//    // ù ��° ������ ���̾� (ū ����)
//    float2 vNoiseUV1 = vVortexUV + float2(0.1f, g_fVerticalFlow) * g_fTime * 0.3f;
//    float2 vNoiseUV2 = vVortexUV + float2(-0.15f, g_fVerticalFlow * 0.8f) * g_fTime * 0.5f;
    
//    // �� ��° ������ ���̾� (������ ����)
//    float2 vDetailUV1 = In.vTexcoord * 2.0f + float2(0.2f, g_fVerticalFlow * 1.5f) * g_fTime * 0.7f;
//    float2 vDetailUV2 = In.vTexcoord * 1.5f + float2(-0.1f, g_fVerticalFlow * 1.2f) * g_fTime * 0.6f;
    
//    // -- 4. ���� �ְ� ȿ�� --
//    // �⺻ �ҿ뵹�� �ְ�
//    float2 vVortexDistortion = (g_OtherTextures[0].Sample(DefaultSampler, vNoiseUV1).rg * 2.f - 1.f) * g_fDistortionStrength;
    
//    // ������ ���� �ְ�
//    float2 vTurbulenceDistortion = (g_OtherTextures[0].Sample(DefaultSampler, vDetailUV1).rg * 2.f - 1.f) * g_fTurbulence * 0.02f;
    
//    // ���̺� �ٸ� �ְ� ����
//    vVortexDistortion *= fHeightRatio * 1.5f;
//    vTurbulenceDistortion *= (1.0f - fHeightRatio * 0.5f);
    
//    // ���� �ְ�� UV
//    float2 vFinalDistortedUV = In.vTexcoord + vVortexDistortion + vTurbulenceDistortion;
    
//    // -- 5. ���� ħ�� ����ũ --
//    // ������ ħ�� ������ ����
//    float fErosionMask1 = g_OtherTextures[0].Sample(DefaultSampler, vNoiseUV2).r;
//    float fErosionMask2 = g_OtherTextures[0].Sample(DefaultSampler, vDetailUV2).r;
    
//    // ���̺��� �ٸ� ħ�� ����
//    float fCombinedErosion = lerp(fErosionMask1, fErosionMask2, fHeightRatio);
    
//    // ���� �Ӱ谪 (�ð��� ���̿� ���� ��ȭ)
//    float fDynamicThreshold = g_fErosionThreshold + sin(g_fTime * 2.0f + fHeightRatio * 3.14159f) * 0.1f;
    
//    clip(fCombinedErosion - fDynamicThreshold);
    
//    // -- 6. ���� ���� ���ø� --
//    // ���� ���� �ؽ�ó (�ҿ뵹�� ȿ�� ����)
//    float2 vBloodFlowUV = vFinalDistortedUV + float2(sin(fAngle) * 0.1f, g_fVerticalFlow * 0.4f) * g_fTime;
//    vector vBloodColor = g_DiffuseTextures[4].Sample(DefaultSampler, vBloodFlowUV);
    
//    // �߰� ������ ���̾�
//    float2 vDetailFlowUV = vFinalDistortedUV * 0.5f + float2(cos(fAngle) * 0.05f, g_fVerticalFlow * 0.6f) * g_fTime;
//    vector vDetailColor = g_DiffuseTextures[5].Sample(DefaultSampler, vDetailFlowUV);
    
//    // -- 7. ��� �������� �ҿ뵹�� ȿ�� ���� --
//    float2 vRisingMaskUV = In.vTexcoord + float2(sin(g_fTime + fAngle) * 0.1f, g_fTime * 2.0f);
//    float fRisingMask = g_OtherTextures[1].Sample(DefaultSampler, vRisingMaskUV).r;
    
//    // �ҿ뵹�� �߽ɺ� ����
//    float fVortexCore = saturate(1.0f - fDistance * 2.0f);
//    fRisingMask = saturate(fRisingMask + fVortexCore * 0.5f);
    
//    // -- 8. ���� ������ ȿ�� --
//    vector vViewDir = normalize(g_vCamPosition - In.vWorldPos);
//    float fFresnel = 1.f - saturate(dot(vViewDir.xyz, In.vNormal.xyz));
//    fFresnel = pow(fFresnel, g_fFresnelPower);
    
//    // �ҿ뵹�� ������ ���� ������ ���� ��ȭ
//    vector vDynamicFresnelColor = lerp(g_vFresnelColor, g_vFresnelColor * 1.5f, fVortexIntensity * 0.3f);
//    vector vFresnelEffect = vDynamicFresnelColor * fFresnel;
    
//    // -- 9. ���� ���� ���� --
//    // ���̽� �÷��� ������ ȥ��
//    vector vFinalColor = lerp(vBloodColor, vDetailColor, 0.3f);
    
//    // ��� ������ ȿ�� ����
//    vFinalColor.rgb += vFinalColor.rgb * fRisingMask * 2.0f;
    
//    // �ҿ뵹�� �ھ� ��� ����
//    vFinalColor.rgb += vFinalColor.rgb * fVortexCore * 1.5f;
    
//    // ������ ȿ�� �߰�
//    vFinalColor.rgb += vFresnelEffect.rgb;
    
//    // ���̺� ���� ���� ����
//    vFinalColor.rgb *= (0.7f + fHeightRatio * 0.3f);
    
//    // -- 10. ���� ���� ��� --
//    float fSoftEdge = saturate((fCombinedErosion - fDynamicThreshold) * 15.f);
//    float fFinalAlpha = vBloodColor.a * fSoftEdge;
    
//    // �ҿ뵹�� ȿ���� ���� ���� ��ȭ
//    fFinalAlpha *= (0.8f + fVortexIntensity * 0.2f);
    
//    // �ð��� ���� ��ü ���̵�
//    fFinalAlpha *= (1.f - g_fRatio);
    
//    // ���� ��� ���
//    Out.vDiffuse = float4(vFinalColor.rgb, fFinalAlpha);
//    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
//    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
//    return Out;
//}


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

    pass ShadowPass // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        //PixelShader = compile ps_5_0 PS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }


}
