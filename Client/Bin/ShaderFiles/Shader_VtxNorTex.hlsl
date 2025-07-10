matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);

/* ���� 
*/
texture2D g_DiffuseTexture;
vector g_vMtrlAmbient = 1.f;

sampler DefaultSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = wrap;
    AddressV = wrap;
};

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
    
    vector vMtrlDiffuse = g_Texture.Sample(DefaultSampler, In.vTexcoord * 50.f);
    
    
    float fShade = max(dot((normalize(g_vLightDir) * -1.f), normalize(In.vNormal)), 0.f); // ��� ���.
    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient));
    
    return Out;
}


//int g_iTextureIndex;


// Texture Index�� ���� �ٸ� �ȼ� ���̴��� �����ϱ�.
//PS_OUT PS_MAIN2(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;
    
//    float2 uv = In.vTexcoord;
    
//    float4 baseColor = g_Texture.Sample(DefaultSampler, uv); // ���� �ؽ�ó ��
//    float4 fillerColor = float4(1, 1, 1, 1); // ���̾Ƹ�� �ȿ� ä���� ��
    
//    // ���̾Ƹ�� �߽�
//    float2 center = float2(0.5f, 0.5f);
//    float2 delta = abs(uv - center);
//    bool bIsInDiamond = (delta.x + delta.y) < 0.5f;
    
//    if (bIsInDiamond)
//    {
//        Out.vColor = lerp(baseColor, fillerColor, 0.8f); // �ε巴�� ����
//    }
//    else
//    {
//        Out.vColor = baseColor;
//    }
    
//    return Out;
//}


//float g_
//;
//PS_OUT PS_MAIN3(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;

//    float2 uv = In.vTexcoord;
//    float4 fillerColor = float4(0, 0, 0, 1); // ���̾Ƹ�� �ȿ� ä���� ��
//    float4 baseColor = g_Texture.Sample(DefaultSampler, uv); // ���� �ؽ�ó ��

    
//    // ���̾Ƹ�� �߽�
//    float2 center = float2(0.5f, 0.5f);
//    float2 delta = abs(uv - center);
//    bool bIsInDiamond = (delta.x + delta.y) < 0.51f;

//    // �Ʒ��� ���� ä���
//    bool bIsFillRegion = uv.y > (1.0 - g_fFillRatio);

//    if (bIsInDiamond && bIsFillRegion)
//    {
//        // ���̾Ƹ�� �����̸鼭, ä���� �����̸�
//        Out.vColor = lerp(baseColor, fillerColor, 0.8f); // �ε巴�� ����
//    }
//    else
//    {
//        // �� �ܴ� ���� �ؽ�ó ����
//        Out.vColor = baseColor;
//    }

//    return Out;
//}

//float g_fFade;

//// Fade Out Shader
//PS_OUT PS_MAIN4(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;

//    float2 uv = In.vTexcoord;
//    float4 baseColor = g_Texture.Sample(DefaultSampler, uv);
//    float4 fillerColor = float4(0, 0, 0, 1);
    
//    baseColor.rgb = lerp(baseColor.rgb, float3(0.0, 0.0, 0.0), saturate(g_fFade));
    
//    // �ٽ� �� ��: ������������� ���� ����
//    Out.vColor = baseColor;
    

//    return Out;
//}


technique11 DefaultTechnique
{
    /* Ư�� �н��� �̿��ؼ� ������ �׷��´�. */
    /* �ϳ��� ���� �׷��´�. */ 
    /* ���� ��Ȳ�� ���� �ٸ� ���̵� ��� ��Ʈ(��� + ������Ʈ + ����ŧ�� + ��ָ� + ssao )�� �Կ��ֱ����ؼ� */
    pass DefaultPass
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    //pass LoadingSlotPass
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN();
    //    PixelShader = compile ps_5_0 PS_MAIN2();
    //}

    //pass SkillSlotPass
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN();
    //    PixelShader = compile ps_5_0 PS_MAIN3();
    //}

    //pass FadeOutPass
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN();
    //    PixelShader = compile ps_5_0 PS_MAIN4();
    //}
    



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
