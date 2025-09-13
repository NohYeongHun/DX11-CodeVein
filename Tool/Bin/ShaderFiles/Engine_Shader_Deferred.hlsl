#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
texture2D g_Texture;

vector g_vCamPosition;

vector g_vLightDir;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

texture2D g_DiffuseTexture;
vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
texture2D g_NormalTexture;
texture2D g_DepthTexture;
texture2D g_ShadeTexture;
texture2D g_SpecularTexture;
texture2D g_EmissiveTexture;

// Bloom용 텍스처들
texture2D g_sceneTexture;
texture2D g_BloomTexture;

// Bloom 파라미터들
float g_fBrightThreshold = 1.0f;
float g_fBloomIntensity = 1.0f;
float2 g_vTexelSize = float2(1.0f / 1280.0f, 1.0f / 720.0f);

float threshold = 0.5f; // 조정 가능 (예: 0.8 ~ 0.95)
float soft = 0.20f; // 페이드 범위
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

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
        
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_BACKBUFFER
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    //vector vSpecular : SV_TARGET1;
};

/* 빛연산 */
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
    
    float fShade = max(dot(vNormal * -1.f, normalize(g_vLightDir)), 0.f);
    
    vector vReflect = reflect(normalize(g_vLightDir), vNormal);
    
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vWorldPos;
    

    

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
  
    vWorldPos = vWorldPos * vDepthDesc.y;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    vector vLook = vWorldPos - g_vCamPosition;
    
    //float fSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
    
    
    Out.vShade = g_vLightDiffuse * saturate(fShade + (g_vLightAmbient * g_vMtrlAmbient));
    //Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    
    //Out.vShade = vNormal; // 양수만 표현되므로 (vNormal * 0.5f + 0.5f) 로 하셔도 좋습니다.
    
    return Out;
}


PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    
    // 1. 깊이 텍스처를 샘플링하여 깊이 값을 가져옵니다.
    float fDepth = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
    // 2. 깊이 값이 1.0에 가깝다면 (배경이라면) 픽셀 렌더링을 중단합니다.
    if (fDepth >= 0.9999f)
    {
        discard;
    }
    
    // 3. (오브젝트가 그려진 경우) 기존 로직을 그대로 수행합니다.
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vDiffuse.a == 0.f)
        discard;
    
    
    vector vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);
    //vector vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    //Out.vColor = vDiffuse * vShade + vSpecular;
    
    /*
    만약 vShade(조명 MRT)가 비어 있거나 0으로 클리어된 상태면, diffuse와 곱하면 전부 검정색 된다.
    → 하지만 디폴트 클리어 컬러가 (1,1,1,1)라서 최소한 흰색 조명은 나와야 하니까, 
    */

    Out.vColor = vDiffuse * vShade;
    
    return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_EMISSIVE(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    // 1. 기본 Diffuse Sampling 
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    clip(vDiffuse.a - 0.05f);
    
    // 2. Emissive 공식
    vDiffuse += pow(vDiffuse, 1.1f); 
    
    Out.vColor = vDiffuse;
    return Out;
}



PS_OUT_BACKBUFFER PS_MAIN_DISTORTION(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (vDiffuse.a == 0.f)
        discard;
    
    vector vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);
    //vector vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    //Out.vColor = vDiffuse * vShade + vSpecular;
    Out.vColor = vDiffuse * vShade;
    
    return Out;
}

// Bloom Bright Pass - 밝은 부분만 추출
PS_OUT_BACKBUFFER PS_MAIN_BRIGHT_PASS(PS_IN In)
{
    //PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    //vector vSceneColor = g_sceneTexture.Sample(DefaultSampler, In.vTexcoord);

    //// 원본 색상에서 임계값만큼 밝기를 뺌 (어두운 부분은 음수가 됨)
    //vector vBrightColor = vSceneColor - g_fBrightThreshold;
    
    //// saturate를 통해 0 미만인 값들을 0으로 만듦
    //Out.vColor = saturate(vBrightColor);
    
    //// 알파 값 유지 (필요 시)
    //Out.vColor.a = vSceneColor.a;
    
    //return Out;
    
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    float4 BrightColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 originalColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    float brightness = dot(originalColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));

    float factor = smoothstep(threshold, threshold + soft, brightness);
    BrightColor = originalColor * factor;
    Out.vColor = BrightColor;
    return Out;
}

// 수평 블러 X
PS_OUT_BACKBUFFER PS_MAIN_BLUR_HORIZONTAL(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    float4 vColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // 가우시안 블러 가중치
    float weights[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
    
    // 현재 픽셀
    vColor += g_sceneTexture.Sample(DefaultSampler, In.vTexcoord) * weights[0];
    
    // 수평 방향 블러
    for (int i = 1; i < 5; ++i)
    {
        float2 offset = float2(g_vTexelSize.x * i, 0.0f);
        vColor += g_sceneTexture.Sample(DefaultSampler, In.vTexcoord + offset) * weights[i];
        vColor += g_sceneTexture.Sample(DefaultSampler, In.vTexcoord - offset) * weights[i];
    }
    
    Out.vColor = vColor;
    return Out;
}

// 수직 블러
PS_OUT_BACKBUFFER PS_MAIN_BLUR_VERTICAL(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    float4 vColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // 가우시안 블러 가중치
    float weights[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
    
    // 현재 픽셀
    vColor += g_sceneTexture.Sample(DefaultSampler, In.vTexcoord) * weights[0];
    
    // 수직 방향 블러
    for (int i = 1; i < 5; ++i)
    {
        float2 offset = float2(0.0f, g_vTexelSize.y * i);
        vColor += g_sceneTexture.Sample(DefaultSampler, In.vTexcoord + offset) * weights[i];
        vColor += g_sceneTexture.Sample(DefaultSampler, In.vTexcoord - offset) * weights[i];
    }
    
    Out.vColor = vColor;
    return Out;
}

// Bloom과 원본 씬 합성
PS_OUT_BACKBUFFER PS_MAIN_BLOOM_COMBINE(PS_IN In)
{
    PS_OUT_BACKBUFFER Out = (PS_OUT_BACKBUFFER) 0;
    
    vector vSceneColor = g_sceneTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vBloomColor = g_BloomTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // ... 이전 코드 ...
    vector vCombinedColor = vSceneColor + (vBloomColor * g_fBloomIntensity);

    // 톤 매핑 적용 (간단한 Reinhard 방식)
    vector vTonemappedColor = vCombinedColor / (vCombinedColor + 1.0f);

    // 최종 출력
    Out.vColor = float4(vTonemappedColor.rgb, 1.0f);
    
    return Out;
}



technique11 DefaultTechnique
{
    
    pass Debug // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Directional // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Point // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Combined // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }

    pass Emissive // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE();
    }

    pass Distortion // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
    }

    pass BrightPass // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BRIGHT_PASS();
    }

    pass BlurHorizontal // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_HORIZONTAL();
    }

    pass BlurVertical // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_VERTICAL();
    }

    pass BloomCombine // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM_COMBINE();
    }


}
