#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CPlayer final : public CContainerObject
{

#pragma region PLAYER STATE 정의 STATE != ANIMATION
public:
	// Add_State 순서대로 넣습니다.
	enum PLAYER_STATE : _int
	{
		IDLE = 0, WALK, RUN, DODGE,
		STRONG_ATTACK, GUARD, ATTACK,
		STATE_END
	};
#pragma endregion

public:
	typedef struct tagPlayerDesc : public CGameObject::GAMEOBJECT_DESC
	{
		LEVEL eCurLevel;
	}PLAYER_DESC;

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;


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
	//void Set_Camera(CCamera_Action* pCamera) { m_pPlayerCamera = pCamera; };
	void Set_Camera(CCamera_Player* pCamera) { m_pPlayerCamera = pCamera; };


#pragma region PLAYER 함수 정의.
public:
	void HandleState(_float fTimeDelta);
	_vector  Calculate_Move_Direction(ACTORDIR eDir);


public:
	// 키 입력 상태 확인 함수들
	uint16_t Get_KeyInput() { return m_KeyInput; }

	_bool Is_KeyPressed(PLAYER_KEY ePlayerKey) const {
		return m_KeyInput & static_cast<uint16_t>(ePlayerKey);
	}

	_bool Is_KeyUp(PLAYER_KEY ePlayerKey) const {
		return (m_PrevKeyInput & static_cast<uint16_t>(ePlayerKey)) &&
			!(m_KeyInput & static_cast<uint16_t>(ePlayerKey));
	}

	_bool Is_KeyDown(PLAYER_KEY ePlayerKey) const {
		return !(m_PrevKeyInput & static_cast<uint16_t>(ePlayerKey)) &&
			(m_KeyInput & static_cast<uint16_t>(ePlayerKey));
	}

	// 움직임 키 확인
	_bool Is_MovementKeyPressed() const {
		return Is_KeyPressed(PLAYER_KEY::MOVE_FORWARD) ||
			Is_KeyPressed(PLAYER_KEY::MOVE_BACKWARD) ||
			Is_KeyPressed(PLAYER_KEY::MOVE_LEFT) ||
			Is_KeyPressed(PLAYER_KEY::MOVE_RIGHT);
	}
	ACTORDIR Get_Direction() { return m_eCurrentDirection; }
	ACTORDIR Calculate_Direction();
#pragma endregion






#pragma region 이동 관련 함수들
public:
	void Move_By_Camera_Direction_8Way(ACTORDIR eDir, _float fTimeDelta, _float fSpeed);
	void Debug_CameraVectors();
	void Rotate_Player_To_Camera_Direction();
	// 현재 프레임 가져오기.
#pragma endregion

#pragma region 락온
public:
	void Toggle_LockOn();                               // LockOn 토글
	void Search_LockOn_Target();                        // 타겟 검색
	void Set_LockOn_Target(CGameObject* pTarget);       // 타겟 설정
	void Clear_LockOn_Target();                         // 타겟 해제
	void Update_LockOn(_float fTimeDelta);              // LockOn 업데이트
	_bool Is_Valid_LockOn_Target(CGameObject* pTarget);  // 타겟 유효성 검사
	void Rotate_To_LockOn_Target(_float fTimeDelta, _float fSpeed = 3.0f); // 타겟 방향 회전
	_vector Calculate_LockOn_Direction() const;         // LockOn 방향 계산

	// Getter 함수들
	_bool Is_LockOn() const { return m_isLockOn; }
	CGameObject* Get_LockOn_Target() const { return m_pLockOn_Target; }

public:
	// LockOn 방향만 반환하는 함수 (공격 시 사용)
	_vector Get_LockOn_Attack_Direction() const;
	_bool Has_LockOn_Target() const { return m_isLockOn && m_pLockOn_Target; }

private:
	// LockOn 시스템 관련 멤버변수들
	CGameObject* m_pLockOn_Target = { nullptr };         // 현재 LockOn 타겟 => 위치에 UI 띄우기.
	_bool m_isLockOn = { false };                        // LockOn 상태
	_float m_fLockOnRange = 15.0f;                       // LockOn 최대 거리
	_float m_fLockOnAngle = 120.0f;                      // LockOn 각도 (도 단위)
	_float m_fLockOnTimer = 0.0f;                        // LockOn 타이머
	_float m_fLockOnCheckInterval = 0.5f;                // LockOn 유효성 검사 주기
	_float m_fLockOnRotationSpeed = 3.0f;                // LockOn 시 회전 속도
	_bool m_bLockOnRotationEnabled = false;              // LockOn 시 자동 회전 여부
private:
	class m_pLockOn_Target* m_pTarget = { nullptr };
#pragma endregion


public:
	// Animation 교체.
	void Change_Animation(_uint iAnimIndex, _bool IsLoop, _float fDuration, _uint iStartFrame, _bool bEitherBoundary, _bool bSameChange);


#pragma region Player 기본 상태 값
private:
	// Load Model;
	class CLoad_Model* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CWeapon* m_pPlayerWeapon = { nullptr };
	class CFsm* m_pFsmCom = { nullptr };
	class CCamera_Player* m_pPlayerCamera = { nullptr };
	//class CCamera_Action* m_pPlayerCamera = { nullptr };
	LEVEL m_eCurLevel;
#pragma endregion


#pragma region Key 입력 상태 값
private:
	uint16_t m_PrevKeyInput = {};
	uint16_t m_KeyInput = {};
	static const _uint m_KeyboardMappingsCount;
	static const pair<PLAYER_KEY, _ubyte> m_KeyboardMappings[];
	ACTORDIR m_eCurrentDirection = {};

private:
	void Update_KeyInput();
#pragma endregion




private:
	HRESULT Ready_Components(PLAYER_DESC* pDesc);
	HRESULT Ready_Fsm();
	void Register_CoolTime();
	HRESULT Ready_Render_Resources();
	HRESULT Ready_PartObjects();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;

};
NS_END

