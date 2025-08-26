﻿#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CParticle final : public CGameObject
{
public:
	typedef struct tagParicleDesc : CGameObject::GAMEOBJECT_DESC
	{
		LEVEL eCurLevel;
	}PARTICLE_DESC;

private:
	CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle(const CParticle& Prototype);
	virtual ~CParticle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect_Instance* m_pVIBufferCom = { nullptr };
	LEVEL m_eCurLevel = { LEVEL::END };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END

