#pragma once

NS_BEGIN(Client)
class CCamera_Player final : public CCamera
{
public:
	typedef struct tagCameraPlayerDesc : public CCamera::CAMERA_DESC
	{
		_float fMouseSensor{};
		class CPlayer* pTarget = {};
	}CAMERA_PLAYER_DESC;

private:
	explicit CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCamera_Player(const CCamera_Player& Prototype);
	virtual ~CCamera_Player() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

#pragma region 0. 일반 상태. <- 플레이어 추적

private:
	// 0. Player 추적.
	void Update_Chase_Target(_float fTimeDelta);

	// 1. Mouse를 이용한 Input 관리 Pitch Yaw
	void Handle_Mouse_Input(_float fTimeDelta, _float fSensitivityMultiplier = 1.0f);

	// 2. 마우스 커서 제한 관리
	void Enable_Mouse_Clip();
	void Disable_Mouse_Clip();
	void Update_Mouse_Clip();

private:
	class CPlayer* m_pTarget = {};
	_float m_fMouseSensor = {};
	_float4 m_vTargetOffset = {};
	_float m_fYaw = 0.f;
	_float m_fPitch = 0.f;

	// 부드러운 회전을 위한 목표 각도들
	_float m_fTargetYaw = 0.f;
	_float m_fTargetPitch = 0.f;
	_float m_fRotationSmoothSpeed = 8.0f; // 회전 부드러움 속도

	// Q키 눌렀을때 Pitch 조정.
	_bool m_bPitchControlMode = false;    // Q키를 누르고 있을 때만 true
	_float m_fDefaultPitch = 0.0f;       // 기본 Pitch 각도 (도 단위)

	// 마우스 커서 제한
	_bool m_bMouseClipped = false;        // 마우스 클립 상태
	RECT m_rcClipRect = {};              // 클립 영역

	// Pitch 제한값들 (자연스러운 카메라 각도 범위)
	_float m_fMaxPitch = 10.0f;    // 최대 위쪽 각도 
	_float m_fMinPitch = -10.0f;   // 최소 각도 (땅바닥 관통 방지) 

	// 부드러운 추적을 위한 새로운 멤버 변수들
	_float4 m_vCurrentCameraPos = {};   // 현재 카메라 위치
	_float4 m_vTargetCameraPos = {};    // 목표 카메라 위치
	_float m_fSmoothSpeed = 200.0f;     // 추적 속도 (높을수록 빠름)
	_bool m_bFirstUpdate = true;        // 첫 번째 업데이트시 플레이어 위치로 바로 이동.

#pragma endregion

#pragma region 1. 락온 상태. <-몬스터를 추적하되 플레이어를 동일선상에 둡니다. => 카메라 => 플레이어 등짝 => 몬스터
public:
	void Toggle_LockOn_Mode();
	_bool Is_LockOn_Mode() const { return m_bLockOnMode; }
	class CMonster* Get_LockOn_Target() const { return m_pLockOnTarget; }

private:
	// 0. Update_LockOn Camera
	void Update_LockOn_Camera(_float fTimeDelta);

	// 1. 기존 LockOn Target 제거 및 사용 상태 검증(켜기 끄기 시 수행)
	void Clear_LockOn_Target();
	void Enable_LockOn_Mode();
	void Disable_LockOn_Mode();

	// 2. 근처에 Monster 있는지 탐색. LockOnTarget 설정.
	_bool Try_LockOn_Closest_Monster();
	class CMonster* Find_Closest_Monster(_float fMaxDistance = 50.0f);
	void Set_LockOn_Target(class CMonster* pTarget); 

	// 3. LockOn시 Camera가 있어야하는 위치 계산
	void Calculate_LockOn_Camera_Position(_float fTimeDelta);

	// 4. LockOn UI 관리
	void Update_LockOn_UI(_float fTimeDelta);

private:
	class CMonster* m_pLockOnTarget = { nullptr };// LockOn 타겟
	class CLockOnUI* m_pLockOnUI = { nullptr };   // LockOn UI
	_float4 m_vLockOnOffset = {};				  // 락온 모드 전용 오프셋 (더 멀리)
	_bool m_bLockOnMode = { false };              // LockOn 모드 활성화 여부
	_float m_fLockOnSmoothSpeed = 4.0f;           // LockOn 시 카메라 부드러움 (더 빠르게)
	_float m_fMaxLockOnDistance = 50.0f;          // 최대 락온 가능 거리
	_float4 m_vLockOnCameraPos = {};              // LockOn 카메라 목표 위치
	_float m_fLockOnYaw = 0.0f;                   // LockOn 시 Yaw 각도

	// 락온 해제시 부드러운 전환용
	_float m_fTransitionTime = {};       // 전환 진행 시간
	_float m_fTransitionStartYaw = {};   // 전환 시작시 Yaw

#pragma endregion

#pragma region 99. 디버그 용도
public:
	void Debug_CameraVectors();
#pragma endregion

public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};
NS_END

