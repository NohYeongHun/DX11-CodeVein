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

texture2D g_NormalTexture;
texture2D g_NoiseTexture;

/*����*/
texture2D g_DissolveTexture;

vector g_vMtrlAmbient = 1.f;
vector g_vMtrlSpecular = 1.f;


// �ð�.
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


/* /W�� �����Ѵ�. ���������̽��� ��ȯ */
/* ����Ʈ�� ��ȯ�ϰ�.*/
/* �����Ͷ����� : �ȼ��� �����. */

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


PS_OUT_BACKBUFFER PS_DEFFERED_BLOODPILLARA_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    // [�߰�] 1. �ð��� ���� �帣�� UV ��ǥ ���
    // Diffuse �ؽ�ó�� UV. V(����) �������θ� �帣���� ����.
    float2 vDiffusePannedUV = In.vTexcoord + float2(0.f, g_fScrollSpeedV) * g_fTime;
    
    // Noise �ؽ�ó�� UV. Diffuse�ʹ� �ٸ� �ӵ��� �帣�� �Ͽ� ���̰��� �ݴϴ�.
    float2 vNoisePannedUV = In.vTexcoord + float2(g_fScrollSpeedU, g_fScrollSpeedV * 0.5f) * g_fTime;
    
    // [����] 2. ���� UV�� �ؽ�ó ���ø�
    // �⺻�� Noise
    vector vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vNoisePannedUV); // ����: vNoisePannedUV ���
    vector vMtrlDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord); // ������� ����
    
    vector vDarkColor = vector(0.5f, 0.f, 0.f, 1.f); // ��ο� �� ����
    vector vMtrlDiffuse = g_DiffuseTextures[5].Sample(DefaultSampler, vDiffusePannedUV); // ����: vDiffusePannedUV ���
    
    
    if (vMtrlNoise.a < 0.3f)
        discard;
    
   
    // ������ ��(vMtrlNoise.r)�� ���� �� ������ �����մϴ�.
    //Out.vDiffuse.rgb = lerp(vDarkColor.rgb, vMtrlDiffuse.rgb, vMtrlNoise.r);
    Out.vDiffuse.rgb = vMtrlDiffuse.rgb;
    
    float fAlpha = saturate(1.f - g_fRatio);
    Out.vDiffuse.a = vMtrlDiffuse.a * fAlpha; // �ð��� ���� ���İ� ����.
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    // ���� ���� �ð��� ���� ����?
    
    // �ȿ� ���ڰ� 0�̵Ǹ� �ȱ׸���.
    clip(vMtrlDissolve.r - g_fDissolveTime);
    
    return Out;
}




PS_OUT_BACKBUFFER PS_DEFFERED_BLOODPILLARB_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    // [�߰�] 1. �ð��� ���� �帣�� UV ��ǥ ���
    // Diffuse �ؽ�ó�� UV. V(����) �������θ� �帣���� ����.
    float2 vDiffusePannedUV = In.vTexcoord + float2(0.f, g_fScrollSpeedV) * g_fTime;
    
    // Noise �ؽ�ó�� UV. Diffuse�ʹ� �ٸ� �ӵ��� �帣�� �Ͽ� ���̰��� �ݴϴ�.
    float2 vNoisePannedUV = In.vTexcoord + float2(g_fScrollSpeedU, g_fScrollSpeedV * 0.5f) * g_fTime;
    
    // [����] 2. ���� UV�� �ؽ�ó ���ø�
    // �⺻�� Noise
    vector vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vNoisePannedUV); // ����: vNoisePannedUV ���
    vector vMtrlDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord); // ������� ����
    
    vector vDarkColor = vector(0.5f, 0.f, 0.f, 1.f); // ��ο� �� ����
    vector vMtrlDiffuse = g_DiffuseTextures[5].Sample(DefaultSampler, vDiffusePannedUV); // ����: vDiffusePannedUV ���
    
    
    if (vMtrlNoise.a < 0.3f)
        discard;
    
    
    // ������ ��(vMtrlNoise.r)�� ���� �� ������ �����մϴ�.
    //Out.vDiffuse.rgb = lerp(vDarkColor.rgb, vMtrlDiffuse.rgb, vMtrlNoise.r);
    Out.vDiffuse.rgb = vMtrlDiffuse.rgb;
    
    float fAlpha = saturate(1.f - g_fRatio);
    Out.vDiffuse.a = vMtrlDiffuse.a * fAlpha; // �ð��� ���� ���İ� ����.
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    // �ȿ� ���ڰ� 0�̵Ǹ� �ȱ׸���.
    clip(vMtrlDissolve.r - g_fDissolveTime);
    
    return Out;
}

PS_OUT_BACKBUFFER PS_DEFFERED_BLOODPILLARC_MAIN(PS_BACKBUFFER_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    // [�߰�] 1. �ð��� ���� �帣�� UV ��ǥ ���
    // Diffuse �ؽ�ó�� UV. V(����) �������θ� �帣���� ����.
    float2 vDiffusePannedUV = In.vTexcoord + float2(0.f, g_fScrollSpeedV) * g_fTime;
    
    // Noise �ؽ�ó�� UV. Diffuse�ʹ� �ٸ� �ӵ��� �帣�� �Ͽ� ���̰��� �ݴϴ�.
    float2 vNoisePannedUV = In.vTexcoord + float2(g_fScrollSpeedU, g_fScrollSpeedV * 0.5f) * g_fTime;
    
    // [����] 2. ���� UV�� �ؽ�ó ���ø�
    // �⺻�� Noise
    vector vMtrlNoise = g_NoiseTexture.Sample(DefaultSampler, vNoisePannedUV); // ����: vNoisePannedUV ���
    vector vMtrlDissolve = g_DissolveTexture.Sample(DefaultSampler, In.vTexcoord); // ������� ����
    
    vector vDarkColor = vector(0.5f, 0.f, 0.f, 1.f); // ��ο� �� ����
    vector vMtrlDiffuse = g_DiffuseTextures[5].Sample(DefaultSampler, vDiffusePannedUV); // ����: vDiffusePannedUV ���
    
    
    if (vMtrlNoise.a < 0.3f)
        discard;
    
    
    // ������ ��(vMtrlNoise.r)�� ���� �� ������ �����մϴ�.
    //Out.vDiffuse.rgb = lerp(vDarkColor.rgb, vMtrlDiffuse.rgb, vMtrlNoise.r);
    Out.vDiffuse.rgb = vMtrlDiffuse.rgb;
    
    float fAlpha = saturate(1.f - g_fRatio);
    Out.vDiffuse.a = vMtrlDiffuse.a * fAlpha; // �ð��� ���� ���İ� ����.
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    // �ȿ� ���ڰ� 0�̵Ǹ� �ȱ׸���.
    clip(vMtrlDissolve.r - g_fDissolveTime);
    
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


}
