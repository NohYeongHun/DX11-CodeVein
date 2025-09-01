﻿#pragma once

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Point_Instance final :public CVIBuffer_Instance
{
public:
	/* 파티클 인스턴싱을 위해서만 필요한 데이터.  */
	typedef struct tagPointInstanceDesc : public CVIBuffer_Instance::INSTANCE_DESC
	{
		_float3			vPivot;
		_float2			vSpeed;
		_float2			vLifeTime;
		_bool			isLoop;
	}POINT_INSTANCE_DESC;

private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

public:
	void Create_Buffer();

#pragma region 정점 버퍼를 움직여서 효과를 주는 함수들
public:
	void Spread(_float fTimeDelta);
	void Drop(_float fTimeDelta);
#pragma endregion



private:
	_float3	m_vPivot = {};
	_float* m_pSpeeds = {};
	_bool	m_isLoop = {};

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};
NS_END

