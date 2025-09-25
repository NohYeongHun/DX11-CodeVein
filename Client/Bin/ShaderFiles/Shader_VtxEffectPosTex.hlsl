#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_DiffuseTexture;
texture2D g_DiffuseTextures[2];
texture2D g_OpacityTexture;
texture2D g_MaskTexture;


texture2D g_OtherTexture[5];



float g_fTime;
float g_fTimeRatio;
float g_fBloomIntensity = 2.0f;
float g_fScale; 

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


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;    
    
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);    
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    return Out;     
}

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



PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;    
}



PS_OUT PS_HITFLASH_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float sparkShape = g_OpacityTexture.Sample(DefaultSampler, In.vTexcoord).r;

    float radialGlow = g_OtherTexture[2].Sample(DefaultSampler, In.vTexcoord).r;
    
    float finalShape = saturate(sparkShape + (radialGlow * 0.1f));

    float fadeAlpha = sin(saturate(g_fTimeRatio) * 3.14159f);
    
    float finalAlpha = finalShape * fadeAlpha;

    float3 goldColor = float3(2.5f, 1.8f, 0.5f) * g_fBloomIntensity;
    float3 finalColor = goldColor * finalShape;
    
    
    
    Out.vColor = float4(finalColor, finalAlpha);
    
    return Out;
}


PS_OUT PS_LINESLASH_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 slashUV = In.vTexcoord;
    slashUV.y = (slashUV.y + 1.0f) / 4.0f; 
    float4 vMaskTexture = g_OtherTexture[1].Sample(DefaultSampler, slashUV);
    float slashShape = vMaskTexture.r;

    slashShape = pow(slashShape, 2.0f);

    float2 noiseUV = In.vTexcoord * 2.0f + g_fTime * 0.5f;
    float noiseValue = g_OtherTexture[2].Sample(DefaultSampler, noiseUV).r;

    clip(noiseValue - pow(g_fTimeRatio, 2.2f));

    float3 goldColor = float3(2.5f, 1.8f, 0.5f);
    
    float3 finalColor = goldColor * slashShape * g_fBloomIntensity;
    
   
    float finalAlpha = slashShape * saturate((noiseValue - g_fTimeRatio) * 5.0f);

    if (finalAlpha < 0.01f)
        discard;
    
    Out.vColor = float4(finalColor, finalAlpha);
    
    return Out;
}


PS_OUT PS_LINESLASH_RED_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 slashUV = In.vTexcoord;
    slashUV.y = (slashUV.y + 1.0f) / 4.0f;
    
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, slashUV);
    float4 vMask = g_OtherTexture[1].Sample(DefaultSampler, slashUV);
    float slashShape = g_OtherTexture[1].Sample(DefaultSampler, slashUV).r;

    slashShape = pow(slashShape, 2.0f);

    float2 noiseUV = In.vTexcoord * 2.0f + g_fTime * 0.5f;
    float noiseValue = g_OtherTexture[2].Sample(DefaultSampler, noiseUV).r;

    clip(noiseValue - pow(g_fTimeRatio, 2.2f));
    
    vector bloomColor = float4(float3(4.0f, 0.5f, 0.5f) * g_fBloomIntensity, 1.0f);
    
    // 3. ����ũ�� ���� �κп� ��� ������ �����ݴϴ�.
    
    float fMaskBrightness = dot(vMask.rgb, float3(0.299, 0.587, 0.114));
    //    fMaskBrightness ���� �̹� ������ ���Ǿ����ϴ�.
    
    float lifeRatio = g_fTimeRatio;
    float lifeCurve = sin(lifeRatio * 3.14159f);
    if (fMaskBrightness > 0.1f) // �Ӱ谪�� 0.1 ~ 0.5 ���̿��� ����
    {
        // vMtrlDiffuse.rgb�� bloomColor.rgb�� ���մϴ�.
        // lifeCurve�� �����༭ ��ƼŬ ���� ���� �ڿ������� �������� �մϴ�.
        vMtrlDiffuse.rgb += bloomColor.rgb * vMask.rgb * lifeCurve;
    }
    
    float3 finalColor = vMtrlDiffuse.rgb;
    
   
    float finalAlpha = slashShape * saturate((noiseValue - g_fTimeRatio) * 5.0f);

    if (finalAlpha < 0.01f)
        discard;
    
    Out.vColor = float4(finalColor, finalAlpha);
    
    return Out;
}


texture2D g_OtherTextures[3];
texture2D g_MaskTextures[2];

PS_OUT PS_RENKETSU_SLASH_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 uv = In.vTexcoord;
    
    // ����ũ�� ���ø�
    float4 vGlowMask = g_MaskTextures[0].Sample(DefaultSampler, uv); // ��� �۷ο� ����ũ
    float4 vSlashMask = g_MaskTextures[1].Sample(DefaultSampler, uv); // ���� ������ ����ũ
    
    // MaskTextures[0]: �� �κи� ������ (���� �κ� Ŭ����)
    float glowArea = vGlowMask.r;
    clip(glowArea - 0.01f);
    
    if (vGlowMask.r < 0.1f && vGlowMask.g < 0.1f && vGlowMask.b < 0.1f)
        discard;
    
    
    // MaskTextures[1]: ���� �κ��� ������ (����)
    float slashCore = 1.0f - vSlashMask.r;
    
    // 1. ���� �˰� �κ�
    float3 slashColor = float3(0.1f, 0.05f, 0.05f); // ���� ������
    
    // 2. �ֺ� ���� ������ �ƿ��
    // �����ÿ��� �־������� ���� ���������� ��ȭ
    float auraGradient = 1.0f - slashCore; // �����ÿ��� �ּ��� 1�� �����
    float3 auraColor = float3(1.f, 0.01f, 0.01f); // ���� ������
    
    // �ƿ�� ���� (������ �ֺ����� ����)
    float auraRange = smoothstep(0.0f, 0.5f, auraGradient);
    
    // 3. ���� ����
    // ������ �߽��� ������, �ֺ��� ���� ������
    float3 finalColor = slashColor;  //lerp(slashColor, auraColor, auraRange);
    
    // ������ �߽ɺθ� �� ��Ӱ� ����
    if (slashCore > 0.8f)
    {
        finalColor = slashColor * 0.5f; // �߽ɺδ� �� �˰�
    }
    
    // 4. ���İ� ����
    // �����ô� ������, �ƿ��� ������
    float slashAlpha = slashCore; // ������ �κ�
    float auraAlpha = auraRange * 0.4f * (1.0f - slashCore); // �ƿ��� ������
    
    float alpha = saturate(slashAlpha + auraAlpha);
    alpha *= glowArea; // �۷ο� ����ũ ��� ����
    
    Out.vColor = float4(finalColor, 1.f);
    
    return Out;
} 

/* ������ ������ - ���� �������� �˰� ȿ�� */
/* ������ ������ - ����ũ�� ��� �κп��� ȿ�� ���� */
/* �� �����ϰ� ��Ȯ�� ���� */


technique11 DefaultTechnique
{
    pass DefaultPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN(); 
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass HitFlashPass // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HITFLASH_MAIN();
    }

    pass MonsterLineSlashPass // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NonWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LINESLASH_MAIN();
    }

    pass RenketsuLineSlashPass// 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LINESLASH_RED_MAIN();
    }

}
