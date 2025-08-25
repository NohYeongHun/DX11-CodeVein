#pragma once

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagRectInstanceDesc : public CVIBuffer_Instance::INSTANCE_DESC
	{
		_float3			vPivot;
		_float2			vSpeed;
		_float2			vLifeTime;
		_bool			isLoop;
	}RECT_INSTANCE_DESC;

private:
	CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& Prototype);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

#pragma region 렌더링용 효과 함수.
public:
	void Spread(_float fTimeDelta);
	void Drop(_float fTimeDelta);
#pragma endregion



private:
	_float3	m_vPivot = {};
	_float* m_pSpeeds = {};
	_bool	m_isLoop = {};

public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END