#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* 사용할 텍스쳐 목록 */
texture2D g_DiffuseTexture;
texture2D g_GradientTexture;
texture2D g_GradientAlphaTexture;
texture2D g_MaskTexture;

texture2D g_MaskTextures[16];
//Texture2DArray g_MaskTextures;

uint g_MaskMaxidx;

vector g_vCamPosition;


/* 사용할 변수 목록 시간, 크기 => 비율*/
float g_fTimeRatio;
float g_fScaleRatio;

struct VS_IN
{
    float3 vPosition : POSITION;
    
    row_major float4x4 TransformMatrix : WORLD;
    float2 vLifeTime : TEXCOORD0;
    float3 vDir : TEXCOORD1;
    float fSpeed : TEXCOORD2;
    uint iMaskTextureIndex : TEXINDEX0;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float fSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float3 vDir : TEXCOORD1;
    float fSpeed : TEXCOORD2;
    uint iMaskTextureIndex : TEXINDEX0;
};

/* 정점쉐이더 : 정점 위치의 스페이스 변환(로컬 -> 월드 -> 뷰 -> 투영). */ 
/*          : 정점의 구성을 변경.(in:3개, out:2개 or 5개) */
/*          : 정점 단위(정점 하나당 VS_MAIN한번호출) */ 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
 
    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    
    Out.vPosition = mul(vPosition, g_WorldMatrix); // 객체의 월드 Matrix 곱해줌.
    Out.fSize = length(In.TransformMatrix._11_12_13);
    Out.vLifeTime = In.vLifeTime;
    Out.vDir = mul(In.vDir, In.fSpeed);
    Out.fSpeed = In.fSpeed;
    Out.iMaskTextureIndex = In.iMaskTextureIndex;
    
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float fSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float3 vDir : TEXCOORD1;
    float fSpeed : TEXCOORD2;
    uint iMaskTextureIndex : TEXINDEX0;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    uint iMaskTextureIndex : TEXINDEX0;
};

//GS_MAIN(triangle GS_IN In[3])
//GS_MAIN(line GS_IN In[2])

[maxvertexcount(6)]
void GS_Stretched_Billboard_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
    GS_OUT Out[4];

    vector vRight, vUp, vLook;
    vLook = normalize(g_vCamPosition - In[0].vPosition);
    float3 vDir = normalize(In[0].vDir);

    float speed = In[0].fSpeed; // 개별 속도값 사용
    float stretchScale = In[0].fSize * (1.0f + speed * 0.3f); // 0.1f는 보정값 너무 길게 늘어나면 수정 필요 
    //늘어지는 효과

    // 속도 방향을 Up으로 사용
    vUp = vector(vDir, 0.f) * stretchScale * 0.5f;

    // 카메라와 직교하는 Right 축
    vRight = normalize(vector(cross(vDir, vLook), 0.f)) * In[0].fSize * 0.5f;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = mul(In[0].vPosition + vRight + vUp, matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].iMaskTextureIndex = In[0].iMaskTextureIndex;

    Out[1].vPosition = mul(In[0].vPosition - vRight + vUp, matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].iMaskTextureIndex = In[0].iMaskTextureIndex;

    Out[2].vPosition = mul(In[0].vPosition - vRight - vUp, matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].iMaskTextureIndex = In[0].iMaskTextureIndex;

    Out[3].vPosition = mul(In[0].vPosition + vRight - vUp, matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].iMaskTextureIndex = In[0].iMaskTextureIndex;

    Vertices.Append(Out[0]);
    Vertices.Append(Out[1]);
    Vertices.Append(Out[2]);
    Vertices.RestartStrip();

    Vertices.Append(Out[0]);
    Vertices.Append(Out[2]);
    Vertices.Append(Out[3]);
    Vertices.RestartStrip();
}

/* /W을 수행한다. 투영스페이스로 변환 */
/* 뷰포트로 변환하고.*/
/* 래스터라이즈 : 픽셀을 만든다. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    uint iMaskTextureIndex : TEXINDEX0;
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
    
    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    

    return Out;
}

PS_OUT PS_DEBUG_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = float4(1.f, 0.f, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_DIFFUSE_MASK_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    //Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vSourDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDestDiffuse = float4(0, 0, 0, 1);
    vector vMask = float4(0, 0, 0, 0);
    
    // 동적 인덱싱을 조건문으로 처리
    if (In.iMaskTextureIndex == 0)
    {
        vDestDiffuse = g_MaskTextures[0].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[0].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 1)
    {
        vDestDiffuse = g_MaskTextures[1].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[1].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 2)
    {
        vDestDiffuse = g_MaskTextures[2].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[2].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 3)
    {
        vDestDiffuse = g_MaskTextures[3].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[3].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 4)
    {
        vDestDiffuse = g_MaskTextures[4].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[4].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 5)
    {
        vDestDiffuse = g_MaskTextures[5].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[5].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 6)
    {
        vDestDiffuse = g_MaskTextures[6].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[6].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 7)
    {
        vDestDiffuse = g_MaskTextures[7].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[7].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 8)
    {
        vDestDiffuse = g_MaskTextures[8].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[8].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 9)
    {
        vDestDiffuse = g_MaskTextures[9].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[9].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 10)
    {
        vDestDiffuse = g_MaskTextures[10].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[10].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 11)
    {
        vDestDiffuse = g_MaskTextures[11].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[11].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 12)
    {
        vDestDiffuse = g_MaskTextures[12].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[12].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 13)
    {
        vDestDiffuse = g_MaskTextures[13].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[13].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 14)
    {
        vDestDiffuse = g_MaskTextures[14].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[14].Sample(DefaultSampler, In.vTexcoord);
    }
    else if (In.iMaskTextureIndex == 15)
    {
        vDestDiffuse = g_MaskTextures[15].Sample(DefaultSampler, In.vTexcoord);
        vMask = g_MaskTextures[15].Sample(DefaultSampler, In.vTexcoord);
    }

    if (vDestDiffuse.r < 0.1f && vDestDiffuse.g < 0.1f && vDestDiffuse.b < 0.1f)
        discard;
    vector vMtrlDiffuse = vDestDiffuse * (1.f - vMask) + vSourDiffuse * (vMask);

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
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_Stretched_Billboard_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass QueenKnightWarpPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_Stretched_Billboard_MAIN();
        PixelShader = compile ps_5_0 PS_DIFFUSE_MASK_MAIN();
    }

    pass DebugPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_Stretched_Billboard_MAIN();
        PixelShader = compile ps_5_0 PS_DEBUG_MAIN();
    }

   
}
