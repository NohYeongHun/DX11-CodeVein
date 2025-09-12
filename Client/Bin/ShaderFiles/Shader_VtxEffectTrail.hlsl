#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float g_fAlpha = 1.f;
texture2D g_DiffuseTexture;
texture2D g_DepthTexture;
texture2D g_DissolveTexture;
texture2D g_SmokeDstTexture;

// 멀티 텍스처 시스템
texture2D g_BaseTexture;      // 기본 검 궤적 텍스처
texture2D g_DetailTexture;    // 슬래시 디테일 텍스처  
texture2D g_GlowTexture;      // 발광 효과 텍스처

// 블렌딩 파라미터
float g_fDetailBlendRatio = 0.3f;   // 디테일 텍스처 블렌드 비율
float g_fGlowIntensity = 0.5f;      // 발광 강도
float g_fUVScrollSpeed = 1.0f;      // UV 스크롤 속도

// Ribbon Trail 파라미터
float g_Time = 0.0f;               // 시간 (자동으로 업데이트)
float g_ScrollSpeed = 0.5f;        // 스크롤 속도 (천천히)
float g_FadePower = 2.0f;          // 페이드 거듭제곱 (더 부드럽게)
float g_Intensity = 0.8f;          // 전체 강도 (약간 낮춤)
float4 g_TrailColor = float4(0.8f, 0.4f, 1.0f, 1.0f); // 리본 트레일 색상 (보라색)

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
    float fFade : TEXCOORD1; // 페이드값.
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

//struct PS_IN_SOFTEFFECT
//{
//    float4 vPosition : SV_POSITION;
//    float2 vTexcoord : TEXCOORD0;
//    float4 vProjPos : TEXCOORD1;
//};

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

    Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    Out.vDiffuse.a = Out.vDiffuse.r;

    // UV의 X좌표로 검정->빨강 그라디언트 생성
    float gradientFactor = In.vTexcoord.x;
    
    // 검정(0,0,0)에서 빨강(1,0,0)으로 보간
    Out.vDiffuse.rgb = lerp(float3(0.f, 0.f, 0.f), float3(1.f, 0.f, 0.f), gradientFactor);
    
    Out.vDiffuse.a *= g_fAlpha;
    if (Out.vDiffuse.a <= 0.01f)
        discard;

    return Out;
}


// 멀티 텍스처 블렌딩 픽셀 셰이더
PS_OUT PS_MULTI_TRAIL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // 기본 검 궤적 텍스처
    float4 baseColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    // 슬래시 디테일 텍스처 (UV 스케일링으로 디테일 강화)
    float2 detailUV = In.vTexcoord * 2.0f;
    float4 detailColor = g_DetailTexture.Sample(DefaultSampler, detailUV);
    
    // Base와 Detail 텍스처만 블렌딩 (SpWeapon 제외)
    float4 blendedColor = baseColor; // Base를 메인으로
    blendedColor = lerp(blendedColor, detailColor, g_fDetailBlendRatio); // 디테일 블렌드
    
    // 알파 값 계산 - Base와 Detail만 조합
    float combinedAlpha = max(baseColor.r, detailColor.r);
    combinedAlpha = pow(combinedAlpha, 0.7f); // 감마 보정으로 알파 밝게
    combinedAlpha = saturate(combinedAlpha * 1.5f); // 알파 50% 증가
    Out.vDiffuse.a = combinedAlpha * g_fAlpha;
    
    // 최종 색상 설정 (Base + Detail만 사용)
    Out.vDiffuse.rgb = blendedColor.rgb * g_fGlowIntensity;
    
    if (Out.vDiffuse.a <= 0.01f)
        discard;
        
    return Out;
}

//PS_OUT PS_STRETCH_TRAIL(PS_IN In)
//{
//    PS_OUT Out = (PS_OUT) 0;
    
//    // 1. UV 스크롤 (시간 기반) - 리본이 잡아당겨지는 효과
//    float2 scrollUV = In.vTexcoord;
//    //scrollUV.x += g_Time * g_ScrollSpeed; // U 방향으로 스크롤
//    //scrollUV.x += g_Time * g_ScrollSpeed; // U 방향으로 스크롤
    
//    // 2. SP_Weapon 텍스처 샘플링 (GlowTexture 사용)
//    //float4 weaponColor = g_GlowTexture.Sample(DefaultSampler, scrollUV);
//    float4 weaponColor = g_BaseTexture.Sample(DefaultSampler, scrollUV);
    
//    // 3. 시간 기반 페이드 (거리가 멀수록 투명)
//    float fade = pow(In.fFade, g_FadePower);
    
//    // 4. 엣지 페이드 (리본 가장자리 부드럽게)
//    float edgeFade = abs(In.vTexcoord.y - 0.5f) * 2.0f; // 0.0(중앙) ~ 1.0(가장자리)
//    edgeFade = 1.0f - smoothstep(0.5f, 1.0f, edgeFade); // 더 넓은 범위로 부드럽게
    
//    // 5. SP_Weapon 텍스처의 원본 색상을 그대로 사용 (리본 형태로)
//    Out.vDiffuse = float4(1.0f, 0.0f, 0.0f, 1.0f); // 강제 빨간색
    
//    //Out.vDiffuse = weaponColor; // SP_Weapon 원본 색상
//    //Out.vDiffuse.rgb = float3(1.f, 0.f, 0.f);
//    //Out.vDiffuse.a = weaponColor.a * fade * edgeFade * g_Intensity * g_fAlpha; // 알파는 텍스처의 원본 알파 채널 사용
    
    
//    //if (Out.vDiffuse.a <= 0.01f)
//    //    discard;
    
//    return Out;
//}

PS_OUT PS_STRETCH_TRAIL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // 1. UV 스크롤 (시간 기반) - 리본이 잡아당겨지는 효과
    float2 scrollUV = In.vTexcoord;
    scrollUV.x += g_Time * g_ScrollSpeed; // U 방향으로 스크롤
    float4 weaponColor = g_BaseTexture.Sample(DefaultSampler, scrollUV);
    Out.vDiffuse = weaponColor;
    
    
    //Out.vDiffuse = g_BaseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Out.vDiffuse.a <= 0.01f)
        discard;
    //Out.vDiffuse.rgb = float3(128.f / 255.f, 128.f / 255.f, 128.f / 255.f);
    //Out.vDiffuse.rgb = float3(0.f, 0.f, 0.f);
    //Out.vDiffuse.a = 1.f;
    
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

    pass MultiTrail
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_Default, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MULTI_TRAIL();
    }

    pass StretchTrail
    {
        SetRasterizerState(RS_Default);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        //SetDepthStencilState(DSS_WeightBlend, 0);
        SetDepthStencilState(DSS_WeightBlend, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_STRETCH_TRAIL();
    }
}