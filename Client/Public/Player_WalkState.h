#pragma once
#include "PlayerState.h"

/* 모든 동작의 기본이 되는 동작. */
NS_BEGIN(Client)
class CPlayer_WalkState final : public CPlayerState
{
public:
	typedef struct tagPlayerWalkEnterDesc
	{
		_uint iAnimation_Idx = {};
	}WALK_ENTER_DESC;

public:
	typedef struct tagPlayerWalkStateDesc : public CPlayerState::PLAYER_STATE_DESC
	{

	}WALKSTATE_DESC;

private:
	explicit CPlayer_WalkState();
	virtual ~CPlayer_WalkState() = default;

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

public:
	void Handle_Input(_float fTimeDelta);

private:
	_uint		   m_iCurIdx = {};
	_bool		   m_iNextIdx = {};
	DIR			   m_eDir = { DIR::END };
	_bool		   m_isLoop = { true };
	_bool		   m_isKeyInput = { false };
	

public:
	static CPlayer_WalkState* Create(_uint iStateNum, void* pArg);
	virtual void Free() override;


};
NS_END
