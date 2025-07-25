﻿#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4	vEye, vAt;
		_float fFovy{}, fNear{}, fFar{};
	}CAMERA_DESC;

protected:
	explicit CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;

public:
	const _vector Get_LookVector();
	const _vector Get_RightVector();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


public:
	void Update_PipeLines();

protected:
	_float m_fFovy{};
	_float m_fAspect{};
	_float m_fNear{}, m_fFar{};


public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free();
};
NS_END

