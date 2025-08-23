#pragma once
#include "Loader_Personal.h"

NS_BEGIN(Client)
class CLoader_StageOne final : public CLoader_Personal
{
public:
    CLoader_StageOne() = default;
    virtual ~CLoader_StageOne() = default;

public:
    virtual HRESULT Loading_Resource(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance) override;
    LEVEL m_eCur_Level = LEVEL::STAGEONE;

private:

    /* 가장 먼저 초기화 */
    HRESULT Add_Prototype_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_Navigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);

    HRESULT Add_Prototype_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    //HRESULT Add_Prototype_QueenKnight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_GiantWhiteDevil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_WolfDevil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_SlaveVampire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);

    HRESULT Add_Prototype_Camera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
    HRESULT Add_Prototype_Camera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);

    HRESULT Add_Prototype_SkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);

};
NS_END

