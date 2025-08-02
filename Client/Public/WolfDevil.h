#pragma once
#include "Monster.h"

NS_BEGIN(Client)
class CWolfDevil final : public CMonster
{
public:
	// 어떤 타입의 Collider를 제어할것인지 정의.
	enum PART_TYPE : _uint
	{

	};

public:
	typedef struct tagWolfDevilDesc : public CMonster::MONSTER_DESC
	{

	}WOLFDEVIL_DESC;

#pragma region 기본 함수들
private:
	explicit CWolfDevil(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWolfDevil(const CWolfDevil& Prototype);
	virtual ~CWolfDevil() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Update_AI(_float fTimeDelta) override;
#pragma endregion



#pragma region 0. 몬스터는 충돌에 대한 상태제어를 할 수 있어야한다. => 충돌에 따라 상태가 변하기도, 수치값이 바뀌기도한다.
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);
#pragma endregion

#pragma region 1. 몬스터는 자신에게 필요한 수치값들을 초기화해야한다.
public:
	virtual HRESULT Initialize_Stats() override;
#pragma endregion


#pragma region 2. 몬스터는 내 애니메이션이 무엇인지 알아야한다.
public:
	virtual HRESULT InitializeAction_ToAnimationMap() override;
#pragma endregion

#pragma region 3. 몬스터는 자신이 어떤 버프를 소유할 수 있는지를 알아야 한다. => 그리고 그에 맞는 쿨타임도 알아야한다.(몬스터마다 달라질 수 있다.)
public:
	virtual HRESULT Initialize_BuffDurations() override;
#pragma endregion


#pragma region 4. BEHAVIOUR TREE
private:
	class CMonsterTree* m_pTree = { nullptr };

#pragma endregion


#pragma region 5. 기본적으로 몬스터가 필요한 상태들을 정의합니다.
private:
	HRESULT Ready_Components(WOLFDEVIL_DESC* pDesc);
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
	static CWolfDevil* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;
};
NS_END

