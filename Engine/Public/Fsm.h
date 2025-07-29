#pragma once
#include "Component.h"

/* 
* State들을 관리하는 객체.
*/
NS_BEGIN(Engine)
class ENGINE_DLL CFsm final : public CComponent
{
public:
	typedef struct tagFsmDesc
	{
		class CGameObject* pOwner = { nullptr };
	}FSM_DESC;
private:
	explicit CFsm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFsm(const CFsm& Prototype);


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	void Update(_float fTimeDelta);

public:
	_uint				Get_CurrentState() { return m_pNowState->Get_StateNum(); }
	_uint				Get_PrevState() { return m_pPreState->Get_StateNum(); }

public:
	class CGameObject* Get_Owner() { return m_pOwner; }


public:
	void Add_State(class CState* pState);
	void Change_State(_uint iState, void* pArg);

	void Register_StateCoolTime(_uint iStateNum, _float fCoolTime);
	_float Get_StateCoolTime(_uint iStateNum);
	void Set_StateCoolTime(_uint iStateNum, _float fCoolTime);
	void Set_StateCoolTime(_uint iStateNum);
	_bool Is_CoolTimeEnd(_uint iStateNum);


	void Register_StateExitCoolTime(_uint iStateNum, _float fCoolTime);
	_float Get_StateExitCoolTime(_uint iStateNum);
	void Set_StateExitCoolTime(_uint iStateNum, _float fCoolTime);
	void Set_StateExitCoolTime(_uint iStateNum);
	_bool Is_ExitCoolTimeEnd(_uint iStateNum);

private:
	class CGameObject* m_pOwner = { nullptr };
	vector<CState*>		m_States;	//모든 상태를 갖고 있는 벡터 - 인덱스 접근용

	CState* m_pNowState = { nullptr };			//지금 상태
	CState* m_pPreState = { nullptr };			//이전 상태 => 이전 상태 추적이 필요할 수 도 있음.

private:
	unordered_map<_uint, _float> m_fCoolTimes = {};    // 등록용 쿨타임.
	unordered_map<_uint, _float> m_fUseCoolTimes = {}; // 사용 쿨타임 => 해당 쿨타임이 0.f이면 사용 가능.

	// Exit Time => 어떤 모션이 실행 중일 때 다른 모션으로의 강제 전환을 막는 시간.
	unordered_map<_uint, _float> m_fExitCoolTimes = {};    // 등록용 쿨타임.
	unordered_map<_uint, _float> m_fExitUseCoolTimes = {}; // 사용 쿨타임 => 해당 쿨타임이 0.f이면 사용 가능.


public:
	static CFsm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END

