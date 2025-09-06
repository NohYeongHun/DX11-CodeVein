#pragma once
#include "Monster.h"

NS_BEGIN(Client)
class CGiant_WhiteDevil final : public CMonster
{
public:
	enum PART_TYPE : _uint
	{
		PART_WEAPON = 0,
		PART_BODY = 1,
		PART_END
	};

	enum GIANT_BUFF_FLAGS : _uint
	{
		GIANT_BUFF_NONE = 0,
		GIANT_BUFF_COMBO_ATTACK = 1 << 1,
	};

public:
	typedef struct tagGiantWhiteDevil : public CMonster::MONSTER_DESC
	{
		ANIMESH_SHADERPATH eShaderPath = { ANIMESH_SHADERPATH::NONE };
	}GIANTWHITEDEVIL_DESC;

private:
	explicit CGiant_WhiteDevil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGiant_WhiteDevil(const CGiant_WhiteDevil& Prototype);
	virtual ~CGiant_WhiteDevil() = default;


#pragma region 0. 기본 함수 관리
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Finalize_Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

#pragma endregion


#pragma region 1. 충돌 함수 정의
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);
	virtual void Collider_All_Active(_bool bActive);

#pragma endregion

#pragma region 2. 몬스터 AI 관리.
public:
	// AI에 대한 호출 순서를 정의합니다.
	virtual void Update_AI(_float fTimeDelta) override;

private:
	class CGiant_WhiteDevilTree* m_pTree = { nullptr };

#pragma endregion


#pragma region 3. 몬스터는 자신에게 필요한 수치값들을 초기화해야한다.
public:
	virtual HRESULT Initialize_Stats() override;
#pragma endregion

#pragma region 4. 몬스터는 자신의 애니메이션을 관리해야한다.
	virtual HRESULT InitializeAction_ToAnimationMap() override;
#pragma endregion

#pragma region 5. 사용되는 버프/디버프에 대한 타이머를 관리해야한다.
	virtual HRESULT Initialize_BuffDurations() override;
#pragma endregion

#pragma region 6. 특수한 상태를 제어하기 위한 함수들입니다.


public:
	/* 어떤 파츠의 Colider를 제어할 것인지? */
	virtual void Enable_Collider(_uint iType) override;
	virtual void Disable_Collider(_uint iType) override;
#pragma endregion

#pragma region 7. 보스몹 체력 UI 관리
public:
	virtual void Take_Damage(_float fDamage, CGameObject* pGameObject) override;

	virtual void Increase_HpUI(_float fHp, _float fTime) override;
	virtual void Decrease_HpUI(_float fHp, _float fTime) override;


private:
	HRESULT Initailize_UI();


private:
	class CBossHpBarUI* m_pBossHpBarUI = { nullptr };

#pragma endregion

#pragma region 99. 초기화 함수들 정의
private:
	class CWhiteLargeHalberd* m_pWeapon = { nullptr };

private:
	HRESULT Ready_Components(GIANTWHITEDEVIL_DESC* pDesc);
	HRESULT Ready_Colliders(GIANTWHITEDEVIL_DESC* pDesc);
	HRESULT Ready_Navigations();
	HRESULT Ready_BehaviorTree();
	HRESULT Ready_PartObjects();
	HRESULT Ready_Render_Resources();
	

public:
	static CGiant_WhiteDevil* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;
#pragma endregion

#pragma region IMGUI Render

#ifdef _DEBUG
private:
	void ImGui_Render();
#endif // _DEBUG

#pragma endregion

};
NS_END