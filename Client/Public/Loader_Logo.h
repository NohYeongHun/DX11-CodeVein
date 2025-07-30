#pragma once
#include "Loader_Personal.h"

NS_BEGIN(Client)
class CLoader_Logo final :
    public CLoader_Personal
{
public:
    CLoader_Logo() = default;
    virtual ~CLoader_Logo() = default;

public:
    virtual HRESULT Loading_Resource(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance) override;
    LEVEL m_eCur_Level = LEVEL::LOGO;

private:
    HRESULT Add_Prototype_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_Component(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_Block(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    

};

NS_END