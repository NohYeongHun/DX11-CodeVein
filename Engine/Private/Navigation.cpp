#include "Navigation.h"

_float4x4 CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent( Prototype )
	, m_Cells{ Prototype.m_Cells }
#ifdef _DEBUG	
	, m_pShader{ Prototype.m_pShader }
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG	
	Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationFilePath)
{
	_ulong			dwByte = { };
	HANDLE			hFile = CreateFile(pNavigationFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	while (true)
	{
		_float3		vPoints[3];

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints);
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize_Clone(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

	m_iCurrentCellIndex = pDesc->iCurrentCellIndex;

	return S_OK;
}

void CNavigation::Update(_fmatrix WorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);

}

_bool CNavigation::isMove(_fvector vPosition)
{
	_int		iNeighborIndex = -1;

	if (m_iCurrentCellIndex == -1)
		return false;

	/* 현재 존재하는 쎌안에서 움직였다. */
	if (true == m_Cells[m_iCurrentCellIndex]->isIn(vPosition, &iNeighborIndex, &m_vLastNormal))
		return true;

	/* 현재 존재하는 쎌을 벗어난다.  */
	else
	{
		/* 나간 방향에 이웃셀이 존재한다. */
		if (0 <= iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex, &m_vLastNormal))
					break;
			}

			m_iCurrentCellIndex = iNeighborIndex;
			return true;
		}

		/* 나간 방향에 이웃셀이 존재하지않는다. . */
		else
		{
			/*슬라이딩을 위한 리턴을 정의해도 된다. */
			return false;
		}
	}


	return false;
}

_float CNavigation::Compute_Height(_vector vPosition, _float fOffset)
{
	if (m_iCurrentCellIndex == -1)
		return XMVectorGetY(vPosition);


	_float3 cellA = m_Cells[m_iCurrentCellIndex]->Get_PointValue(CELLPOINT::A);
	_float3 cellB = m_Cells[m_iCurrentCellIndex]->Get_PointValue(CELLPOINT::B);
	_float3 cellC = m_Cells[m_iCurrentCellIndex]->Get_PointValue(CELLPOINT::C);

	_vector PointA = XMLoadFloat3(&cellA);
	PointA = XMVectorSetW(PointA, 1.f);
	_vector PointB = XMLoadFloat3(&cellB);
	PointB = XMVectorSetW(PointB, 1.f);
	_vector PointC = XMLoadFloat3(&cellC);
	PointC = XMVectorSetW(PointC, 1.f);

	_vector vPlane = XMPlaneFromPoints(PointA, PointB, PointC);

	_float		fHeight = (-XMVectorGetX(vPlane) * XMVectorGetX(vPosition) - XMVectorGetZ(vPlane) * XMVectorGetZ(vPosition) - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane) + fOffset;

	return fHeight;
}

_vector CNavigation::Get_CurrentCellCenter()
{
	return m_Cells[m_iCurrentCellIndex]->Get_Center();
}

HRESULT CNavigation::Add_Cell(_float3 vPointA, _float3 vPointB, _float3 vPointC)
{
    // 3개의 점으로 Cell 생성
    _float3 vPoints[3] = { vPointA, vPointB, vPointC };

    CCell* pNewCell = CCell::Create(m_pDevice, m_pContext, vPoints);
    if (nullptr == pNewCell)
    {
        MSG_BOX(TEXT("Failed to Create Cell"));
        return E_FAIL;
    }

    // 새 셀의 인덱스 설정 (CCell에 SetIndex 함수 필요하다면)
    // pNewCell->Set_Index(m_Cells.size());

    // 셀 벡터에 추가
    m_Cells.push_back(pNewCell);

    // 인접 관계 재설정
    Setup_Cell_Neighbors();

    return S_OK;
}

void CNavigation::Remove_Cell(_int iCellIndex)
{
    // 유효한 인덱스 확인
    if (iCellIndex < 0 || iCellIndex >= (_int)m_Cells.size())
        return;

    // 현재 셀이 삭제되는 경우 초기화
    if (m_iCurrentCellIndex == iCellIndex)
        m_iCurrentCellIndex = -1;
    // 현재 셀 인덱스가 삭제되는 셀보다 뒤에 있으면 조정
    else if (m_iCurrentCellIndex > iCellIndex)
        m_iCurrentCellIndex--;

    // 메모리 해제
    Safe_Release(m_Cells[iCellIndex]);

    // 벡터에서 제거
    m_Cells.erase(m_Cells.begin() + iCellIndex);

    // 인접 관계 재설정 (인덱스가 변경되었으므로)
    Setup_Cell_Neighbors();
}

_int CNavigation::Get_CellCount() const
{
    return static_cast<_int>(m_Cells.size());
}

_int CNavigation::Find_Cell_By_Position(_float3 vPosition)
{
    _vector vPos = XMLoadFloat3(&vPosition);

    // 모든 셀을 순회하면서 위치가 포함된 셀 찾기
    for (size_t i = 0; i < m_Cells.size(); ++i)
    {
        _int iNeighborIndex = -1;
        _float3 vNormal = {};

        if (m_Cells[i]->isIn(vPos, &iNeighborIndex, &vNormal))
        {
            return static_cast<_int>(i);
        }
    }

    return -1; // 어떤 셀에도 속하지 않음
}

_float3* CNavigation::Get_Cell_Points(_int iCellIndex)
{
    // 유효한 인덱스 확인
    if (iCellIndex < 0 || iCellIndex >= (_int)m_Cells.size())
        return nullptr;

    // 임시 배열에 셀의 점들 복사 (정적 변수 사용 - 주의: 스레드 안전하지 않음)
    static _float3 s_CellPoints[3];

    s_CellPoints[0] = m_Cells[iCellIndex]->Get_PointValue(CELLPOINT::A);
    s_CellPoints[1] = m_Cells[iCellIndex]->Get_PointValue(CELLPOINT::B);
    s_CellPoints[2] = m_Cells[iCellIndex]->Get_PointValue(CELLPOINT::C);

    return s_CellPoints;
}

HRESULT CNavigation::Export_To_File(const _tchar* pFilePath)
{
    if (m_Cells.empty())
    {
        MSG_BOX(TEXT("저장할 네비게이션 데이터가 없습니다."));
        return E_FAIL;
    }

    // 파일 생성
    HANDLE hFile = CreateFile(pFilePath, GENERIC_WRITE, 0, nullptr,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX(TEXT("파일 생성에 실패했습니다."));
        return E_FAIL;
    }

    _ulong dwByte = 0;

    // 헤더 정보 저장 (셀 개수)
    _uint iCellCount = static_cast<_uint>(m_Cells.size());
    WriteFile(hFile, &iCellCount, sizeof(_uint), &dwByte, nullptr);

    // 각 셀의 점들을 저장
    for (auto& pCell : m_Cells)
    {
        _float3 vPoints[3];
        vPoints[0] = pCell->Get_PointValue(CELLPOINT::A);
        vPoints[1] = pCell->Get_PointValue(CELLPOINT::B);
        vPoints[2] = pCell->Get_PointValue(CELLPOINT::C);

        WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
    }

    CloseHandle(hFile);

    // 성공 메시지
    /*_tchar szMessage[256];
    _stprintf_s(szMessage, TEXT("네비게이션 데이터가 저장되었습니다.\n파일: %s\n셀 개수: %d"),
        pFilePath, iCellCount);
    MSG_BOX(szMessage);*/

    return S_OK;
}

HRESULT CNavigation::Import_From_File(const _tchar* pFilePath)
{
    // 파일 열기
    HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX(TEXT("파일을 찾을 수 없습니다."));
        return E_FAIL;
    }

    // 기존 데이터 정리
    for (auto& pCell : m_Cells)
        Safe_Release(pCell);
    m_Cells.clear();
    m_iCurrentCellIndex = -1;

    _ulong dwByte = 0;

    // 헤더 정보 읽기 (셀 개수)
    _uint iCellCount = 0;
    ReadFile(hFile, &iCellCount, sizeof(_uint), &dwByte, nullptr);

    if (dwByte == 0)
    {
        CloseHandle(hFile);
        MSG_BOX(TEXT("잘못된 파일 형식입니다."));
        return E_FAIL;
    }

    // 각 셀 데이터 읽기
    for (_uint i = 0; i < iCellCount; ++i)
    {
        _float3 vPoints[3];
        ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

        if (dwByte == 0)
        {
            CloseHandle(hFile);
            MSG_BOX(TEXT("파일 읽기 중 오류가 발생했습니다."));
            return E_FAIL;
        }

        // 셀 생성
        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints);
        if (nullptr == pCell)
        {
            CloseHandle(hFile);
            MSG_BOX(TEXT("셀 생성에 실패했습니다."));
            return E_FAIL;
        }

        m_Cells.push_back(pCell);
    }

    CloseHandle(hFile);

    // 인접 관계 설정
    Setup_Cell_Neighbors();

    // 성공 메시지
    //_tchar szMessage[256];
    //_wstrprintf_s(szMessage, TEXT("네비게이션 데이터가 로드되었습니다.\n파일: %s\n셀 개수: %d"),
    //    pFilePath, iCellCount);
    //MSG_BOX(szMessage);

    return S_OK;
}

void CNavigation::Setup_Cell_Neighbors()
{
    // 모든 셀 쌍에 대해 인접 관계 확인
    for (size_t i = 0; i < m_Cells.size(); ++i)
    {
        for (size_t j = i + 1; j < m_Cells.size(); ++j)
        {
            if (Are_Cells_Adjacent(static_cast<_int>(i), static_cast<_int>(j)))
            {
                // 인접 관계 설정 로직
                // CCell 클래스에 SetUp_Neighbor 함수 사용
                // 어떤 모서리가 공유되는지 확인 필요
                Setup_Adjacent_Cells(static_cast<_int>(i), static_cast<_int>(j));
            }
        }
    }
}

_bool CNavigation::Are_Cells_Adjacent(_int iCellA, _int iCellB)
{
    if (iCellA < 0 || iCellA >= (_int)m_Cells.size() ||
        iCellB < 0 || iCellB >= (_int)m_Cells.size() ||
        iCellA == iCellB)
        return false;

    CCell* pCellA = m_Cells[iCellA];
    CCell* pCellB = m_Cells[iCellB];

    // 셀 A의 각 모서리에 대해 셀 B와 공유하는 모서리가 있는지 확인
    _float3 pointsA[3] = {
        pCellA->Get_PointValue(CELLPOINT::A),
        pCellA->Get_PointValue(CELLPOINT::B),
        pCellA->Get_PointValue(CELLPOINT::C)
    };

    _float3 pointsB[3] = {
        pCellB->Get_PointValue(CELLPOINT::A),
        pCellB->Get_PointValue(CELLPOINT::B),
        pCellB->Get_PointValue(CELLPOINT::C)
    };

    // 공유하는 점의 개수 확인 (2개 이상이면 인접)
    _int iSharedPoints = 0;
    const _float fEpsilon = 0.001f; // 부동소수점 오차 허용치

    for (_int i = 0; i < 3; ++i)
    {
        for (_int j = 0; j < 3; ++j)
        {
            _vector vPointA = XMLoadFloat3(&pointsA[i]);
            _vector vPointB = XMLoadFloat3(&pointsB[j]);

            // 거리 계산
            _vector vDiff = XMVectorSubtract(vPointA, vPointB);
            _float fDistance = XMVectorGetX(XMVector3Length(vDiff));

            if (fDistance < fEpsilon)
            {
                iSharedPoints++;
                break; // 같은 점을 중복으로 세지 않기 위해
            }
        }
    }

    return iSharedPoints >= 2; // 2개 이상의 점을 공유하면 인접
}

_wstring CNavigation::Get_Cell_Info(_int iCellIndex)
{
    if (iCellIndex < 0 || iCellIndex >= (_int)m_Cells.size())
        return L"Invalid Cell Index";

    CCell* pCell = m_Cells[iCellIndex];
    _float3 pointA = pCell->Get_PointValue(CELLPOINT::A);
    _float3 pointB = pCell->Get_PointValue(CELLPOINT::B);
    _float3 pointC = pCell->Get_PointValue(CELLPOINT::C);

    _tchar szInfo[512];
    swprintf_s(szInfo, L"Cell %d:\nA(%.2f, %.2f, %.2f)\nB(%.2f, %.2f, %.2f)\nC(%.2f, %.2f, %.2f)",
        iCellIndex,
        pointA.x, pointA.y, pointA.z,
        pointB.x, pointB.y, pointB.z,
        pointC.x, pointC.y, pointC.z);

    return _wstring(szInfo);
}

// ============================================================================
// 인접 셀 설정을 위한 보조 함수
// ============================================================================

void CNavigation::Setup_Adjacent_Cells(_int iCellA, _int iCellB)
{
    CCell* pCellA = m_Cells[iCellA];
    CCell* pCellB = m_Cells[iCellB];

    // 공유하는 모서리 찾기
    _float3 pointsA[3] = {
        pCellA->Get_PointValue(CELLPOINT::A),
        pCellA->Get_PointValue(CELLPOINT::B),
        pCellA->Get_PointValue(CELLPOINT::C)
    };

    _float3 pointsB[3] = {
        pCellB->Get_PointValue(CELLPOINT::A),
        pCellB->Get_PointValue(CELLPOINT::B),
        pCellB->Get_PointValue(CELLPOINT::C)
    };

    // 공유 모서리 찾기 및 설정
    for (_int lineA = 0; lineA < 3; ++lineA)
    {
        _int nextA = (lineA + 1) % 3;

        for (_int lineB = 0; lineB < 3; ++lineB)
        {
            _int nextB = (lineB + 1) % 3;

            // A셀의 lineA 모서리와 B셀의 lineB 모서리가 같은지 확인
            if (pCellA->Compare_Points(&pointsA[lineA], &pointsA[nextA]) &&
                pCellB->Compare_Points(&pointsA[lineA], &pointsA[nextA]))
            {
                // 인접 관계 설정
                pCellA->SetUp_Neighbor(static_cast<CCell::LINE>(lineA), pCellB);
                pCellB->SetUp_Neighbor(static_cast<CCell::LINE>(lineB), pCellA);
                return;
            }
        }
    }
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	m_pShader->Begin(0);

	for (auto& pCell : m_Cells)
		pCell->Render();

	return S_OK;
}

#endif

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationFilePath)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG	
	Safe_Release(m_pShader);
#endif
}
