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
	_float Get_Yaw() const;
	void Set_Yaw(_float fYaw);
	void Add_Yaw(_float fYawDelta);
	void Set_TargetOffset(_float4 vOffset);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


private:
	class CPlayer* m_pTarget = {};
	_float m_fMouseSensor = {};
	_float4 m_vTargetOffset = {};
	_float m_fYaw = 0.f;
	_float m_fPitch = 0.f; // 필요 

public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};
NS_END

