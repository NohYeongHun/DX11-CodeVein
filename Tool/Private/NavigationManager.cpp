#include "NavigationManager.h"

CNavigationManager::CNavigationManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice } 
    , m_pContext { pContext }
    , m_pGameInstance{ CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}



HRESULT CNavigationManager::Initialize()
{
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        CRASH("Failed Create Sahder");
    

    return S_OK;
}

HRESULT CNavigationManager::Render()
{

    if (m_Cells.size() == 0)
        return S_OK;

    _float4x4			WorldMatrix;
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
    //WorldMatrix._42 += 0.1f;

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
    {
        CRASH("Failed Search ViewMatrix");
        return E_FAIL;
    }
        
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    _float4		vColor = _float4(0.f, 1.f, 0.f, 1.f);

    _int i = 0;

    
    for (auto& pCell : m_Cells)
    {
        
        if (nullptr != pCell)
        {
            if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
            {
                CRASH("Failed Bind Matrix World");
                return E_FAIL;
            }
                

            if (i == m_iSelectCellIndex)
                vColor = _float4(1.f, 0.f, 0.f, 1.f);
            else
                vColor = _float4(0.f, 1.f, 0.f, 1.f);

            m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));
            m_pShader->Begin(0);
            pCell->Render();
        }
        i++;
    }


    return S_OK;
}

CCell* CNavigationManager::Get_Cell()
{
    if (m_Cells.size() == 0 || m_iClickedCellIndex >= m_Cells.size())
        return nullptr;

    return m_Cells[m_iClickedCellIndex];
}

CCell* CNavigationManager::Get_Cell(_uint iIndex)
{
    if (m_Cells.size() == 0 || iIndex >= m_Cells.size())
        return nullptr;

    return m_Cells[iIndex];
}

CCell* CNavigationManager::Find_PickingCell(_vector vPickingPos)
{
    _float fMinDistance = FLT_MAX;
    CCell* pMinCell = nullptr;

    for (_uint i = 0; i < m_Cells.size(); ++i)
    {

        CCell* pCell = m_Cells[i];
        if (nullptr == pCell)
            continue;

        _vector vCenter = pCell->Get_Center();
        _float fDistance = XMVectorGetX(XMVector3Length(vCenter - vPickingPos));

        if (fMinDistance > fDistance)
        {
            fMinDistance = fDistance;
            pMinCell = m_Cells[i];
            m_iClickedCellIndex = i;
        }
    }

    if (fMinDistance != FLT_MAX)
    {
        return pMinCell;
    }
    return nullptr;
}

void CNavigationManager::Update_ClickedPosition(_float3 vClickedPoint)
{
    if (m_vClickedPoints.size() != 0)
    {
        m_vClickedPoints.back() = vClickedPoint;
        _vector vPosition = XMLoadFloat3(&vClickedPoint);
        vPosition = XMVectorSetW(vPosition, 1.f);
    }
}

void CNavigationManager::Clear_ClickedPosition()
{
    m_vClickedPoints.clear();
    m_fMinDistance = FLT_MAX;
}

HRESULT CNavigationManager::Add_Cell(_float3* pPoints, _bool bCheckOverlap)
{
    // 시계 방향으로 정리한다 0 1 2
    Sort_CellByPosition(pPoints);
    // 마우스 Lay와 Cell의 법선 벡터를 내적하여 2차 Sort를 한다.
    Sort_CellByDot(pPoints);

    if (Check_Cell(pPoints))
    {
        return E_FAIL;
    }

    // 추가한다.
    CCell* pCell = CCell::Create(m_pDevice, m_pContext, pPoints, (_int)m_Cells.size());
    if (nullptr == pCell)
    {
        return E_FAIL;
    }
        
    m_Cells.push_back(pCell);
    return S_OK;
}

void CNavigationManager::Sort_CellByPosition(_float3* pPoints)
{
    //Position 기준으로 1단계로 시계방향으로 정리한다 
    _float3 vTempPoss[3];

    memcpy(vTempPoss, pPoints, sizeof(_float3) * 3);

    //3개의 점을 순회해서 x 좌표가 가장 적은 인덱스를 찾는다.
    for (_uint i = 0; i < 2; ++i)
    {
        _uint iMinIndex = i;
        for (_uint j = i + 1; j < 3; ++j)
        {

            if (vTempPoss[iMinIndex].x > vTempPoss[j].x)
            {
                iMinIndex = j;
            }
        }

        _float3 Temp;
        Temp = vTempPoss[i];
        vTempPoss[i] = vTempPoss[iMinIndex];
        vTempPoss[iMinIndex] = Temp;
    }

    // 중간 x값을 찾는다. [1]  
    // 작은 x값을 찾는다. [0]
    // 만약 작은 x값의 z값이 중간 x값의 z값보다 작다면 반대로
    if (vTempPoss[0].z < vTempPoss[1].z)
    {
        pPoints[0] = vTempPoss[1];
        pPoints[1] = vTempPoss[2];
        pPoints[2] = vTempPoss[0];
    }
    else
    {
        pPoints[0] = vTempPoss[1];
        pPoints[1] = vTempPoss[0];
        pPoints[2] = vTempPoss[2];
    }
}

/* 면의 방향이 반대일때 뒤집어줍니다. */
void CNavigationManager::Sort_CellByDot(_float3* pPoints)
{
    //각 선분을 외적해서 면이 바라보는 방향벡터를 구한다.
    _vector vABDir = XMLoadFloat3(&pPoints[1]) - XMLoadFloat3(&pPoints[0]);
    _vector vBCDir = XMLoadFloat3(&pPoints[2]) - XMLoadFloat3(&pPoints[1]);
    _vector vCloss = XMVector3Cross(vABDir, vBCDir);

    _vector vMouseDir = XMLoadFloat3(&m_pGameInstance->Get_RayDir()) * -1.f;
    vCloss = XMVector3Normalize(vCloss);
    vMouseDir = XMVector3Normalize(vMouseDir);

    // 면의 방향벡터와 마우스의 방향벡터를 뒤집은 벡터를 내적
    // 양수면 뒤집어준다.
    if (0.f > XMVectorGetX(XMVector3Dot(vCloss, vMouseDir)))
    {
        _float3 vTemp = pPoints[0];
        pPoints[0] = pPoints[2];
        pPoints[2] = vTemp;
    }

}

_bool CNavigationManager::Check_Cell(_float3* pPoints)
{
    for (_uint i = 0; i < m_Cells.size(); ++i)
    {
        if (nullptr == m_Cells[i])
            continue;

        _float3 vPoint[ENUM_CLASS(CELLPOINT::END)];

        vPoint[ENUM_CLASS(CELLPOINT::A)] = m_Cells[i]->Get_PointPos(CELLPOINT::A);
        vPoint[ENUM_CLASS(CELLPOINT::B)] = m_Cells[i]->Get_PointPos(CELLPOINT::B);
        vPoint[ENUM_CLASS(CELLPOINT::C)] = m_Cells[i]->Get_PointPos(CELLPOINT::C);

        if (XMVector3Equal(XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&vPoint[ENUM_CLASS(CELLPOINT::A)]))
            && XMVector3Equal(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&vPoint[ENUM_CLASS(CELLPOINT::B)]))
            && XMVector3Equal(XMLoadFloat3(&pPoints[2]), XMLoadFloat3(&vPoint[ENUM_CLASS(CELLPOINT::C)]))
        )
        {
            return true;
        }
    }

    return false;
}

void CNavigationManager::Clear_Cells()
{
    for (auto& pCell : m_Cells)
        Safe_Release(pCell);
    m_Cells.clear();
}

void CNavigationManager::Cancle_Cell()
{
    if (m_Cells.size() == 0)
        return;

    Safe_Release(m_Cells.back());
    m_Cells.pop_back();
}

void CNavigationManager::Erase_Cell()
{
    auto iter = m_Cells.begin();
    while (iter != m_Cells.end())
    {
        if (*iter == m_Cells[m_iClickedCellIndex])
        {
            iter = m_Cells.erase(iter);
            return;
        }
        else
            ++iter;
    }
}

void CNavigationManager::Click_Position(_vector vPosition)
{
    _float3 vClickPosition;

    //셀이 1개 이상 있으면 점을 찍을 때 가장 가까운 점이 있는지 확인.
    //찍은 점의 위치가 기존 셀의 점의 위치와 비슷하다면 해당 점을 찍게한다.
    vClickPosition = Find_MinDistance(vPosition);

    //만약 순회해서 찾은 셀의 점이 거리가 멀다면 그냥 기존에 찍은 점을 pushback하게 한다.
    if (m_fMinDistance > 1.f)
        XMStoreFloat3(&vClickPosition, vPosition);

    m_fMinDistance = FLT_MAX;
    if (2 < m_vClickedPoints.size())
        return;

    m_vClickedPoints.push_back(vClickPosition);
    m_vClickedPos = vClickPosition;

    //찍은 점의 개수가 3개가 된다면. 삼각형이 그려지니 Cell에 추가한다.
    if (3 == m_vClickedPoints.size())
    {
        _float3 vPoss[3];
        for (_int i = 0; i < 3; ++i)
            vPoss[i] = m_vClickedPoints[i];

        Add_Cell(vPoss, true);
        Clear_ClickedPosition();
    }
}

_float3 CNavigationManager::Find_MinDistance(_vector vPosition)
{
    // 셀이 없다면 무조건 그냥 찍은 좌표로 준다.
    _float3 vClickPosition;
    XMStoreFloat3(&vClickPosition, vPosition);

    if (m_Cells.size() == 0)
        return vClickPosition;

    //셀을 순회한다.
    for (_uint i = 0; i < m_Cells.size(); ++i)
    {
        CCell* pCell = m_Cells[i];
        if (nullptr == pCell)
            continue;

        _vector vPoint[ENUM_CLASS(CELLPOINT::END)];

        //셀마다 각 포인트 A,B,C를 받아온다

        _float3 fPointA = pCell->Get_PointPos(CELLPOINT::A);
        _float3 fPointB = pCell->Get_PointPos(CELLPOINT::B);
        _float3 fPointC = pCell->Get_PointPos(CELLPOINT::C);

        vPoint[ENUM_CLASS(CELLPOINT::A)] = XMLoadFloat3(&fPointA);
        vPoint[ENUM_CLASS(CELLPOINT::A)] = XMVectorSetW(vPoint[ENUM_CLASS(CELLPOINT::A)], 1.f);
        vPoint[ENUM_CLASS(CELLPOINT::B)] = XMLoadFloat3(&fPointB);
        vPoint[ENUM_CLASS(CELLPOINT::B)] = XMVectorSetW(vPoint[ENUM_CLASS(CELLPOINT::B)], 1.f);
        vPoint[ENUM_CLASS(CELLPOINT::C)] = XMLoadFloat3(&fPointC);
        vPoint[ENUM_CLASS(CELLPOINT::C)] = XMVectorSetW(vPoint[ENUM_CLASS(CELLPOINT::C)], 1.f);

        //내가 피킹한 마우스 Postion과 셀과의 거리를 구한다.
        //그 중에서 가장 작은 값을 체크
        _float fDis_A = XMVectorGetX(XMVector3Length(vPoint[ENUM_CLASS(CELLPOINT::A)] - vPosition));
        _float fDis_B = XMVectorGetX(XMVector3Length(vPoint[ENUM_CLASS(CELLPOINT::B)] - vPosition));
        _float fDis_C = XMVectorGetX(XMVector3Length(vPoint[ENUM_CLASS(CELLPOINT::C)] - vPosition));
        _float fMinDistance = min(min(fDis_A, fDis_B), fDis_C);

        //만약 이전에 순회한 셀보다 거리가 크다면 continue,
        //더 작다면 해당 점을 click 한 점으로 Store해준다.
        if (m_fMinDistance < fMinDistance)
            continue;

        m_fMinDistance = fMinDistance;
        if (fDis_A == m_fMinDistance)
            XMStoreFloat3(&vClickPosition, vPoint[ENUM_CLASS(CELLPOINT::A)]);
        else if (fDis_B == m_fMinDistance)
            XMStoreFloat3(&vClickPosition, vPoint[ENUM_CLASS(CELLPOINT::B)]);
        else if (fDis_C == m_fMinDistance)
            XMStoreFloat3(&vClickPosition, vPoint[ENUM_CLASS(CELLPOINT::C)]);

    }

    return vClickPosition;
}


// 피킹한 점 선택하기.
void CNavigationManager::Add_Point(_float3 vPos)
{
    _float3 vClickPos = {};
    
}

#pragma region Navigation Save Load
const vector<class CCell*>& CNavigationManager::Get_CellContainers()
{
    return m_Cells;
}
void CNavigationManager::Load_CellContainters(NAVIGATIONSAVE_DESC& naviDesc)
{
    // 1. 현재 Cell들을 제거합니다.
    Clear_Cells();

    vector<CELLSAVE_DESC>& cellVector = naviDesc.Cells;

    for (_uint i = 0; i < naviDesc.iCellCount; ++i)
    {
        _float3 vPointArray[ENUM_CLASS(CELLPOINT::END)] = {
            cellVector[i].vPointA,
            cellVector[i].vPointB,
            cellVector[i].vPointC,
        };

        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPointArray, i);
        if (nullptr == pCell)
        {
            CRASH("Failed Load Cell Info");
        }

        m_Cells.emplace_back(pCell);
    }
    
       
}
#pragma endregion



CNavigationManager* CNavigationManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNavigationManager* pInstance = new CNavigationManager(pDevice, pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Create Failed CNavigation Manager"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CNavigationManager::Free()
{
    CBase::Free();
    Clear_Cells();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pShader);
}
