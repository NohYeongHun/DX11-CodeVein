#pragma once
#include "PlayerState.h"

/* 모든 동작의 기본이 되는 동작. */
NS_BEGIN(Client)
/* 48 */
class CPlayer_StrongAttackState final : public CPlayerState
{
public:
	typedef struct tagPlayerStrongAttackEnterDesc
	{
		_uint iAnimation_Idx = {};
		ACTORDIR eDirection = {};
	}STRONG_ENTER_DESC;

public:
	typedef struct tagPlayerStrongAttackStateDesc : public CPlayerState::PLAYER_STATE_DESC
	{

	}STRONG_ATTACK_STATE_DESC;

private:
	explicit CPlayer_StrongAttackState();
	virtual ~CPlayer_StrongAttackState() = default;

public:
	// 시작 초기화
	virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;

	// State 시작 시.
	virtual void Enter(void* pArg) override;

	// State 실행 로직
	virtual void Update(_float fTimeDelta) override;

	// State Exit
	virtual void Exit() override;

	// State 초기값으로 설정
	virtual void Reset() override;

	void Change_State();


public:
	static CPlayer_StrongAttackState* Create(_uint iStateNum, void* pArg);
	virtual void Free() override;


};
NS_END
