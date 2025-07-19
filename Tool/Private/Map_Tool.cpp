#include "Map_Tool.h"


CMap_Tool::CMap_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pDeviceContext { pContext }
    , m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDeviceContext);
    Safe_AddRef(m_pDevice);
}


HRESULT CMap_Tool::Initialize(LEVEL eLevel)
{
    m_eRenderType = RENDERTYPE::MODEL_CREATE;
    m_eCurLevel = eLevel;

    if (FAILED(Ready_Imgui()))
        return E_FAIL;

    if (FAILED(Ready_Events()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMap_Tool::Ready_Events()
{
    m_pGameInstance->Subscribe(EventType::SELECTED_MODEL, Get_ID(), [this](void* pData)
        {
            TOOL_SELECT_OBJECT_DESC* pDesc = static_cast<TOOL_SELECT_OBJECT_DESC*>(pData);
            Change_SelectObject(pDesc->pSelectedObject);
        });

    m_Events.push_back(EventType::SELECTED_MODEL);

    return S_OK;
}

void CMap_Tool::Change_SelectObject(CGameObject* pSelectedObject)
{
    m_pSelectedObject = pSelectedObject;
}

void CMap_Tool::Render()
{
    Render_Prototype_Hierarchy();
    Render_Layer_Hierarchy();
    if (nullptr != m_pSelectedObject)
    {
        _char szFullPath[MAX_PATH] = {};
        WideCharToMultiByte(CP_ACP, 0, m_pSelectedObject->Get_ObjectTag().c_str(), -1, szFullPath, MAX_PATH, nullptr, nullptr);
        string strValue = szFullPath;

        CTransform* pTransform = static_cast<CTransform*>(m_pSelectedObject->Get_Component(L"Com_Transform"));
        
        Transform_Render(strValue, pTransform);
    }
}


#pragma region Prototype Manager Hierarchy (생성 가능한 객체) 
void CMap_Tool::Render_Prototype_Hierarchy()
{
    /*ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Always);
    ImGui::Begin("Prototype_Hierarchy", nullptr, ImGuiWindowFlags_MenuBar);*/

    static int iSelectedIndex = -1;
    _uint id = 0;

    _wstring objTag = {};
    _wstring modelTag = {};

    for (auto& pair : m_PrototypeNames)
    {
        if (ImGui::Selectable(pair.second.c_str(), id == iSelectedIndex))
        {
            iSelectedIndex = id;
            m_wSelected_PrototypeObjTag = _wstring(pair.first.begin(), pair.first.end());
            m_wSelected_PrototypeModelTag = _wstring(pair.second.begin(), pair.second.end());

            m_Selected_PrototypeModelTag = pair.second;
            m_Selected_PrototypeObjTag = pair.first;

            /*m_Selected_PrototypeModelTag = pair.second;
            WideCharToMultiByte(CP_ACP, 0, modelTag.c_str(), -1, m_Selected_PrototypeModelTag, MAX_PATH, nullptr, nullptr);*/
        }
        
    }
    ++id;

    if (iSelectedIndex >= 0 && iSelectedIndex < m_PrototypeNames.size())
    {
        Render_Prototype_Inspector();
        
    }


}

void CMap_Tool::Render_Prototype_Inspector()
{
    ImGui::Begin("Prototype_Inspector");

    ImGui::Text(m_Selected_PrototypeModelTag.c_str());

    static float fPosition[3] = { 0.f, 0.f, 0.f};
    ImGui::InputFloat3("Position", fPosition);

    static float fRotation[3] = { 0.f, 0.f, 0.f };
    ImGui::InputFloat3("Rotation", fRotation);

    static float fScale[3] = { 1.f, 1.f, 1.f };
    ImGui::InputFloat3("Scale", fScale);

    /* 인스턴스 생성. */
    if (ImGui::Button("Create Instance"))
    {
        MODEL_CREATE_DESC Desc{};
        Desc.pModelTag = m_wSelected_PrototypeModelTag.c_str();
		Desc.vPosition = _float4(fPosition[0], fPosition[1], fPosition[2], 1.f);
		Desc.vRotate = _float3(fRotation[0], fRotation[1], fRotation[2]);
		Desc.vScale = _float3(fScale[0], fScale[1], fScale[2]);


        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel)
            , TEXT("Layer_Map_Parts")
            , ENUM_CLASS(m_eCurLevel)
            , m_wSelected_PrototypeObjTag, &Desc)))
        {
            MSG_BOX(TEXT("Add GameoBject_To_Layer Failed"));
            return;
        }
		
    }

    ImGui::End();
}

// 음.. 솔직히 Model Component 이름만 알면 되지 않나?

/* 프로토타입 인덱스, 객체 이름, 모델 컴포넌트 이름.*/
void CMap_Tool::Register_Prototype_Hierarchy(_uint iPrototypeLevelIndex, const _wstring& strObjectTag, const _wstring& strModelPrefix)
{
    /* 실제 Clone 작업은.*/
    list<_wstring> outList = {};
    m_pGameInstance->Get_PrototypeName_List(outList, ENUM_CLASS(m_eCurLevel), TEXT("MapPart_"));

    _char szFullPath[MAX_PATH] = {};
    WideCharToMultiByte(CP_ACP, 0, strObjectTag.c_str(), -1, szFullPath, MAX_PATH, nullptr, nullptr);
    string strObject = szFullPath;

    for (auto& modelName : outList)
    {
        _char szModelName[MAX_PATH] = {};
        WideCharToMultiByte(CP_ACP, 0, modelName.c_str(), -1, szModelName, MAX_PATH, nullptr, nullptr);
        m_PrototypeNames.emplace_back(strObject, szModelName);
    }

}


#pragma region LAYER
void CMap_Tool::Render_Layer_Hierarchy()
{
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Always);
    ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_MenuBar);

    _uint id = 0;
    for (auto& pair : m_Layer_Objects)
    {
        if (ImGui::TreeNode(pair.first.c_str()))
        {
            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void CMap_Tool::Register_Layer_HierarchyObjects(CGameObject* pGameObject)
{
    static int id = 0;
    const _tchar* wstrValue = pGameObject->Get_ObjectTag().c_str();

    _char szFullPath[MAX_PATH] = {};
    WideCharToMultiByte(CP_ACP, 0, wstrValue, -1, szFullPath, MAX_PATH, nullptr, nullptr);
    string strValue = szFullPath;
    strValue += to_string(id++);

    m_Layer_Objects.emplace_back(make_pair(strValue, pGameObject));
}

void CMap_Tool::Register_Layer_Hierarchy(CLayer* pLayer)
{
    if (nullptr == pLayer)
        return;

    if (pLayer->Get_GameObjects().size() == 0)
        return;

    for (auto& pGameObject : pLayer->Get_GameObjects())
    {
        if (nullptr != pGameObject)
            Register_Layer_HierarchyObjects(pGameObject);
    }
}
#pragma endregion



void CMap_Tool::Transform_Render(const string& name, CTransform* pTransform)
{
    ImGui::Text("%s", name.c_str());
    ImGui::PushID(name.c_str());  // ID 충돌 방지용

    ImGui::PushItemWidth(80.0f);
    static const char* axisLabels[3] = { "X", "Y", "Z" };

    // 1. 위치 벡터 복사
    _vector vPos = pTransform->Get_State(STATE::POSITION);

    for (int i = 0; i < 3; ++i)
    {
        ImGui::PushID(i);

        ImGui::Text("%s", axisLabels[i]);
        ImGui::SameLine();

        // 2. 해당 축에 대한 포인터 얻기
        _float* pValue = &vPos.m128_f32[i];

        // 3. ImGui로 값 조작
        ImGui::InputFloat("##Value", pValue, 0.0f, 0.0f, "%.3f");
        ImGui::SameLine();

        if (ImGui::Button("<")) *pValue -= m_Interval;
        ImGui::SameLine();
        if (ImGui::Button(">")) *pValue += m_Interval;

        ImGui::PopID();
    }

    // 4. 조작이 끝난 후 원래 Transform에 다시 반영
    pTransform->Set_State(STATE::POSITION, vPos);

    ImGui::PopItemWidth();
    ImGui::PopID();
}
#pragma endregion





HRESULT CMap_Tool::Ready_Imgui()
{
    m_pImgui_Manager = CImgui_Manager::Get_Instance(m_pDevice, m_pDeviceContext);
    if (nullptr == m_pImgui_Manager)
    {
        MSG_BOX(TEXT("Failed to Created : CImgui_Manager"));
        return E_FAIL;
    }
    
    return S_OK;
}

CMap_Tool* CMap_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
    CMap_Tool* pInstance = new CMap_Tool(pDevice, pContext);
    if (FAILED(pInstance->Initialize(eLevel)))
    {
        MSG_BOX(TEXT("Create Failed : CMap_Tool"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap_Tool::Free()
{
    __super::Free();
    for (auto& val : m_Events)
        m_pGameInstance->UnSubscribe(val, Get_ID());

    m_Events.clear();

    m_Layer_Objects.clear();
    m_PrototypeNames.clear();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDeviceContext);
    Safe_Release(m_pDevice);
}
