#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CSkyBoss final : public CMonster
{

#pragma region SKYBOSS STATE 정의 STATE != ANIMATION
public:
	// Add_State 순서대로 넣습니다.
	enum SKYBOSS_STATE : _int
	{
		IDLE = 0, WALK, RUN, DODGE,
		STRONG_ATTACK, GUARD, ATTACK,
		STATE_END
	};
#pragma endregion

public:
	typedef struct tagSkyBossDesc : public CMonster::MONSTER_DESC
	{

	}SKYBOSS_DESC;

private:
	explicit CSkyBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSkyBoss(const CSkyBoss& Prototype);
	virtual ~CSkyBoss() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

#pragma region 충돌 함수 정의
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);
#pragma endregion



#pragma region BOSS 함수 정의.

#pragma endregion
private:
	LEVEL m_eCurLevel;
	ACTORDIR m_eCurrentDirection = {};
	

#pragma region BEHAVIOUR TREE
private:
	CSkyBossTree* m_pTree = { nullptr };

public:
	_bool Is_StrongHit();
#pragma endregion


#pragma region READY OJBECT, COMPONENT
private:
	HRESULT Ready_Components(SKYBOSS_DESC* pDesc);
	HRESULT Ready_BehaviourTree();
	HRESULT Ready_Render_Resources();
	HRESULT Ready_PartObjects();
#pragma endregion



public:
	static CSkyBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;

};
NS_END

