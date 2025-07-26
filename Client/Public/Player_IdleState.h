#pragma once
#include "PlayerState.h"

/* 모든 동작의 기본이 되는 동작. */
NS_BEGIN(Client)
class CPlayer_IdleState final : public CPlayerState
{
public:
	typedef struct tagPlayerIdleEnterDesc
	{
		_uint iAnimation_Index = {};
	}IDLE_ENTER_DESC;

public:
	typedef struct tagPlayerIdleStateDesc : public CPlayerState::PLAYER_STATE_DESC
	{

	}IDLESTATE_DESC;

private:
	explicit CPlayer_IdleState();
	virtual ~CPlayer_IdleState() = default;

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
	void Handle_Input();

private:
	_uint          m_iAnimation_IdleIdx = { };
	_bool		   m_isLoop = { true }; // Loop 상태인가?
	_bool		   m_isKeyInput = { false };
	_int		   m_iNextIdx = { -1};
	DIR			   m_eDir = { DIR::END };

public:
	static CPlayer_IdleState* Create(_uint iStateNum, void* pArg);
	virtual void Free() override;


};
NS_END
