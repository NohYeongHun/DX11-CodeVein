sampler g_LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

sampler g_PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

sampler g_LinearClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

sampler g_PointClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

//D3D11_RASTERIZER_DESC
RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = FALSE;
};

RasterizerState RS_Wireframe
{
    FillMode = Wireframe;
    CullMode = Back;
    FrontCounterClockwise = FALSE;
};

RasterizerState RS_Cull_None
{
    FillMode = Solid;
    CullMode = NONE;
    FrontCounterClockwise = FALSE;
};



DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

DepthStencilState DSS_Draw_Priority
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};

BlendState BS_One
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    SrcBlend = One;
    DestBlend = One;
    BlendOp = Add;
};



