#pragma once
#include "GameObject.h"

/* Map Tool 상에서 배치하고 생성하고 수정하는 Class */
/* Client용 Class가 하나 더 필요함. */
NS_BEGIN(Tool)
class CToolMap_Part final : public CGameObject
{
public:
	/* 특정한 상황에 구조체가 달라져야합니다. */
	enum class ARG_TYPE : uint32_t
	{
		CREATE,
		MODEL_LOAD
	};

	typedef struct tagMapPartDesc
	{
		ARG_TYPE eArgType = {};
		void* pData = {};
	}MAP_PART_DESC;

private:
	CToolMap_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolMap_Part(const CToolMap_Part& Prototype);
	virtual ~CToolMap_Part() = default;
	
/* 충돌된 Map Part는 Imgui에서 조작할 수 있는 Transform 주소를 반환합니다. */
public:
	//const MAP_PART_INFO& Save_NonAminModel(_fmatrix PreTransformMatrix);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);

	virtual HRESULT Initialize_Craete(MODEL_CREATE_DESC* pDesc);

	virtual HRESULT Initialize_Load(MAP_PART_INFO* pDesc);

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

#pragma region 충돌 함수 정의
public:
	virtual void On_Collision_Enter(CGameObject* pOther);
	virtual void On_Collision_Stay(CGameObject* pOther);
	virtual void On_Collision_Exit(CGameObject* pOther);

	/* Ray에 맞았는지 확인. */
	virtual const _bool Is_Ray_LocalHit(_float3* pOutLocalPos, _float3* pOutLocalNormal, _float* pOutDist) override;
	virtual const _bool Is_Ray_LocalHit(MODEL_PICKING_INFO* pPickingInfo, _float* pOutDist) override;

#pragma endregion


private:
	class CTool_Model* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	_wstring m_strModelTag = {};
	_wstring m_PartName = {  };
	int m_iPartID = {};


private:
	HRESULT Ready_Components();
	HRESULT Ready_Transform(MODEL_CREATE_DESC* pDesc);
	HRESULT Ready_Transform(MAP_PART_INFO* pDesc);
	HRESULT Ready_Render_Resources();

public:
	static CToolMap_Part* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Destroy();
	virtual void Free() override;

};
NS_END

