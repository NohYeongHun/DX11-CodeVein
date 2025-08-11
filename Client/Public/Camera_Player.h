#pragma once

NS_BEGIN(Client)
class CCamera_Player final : public CCamera
{
public:
	typedef struct tagCameraPlayerDesc : public CCamera::CAMERA_DESC
	{
		_float fMouseSensor{};
		class CPlayer_Body* pBodyTarget = {};
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

public:
	_float Get_Yaw() const;
	void Set_Yaw(_float fYaw);
	void Add_Yaw(_float fYawDelta);
	void Set_TargetOffset(_float4 vOffset);

public:
	// 줌 관련 함수들
	void Start_Zoom_In(_float fZoomDuration = 0.3f);
	void Start_Zoom_Out(_float fZoomDuration = 0.3f);
	void Update_Zoom(_float fTimeDelta);
	void Reset_Zoom();

public:
	void Debug_CameraVectors();

#pragma region 보간 부드럽게
private:
	void Handle_Mouse_Input(_float fTimeDelta, _float fSensitivityMultiplier = 1.0f);
#pragma endregion


#pragma region Target Lock On
public:
	void Set_LockOn_Target(class CMonster* pTarget);
	void Clear_LockOn_Target();
	void Enable_LockOn_Mode();
	void Disable_LockOn_Mode();
	void Update_LockOn_Camera(_float fTimeDelta);
	void Update_Transition(_float fTimeDelta);
	_bool Is_LockOn_Mode() const { return m_bLockOnMode; }
	class CMonster* Get_LockOn_Target() const { return m_pLockOnTarget; }
	
	// LockOn UI 관리
	void Update_LockOn_UI(_float fTimeDelta);

	// 가장 가까운 적 탐지
	class CMonster* Find_Closest_Monster(_float fMaxDistance = 50.0f);
	_bool Try_LockOn_Closest_Monster();
	void Toggle_LockOn_Mode();

	// 마우스 커서 제한 관리
	void Enable_Mouse_Clip();
	void Disable_Mouse_Clip();
	void Update_Mouse_Clip();


private:
	// ✨ LockOn 관련 멤버 변수들 추가
	class CMonster* m_pLockOnTarget = { nullptr };    // LockOn 타겟
	_bool m_bLockOnMode = { false };               // LockOn 모드 활성화 여부
	class CLockOnUI* m_pLockOnUI = { nullptr };    // LockOn UI

	// LockOn 카메라 설정값들
	_float m_fLockOnDistance = 10.0f;               // LockOn 시 카메라 거리
	_float m_fLockOnHeight = 5.f;                 // LockOn 시 카메라 높이
	_float m_fLockOnSmoothSpeed = 4.0f;           // LockOn 시 카메라 부드러움 (더 빠르게)
	_float m_fLockOnYawSpeed = 2.0f;              // LockOn 시 Yaw 회전 속도

	// 화면 중심 락온 탐지 설정값들
	_float m_fMaxLockOnDistance = 50.0f;          // 최대 락온 가능 거리
	_float m_fScreenCenterThreshold = 0.3f;       // 화면 중심 기준 락온 임계값 (0~1)

	// LockOn 카메라 위치 계산용
	_float4 m_vLockOnCameraPos = {};              // LockOn 카메라 목표 위치
	_float m_fLockOnYaw = 0.0f;                   // LockOn 시 Yaw 각도
	_float m_fLockOnPitch = 15.0f;                // LockOn 시 Pitch 각도 (자연스러운 각도)

	void Update_Chase_Target(_float fTimeDelta);
	void Calculate_LockOn_Camera_Position(_float fTimeDelta);  // LockOn 카메라 위치 계산
	//void Update_Normal_Camera(_float fTimeDelta);    // 일반 카메라 모드

#pragma endregion


private:
	class CPlayer_Body* m_pBodyTarget = {};
	class CPlayer* m_pTarget = {};
	_float m_fMouseSensor = {};
	_float4 m_vTargetOffset = {};
	_float4 m_vLockOnOffset = {};         // 락온 모드 전용 오프셋 (더 멀리)
	_float m_fYaw = 0.f;
	_float m_fPitch = 0.f;
	
	// 부드러운 회전을 위한 목표 각도들
	_float m_fTargetYaw = 0.f;
	_float m_fTargetPitch = 0.f;
	_float m_fRotationSmoothSpeed = 8.0f; // 회전 부드러움 속도
	
	// Pitch 조작 모드
	_bool m_bPitchControlMode = false;    // Q키를 누르고 있을 때만 true
	_float m_fDefaultPitch = 0.0f;       // 기본 Pitch 각도 (도 단위)
	
	// 마우스 커서 제한
	_bool m_bMouseClipped = false;        // 마우스 클립 상태
	RECT m_rcClipRect = {};              // 클립 영역
	
	// Pitch 제한값들 (자연스러운 카메라 각도 범위)
	_float m_fMaxPitch = 10.0f;    // 최대 위쪽 각도 
	_float m_fMinPitch = -10.0f;   // 최소 각도 (땅바닥 관통 방지) 

private:
	// 부드러운 추적을 위한 새로운 멤버 변수들
	_float4 m_vCurrentCameraPos = {};    // 현재 카메라 위치
	_float4 m_vTargetCameraPos = {};     // 목표 카메라 위치
	_float m_fSmoothSpeed = 200.0f;        // 추적 속도 (높을수록 빠름)
	_bool m_bFirstUpdate = true;         // 첫 번째 업데이트 체크

private:
	_float4 m_vOriginalOffset = {};      // 원래 오프셋을 저장
	_float4 m_vZoomTargetOffset = {};    // 줌인 시 목표 오프셋
	_bool m_bIsZooming = {};             // 줌 상태 플래그
	_float m_fZoomLerpTime = {};         // 줌 보간 시간
	_float m_fZoomMaxTime = {};          // 줌 전체 시간
	_bool m_bZoomIn = {};                // 줌인 중인지 줌아웃 중인지

	// 락온 해제시 부드러운 전환용
	_bool m_bLockOnTransitioning = {};   // 락온 해제 전환 중인지
	_float m_fTransitionTime = {};       // 전환 진행 시간
	_float m_fTransitionDuration = 0.5f; // 전환 총 시간
	_float4 m_vTransitionStartOffset = {}; // 전환 시작시 오프셋
	_float m_fTransitionStartYaw = {};   // 전환 시작시 Yaw
	
	// 카메라 모드 전환용 변수들
	_bool m_bTransitioning = {};         // 전환 중인지 여부
	_float m_fMaxTransitionTime = {};    // 최대 전환 시간
	_float m_fTransitionStartPitch = {}; // 전환 시작시 Pitch
	_float4 m_vTransitionStartPos = {};  // 전환 시작시 카메라 위치


public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};
NS_END

