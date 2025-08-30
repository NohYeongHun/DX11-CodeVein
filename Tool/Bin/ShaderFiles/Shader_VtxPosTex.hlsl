matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

// Texture ���� Diffuse, Gradient, GradientAlpha, Mask
texture2D g_DiffuseTexture;
texture2D g_GradientTexture;
texture2D g_GradientAlphaTexture;
texture2D g_MaskTexture;

/* ����� ���� ��� �ð�, ũ�� => ����*/
float g_fTimeRatio;
float g_fScaleRatio;

sampler DefaultSampler = sampler_state
{
    filter = min_mag_mip_linear; 
};

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



PS_OUT PS_MAIN2(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    // UV ��ǥ�� �߽ɿ��� �����ϸ�
    float2 center = float2(0.5f, 0.5f);
    float2 scaledUV = center + (In.vTexcoord - center) / g_fScaleRatio;
    
    // �����ϵ� UV�� ������ ����� discard
    if (scaledUV.x < 0.0f || scaledUV.x > 1.0f || scaledUV.y < 0.0f || scaledUV.y > 1.0f)
        discard;

    // �ؽ�ó ���� ������ �������̸� ����
    vector vSourDiffuse = g_DiffuseTexture.Sample(DefaultSampler, scaledUV);
    //vector vDestDiffuse = g_MaskTexture.Sample(DefaultSampler, scaledUV);
    vector vDestDiffuse = g_GradientTexture.Sample(DefaultSampler, scaledUV);
    
    // �ҽ� �ؽ�ó�� �������̸� discard
    if (vDestDiffuse.r < 0.1f && vDestDiffuse.g < 0.1f && vDestDiffuse.b < 0.1f)
        discard;
    
    
    vector vMask = g_GradientTexture.Sample(DefaultSampler, scaledUV);
    vector vMtrlDiffuse = vDestDiffuse * (1.f - vMask) + vSourDiffuse * (vMask);
    
    // �ð��� ���� ���� ���̵�ƿ�
    float fadeAlpha = 1.0f - g_fTimeRatio;
    vMtrlDiffuse.a *= fadeAlpha;
    
    Out.vColor = vMtrlDiffuse;
    
    return Out;
}

technique11 DefaultTechnique
{
    /* Ư�� �н��� �̿��ؼ� ������ �׷��´�. */
    /* �ϳ��� ���� �׷��´�. */ 
    /* ���� ��Ȳ�� ���� �ٸ� ���̵� ��� ��Ʈ(��� + ������Ʈ + ����ŧ�� + ��ָ� + ssao )�� �Կ��ֱ����ؼ� */
    pass DefaultPass
    {
        GeometryShader = NULL;
        VertexShader = compile vs_5_0 VS_MAIN();   
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass EffectTexture_LineSlashPass // 2�� �н�
    {
        GeometryShader = NULL;
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN2();
    }


}
