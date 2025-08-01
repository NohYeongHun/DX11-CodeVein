#pragma once
#include "Monster.h"

NS_BEGIN(Client)
class CWolfDevil final : public CMonster
{
public:
	typedef struct tagWolfDevilDesc : public CMonster::MONSTER_DESC
	{

	}WOLFDEVIL_DESC;

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

#pragma region 충돌 함수 정의
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);
#pragma endregion

#pragma region BUFF FLAG 관리
public:
	virtual HRESULT Initialize_BuffDurations();
#pragma endregion

#pragma region 애니메이션 변경.
public:
	virtual HRESULT InitializeAction_ToAnimationMap() override;
#pragma endregion



#pragma region BEHAVIOUR TREE
private:
	class CMonsterTree* m_pTree = { nullptr };

#pragma endregion


#pragma region READY OJBECT, COMPONENT
private:
	HRESULT Ready_Components(WOLFDEVIL_DESC* pDesc);
	HRESULT Ready_BehaviourTree();
	HRESULT Ready_Render_Resources();
	HRESULT Ready_PartObjects();
#pragma endregion



public:
	static CWolfDevil* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;
};
NS_END

