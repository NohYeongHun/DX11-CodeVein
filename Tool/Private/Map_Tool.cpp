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
    if(FAILED(Ready_Imgui()))
        return E_FAIL;


    return S_OK;
}

/* 목적을 어떻게 할거냐?.. */
void CMap_Tool::ImGui_Render()
{
	struct LevelButton {
		const char* label;
		_wstring vibuffer_type;
	};

	const LevelButton tab[] = {
	{ "Texture", TEXT("Layer_Tile")},
	{ "Map Cube", TEXT("Layer_Cube")},
	{ "Collider Cube", TEXT("Layer_Collider")},
	{"QusetionBlock", TEXT("Layer_QuestionBlock") },
	{"NormalBlock", TEXT("Layer_NormalBlock") },
	{"Broken block", TEXT("Layer_BrokenBlock") }
	};

	ImGui::Begin("Map_Tool", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginTabBar("tab"))
	{
		for (auto& el : tab)
		{
			if (ImGui::BeginTabItem(el.label))
			{
				ImGui_MenuBar_Render();

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

void CMap_Tool::ImGui_MenuBar_Render()
{
	if (ImGui::BeginMenuBar())
	{
		// 파일 메뉴. => 특정 타입의 파일을 불러오거나 확인할 수 있습니다.
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("open"))
			{
				IGFD::FileDialogConfig config;
				config.path = "../SaveFile/";
				config.flags = ImGuiFileDialogFlags_ReadOnlyFileNameField;

				// 파일 다이얼로그 열기
				// ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", config);
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".bin", config);
			}
			if (ImGui::MenuItem("save"))
			{
				IGFD::FileDialogConfig config;
				config.path = "../SaveFile/";
				config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;

				ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey", "Choose File", ".bin", config);

			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// 읽기용 로직
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string load_path = ImGuiFileDialog::Instance()->GetFilePathName();
			// ImGui::Text("Selected file: %s", filePath.c_str());
			//m_pTile_Loader->Load_Tile(load_path, LEVEL::LEVEL_MAPEDIT);
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

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
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDeviceContext);
    Safe_Release(m_pDevice);
}
