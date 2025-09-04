#include "Light.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
    m_LightDesc = LightDesc;
    return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{

	_uint		iPassIndex = { 1 };

	if (LIGHT_DESC::DIRECTIONAL == m_LightDesc.eType)
	{
		iPassIndex = static_cast<_uint>(RENDERER_SHADERTYPE::DIRECTIONAL);

		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
	}
	else if (LIGHT_DESC::POINT == m_LightDesc.eType)
	{
		iPassIndex = static_cast<_uint>(RENDERER_SHADERTYPE::POINT);
	}

	pShader->Begin(iPassIndex);

	pVIBuffer->Bind_Resources();
	pVIBuffer->Render();

    return S_OK;
}

CLight* CLight::Create(const LIGHT_DESC& LightDesc)
{
    CLight* pInstance = new CLight();
    if (FAILED(pInstance->Initialize(LightDesc)))
    {
        MSG_BOX(TEXT("Create Failed Light"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLight::Free()
{
    CBase::Free();
}
