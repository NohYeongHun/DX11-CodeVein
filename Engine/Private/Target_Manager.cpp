CTarget_Manager::CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTarget_Manager::Initialize()
{
	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	if (nullptr != Find_RenderTarget(strTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iSizeX, iSizeY, ePixelFormat, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_ShaderResource(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CTarget_Manager::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>		MRTList;
		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(strMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool bClear)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

	ASSERT_CRASH(pMRTList);

	Safe_Release(m_pBackBuffer);
	Safe_Release(m_pOriginalDSV);


	m_pContext->OMGetRenderTargets(1, &m_pBackBuffer, &m_pOriginalDSV);

	_uint		iNumRenderTargets = {};

	ID3D11RenderTargetView* RenderTargets[8] = { nullptr };

	for (auto& pRenderTarget : *pMRTList)
	{
		// 그리기 전 Clear
		if (bClear)
			pRenderTarget->Clear();
		RenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV();
	}


	

	if (nullptr != pDSV)
		m_pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	ID3D11DepthStencilView* pFinalDSV = pDSV;

	// 2. 만약 함수 인자로 DSV가 들어오지 않았다면, 기존의 DSV를 사용하도록 설정합니다.
	if (nullptr == pFinalDSV)
		pFinalDSV = m_pOriginalDSV;

	// 3. 특정 후처리 패스일 경우, 위 결정과 상관없이 DSV를 무조건 nullptr로 덮어씁니다.
	if (strMRTTag == TEXT("MRT_BloomBlurX") ||
		strMRTTag == TEXT("MRT_BloomBlurY") ||
		strMRTTag == TEXT("MRT_BrightPass"))
	{
		pFinalDSV = nullptr;
	}

	// 4. 최종적으로 결정된 pFinalDSV를 사용하여 렌더 타겟을 한 번만 설정합니다.
	m_pContext->OMSetRenderTargets(iNumRenderTargets, RenderTargets, pFinalDSV);



	
	return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
	m_pContext->OMSetRenderTargets(1, &m_pBackBuffer, m_pOriginalDSV);

	//Safe_Release(m_pOriginalDSV);
	//Safe_Release(m_pBackBuffer);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CTarget_Manager::Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
	{
		CRASH("Failed Ready Debug");
		return E_FAIL;
	}
		

	return  pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CTarget_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
		{
			if (nullptr != pRenderTarget)
				pRenderTarget->Render(pShader, pVIBuffer);
		}
	}

	return S_OK;
}
#endif // _DEBUG



CRenderTarget* CTarget_Manager::Find_RenderTarget(const _wstring& strTargetTag)
{
	auto	iter = m_RenderTargets.find(strTargetTag);
	if (iter == m_RenderTargets.end())
		return nullptr;


	return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _wstring& strMRTTag)
{
	auto	iter = m_MRTs.find(strMRTTag);
	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

CTarget_Manager* CTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTarget_Manager* pInstance = new CTarget_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CTarget_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTarget_Manager::Free()
{
	CBase::Free();
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);
		Pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);
	m_RenderTargets.clear();

	Safe_Release(m_pBackBuffer);
	Safe_Release(m_pOriginalDSV);
	

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
