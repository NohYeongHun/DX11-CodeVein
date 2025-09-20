#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vCamPosition;

/*재질*/
//texture2D g_DiffuseTexture;
//texture2D g_NormalTexture;


//// 시간.
//float g_fDissolveTime;
//float g_fReverseDissolveTime;

//vector g_vMtrlAmbient = 1.f;
//vector g_vMtrlSpecular = 1.f;
texture2D g_DiffuseTexture;
texture2D g_DiffuseTextures[6];

texture2D g_OtherTextures[13];

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

/* 정점쉐이더 : 정점 위치의 스페이스 변환(로컬 -> 월드 -> 뷰 -> 투영). */ 
/*          : 정점의 구성을 변경.(in:3개, out:2개 or 5개) */
/*          : 정점 단위(정점 하나당 VS_MAIN한번호출) */ 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    /* 정점의 로컬위치 * 월드 * 뷰 * 투영 */ 
        
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


/* /W을 수행한다. 투영스페이스로 변환 */
/* 뷰포트로 변환하고.*/
/* 래스터라이즈 : 픽셀을 만든다. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;

};

struct PS_OUT
{
    // 렌더 타겟(Target_Distortion)에 노멀 벡터를 기록합니다.
    float4 vNormal : SV_TARGET0;
};



// 왜곡 지도 생성용 픽셀 셰이더
PS_OUT PS_DISTORTION_MAP_MAIN(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
   
    // 텍스처에서 노멀 값을 읽어옵니다. (노멀맵 사용 시)
    vector vNormalDesc = g_OtherTextures[10].Sample(DefaultSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    vNormal = mul(vNormal, WorldMatrix);
    
    // 노멀 벡터의 범위를 0 ~ 1로 변환하여 출력합니다.
    // 렌더 타겟에는 음수 값을 저장할 수 없기 때문입니다.
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);

    return Out;
}


technique11 DefaultTechnique
{
    /* 특정 패스를 이용해서 점정을 그려냈다. */
    /* 하나의 모델을 그려냈다. */ 
    /* 모델의 상황에 따라 다른 쉐이딩 기법 세트(명암 + 림라이트 + 스펙큘러 + 노멀맵 + ssao )를 먹여주기위해서 */
    pass SwordWindPass // 0 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        //PixelShader = compile ps_5_0 PS_MAIN();
        PixelShader = compile ps_5_0 PS_DISTORTION_MAP_MAIN();
    }


   
}