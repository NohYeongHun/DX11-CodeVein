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


/* 맵툴이 카메라 소유시키기 .*/
HRESULT CMap_Tool::Initialize(LEVEL eLevel)
{
    m_eCurLevel = eLevel;

    /* 카메라 초기화 */
    list<CGameObject*> cameraList = m_pGameInstance->Get_Layer(ENUM_CLASS(m_eCurLevel), TEXT("Layer_Camera"))
        ->Get_GameObjects();

    auto iter = cameraList.begin();

    if (iter == cameraList.end())
        return E_FAIL;

    m_pCamera = static_cast<CCamera_Free*>(*iter);
    Safe_AddRef(m_pCamera);

    /* SaveFile Loader 초기화 */
    m_pSaveFile_Loader = CSaveFile_Loader::Create();

    m_pCameraTransformCom = static_cast<CTransform*>(m_pCamera->Get_Component(L"Com_Transform"));
    
    if (FAILED(Ready_Imgui()))
        return E_FAIL;


    return S_OK;
}


void CMap_Tool::Change_SelectObject(CGameObject* pSelectedObject)
{
    m_pSelectedObject = pSelectedObject;
}

void CMap_Tool::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Get_KeyUp(DIK_C))
        m_eToolMode = TOOLMODE::CREATE;

    if (m_pGameInstance->Get_KeyUp(DIK_E))
        m_eToolMode = TOOLMODE::EDIT;


    if (m_IsPossible_Picking && m_pGameInstance->Get_MouseKeyUp(MOUSEKEYSTATE::RB))
        Update_Picking(ENUM_CLASS(m_eCurLevel), TEXT("Layer_Map_Parts"));

    Handle_SelectedObject();
}

struct LevelButton {
    const char* label;
    const _wstring vibuffer_type;
};

void CMap_Tool::Render()
{
    if (m_eToolMode == TOOLMODE::CREATE)
        Render_Model_Create();
    else
        Render_Model_Edit();

    // MenuBar 렌더링.
    Render_MenuBar();

    // 디버그 정보는 항상 렌더링
    Render_Debug_Window();

}

void CMap_Tool::Render_MenuBar()
{
    if (!m_IsPossible_SaveLoad)
        return;

    ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("open"))
            {
                IGFD::FileDialogConfig config;
                config.path = "../Bin/Resources/SaveFile/";
                config.flags = ImGuiFileDialogFlags_ReadOnlyFileNameField;

                // 파일 다이얼로그 열기
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".dat", config);
            }
            if (ImGui::MenuItem("save"))
            {
                IGFD::FileDialogConfig config;
                config.path = "../Bin/Resources/SaveFile/";
                config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;

                ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey", "Choose File", ".dat", config);

            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // 읽기용 로직
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string load_path = ImGuiFileDialog::Instance()->GetFilePathName();
            m_pSaveFile_Loader->Load_File(load_path, m_eCurLevel);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // 저장용 로직
    if (ImGuiFileDialog::Instance()->Display("SaveFileDlgKey"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string save_path = ImGuiFileDialog::Instance()->GetFilePathName();
            m_pSaveFile_Loader->Save_File(save_path);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::End();
}





/* Debug 모드에서 현재 상태값에 대한 지정을 수행합니다. */
void CMap_Tool::Render_Debug_Window()
{
    ImGuiIO& io = ImGui::GetIO();

    // 윈도우 하단에서 150 → 200~250 정도 위로
    ImVec2 windowPos = ImVec2(10.f, io.DisplaySize.y - 250.f);
    ImVec2 windowSize = ImVec2(300.f, 200.f);

    // Cond_Once: 최초 한 번만 위치 적용 → 이후 드래그 가능
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);

    // NoCollapse만 유지, 이동 가능하게
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoCollapse);

    
    _float3 camPos = {};
    XMStoreFloat3(&camPos,m_pCameraTransformCom->Get_State(STATE::POSITION));
    ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camPos.x, camPos.y, camPos.z);

    const char* modeStr = (m_eToolMode == TOOLMODE::CREATE) ? "CREATE" : "EDIT";
    ImGui::Text("Tool Mode: %s", modeStr);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

    // 체크박스 추가 - 피킹 가능 여부
    ImGui::Checkbox("Enable Picking", &m_IsPossible_Picking);
    // 체크박스 추가 - 파일 Load Save 여부
    ImGui::Checkbox("Enable SaveLoad", &m_IsPossible_SaveLoad);

    ImGui::End();
}

void CMap_Tool::Handle_SelectedObject()
{
    if (nullptr == m_pSelectedObject)
        return;

    if (m_eToolMode == TOOLMODE::CREATE)
        Handle_CreateMode_SelectedObject();
    else if (m_eToolMode == TOOLMODE::EDIT)
        Handle_EditMode_SelectedObject();
}   




#pragma region CREATE_MODE


void CMap_Tool::Render_Model_Create()
{
    if (m_IsPossible_SaveLoad)
        return;

    Render_Prototype_Hierarchy();
}


void CMap_Tool::Render_Prototype_Hierarchy()
{
    ImGui::SetNextWindowPos(ImVec2(g_iWinSizeX - 310.f, 10.f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Once);
    ImGui::Begin("Prototype_Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse);

    static int iSelectedIndex = -1;
    _uint id = 0;

    _wstring objTag = {};
    _wstring modelTag = {};
    ImVec2 hierarchyPos = ImGui::GetWindowPos();
    ImVec2 hierarchySize = ImGui::GetWindowSize();
    

    for (auto& pair : m_PrototypeNames)
    {
        if (ImGui::Selectable(pair.second.c_str(), id == iSelectedIndex))
        {
            m_PrototypeinspectorPos = ImVec2(hierarchyPos.x - 310.f, hierarchyPos.y); // 왼쪽 붙이기

            iSelectedIndex = id;
            m_wSelected_PrototypeObjTag = _wstring(pair.first.begin(), pair.first.end());
            m_wSelected_PrototypeModelTag = _wstring(pair.second.begin(), pair.second.end());

            m_Selected_PrototypeModelTag = pair.second;
            m_Selected_PrototypeObjTag = pair.first;
        }
        
    }

    if (iSelectedIndex >= 0 && iSelectedIndex < m_PrototypeNames.size())
        Render_Prototype_Inspector(m_PrototypeinspectorPos);

    ImGui::End();
}

void CMap_Tool::Render_Prototype_Inspector(ImVec2 vPos)
{

    ImGui::SetNextWindowPos(vPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Once);
    ImGui::Begin("Prototype_Transform");

    ImGui::Text(m_Selected_PrototypeModelTag.c_str());

    if (!m_IsPicking_Create)
    {
        static float fPosition[3] = { 0.f, 0.f, 0.f };
        ImGui::InputFloat3("Position", fPosition);

        static float fRotation[3] = { 0.f, 0.f, 0.f };
        ImGui::InputFloat3("Rotation", fRotation);

        static float fScale[3] = { 1.f, 1.f, 1.f };
        ImGui::InputFloat3("Scale", fScale);

        if (ImGui::Button("Create Instance"))
        {
            CToolMap_Part::MAP_PART_DESC Desc{};
            Desc.eArgType = CToolMap_Part::ARG_TYPE::CREATE;

            MODEL_CREATE_DESC CreateDesc{};
            CreateDesc.pModelTag = m_wSelected_PrototypeModelTag.c_str();
            CreateDesc.vPosition = _float4(fPosition[0], fPosition[1], fPosition[2], 1.f);
            CreateDesc.vRotate = _float3(fRotation[0], fRotation[1], fRotation[2]);
            CreateDesc.vScale = _float3(fScale[0], fScale[1], fScale[2]);

            /* 구조체 데이터 넣기. */
            Desc.pData = reinterpret_cast<void*>(&CreateDesc);

            if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel)
                , TEXT("Layer_Map_Parts")
                , ENUM_CLASS(m_eCurLevel)
                , m_wSelected_PrototypeObjTag, &Desc)))
            {
                MSG_BOX(TEXT("Add GameObject_To_Layer Failed"));
                return;
            }
        }
    }
    else if (m_IsPicking_Create)
    {
        static float fInterval[3] = { 0.f, 0.f, 0.f };
        ImGui::InputFloat3("Interval", fInterval);
        m_vInterval = _float3(fInterval[0], fInterval[1], fInterval[2]);
    }

    /* 인스턴스 생성. */
    ImGui::Checkbox("Picking Create", &m_IsPicking_Create);

    ImGui::End();
}


/* 프로토타입 인덱스, 객체 이름, 모델 컴포넌트 이름.*/
void CMap_Tool::Register_Prototype_Hierarchy(_uint iPrototypeLevelIndex, const _wstring& strObjectTag, const _wstring& strModelPrefix)
{
    /* 실제 Clone 작업은.*/
    list<_wstring> outList = {};
    //m_pGameInstance->Get_PrototypeName_List(outList, ENUM_CLASS(m_eCurLevel), TEXT("MapPart_"));
    

    _char szFullPath[MAX_PATH] = {};
    WideCharToMultiByte(CP_ACP, 0, strObjectTag.c_str(), -1, szFullPath, MAX_PATH, nullptr, nullptr);
    string strObject = szFullPath;


    for (_uint i = 0; i < Model_PrototypeSize; ++i)
        outList.push_back(Model_Prototypes[i].prototypeName);
    

    for (auto& modelName : outList)
    {
        _char szModelName[MAX_PATH] = {};
        WideCharToMultiByte(CP_ACP, 0, modelName.c_str(), -1, szModelName, MAX_PATH, nullptr, nullptr);
        m_PrototypeNames.emplace_back(strObject, szModelName);
    }

}
void CMap_Tool::Handle_CreateMode_SelectedObject()
{
    /* Create Mode에서 Selecetd Object의 역할*/

    /* 생성할 위치. 월드 좌표 반영이 안됨. */
    _float3 vPos = {};
    XMStoreFloat3(&vPos, XMLoadFloat3(&m_RayHitDesc.vHitPoint));

    CToolMap_Part::MAP_PART_DESC Desc{};
    Desc.eArgType = CToolMap_Part::ARG_TYPE::CREATE;

    MODEL_CREATE_DESC CreateDesc{};
    CreateDesc.pModelTag = m_wSelected_PrototypeModelTag.c_str();
    CreateDesc.vPosition = _float4(vPos.x + m_vInterval.x, vPos.y + m_vInterval.y, vPos.z + m_vInterval.z, 1.f);
    CreateDesc.vRotate = _float3(0.f, 0.f, 0.f);
    CreateDesc.vScale = _float3(1.f, 1.f, 1.f);

    /* 구조체 데이터 넣기. */
    Desc.pData = reinterpret_cast<void*>(&CreateDesc);

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(m_eCurLevel)
        , TEXT("Layer_Map_Parts")
        , ENUM_CLASS(m_eCurLevel)
        , m_wSelected_PrototypeObjTag, &Desc)))
    {
        MSG_BOX(TEXT("Create GameObject_To_Layer Failed"));
        return;
    }

    // 이미 생성했으면? => 계속 생성 방지. 
    m_pSelectedObject = nullptr; // 계속 생성 방지.
}
#pragma endregion


#pragma region EDIT MODE

void CMap_Tool::Render_Model_Edit()
{
    if (m_IsPossible_SaveLoad)
        return;

    Render_Edit_Hierarchy();
}

/*
*  Object Layer에서 Hierarchy에 불러옵니다.
* 객체들이 생성되고 삭제될 수 있으므로 동적으로 불러와야 합니다.
* Layer에 변동사항이 있을때마다 호출합니다.
*/
void CMap_Tool::Load_Layer(const _wstring& strLayerTag)
{
    for (auto& pair : m_Layer_Objects)
    {
        Safe_Release(pair.second);
        m_Layer_Objects.clear();
    }
        

    CLayer* pLayer = m_pGameInstance->Get_Layer(ENUM_CLASS(m_eCurLevel), strLayerTag);

    if (nullptr == pLayer)
        return;

    // 1. Layer Objects에 지정해둡니다.
    list<CGameObject*> GameObjects = pLayer->Get_GameObjects();

    // 2. Layer에서 _wstring => string으로 치환해줍니다.
    for (auto& pGameObject : GameObjects)
    {
        // wstring to string
        const _tchar* wstrValue = pGameObject->Get_ObjectTag().c_str();
        _char szFullPath[MAX_PATH] = {};
        WideCharToMultiByte(CP_ACP, 0, wstrValue, -1, szFullPath, MAX_PATH, nullptr, nullptr);
        string strValue = szFullPath;

        strValue += pGameObject->Get_ID();
        m_Layer_Objects.emplace_back(strValue, pGameObject);
        Safe_AddRef(pGameObject);
    }
}

void CMap_Tool::Render_Edit_Hierarchy()
{
    ImGui::SetNextWindowPos(ImVec2(g_iWinSizeX - 310.f, 10.f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Once);
    ImGui::Begin("Edit_Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse);

    static int iSelectedIndex = -1;
    _uint id = 0;

    _wstring objTag = {};
    _wstring modelTag = {};
    ImVec2 hierarchyPos = ImGui::GetWindowPos();
    ImVec2 hierarchySize = ImGui::GetWindowSize();

    

    for (auto& pair : m_Layer_Objects)
    {
        // 선택되었을 때만.
        if (ImGui::Selectable(pair.first.c_str(), id == iSelectedIndex))
        {
            m_EditinspectorPos = ImVec2(hierarchyPos.x - 310.f, hierarchyPos.y); // 왼쪽 붙이기
            m_iSelectedIndex = id; // 선택한 인덱스.
            m_Selected_EditObjTag = pair.first;
            m_pSelectedObject = pair.second;
        }
    }


    if (iSelectedIndex >= 0 && iSelectedIndex < m_PrototypeNames.size())
        Render_Edit_Inspector(m_EditinspectorPos);

    ImGui::End();
}

void CMap_Tool::Render_Edit_Inspector(ImVec2 vPos)
{
    ImGui::SetNextWindowPos(vPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Once);
    ImGui::Begin("Edit_Transform");

    ImGui::Text(m_Selected_EditObjTag.c_str());


    if (nullptr == m_pSelectedObject)
        return;

    CTransform* pTransformCom = static_cast<CTransform*>(m_pSelectedObject->Get_Component(L"Com_Transform"));
    
    pTransformCom->Get_State(STATE::POSITION);

        
    
    static float fPosition[3] = { 0.f, 0.f, 0.f };
    ImGui::InputFloat3("Position", fPosition);

    static float fRotation[3] = { 0.f, 0.f, 0.f };
    ImGui::InputFloat3("Rotation", fRotation);

    static float fScale[3] = { 1.f, 1.f, 1.f };
    ImGui::InputFloat3("Scale", fScale);

    
    

    /* 인스턴스 생성. */
    ImGui::Checkbox("Picking Create", &m_IsPicking_Create);

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
void CMap_Tool::Handle_EditMode_SelectedObject()
{

}
#pragma endregion


/* Selected Object를 설정해줍니다. */
void CMap_Tool::Update_Picking(_uint iLayerLevelIndex, const _wstring& strLevelLayerTag)
{
    _float fOutDist = {};
    m_RayHitDesc = m_pGameInstance->Get_PickingLocalObject(iLayerLevelIndex, strLevelLayerTag,
        &fOutDist);

    m_pSelectedObject = m_RayHitDesc.pHitObject;
    
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

    m_PrototypeNames.clear();

    /* Edit Mode Object Clear */
    for (auto& pair : m_Layer_Objects)
        Safe_Release(pair.second);
    m_Layer_Objects.clear();

    Safe_Release(m_pSaveFile_Loader);
    Safe_Release(m_pCamera);
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDeviceContext);
    Safe_Release(m_pDevice);
}
