matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

// Texture 종류 Diffuse, Gradient, GradientAlpha, Mask
texture2D g_DiffuseTexture;
texture2D g_GradientTexture;
texture2D g_GradientAlphaTexture;
texture2D g_MaskTexture;

/* 사용할 변수 목록 시간, 크기 => 비율*/
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

/* 정점쉐이더 : 정점 위치의 스페이스 변환(로컬 -> 월드 -> 뷰 -> 투영). */ 
/*          : 정점의 구성을 변경.(in:3개, out:2개 or 5개) */
/*          : 정점 단위(정점 하나당 VS_MAIN한번호출) */ 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;    
    
    /* 정점의 로컬위치 * 월드 * 뷰 * 투영 */ 
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);    
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    
    return Out;     
}

/* /W을 수행한다. 투영스페이스로 변환 */
/* 뷰포트로 변환하고.*/
/* 래스터라이즈 : 픽셀을 만든다. */

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

/* 만든 픽셀 각각에 대해서 픽셀 쉐이더를 수행한다. */
/* 픽셀의 색을 결정한다. */



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
    // UV 좌표를 중심에서 스케일링
    float2 center = float2(0.5f, 0.5f);
    float2 scaledUV = center + (In.vTexcoord - center) / g_fScaleRatio;
    
    // 스케일된 UV가 범위를 벗어나면 discard
    if (scaledUV.x < 0.0f || scaledUV.x > 1.0f || scaledUV.y < 0.0f || scaledUV.y > 1.0f)
        discard;

    // 텍스처 없이 검정색 오버레이만 생성
    vector vSourDiffuse = g_DiffuseTexture.Sample(DefaultSampler, scaledUV);
    //vector vDestDiffuse = g_MaskTexture.Sample(DefaultSampler, scaledUV);
    vector vDestDiffuse = g_GradientTexture.Sample(DefaultSampler, scaledUV);
    
    // 소스 텍스처가 검정색이면 discard
    if (vDestDiffuse.r < 0.1f && vDestDiffuse.g < 0.1f && vDestDiffuse.b < 0.1f)
        discard;
    
    
    vector vMask = g_GradientTexture.Sample(DefaultSampler, scaledUV);
    vector vMtrlDiffuse = vDestDiffuse * (1.f - vMask) + vSourDiffuse * (vMask);
    
    // 시간에 따른 알파 페이드아웃
    float fadeAlpha = 1.0f - g_fTimeRatio;
    vMtrlDiffuse.a *= fadeAlpha;
    
    Out.vColor = vMtrlDiffuse;
    
    return Out;
}

technique11 DefaultTechnique
{
    /* 특정 패스를 이용해서 점정을 그려냈다. */
    /* 하나의 모델을 그려냈다. */ 
    /* 모델의 상황에 따라 다른 쉐이딩 기법 세트(명암 + 림라이트 + 스펙큘러 + 노멀맵 + ssao )를 먹여주기위해서 */
    pass DefaultPass
    {
        GeometryShader = NULL;
        VertexShader = compile vs_5_0 VS_MAIN();   
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass EffectTexture_LineSlashPass // 2번 패스
    {
        GeometryShader = NULL;
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN2();
    }


}
