#pragma once

NS_BEGIN(Client)
class CCamera_Action final : public CCamera
{
public:
	typedef struct tagCameraActionDesc : public CCamera::CAMERA_DESC
	{
		_float fMouseSensor{};
		class CPlayer* pTarget = {};
	}CAMERA_ACTION_DESC;

private:
	explicit CCamera_Action(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCamera_Action(const CCamera_Action& Prototype);
	virtual ~CCamera_Action() = default;

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
	void Debug_CameraVectors();
	

#pragma region SpringArm 기본 설정
private:
	class CPlayer* m_pTarget = {};
	_float m_fMouseSensor = {};

	// SpringArm 핵심 멤버들
	_float m_fArmLength = 5.0f;            // SpringArm 길이
	_float m_fTargetArmLength = 5.0f;      // 목표 SpringArm 길이
	_float m_fMinArmLength = 2.0f;         // 최소 거리
	_float m_fMaxArmLength = 10.0f;        // 최대 거리

	_float4 m_vSocketOffset = { 0.f, 2.5f, 0.f, 0.f };  // 타겟으로부터의 오프셋
	_float4 m_vOriginalSocketOffset;                      // 원본 소켓 오프셋

	_float m_fYaw = 0.f;
	_float m_fPitch = 0.f;

	// 충돌 검사
	_bool m_bDoCollisionTest = true;
	_float m_fProbeSize = 0.3f;           // 충돌 검사용 구체 크기

	// 부드러운 추적 관련
	_float4 m_vCurrentCameraPos = {};      // 현재 카메라 위치
	_float4 m_vDesiredCameraPos = {};      // 원하는 카메라 위치
	_bool m_bFirstUpdate = true;

	// 플레이어 움직임 추적 설정 (최우선)
	_float m_fCameraLagSpeed = 25.0f;          // 기본 추적 속도 (높을수록 빠름)
	_float m_fCameraLagMaxTimeStep = 1.0f / 60.0f; // 최대 시간 단계
	_float m_fMovementThreshold = 0.01f;        // 움직임 감지 임계값 (더 민감하게)
	_float m_fStopThreshold = 0.005f;           // 정지 감지 임계값 (더 민감하게)
	_bool m_bPlayerWasMoving = false;          // 이전 프레임에서 플레이어가 움직였는지
	_float m_fPlayerStopTime = 0.0f;           // 플레이어가 정지한 시간
#pragma endregion

#pragma region LockOn 시스템
public:
	// 락온 관련 함수들
	void ToggleLockOn();
	void SetLockOnTarget(class CGameObject* pTarget);
	void ClearLockOnTarget();
	class CGameObject* GetLockOnTarget() const { return m_pLockOnTarget; }
	_bool IsLockOnActive() const { return m_pLockOnTarget != nullptr; }

	// 락온 타겟 검색
	class CGameObject* FindNearestLockOnTarget();
	class CGameObject* FindNextLockOnTarget(_bool bClockwise = true);

private:
	// 락온 관련 멤버 변수들
	class CGameObject* m_pLockOnTarget = nullptr;
	_float m_fLockOnRange = 15.0f;             // 락온 가능 거리
	_float m_fLockOnAngle = 60.0f;             // 락온 가능 각도 (도 단위)
	_float m_fLockOnTransitionSpeed = 8.0f;    // 락온 전환 속도
	_float4 m_vLockOnOffset = { 0.f, 1.5f, 0.f, 0.f }; // 락온 시 타겟 오프셋

	// 락온 상태에서의 카메라 설정
	_float m_fLockOnCameraDistance = 4.0f;     // 락온 시 카메라 거리
	_float m_fLockOnCameraHeight = 2.0f;       // 락온 시 카메라 높이

	// 락온 타겟 후보 리스트 (필요시)
	vector<class CGameObject*> m_LockOnCandidates;
	_int m_iCurrentLockOnIndex = -1;
#pragma endregion

#pragma region 줌 인/아웃 (기존 유지)
public:
	void Reset_Zoom();
	void Start_Zoom_In(_float fZoomDuration = 0.3f);
	void Start_Zoom_Out(_float fZoomDuration = 0.3f);
	void Update_Zoom(_float fTimeDelta);

private:
	_float4 m_vZoomTargetOffset;
	_bool m_bIsZooming = false;
	_float m_fZoomLerpTime = 0.f;
	_float m_fZoomMaxTime = 0.3f;
	_bool m_bZoomIn = false;
#pragma endregion

#pragma region 카메라 업데이트 함수들
private:
	void Update_SpringArm_Camera(_float fTimeDelta);       // SpringArm 방식 카메라 업데이트
	void Update_LockOn_Camera(_float fTimeDelta);          // 락온 카메라 업데이트
	void Update_Camera_Collision();                        // 카메라 충돌 검사
	void Update_Camera_Smoothing(_float fTimeDelta);       // 부드러운 카메라 이동
	void Update_Smooth_Movement(_float fTimeDelta);

	// 보조 함수들
	_vector Calculate_SpringArm_Position();                // SpringArm 위치 계산
	_vector Calculate_LockOn_Position();                   // 락온 시 카메라 위치 계산
	_bool Is_Player_Moving();                             // 플레이어 움직임 검사
	_float Calculate_Dynamic_LagSpeed(_float fTimeDelta); // 동적 추적 속도 계산
#pragma endregion

public:
	static CCamera_Action* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};
NS_END