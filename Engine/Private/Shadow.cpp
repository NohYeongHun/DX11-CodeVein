#include "Shadow.h"
CShadow::CShadow()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	ZeroMemory(&m_Matrices, sizeof(m_Matrices));
	Safe_AddRef(m_pGameInstance);
}

const _float4x4* CShadow::Get_Transform_Float4x4(D3DTS eTransformState) const
{
	return &m_Matrices[ENUM_CLASS(eTransformState)];
}

HRESULT CShadow::Initialize(_uint iWinSizeX, _uint iWinSizeY)
{
	m_fViewportWidth = iWinSizeX;
	m_fViewportHeight = iWinSizeY;

	return S_OK;
}

HRESULT CShadow::Ready_ShadowLight(SHADOW_LIGHT_DESC LightDesc)
{
	m_ShadowLightDesc = LightDesc;

	XMStoreFloat4x4(&m_Matrices[ENUM_CLASS(D3DTS::VIEW)],
		XMMatrixLookAtLH(XMLoadFloat4(&LightDesc.vEye), XMLoadFloat4(&LightDesc.vAt), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	XMStoreFloat4x4(&m_Matrices[ENUM_CLASS(D3DTS::PROJ)],
		XMMatrixPerspectiveFovLH(LightDesc.fFovy, m_fViewportWidth / m_fViewportHeight, LightDesc.fNear, LightDesc.fFar));

	return S_OK;
}

void CShadow::Update()
{
	/*_vector vCameraPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());

	SHADOW_LIGHT_DESC ShadowLightDesc{};
	_float4 vEye, vAt;
	XMStoreFloat4(&vEye, vCameraPos + XMVectorSet(100.f, 100.f, 0.f, 0.f));
	XMStoreFloat4(&vAt, vCameraPos);

	ShadowLightDesc.vEye = vEye;
	ShadowLightDesc.vAt = vAt;
	ShadowLightDesc.fFovy = XMConvertToRadians(60.f);
	ShadowLightDesc.fNear = 0.1f;
	ShadowLightDesc.fFar = 1000.f;

	m_pGameInstance->Ready_ShadowLight(ShadowLightDesc);*/
	



}
#ifdef _DEBUG
void CShadow::Render()
{
	
}
#endif // _DEBUG





CShadow* CShadow::Create(_uint iWinSizeX, _uint iWinSizeY)
{
	CShadow* pInstance = new CShadow();

	if (FAILED(pInstance->Initialize(iWinSizeX, iWinSizeY)))
	{
		MSG_BOX(TEXT("Failed to Created : CShadow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShadow::Free()
{
	CBase::Free();
	Safe_Release(m_pGameInstance);


}
