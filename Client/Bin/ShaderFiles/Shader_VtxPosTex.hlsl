#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;


float g_fFillRatio;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;    
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

/* �������̴� : ���� ��ġ�� �����̽� ��ȯ(���� -> ���� -> �� -> ����). */ 
/*          : ������ ������ ����.(in:3��, out:2�� or 5��) */
/*          : ���� ����(���� �ϳ��� VS_MAIN�ѹ�ȣ��) */ 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;    
    
    /* ������ ������ġ * ���� * �� * ���� */ 
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);    
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    return Out;     
}

/* /W�� �����Ѵ�. ���������̽��� ��ȯ */
/* ����Ʈ�� ��ȯ�ϰ�.*/
/* �����Ͷ����� : �ȼ��� �����. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;    
    float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* ���� �ȼ� ������ ���ؼ� �ȼ� ���̴��� �����Ѵ�. */
/* �ȼ��� ���� �����Ѵ�. */



PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    //Out.vColor.rgb = In.vTexcoord.y;
    
    return Out;    
}


// Texture Index�� ���� �ٸ� �ȼ� ���̴��� �����ϱ�.
PS_OUT PS_MAIN2(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 uv = In.vTexcoord;
    
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv); // ���� �ؽ�ó ��
    float4 fillerColor = float4(1, 1, 1, 1); // ���̾Ƹ�� �ȿ� ä���� ��
    
    // ���̾Ƹ�� �߽�
    float2 center = float2(0.5f, 0.5f);
    float2 delta = abs(uv - center);
    bool bIsInDiamond = (delta.x + delta.y) < 0.5f;
    
    if (bIsInDiamond)
    {
        Out.vColor = lerp(baseColor, fillerColor, 0.8f); // �ε巴�� ����
    }
    else
    {
        Out.vColor = baseColor;
    }
    
    return Out;
}

PS_OUT PS_MAIN3(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 uv = In.vTexcoord;
    float4 fillerColor = float4(0, 0, 0, 1); // ���̾Ƹ�� �ȿ� ä���� ��
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv); // ���� �ؽ�ó ��

    
    // ���̾Ƹ�� �߽�
    float2 center = float2(0.5f, 0.5f); 
    float2 delta = abs(uv - center);
    bool bIsInDiamond = (delta.x + delta.y) < 0.51f;

    // �Ʒ��� ���� ä���
    bool bIsFillRegion = uv.y > (1.0 - g_fFillRatio); 

    if (bIsInDiamond && bIsFillRegion)
    {
        // ���̾Ƹ�� �����̸鼭, ä���� �����̸�
        Out.vColor = lerp(baseColor, fillerColor, 0.8f); // �ε巴�� ����
    }
    else
    {
        // �� �ܴ� ���� �ؽ�ó ����
        Out.vColor = baseColor;
    }

    return Out;
}

float g_fFade;

// Fade Out Shader
PS_OUT PS_MAIN4(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 uv = In.vTexcoord;
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv);
    float4 fillerColor = float4(0, 0, 0, 1);
   
    // Alpha Blend
    baseColor.rgb = lerp(baseColor.rgb, float3(0.0, 0.0, 0.0), saturate(g_fFade));
    
    // �ٽ� �� ��: ������������� ���� ����
    Out.vColor = baseColor;
    

    return Out;
}

float g_fAlpha;

// Logo Alpha Light ����
PS_OUT PS_MAIN5(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor.a = g_fAlpha;
    
    return Out;
}

float g_fRightRatio; 
float g_fLeftRatio;
bool g_bIncrease;

// HP Bar Progress �뵵.
PS_OUT PS_MAIN6(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 uv = In.vTexcoord;
    float4 fillerColor = float4(0.5, 0.5, 0.5, 1); // �پ�� ü�¿� ä���� ȸ��.
    float4 fillerBlack = float4(0, 0, 0, 1); // �پ�� ü�¿� ä���� ȸ��.
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv); // ���� �ؽ�ó ��

    bool bIsFillGray = uv.x < g_fRightRatio && uv.x > g_fLeftRatio;
    bool bIsFill; 
    
    if (g_bIncrease)
        bIsFill = uv.x > g_fLeftRatio; // uv.x�� g_fLeftRatio���� ũ�ٸ�?
    else
        bIsFill = uv.x > (1.0 - g_fFillRatio); // �̰� ������. => �̸� �����ع����� �Ⱥ���.
       
    
    
    if (bIsFill)
    {
        // �����ؾ��Ѵٸ�?
        if (bIsFillGray)
        {
            Out.vColor = lerp(baseColor, fillerColor, 0.8f); // �ε巴�� ����  
        }
        else
            Out.vColor = fillerBlack; // ���� ����.
    }
    else
    {
        // �� �ܴ� ���� �ؽ�ó ����
        Out.vColor = baseColor;
    }

    return Out;
}

PS_OUT PS_MAIN7(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = float4(0.f, 0.f, 0.f, 0.9f);
    //Out.vColor.rgb = In.vTexcoord.y;
    
    return Out;
}

PS_OUT PS_MAIN8(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 uv = In.vTexcoord;
    float4 baseColor = g_Texture.Sample(DefaultSampler, uv); // ���� �ؽ�ó ��
    
    Out.vColor = baseColor;
    
    //Out.vColor.rgb = In.vTexcoord.y;
    
    return Out;
}


technique11 DefaultTechnique
{
    /* Ư�� �н��� �̿��ؼ� ������ �׷��´�. */
    /* �ϳ��� ���� �׷��´�. */ 
    /* ���� ��Ȳ�� ���� �ٸ� ���̵� ��� ��Ʈ(��� + ������Ʈ + ����ŧ�� + ��ָ� + ssao )�� �Կ��ֱ����ؼ� */
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();   
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass LoadingSlotPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN2();
    }

    pass SkillSlotPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN3();
    }

    pass FadeOutPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN4();
    }

    pass TitleBackGroundPass // Alpha Blend�� (Alpha ���� ����)
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN5();
    }

    pass HPProgressBarPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN6();
    }

    pass BlackColorPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN7();
    }

    pass LockOnPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0, 0, 0, 0), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN8();
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
