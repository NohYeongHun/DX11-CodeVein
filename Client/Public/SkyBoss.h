#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CSkyBoss final : public CMonster
{

public:
	// 어떤 타입의 Collider를 제어할것인지 정의.
	enum PART_TYPE : _uint
	{

	};

public:
	typedef struct tagSkyBossDesc : public CMonster::MONSTER_DESC
	{

	}SKYBOSS_DESC;

private:
	explicit CSkyBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSkyBoss(const CSkyBoss& Prototype);
	virtual ~CSkyBoss() = default;


#pragma region 기본 함수 관리
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


public:
	virtual void Update_AI(_float fTimeDelta) override;
#pragma endregion

#pragma region 1. 몬스터는 자신에게 필요한 수치값들을 초기화해야한다.
public:
	virtual HRESULT Initialize_Stats() override;
#pragma endregion

#pragma region 애니메이션 관리
	virtual HRESULT InitializeAction_ToAnimationMap() override;
#pragma endregion

#pragma region 타이머 관리
	// CMonster을(를) 통해 상속됨
	virtual HRESULT Initialize_BuffDurations() override;
#pragma endregion



#pragma region BOSS 함수 정의.

#pragma endregion

#pragma region BEHAVIOUR TREE
private:
	CSkyBossTree* m_pTree = { nullptr };

#pragma endregion


#pragma region READY OJBECT, COMPONENT
private:
	HRESULT Ready_Components(SKYBOSS_DESC* pDesc);
	HRESULT Ready_BehaviourTree();
	HRESULT Ready_Render_Resources();
	HRESULT Ready_PartObjects();
#pragma endregion

#pragma region 6. 특수한 상태를 제어하기 위한 함수들
public:
	virtual void Enable_Collider(_uint iType) override;
	virtual void Disable_Collider(_uint iType) override;
#pragma endregion

public:
	static CSkyBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;




};
NS_END

