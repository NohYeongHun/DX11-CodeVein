#pragma once
#include "Loader_Personal.h"

NS_BEGIN(Client)
class CLoader_GamePlay final :
    public CLoader_Personal
{
public:
    CLoader_GamePlay() = default;
    virtual ~CLoader_GamePlay() = default;

public:
    virtual HRESULT Loading_Resource(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance) override;
    LEVEL m_eCur_Level = LEVEL::GAMEPLAY;

private:
    HRESULT Add_Prototype_Component(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);

    HRESULT Add_Prototype_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_Camera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_Camera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);

    
    //HRESULT Add_Prototype_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    
    

};

NS_END