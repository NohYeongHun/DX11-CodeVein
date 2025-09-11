sampler DefaultSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = wrap;
    AddressV = wrap;
};

sampler PointSampler = sampler_state
{
    filter = min_mag_mip_point;
    AddressU = wrap;
    AddressV = wrap;
};


RasterizerState RS_Default
{
    FillMode = solid;
    CullMode = back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_CW
{    
    CullMode = front;    
};

RasterizerState RS_Cull_CCW
{
    CullMode = None;
    FillMode = solid;
    FrontCounterClockwise = true;
};

RasterizerState RS_Cull_CCW_WIREFRAME
{
    CullMode = None;
    FillMode = wireframe;
    FrontCounterClockwise = true;
};


DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

DepthStencilState DSS_NonWrite
{
    DepthEnable = true;
    DepthWriteMask = zero;
};


DepthStencilState DSS_UI
{
    DepthEnable = false;
    DepthWriteMask = all;
    DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_WeightBlend
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
    DepthFunc = LESS_EQUAL;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
};


BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = Add;    
};

BlendState BS_AlphaBlend_Additive // 새로운 가산 혼합 상태
{
    BlendEnable[0] = true;
    
    // (SourceColor * SourceAlpha) + (DestinationColor * 1)
    // 소스(이펙트)의 알파 값만큼 기존 화면에 더해져서 빛나는 효과를 냄
    SrcBlend = SRC_ALPHA;
    DestBlend = ONE;
    BlendOp = ADD;
};


BlendState BS_Blend_Inv
{
    BlendEnable[0] = TRUE;
    BlendOp[0] = Add;
    SrcBlend[0] = DEST_COLOR;
    DestBlend[0] = ZERO;
    SrcBlendAlpha[0] = INV_SRC_ALPHA;
    DestBlendAlpha[0] = SRC_ALPHA;
};


BlendState BS_WeightBlend_Client
{
    BlendEnable[0] = TRUE;
    BlendOp[0] = Add;
    SrcBlend[0] = ONE;
    DestBlend[0] = ONE;
};