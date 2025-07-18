#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& Prototype) = delete;
	virtual ~CTransform() = default;

public:	
	
	_vector Get_State(STATE eState) const {
		return XMLoadFloat4(reinterpret_cast<const _float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]));
	}	

	_float3 Get_Scaled() const {
		return _float3(
			XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))), 
			XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
			XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
		);
	}

	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]), vState);
	}

	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	

	_matrix Get_WorldMatrix_Inverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);

public:
	HRESULT Bind_Shader_Resource(class CShader* pShader, const _char* pConstantName);




#pragma region 헬퍼 기능 - 외부 툴 연계
public:
	void Editor();
#pragma endregion

public:
	void Scale(_float3 vScale);
	void Scaling(_float3 vScale);
	void Go_Straight(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);

	void Rotation(_fvector vAxis, _float fRadian);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void LookAt(_fvector vAt);
	void Set_Rotation(_float3 vAngle);


	void Chase(_fvector vTargetPos, _float fTimeDelta, _float fLimit = 0.f);

public:
	void Update_Transform();
	void Set_Parent(CTransform* pTarget);

private:
	CTransform*		m_pParent = { nullptr };
	_float4x4		m_WorldMatrix = {};
	_float			m_fSpeedPerSec = {};
	_float			m_fRotationPerSec = {};

	_float3	m_vAngle = {};

	_float m_fPitch = {}; // X축 기준으로의 회전.
	_float m_fYaw = {};   // Y축 기준으로의 회전이다.
	_float m_fRoll = {};  // Z축 기죽으로의 회전.
	

	// 외부 변수


public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) { return nullptr; };
	virtual void Free() override;
};

NS_END