#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vBrushPos = vector(30.f, 0.f, 20.f, 1.f);
float g_fRange = 5.f;

/*����*/
texture2D g_DiffuseTexture[2];
texture2D g_MaskTexture;
texture2D g_BrushTexture;
vector g_vMtrlAmbient = 1.f;
vector g_vMtrlSpecular = 1.f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal   : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
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
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix); // ������ �븻 ���͸� World ����� �����ش�.
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

/* /W�� �����Ѵ�. ���������̽��� ��ȯ */
/* ����Ʈ�� ��ȯ�ϰ�.*/
/* �����Ͷ����� : �ȼ��� �����. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    
};

struct PS_DEFFERD_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

/* ���� �ȼ� ������ ���ؼ� �ȼ� ���̴��� �����Ѵ�. */
/* �ȼ��� ���� �����Ѵ�. */



PS_DEFFERD_OUT PS_MAIN(PS_IN In)
{
    PS_DEFFERD_OUT Out = (PS_DEFFERD_OUT) 0;
    
    vector vSourDiffuse = g_DiffuseTexture[0].Sample(DefaultSampler, In.vTexcoord * 50.f);
    vector vDestDiffuse = g_DiffuseTexture[1].Sample(DefaultSampler, In.vTexcoord * 50.f);
    
    vector vMask = g_MaskTexture.Sample(PointSampler, In.vTexcoord);
    vector vBrush = 0.f;
    
    if (g_vBrushPos.x - g_fRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fRange &&
        g_vBrushPos.z - g_fRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fRange)
    {
        float2 vTexcoord;
        
        vTexcoord.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fRange)) / (2.f * g_fRange);
        vTexcoord.y = ((g_vBrushPos.z + g_fRange) - In.vWorldPos.z) / (2.f * g_fRange);
        
        vBrush = g_BrushTexture.Sample(DefaultSampler, vTexcoord);
    }
    
    vector vMtrlDiffuse = vDestDiffuse * vMask.r + vSourDiffuse * (1.f - vMask.r) + vBrush;
    
    Out.vDiffuse = vMtrlDiffuse;
    
    /* In.vNormal(-1 ~ 1) -> Out.vNormal(0 ~ 1) */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    // vector      vNormalDesc = g_NormalTexture.Sample();
    // vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    return Out;
}

//PS_DEFFERD_OUT PS_DEFFERED_MAIN(PS_IN In)
//{
//    PS_DEFFERD_OUT Out = (PS_DEFFERD_OUT) 0;
    
//    vector vSourDiffuse = g_DiffuseTexture[0].Sample(DefaultSampler, In.vTexcoord * 50.f);
//    vector vDestDiffuse = g_DiffuseTexture[1].Sample(DefaultSampler, In.vTexcoord * 50.f);
    
//    vector vMask = g_MaskTexture.Sample(PointSampler, In.vTexcoord);
//    vector vBrush = 0.f;
    
//    if (g_vBrushPos.x - g_fRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fRange &&
//        g_vBrushPos.z - g_fRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fRange)
//    {
//        float2 vTexcoord;
        
//        vTexcoord.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fRange)) / (2.f * g_fRange);
//        vTexcoord.y = ((g_vBrushPos.z + g_fRange) - In.vWorldPos.z) / (2.f * g_fRange);
        
//        vBrush = g_BrushTexture.Sample(DefaultSampler, vTexcoord);
//    }
    
//    vector vMtrlDiffuse = vDestDiffuse * vMask.r + vSourDiffuse * (1.f - vMask.r) + vBrush;
    
//    Out.vDiffuse = vMtrlDiffuse;
    
//    /* In.vNormal(-1 ~ 1) -> Out.vNormal(0 ~ 1) */
//    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
//    // vector      vNormalDesc = g_NormalTexture.Sample();
//    // vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);    
//    return Out;
//}

technique11 DefaultTechnique
{
    /* Ư�� �н��� �̿��ؼ� ������ �׷��´�. */
    /* �ϳ��� ���� �׷��´�. */ 
    /* ���� ��Ȳ�� ���� �ٸ� ���̵� ��� ��Ʈ(��� + ������Ʈ + ����ŧ�� + ��ָ� + ssao )�� �Կ��ֱ����ؼ� */
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);

        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    ///* ���� ��Ȳ�� ���� �ٸ� ���̵� ��� ��Ʈ(���� + �����  )�� �Կ��ֱ����ؼ� */
    //pass DefaultPass1
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN1();

    //}

    ///* ������ ������ ���� ���̴� ������ �ۼ��Ѵ�. */
    ///* ������ ������ ������ ������ �ٸ� ����� �ϴ� ����ü�� ���� �׸��� ��� -> ��������Ŀ� ���̰� ���� �� �ִ�. */ 
    //pass DefaultPass1
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN1();

    //}

}
