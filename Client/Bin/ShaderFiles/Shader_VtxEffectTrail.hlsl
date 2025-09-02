#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float g_fAlpha = 1.f;
texture2D g_DiffuseTexture;
texture2D g_DepthTexture;
texture2D g_DissolveTexture;
texture2D g_SmokeDstTexture;
vector g_ColorBack = vector(1.f, 1.f, 1.f, 1.f);
vector g_ColorFront = vector(1.f, 1.f, 1.f, 1.f);

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

/* DrawIndexed함수를 호출하면. */
/* 인덱스버퍼에 담겨있는 인덱스번째의 정점을 VS_MAIN함수에 인자로 던진다. VS_IN에 저장된다. */
/* 일반적으로 TriangleList로 그릴경우, 정점 세개를 각각 VS_MAIN함수의 인자로 던진다. */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

	/* 정점의 위치에 월드 뷰 투영행렬을 곱한다. 현재 정점은 ViewSpace에 존재하낟. */
	/* 투영행렬까지 곱하면 정점위치의 w에 뷰스페이스 상의 z를 보관한다. == Out.vPosition이 반드시 float4이어야하는 이유. */
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_IN_SOFTEFFECT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
};

/* 이렇게 만들어진 픽셀을 PS_MAIN함수의 인자로 던진다. */
/* 리턴하는 색은 Target0 == 장치에 0번째에 바인딩되어있는 렌더타겟(일반적으로 백버퍼)에 그린다. */
/* 그래서 백버퍼에 색이 그려진다. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vDiffuse.a = Out.vDiffuse.r;
    Out.vDiffuse.rgb = g_ColorFront.rgb;

    Out.vDiffuse.a *= g_fAlpha;

    if (Out.vDiffuse.a <= 0.01f)
        discard;

    return Out;
}

PS_OUT PS_TRAIL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //vector texColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    //// Diffuse 텍스처의 색상을 그대로 사용
    //Out.vDiffuse.rgb = texColor.rgb;
    //Out.vDiffuse.a = texColor.a * g_fAlpha;
    
    //if (Out.vDiffuse.a <= 0.01f)
    //    discard;
    
    Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vDiffuse.a = Out.vDiffuse.r;

    vector GetColorBack = g_ColorBack / 255.f;
    vector GetColorFront = g_ColorFront / 255.f;

    //Out.vDiffuse.rgb = GetColorBack.rgb * (1 - Out.vDiffuse.r) + GetColorFront.rgb * Out.vDiffuse.r;

    Out.vDiffuse.rgb = float3(0.f, 0.f, 0.f);
    
    Out.vDiffuse.a *= g_fAlpha;
    if (Out.vDiffuse.a <= 0.0f)
        discard;

    return Out;
}

technique11 DefaultTechnique
{
    pass Trail
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAIL();
    }
}