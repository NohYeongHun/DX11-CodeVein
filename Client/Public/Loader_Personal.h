#pragma once
#include "Base.h"

NS_BEGIN(Client)

class CLoader_Personal abstract :
    public CBase
{
public:
    CLoader_Personal() = default;
    virtual ~CLoader_Personal() = default;

public:
    virtual HRESULT Loading_Resource(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance) PURE;
};

NS_END
