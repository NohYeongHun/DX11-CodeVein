#pragma once
#include "PlayerState.h"

NS_BEGIN(Client)
class CPlayer_FirstSkillState final : public CPlayerState
{
public:
	typedef struct tagPlayerFirstSkillEnterDesc
	{
		_uint iAnimation_Idx = {};
		ACTORDIR eDirection = {};
	}FIRSTSKILL_ENTER_DESC;


public:
	typedef struct tagPlayerFirstSkillStateDesc : public CPlayerState::PLAYER_STATE_DESC
	{

	}FIRSTSKILL_STATE_DESC;

private:
	explicit CPlayer_FirstSkillState();
	virtual ~CPlayer_FirstSkillState() = default;

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

private:
	void Update_Sound(_float fTimeDelta);

private:
	_bool m_IsFirstAttack = {};
	_bool m_IsSecondAttack = {};
	_bool m_IsThirdAttack = {};
	_bool m_IsFourthAttack = {};
	_bool m_IsFifthAttack = {};

	_float m_fFirstAttackFrame = {};
	_float m_fSecondAttackFrame = {};
	_float m_fThirdAttackFrame = {};
	_float m_fFourthAttackFrame = {};
	_float m_fFifthAttackFrame = {};

	_wstring m_strSoundFile = {};

public:
	static CPlayer_FirstSkillState* Create(_uint iStateNum, void* pArg);
	virtual void Free() override;
};
NS_END

