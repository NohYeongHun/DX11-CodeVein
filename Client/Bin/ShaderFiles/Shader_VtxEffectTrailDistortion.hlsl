#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DistortionTexture;

// 변수
float g_fAlpha = 1.f;
float g_fTime = 1.f;
float g_fDistortionPower = 2.0f; // 왜곡 강도 조절용 변수 추가
float g_fNoiseScale = 1.5f;


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float fFade : TEXCOORD1; // 페이드값.
};

/* DrawIndexed함수를 호출하면. */
/* 인덱스버퍼에 담겨있는 인덱스번째의 정점을 VS_MAIN함수에 인자로 던진다. VS_IN에 저장된다. */
/* 일반적으로 TriangleList로 그릴경우, 정점 세개를 각각 VS_MAIN함수의 인자로 던진다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP, matVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    matVP = mul(g_ViewMatrix, g_ProjMatrix);

	/* 정점의 위치에 월드 뷰 투영행렬을 곱한다. 현재 정점은 ViewSpace에 존재한다. */
	/* 투영행렬까지 곱하면 정점위치의 w에 뷰스페이스 상의 z를 보관한다. == Out.vPosition이 반드시 float4이어야하는 이유. */
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.fFade = 1.f - In.vTexcoord.x;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float fFade : TEXCOORD1;
};



struct PS_OUT
{
    float4 vNormal : SV_TARGET0;
};



/* 디스토션 텍스쳐에 어떻게 맥이지? */
//PS_OUT PS_MAIN(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;
    
    
//    // 0. UV Scrolling 추가.
//    float2 scrolledUV = In.vTexcoord + float2(g_fTime * -1.f, 0.f);

//    // 1. 왜곡 패턴이 담긴 텍스처를 샘플링합니다. 
//    float3 vTangentNormal = g_DistortionTexture.Sample(DefaultSampler, scrolledUV).xyz;
    
//    // 2. Fade 효과 적용.
//    vTangentNormal *= In.fFade;

    
//    // 3. 노멀 벡터의 범위(-1.0 ~ 1.0)를 텍스처에 저장 가능한 색상 범위(0.0 ~ 1.0)로 변환하여 출력합니다.
//    //    vTangentNormal 값은 이미 -1~1 범위라고 가정합니다. (만약 0~1 범위의 텍스처라면 vTangentNormal * 2.f - 1.f 가 필요)
//    Out.vNormal.xyz = vTangentNormal * 0.5f + 0.5f;
//    Out.vNormal.w = 1.f; // 알파 값

//    return Out;
//}

// 더 강한 디스토션을 위한 대체 픽셀 셰이더
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // 극단적으로 강한 디스토션 효과
    float2 baseUV = In.vTexcoord;
    
    // 웨이브 패턴 추가
    float wave = sin(baseUV.x * 10.0f + g_fTime * 5.0f) * 0.1f;
    baseUV.y += wave;
    
    // 3개의 레이어 사용
    float2 uv1 = baseUV * 2.0f + float2(g_fTime * -3.0f, g_fTime * 0.5f);
    float2 uv2 = baseUV * 3.5f + float2(g_fTime * 2.0f, g_fTime * -1.0f);
    float2 uv3 = baseUV * 1.3f + float2(g_fTime * -1.5f, g_fTime * 2.0f);
    
    float3 dist1 = g_DistortionTexture.Sample(DefaultSampler, uv1).xyz * 2.0f - 1.0f;
    float3 dist2 = g_DistortionTexture.Sample(DefaultSampler, uv2).xyz * 2.0f - 1.0f;
    float3 dist3 = g_DistortionTexture.Sample(DefaultSampler, uv3).xyz * 2.0f - 1.0f;
    
    // 복잡한 블렌딩
    float3 vTangentNormal = dist1 * 0.5f + dist2 * 0.3f + dist3 * 0.2f;
    
    // 극단적인 증폭
    vTangentNormal *= 3.0f;
    
    // 중앙 강조
    float centerBoost = 1.0f - pow(abs(In.vTexcoord.y - 0.5f) * 2.0f, 1.2f);
    vTangentNormal *= centerBoost * 1.5f;
    
    // 페이드
    vTangentNormal *= pow(In.fFade, 0.5f);
    
    // 클램핑
    vTangentNormal = clamp(vTangentNormal, -1.0f, 1.0f);
    
    Out.vNormal.xyz = vTangentNormal * 0.5f + 0.5f;
    Out.vNormal.w = 1.0f;
    
    return Out;
}



technique11 DefaultTechnique
{
    pass Trail
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AdditiveBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

   
}