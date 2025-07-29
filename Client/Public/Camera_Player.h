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

private:
	void Update_Chase_Target(_float fTimeDelta);

	

private:
	class CPlayer_Body* m_pBodyTarget = {};
	class CPlayer* m_pTarget = {};
	_float m_fMouseSensor = {};
	_float4 m_vTargetOffset = {};
	_float m_fYaw = 0.f;
	_float m_fPitch = 0.f; // 필요 

private:
	// 부드러운 추적을 위한 새로운 멤버 변수들
	_float4 m_vCurrentCameraPos = {};    // 현재 카메라 위치
	_float4 m_vTargetCameraPos = {};     // 목표 카메라 위치
	_float m_fSmoothSpeed = 5.0f;        // 추적 속도 (높을수록 빠름)
	_bool m_bFirstUpdate = true;         // 첫 번째 업데이트 체크

private:
	_float4 m_vOriginalOffset;      // 원래 오프셋을 저장
	_float4 m_vZoomTargetOffset;    // 줌인 시 목표 오프셋
	_bool m_bIsZooming;             // 줌 상태 플래그
	_float m_fZoomLerpTime;         // 줌 보간 시간
	_float m_fZoomMaxTime;          // 줌 전체 시간
	_bool m_bZoomIn;                // 줌인 중인지 줌아웃 중인지


public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};
NS_END

