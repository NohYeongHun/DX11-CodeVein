#pragma once

NS_BEGIN(Client)
class CCamera_Player final : public CCamera
{
public:
	typedef struct tagCameraPlayerDesc : public CCamera::CAMERA_DESC
	{
		_float fMouseSensor{};
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


private:
	_float m_fMouseSensor = {};

public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};
NS_END

