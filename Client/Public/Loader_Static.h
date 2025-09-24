#pragma once
NS_BEGIN(Client)
class CLoader_Static final : public CLoader_Personal
{
public:
    CLoader_Static() = default;
    virtual ~CLoader_Static() = default;


public:
    virtual HRESULT Loading_Resource(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance) override;

private:
    LEVEL m_eCurLevel = LEVEL::STATIC;

#pragma region LOADING 함수

#pragma region PROTOTYPE
private:
	HRESULT Add_Prototype_ForShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_ForVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_ForCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_ForFadeout(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);

	HRESULT Add_Prototype_Fsm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_ForPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_HUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_SkillUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_MonsterUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_Fonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);

#pragma endregion

#pragma region EFFECT
private:
	HRESULT Add_Prototype_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_Dissolve_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_Slash_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_Renketsu_Slash_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_HitFlash_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_Particle_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_Trail_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_ParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_BloodPillar_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_SwordWind_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_SwordWindCircle_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);
	HRESULT Add_Prototype_BloodAura_Effects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance);

#pragma endregion


	

#pragma endregion



};
NS_END

