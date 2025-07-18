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


HRESULT CMap_Tool::Initialize()
{
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
    Render_Hierarchy();
    if (nullptr != m_pSelectedObject)
    {
        _char szFullPath[MAX_PATH] = {};
        WideCharToMultiByte(CP_ACP, 0, m_pSelectedObject->Get_ObjectTag().c_str(), -1, szFullPath, MAX_PATH, nullptr, nullptr);
        string strValue = szFullPath;

        CTransform* pTransform = static_cast<CTransform*>(m_pSelectedObject->Get_Component(L"Com_Transform"));
        
        Transform_Render(strValue, pTransform);
    }
}

void CMap_Tool::Render_Hierarchy()
{
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Always);
    ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_MenuBar);

    _uint id = 0;
    for (auto& pair : m_HierarchyObjects)
    {
        if (ImGui::TreeNode(pair.first.c_str()))
        {
            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void CMap_Tool::Register_Hierarchy_Objects(CGameObject* pGameObject)
{
    static int id = 0;
    const _tchar* wstrValue = pGameObject->Get_ObjectTag().c_str();

    _char szFullPath[MAX_PATH] = {};
    WideCharToMultiByte(CP_ACP, 0, wstrValue, -1, szFullPath, MAX_PATH, nullptr, nullptr);
    string strValue = szFullPath;
    strValue += to_string(id++);

    m_HierarchyObjects.emplace_back(make_pair(strValue, pGameObject));
}

void CMap_Tool::Register_Hierarchy_Layer(CLayer* pLayer)
{
    for (auto& pGameObject : pLayer->Get_GameObjects())
    {
        if (nullptr != pGameObject)
            Register_Hierarchy_Objects(pGameObject);
    }
}

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

//void CMap_Tool::Transform_Render(const string& name, _float3& transform)
//{
//    ImGui::Text("%s", name.c_str());
//    ImGui::PushID(name.c_str());  // ID 충돌 방지용
//
//    ImGui::PushItemWidth(80.0f);
//    static const char* axisLabels[3] = { "X", "Y", "Z" };
//    
//    for (int i = 0; i < 3; ++i)
//    {
//        ImGui::PushID(i);
//
//        const char* axis = (i == 0) ? "X" : (i == 1) ? "Y" : "Z";
//
//        ImGui::Text("%s", axis);
//        ImGui::SameLine();
//
//        _float* pValue = nullptr;
//        switch (i)
//        {
//        case 0: pValue = &transform.x; break;
//        case 1: pValue = &transform.y; break;
//        case 2: pValue = &transform.z; break;
//        }
//
//        ImGui::InputFloat("##Value", pValue, 0.0f, 0.0f, "%.3f");
//        ImGui::SameLine();
//
//        if (ImGui::Button("<"))
//            *pValue -= m_Interval;
//        ImGui::SameLine();
//
//        if (ImGui::Button(">"))
//            *pValue += m_Interval;
//
//        ImGui::PopID();
//    }
//
//    ImGui::PopItemWidth();
//    ImGui::PopID(); // label
//}

///* 목적을 어떻게 할거냐?.. */
//void CMap_Tool::ImGui_Render()
//{
//	struct LevelButton {
//		const char* label;
//		_wstring vibuffer_type;
//	};
//
//	const LevelButton tab[] = {
//	{ "Texture", TEXT("Layer_Tile")},
//	{ "Map Cube", TEXT("Layer_Cube")},
//	{ "Collider Cube", TEXT("Layer_Collider")},
//	{"QusetionBlock", TEXT("Layer_QuestionBlock") },
//	{"NormalBlock", TEXT("Layer_NormalBlock") },
//	{"Broken block", TEXT("Layer_BrokenBlock") }
//	};
//
//	ImGui::Begin("Map_Tool", nullptr, ImGuiWindowFlags_MenuBar);
//
//	if (ImGui::BeginTabBar("tab"))
//	{
//		for (auto& el : tab)
//		{
//			if (ImGui::BeginTabItem(el.label))
//			{
//				ImGui_MenuBar_Render();
//
//				ImGui::EndTabItem();
//			}
//		}
//		ImGui::EndTabBar();
//	}
//
//	ImGui::End();
//}
//
//void CMap_Tool::ImGui_MenuBar_Render()
//{
//	if (ImGui::BeginMenuBar())
//	{
//		// 파일 메뉴. => 특정 타입의 파일을 불러오거나 확인할 수 있습니다.
//		if (ImGui::BeginMenu("File"))
//		{
//			if (ImGui::MenuItem("open"))
//			{
//				IGFD::FileDialogConfig config;
//				config.path = "../SaveFile/";
//				config.flags = ImGuiFileDialogFlags_ReadOnlyFileNameField;
//
//				// 파일 다이얼로그 열기
//				// ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", config);
//				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".bin", config);
//			}
//			if (ImGui::MenuItem("save"))
//			{
//				IGFD::FileDialogConfig config;
//				config.path = "../SaveFile/";
//				config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
//
//				ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey", "Choose File", ".bin", config);
//
//			}
//			ImGui::EndMenu();
//		}
//		ImGui::EndMenuBar();
//	}
//
//	// 읽기용 로직
//	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
//		if (ImGuiFileDialog::Instance()->IsOk()) {
//			std::string load_path = ImGuiFileDialog::Instance()->GetFilePathName();
//			// ImGui::Text("Selected file: %s", filePath.c_str());
//			//m_pTile_Loader->Load_Tile(load_path, LEVEL::LEVEL_MAPEDIT);
//		}
//		ImGuiFileDialog::Instance()->Close();
//	}
//}

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

CMap_Tool* CMap_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMap_Tool* pInstance = new CMap_Tool(pDevice, pContext);
    if (FAILED(pInstance->Initialize()))
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

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDeviceContext);
    Safe_Release(m_pDevice);
}
