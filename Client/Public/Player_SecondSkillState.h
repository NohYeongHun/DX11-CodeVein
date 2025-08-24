#pragma once
#include "PlayerState.h"

NS_BEGIN(Client)
class CPlayer_SecondSkillState final : public CPlayerState
{
public:
	typedef struct tagPlayerSecondSkillEnterDesc
	{
		_uint iAnimation_Idx = {};
		ACTORDIR eDirection = {};
	}SECONDSKILL_ENTER_DESC;


public:
	typedef struct tagPlayerSecondSkillStateDesc : public CPlayerState::PLAYER_STATE_DESC
	{

	}SECONDSKILL_STATE_DESC;

private:
	explicit CPlayer_SecondSkillState();
	virtual ~CPlayer_SecondSkillState() = default;


#pragma region 해당 STATE에서만 사용하는 변수
private:
	_float m_fIncreaseDamage = {};
#pragma endregion

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
	static CPlayer_SecondSkillState* Create(_uint iStateNum, void* pArg);
	virtual void Free() override;
};
NS_END

