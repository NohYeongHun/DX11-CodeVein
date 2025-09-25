#pragma once
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

#pragma region 정보 가져오기.
public:
	const _float Get_FovY() { return m_fFovy; }
	const _float Get_Aspect() { return m_fAspect; }
	const _float Get_Near() { return m_fNear; }
	const _float Get_Far() { return m_fFar; }

public:
	_bool Is_In_Camera_Frustum(_vector vWorldPos) const;
	_float Get_Screen_Distance_From_Center(_vector vWorldPos) const;
#pragma endregion


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	// 쉐이킹 시작 함수
	virtual void StartShake(_float duration, _float magnitude) {};

protected:
	_float m_fShakeTime = 0.f; // 쉐이킹 지속시간 (남은 시간)
	_float m_fShakeDuration = 0.f; // 쉐이킹 지속시간 (남은 시간)
	_float m_fShakeMagnitude = 0.f; // 쉐이킹 강도

public:
	void Update_PipeLines();

protected:
	_float m_fFovy{};
	_float m_fAspect{};
	_float m_fNear{}, m_fFar{};


public:
	virtual void Play_CutScene(class CTransform* pTransform, const _float4x4* pSocketMatrix) {};

protected:
	_bool m_IsPlayCutScene = { };


public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free();
};
NS_END

