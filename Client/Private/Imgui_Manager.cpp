#include "Imgui_Manager.h"

// SingleTon
CImgui_Manager* CImgui_Manager::m_pInstance = nullptr;

CImgui_Manager::CImgui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice}
    , m_pDeviceContext { pContext }
    , m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);
    Safe_AddRef(m_pGameInstance);
    
}

void CImgui_Manager::Render_Begin()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

}

void CImgui_Manager::Render()
{
    Render_Begin();

    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Always);
    ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("open");
            ImGui::MenuItem("save");
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // Slider
    static float sliderValue = 0.0f;
    ImGui::SliderFloat("Slider", &sliderValue, 0.0f, 1.0f);


    // Button
    if (ImGui::Button("Test Me")) {
        // 버튼이 클릭되었을 때 실행될 코드
        MessageBoxA(nullptr, "Button clicked!", "Info", MB_OK);

    }
    // text
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::Text("Delta_Time: %.5f", ImGui::GetIO().DeltaTime);
    ImGui::End();

    Render_End();
}

void CImgui_Manager::Change_Level()
{
}

void CImgui_Manager::Render_End()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

HRESULT CImgui_Manager::Initialize_Clone(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    // 1. 컨텍스트 생성.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    // SetUp Imgui
    if (!ImGui_ImplWin32_Init(hWnd))
        MessageBoxA(nullptr, "ImGui Win32 Init Failed", "Error", MB_OK);

    if (!ImGui_ImplDX11_Init(pDevice, pContext))
        MessageBoxA(nullptr, "ImGui DX11 Init Failed", "Error", MB_OK);

    ImGui::StyleColorsDark();
    ImFont* font = ImGui::GetIO().Fonts->AddFontDefault();
    ImGui::GetIO().FontDefault = font;


    return S_OK;
}

CImgui_Manager* CImgui_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CImgui_Manager* pInstance = new CImgui_Manager(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Clone(g_hWnd, pDevice, pContext)))
    {
        MSG_BOX(TEXT("Create Failed : CImgui_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CImgui_Manager::Free()
{
    CBase::Free();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDeviceContext);
    Safe_Release(m_pDevice);

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

CImgui_Manager* CImgui_Manager::Get_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (nullptr != m_pInstance)
    {
        return m_pInstance;
    }
    m_pInstance = Create(pDevice, pContext);
    return m_pInstance;
}
