#pragma once

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagRectInstanceDesc : public CVIBuffer_Instance::INSTANCE_DESC
	{
	}RECTINSTANCE_DESC;

private:
	CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& Prototype);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END