#include "Launcher.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace OpenGLEngine
{	
	Launcher::Launcher()
	{
		config = YAML::LoadFile("config.yaml");
		m_ProjectManager = std::make_unique<ProjectManager>();
	}

	void Launcher::OnAttach()
	{
		
	}

	void Launcher::OnDetach()
	{

	}

	void Launcher::OnUpdate(double dt)
	{

	}

	void Launcher::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_AutoHideTabBar;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->GetWorkPos());
			ImGui::SetNextWindowSize(viewport->GetWorkSize());
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);

		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			static auto first_time = true;
			if (first_time) {
				ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
				ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
				ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

				auto dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking windows into it
				auto dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.4f, nullptr, &dock_main_id);
				auto dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.6f, nullptr, &dock_main_id);

				ImGui::DockBuilderDockWindow("Create Project", dock_id_left);
				ImGui::DockBuilderDockWindow("Project Viewer", dock_id_right);

				ImGui::DockBuilderFinish(dockspace_id);
				first_time = false;
			}
		}

		ImGui::Begin("Create Project", false);

		m_ProjectManager->CreateNewProject();

		m_ProjectManager->OnImGuiRender();

		ImGui::End();

		ImGui::Begin("Project Viewer", false);
		// TODO project viewer
		ImGui::End();

		ImGui::End();
	}

	void Launcher::OnEvent(Event& e)
	{

	}
}