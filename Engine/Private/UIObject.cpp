#include "UIObject.h"
#include "Transform.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
	: CGameObject( Prototype )
	, m_RenderMatrix { Prototype.m_RenderMatrix }
{
}

HRESULT CUIObject::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_RenderMatrix, XMMatrixIdentity());
	return S_OK;
}

HRESULT CUIObject::Initialize_Clone(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	// 문제.. Scale값이 m_fSizeX, m_fSizeY 값으로 고정되어버림.

	/* 정해놓은 상태대로 유아이를 그리기위해서 월드행렬의 상태를 조절한다.(Transform) */
	/* 뷰, 투영행렬을 직교투영에 맞게끔 생성한다.(Interface) */

	/*D3DXMatrixOrthoLH();*/

	D3D11_VIEWPORT			Viewport{};
	_uint			iNumViewports = { 1 };

	m_pContext->RSGetViewports(&iNumViewports, &Viewport);	

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.0f, 1.f));

	m_iWinSizeX = Viewport.Width;
	m_iWinSizeY = Viewport.Height;


	return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{
	for (auto& pChild : m_Children)
		pChild->Priority_Update(fTimeDelta);
}

void CUIObject::Update(_float fTimeDelta)
{
	// 부모 Transform 갱신 이후에 자식 객체 Update()
	Update_Transform();

	for (auto* child : m_Children)
		child->Update(fTimeDelta);
}

void CUIObject::Late_Update(_float fTimeDelta)
{
	for (auto& pChild : m_Children)
		pChild->Late_Update(fTimeDelta);
	
}

HRESULT CUIObject::Render()
{
	/*for (auto& pChild : m_Children)
		pChild->Render();*/

	return S_OK;
}

_fmatrix CUIObject::Get_RenderMatrix()
{
	return XMLoadFloat4x4(&m_RenderMatrix);
}

void CUIObject::AddChild(CUIObject* pChild)
{
	auto iter = std::find(m_Children.begin(), m_Children.end(), pChild);
	if (iter == m_Children.end())
	{
		pChild->Set_Parent(this);
		m_Children.push_back(pChild);
	}
}

void CUIObject::RemoveChild(CUIObject* pChild)
{
	auto iter = std::find(m_Children.begin(), m_Children.end(), pChild);
	if (iter != m_Children.end())
	{
		(*iter)->Set_Parent(nullptr); // optional: 부모 끊기
		m_Children.erase(iter);
	}
}

void CUIObject::Set_Parent(CUIObject* pParent)
{
	m_pParent = pParent;
}

void CUIObject::Update_Transform()
{
	// 항상 항등으로 초기화합니다.
	XMStoreFloat4x4(&m_RenderMatrix, XMMatrixIdentity());

	_matrix matWorld{};
	// 크기 곱해주기.

	// 위치 설정.
	if (m_pParent)
		// 부모객체가 있는 경우는 부모 객체 기준으로 이동행렬 설정.
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	else
		// 최상위 부모 객체의 경우에는 Window 좌표계 대로 설정.
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - m_iWinSizeX * 0.5f, -m_fY + m_iWinSizeY * 0.5f, 0.f, 1.f));

	/* 로컬 행렬 생성. */
	matWorld = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix());

	if (m_pParent)
		XMStoreFloat4x4(&m_RenderMatrix, matWorld * m_pParent->Get_RenderMatrix());
	else
		// 크기가 곱해지지 않은 행렬 반환.
		XMStoreFloat4x4(&m_RenderMatrix, matWorld);
}

/* 크기는 따로 곱해줘야 한다. */
HRESULT CUIObject::Begin()
{
	_matrix matScale{}, matWorld{};
	matScale = XMMatrixScaling(m_fSizeX, m_fSizeY, 1.f);
	matWorld = matScale * XMLoadFloat4x4(&m_RenderMatrix);

	XMStoreFloat4x4(&m_RenderMatrix, matWorld);


	/* Alpha Blend On, DepthStencil Off*/
	m_pGameInstance->Apply_BlendeState();
	m_pGameInstance->Apply_DepthStencilOff();


	return S_OK;
}

HRESULT CUIObject::End()
{
	/* Defautl Setting 설정 */
	m_pGameInstance->Apply_DefaultStates();

	return S_OK;
}

void CUIObject::Destroy()
{
	__super::Destroy();
}

void CUIObject::Free()
{
	__super::Free();
	for (auto& pChild : m_Children)
		Safe_Release(pChild);

	m_Children.clear();
	m_pParent = nullptr;
}
