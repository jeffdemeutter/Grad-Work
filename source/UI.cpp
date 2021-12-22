#include "pch.h"
#include "UI.h"
#include "IOFiles.h"
#include <thread>
#include "SceneGraph.h"
#include "Progress.h"
#include "Renderer.h"

UI::UI(SDL_Window* pWindow, ID3D11DeviceContext* pDeviceContext)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForD3D(pWindow);
	ImGui_ImplDX11_Init(Renderer::GetDevice(), pDeviceContext);

	// Style
	ImGuiStyle& style = ImGui::GetStyle();
	style.TabRounding = 0.f;
	style.WindowBorderSize = 0.f;
}

UI::~UI()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void UI::RenderUI(float height)
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();

	ImGui::Begin("test", NULL, m_WindowFlags);
	{
		Progress::LoadingPopUp();

		ImGui::SetWindowPos({ 0, 0 });
		ImGui::SetWindowSize({ 300, height });
		if (ImGui::BeginTabBar("Tab"))
		{
			ImportTab();
			MeshTab();

			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UI::ImportTab()
{
	if (ImGui::BeginTabItem("Import"))
	{
		ImGui::Text("For now paste the files into Resources/Input");
		ImGui::TextColored({ 1,0,0,1 }, "Enter the Filename");
		ImGui::Combo("FileType", &m_SelectedFileType, m_pFileTypes, IM_ARRAYSIZE(m_pFileTypes));
		ImGui::InputTextWithHint("Filename", "VoxelFile", m_ImportFile, IM_ARRAYSIZE(m_ImportFile));
		ImGui::InputTextWithHint("MeshName", "Default", m_MeshName, IM_ARRAYSIZE(m_MeshName));
		ImGui::InputFloat3("Position", m_Pos);
		if (ImGui::Button("Import!", { -1, 25 }))
		{
			std::string file{};
			file += m_ImportFile;
			file += m_pFileTypes[m_SelectedFileType];

			std::thread thr(IOFiles::ImportFile, file, m_MeshName, FPoint3{ m_Pos[0], m_Pos[1], m_Pos[2] });
			thr.detach();
		}

		ImGui::EndTabItem();
	}
}

void UI::MeshTab() const
{
	if (ImGui::BeginTabItem("MeshTab"))
	{
		for (BaseObject* pMesh : SceneGraph::GetInstance()->GetObjects())
		{
			if (ImGui::TreeNode(pMesh, pMesh->GetMeshName().c_str()))
			{
				pMesh->RenderUI();
				ImGui::TreePop();
			}
		}

		ImGui::EndTabItem();
	}
}
