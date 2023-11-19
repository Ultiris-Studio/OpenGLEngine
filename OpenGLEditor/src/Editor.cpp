#include "Editor.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "imgui.h"
#include "ImGuizmo.h"

#include "glm/gtc/type_ptr.hpp"

#include "Player.h"

#include "OpenGLEngine/Scene/SceneSerializer.h"

namespace OpenGLEngine
{
	Editor::Editor() : Layer("Editor"), m_ContentBrowserPanel(), m_EntityPropertiePanel(), m_SelectedEntity(nullptr)
	{
		
	}

	void Editor::OnAttach()
	{
		m_Scene = std::make_unique<Scene>();
		m_EditorCamera = std::make_shared<EditorCamera>(glm::vec3(0.0f, 0.0f, 6.0f));
		
		m_frameBuffer = std::make_shared<Framebuffer>(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
		m_frameBuffer->addColorAttachment(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
		m_frameBuffer->setDepthAttachment();
		m_frameBuffer->Create();

#if 0
		Entity* m_Backpack = m_Scene->CreateEntity("Backpack");
		m_Backpack->AddComponent<TransformComponent>();
		m_Backpack->AddComponent<ModelComponent>("Assets\\Models\\BackPack.obj");
		m_Backpack->AddComponent<MaterialComponent>();
		m_Backpack->GetComponent<MaterialComponent>().InitializeMaterial();
		m_Backpack->GetComponent<MaterialComponent>().addTexture("diffuse", "Assets\\Textures\\1001_albedo.jpg");
		m_Backpack->GetComponent<MaterialComponent>().addTexture("specular", "Assets\\Textures\\1001_roughness.jpg");
		m_Backpack->AddComponent<RenderComponent>();
		m_Backpack->GetComponent<RenderComponent>().GenerateShader();
		m_Backpack->AddComponent<NativeScriptComponent>().Bind<Player>();

		Entity* m_Skybox = m_Scene->CreateEntity("Skybox");
		m_Skybox->AddComponent<TransformComponent>();
		m_Skybox->AddComponent<SkyboxComponent>();
		m_Skybox->AddComponent<RenderComponent>();
#endif

		m_ImGuiColor = {
			ImGuiCol_WindowBg,
			ImGuiCol_Header,
			ImGuiCol_HeaderHovered,
			ImGuiCol_HeaderActive,
			ImGuiCol_Button,
			ImGuiCol_ButtonHovered,
			ImGuiCol_ButtonActive,
			ImGuiCol_FrameBg,
			ImGuiCol_FrameBgHovered,
			ImGuiCol_FrameBgActive,
			ImGuiCol_Tab,
			ImGuiCol_TabHovered,
			ImGuiCol_TabActive,
			ImGuiCol_TabUnfocused,
			ImGuiCol_TabUnfocusedActive,
			ImGuiCol_TitleBg,
			ImGuiCol_TitleBgActive,
			ImGuiCol_TitleBgCollapsed,
			ImGuiCol_Border
		};

		m_ThemeName = {
			"WindowBg",
			"Header",
			"HeaderHovered",
			"HeaderActive",
			"Button",
			"ButtonHovered",
			"ButtonActive",
			"FrameBg",
			"FrameBgHovered",
			"FrameBgActive",
			"Tab",
			"TabHovered",
			"TabActive",
			"TabUnfocused",
			"TabUnfocusedActive",
			"TitleBg",
			"TitleBgActive",
			"TitleBgCollapsed",
			"ImGuiCol_Border"
		};
	}

	void Editor::OnDetach()
	{
		
	}

	void Editor::OnUpdate()
	{
		m_frameBuffer->bind();

		Renderer::Clear();
		Renderer::ClearColor(glm::vec4(0.5f, 0.5f, .5f, 1.0f));

		m_EditorCamera->Update();

		if (m_ViewportHovered)
			m_EditorCamera->m_CameraFocus = true;
		else
			m_EditorCamera->m_CameraFocus = false;

		m_Scene->OnUpdate(1.0f);

		Renderer::BeginScene(m_EditorCamera.get());

		m_Scene->RenderScene();

		CalculateLatency();

		Renderer::EndScene();

		m_frameBuffer->unbind();

		if (Input::IsKeyPressed(Key::LeftControl))
		{
			if (Input::IsKeyPressed(Key::S))
			{
				if (m_Scene->getPath() != "")
				{
					SceneSerializer serializer(*m_Scene);
					serializer.Serialize(m_Scene->getPath());
				}
				else
				{
					m_FileBrowser.SaveFile();
					if (m_FileBrowser.GetInfos().m_FilePath != "")
					{
						m_Scene->setName(m_FileBrowser.GetInfos().m_FileName);
						std::cout << m_FileBrowser.GetInfos().m_FilePath << std::endl;
						std::cout << m_FileBrowser.GetInfos().m_SelectedFile << std::endl;
						SceneSerializer serializer(*m_Scene);
						serializer.Serialize(m_FileBrowser.GetInfos().m_FilePath);
					}
				}
			}
		}
	}

	void Editor::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open")) OpenExternalFile();
				ImGui::Separator();

				if (ImGui::MenuItem("New scene"))
				{
					m_Scene = std::make_unique<Scene>();
				}

				if (ImGui::MenuItem("Save scene"))
				{
					if (m_Scene->getPath() != "")
					{
						SceneSerializer serializer(*m_Scene);
						serializer.Serialize(m_Scene->getPath());
					}
					else
					{
						m_FileBrowser.SaveFile();
						m_Scene->setName(m_FileBrowser.GetInfos().m_FileName);
						SceneSerializer serializer(*m_Scene);
						serializer.Serialize(m_FileBrowser.GetInfos().m_FilePath);
					}
				}

				if (ImGui::MenuItem("Load scene"))
				{
					m_FileBrowser.OpenFile();
					m_Scene = std::make_unique<Scene>();
					SceneSerializer serializer(*m_Scene);
					serializer.Deserialize(m_FileBrowser.GetInfos().m_FilePath);
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Quit"))
					OpenGLEngine::Application::Get().Close();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Create new GameObject"))
				{
					m_Scene->CreateEntity("GameObject");
				}

				if (ImGui::MenuItem("Create new Cube")) AddGameObject(CUBE);
				if (ImGui::MenuItem("Create new Sphere")) AddGameObject(SPHERE);
				if (ImGui::MenuItem("Create new Plane")) AddGameObject(PLANE);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("Preference"))
				{
					m_optionMenu = true;
				}
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
				//ImGui::MenuItem("Padding", NULL, &opt_padding);
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		ImVec2 viewportPanelPos = ImGui::GetWindowPos();
		int x, y;
		glfwGetWindowPos(reinterpret_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), &x, &y);

		if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
		{
			m_frameBuffer = std::make_shared<Framebuffer>((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_frameBuffer->addColorAttachment(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
			m_frameBuffer->setDepthAttachment();
			m_frameBuffer->Create();

			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			m_EditorCamera->OnResize(viewportPanelSize.x, viewportPanelSize.y);
		}
		uint32_t textureID = m_frameBuffer->getColorAttachment(0);
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		/////////////////////////////////////////////////////////////////////////////////////////////

		if (m_SelectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			
			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			glm::mat4 cameraProjection = m_EditorCamera->getProjectionMatrix();
			glm::mat4 cameraView = m_EditorCamera->getViewMatrix();

			auto& tc = m_SelectedEntity->GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform));

			if (ImGuizmo::IsUsing)
			{
				glm::vec3 position, rotation, scale;
				Math::DecomposeTransform(transform, position, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Position = position;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////////

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				//std::filesystem::path file = path;
				std::wstring ws(path);
				std::string filePath(ws.begin(), ws.end());
				const size_t slash = filePath.find_last_of("/\\");
				std::string selectedFile = filePath.substr(slash + 1);
				std::string fileExtension = selectedFile.substr(selectedFile.find_last_of(".") + 1);

				if (fileExtension == "obj")
					AddGameObject(filePath);

				if (fileExtension == "scene")
				{
					m_Scene = std::make_unique<Scene>();
					SceneSerializer serializer(*m_Scene);
					serializer.Deserialize(filePath);
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();

		ImGui::PopStyleVar();

		ImGui::Begin("Inspector");

		if (m_SelectedEntity)
			m_EntityPropertiePanel.OnImGuiRender(m_SelectedEntity);

		ImGui::End();

		ImGui::Begin("Scene");

		static int selection_mask = 0x02;

		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 1);
		
		//for (auto it = m_Scene->GetEntityMap().begin(); it != m_Scene->GetEntityMap().end(); it++)
		for (Entity* entity : m_Scene->GetEntityVector())
		{
			std::string id;
			if (m_SelectedEntity)
				id = m_SelectedEntity->GetId();
			else
				id = "-1";

			ImGuiTreeNodeFlags flags = ((id.c_str() == entity->GetId()) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)(intptr_t)entity->GetId(), flags, entity->GetName());

			if (ImGui::IsItemClicked())
			{
				m_SelectedEntity = entity;
			}

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Delete Object")) {
					m_Scene->DestroyEntity(*entity);
					m_SelectedEntity = nullptr;
				}
				ImGui::EndPopup();
			}

			if (opened)
			{
				ImGui::TreePop();
			}
		}
		
		ImGui::PopStyleVar();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path file = path;
				AddGameObject(file.string());
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();

		m_ContentBrowserPanel.OnImGuiRender();

		ImGui::Begin("Open Resource Infos");
		ImGui::Text("Selected file: %s\nFile path: %s\nFile extension: %s\n", m_FileBrowser.GetInfos().m_SelectedFile.c_str(), m_FileBrowser.GetInfos().m_FilePath.c_str(), m_FileBrowser.GetInfos().m_FileExtension.c_str());
		ImGui::End();

		ImGui::Begin("Render Infos");
		ImGui::Text("%d fps \n%d ms", fps, latency);
		ImGui::End();

		if (m_optionMenu)
		{
			OptionMenu();
		}

		ImGui::End();
	}

	void Editor::OnEvent(Event& e)
	{
		m_EditorCamera->OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(std::bind(&Editor::OnKeyPressed, this, std::placeholders::_1));
	}

	void Editor::AddGameObject(DEFAULT_OBJECT_TYPE type)
	{
		Entity* temp = m_Scene->CreateEntity("temp");
		temp->AddComponent<TransformComponent>();
		temp->AddComponent<MaterialComponent>();
		temp->GetComponent<MaterialComponent>().GetMaterial().addVec3("ambient", glm::vec3({ 0.0f, 0.0f, 0.0f }));
		temp->GetComponent<MaterialComponent>().GetMaterial().addVec3("diffuse", glm::vec3({ 0.0f, 0.0f, 0.0f }));
		temp->GetComponent<MaterialComponent>().GetMaterial().addVec3("specular", glm::vec3({ 0.0f, 0.0f, 0.0f }));
		temp->GetComponent<MaterialComponent>().GetMaterial().addBoolean("diffuse", false);
		temp->GetComponent<MaterialComponent>().GetMaterial().addBoolean("specular", false);
		temp->GetComponent<MaterialComponent>().GetMaterial().addFloat("shininess", 32.0f);
		temp->AddComponent<RenderComponent>();
		temp->GetComponent<RenderComponent>().GenerateShader();

		switch (type)
		{
		case CUBE:
			temp->SetName("Cube");
			temp->AddComponent<ModelComponent>("Assets/Models/cube.obj");
			break;
		case SPHERE:
			temp->SetName("Sphere");
			temp->AddComponent<ModelComponent>("Assets/Models/sphere.obj");
			break;
		case PLANE:
			temp->SetName("Plane");
			temp->AddComponent<ModelComponent>("Assets/Models/plane.obj");
			break;
		}
	}

	void Editor::AddGameObject(const std::string& file)
	{
		const size_t slash = file.find_last_of("/\\");
		const std::string m_SelectedFile = file.substr(slash + 1);

		size_t lastindex = m_SelectedFile.find_last_of(".");
		const std::string m_FileName = m_SelectedFile.substr(0, lastindex);

		Entity* temp = m_Scene->CreateEntity(m_FileName);
		temp->AddComponent<TransformComponent>();
		temp->AddComponent<ModelComponent>(file);
		temp->AddComponent<MaterialComponent>();
		temp->GetComponent<MaterialComponent>().GetMaterial().addVec3("ambient", glm::vec3({ 0.0f, 0.0f, 0.0f }));
		temp->GetComponent<MaterialComponent>().GetMaterial().addVec3("diffuse", glm::vec3({ 0.0f, 0.0f, 0.0f }));
		temp->GetComponent<MaterialComponent>().GetMaterial().addVec3("specular", glm::vec3({ 0.0f, 0.0f, 0.0f }));
		temp->GetComponent<MaterialComponent>().GetMaterial().addBoolean("diffuse", false);
		temp->GetComponent<MaterialComponent>().GetMaterial().addBoolean("specular", false);
		temp->GetComponent<MaterialComponent>().GetMaterial().addFloat("shininess", 32.0f);
		temp->AddComponent<RenderComponent>();
		temp->GetComponent<RenderComponent>().GenerateShader();
	}

	void Editor::SaveScene()
	{
	}

	void Editor::LoadScene()
	{
	}

	void Editor::CalculateLatency()
	{
		double currentTime = glfwGetTime();
		nb_frame++;
		if (currentTime - last_time >= 1.0) {
			latency = (1000.0 / double(nb_frame));
			fps = nb_frame;
			nb_frame = 0;
			last_time += 1.0;
		}
	}

	bool Editor::OnKeyPressed(KeyPressedEvent& e)
	{
		switch (e.GetKeyCode())
		{
		case Key::E:
		{
			m_GizmoType = -1;
			break;
		}
		case Key::R:
		{
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		}
		case Key::T:
		{
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		}
		case Key::Y:
		{
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}
		}

		return false;
	}

	void Editor::OpenExternalFile()
	{
		m_FileBrowser.OpenFile();

		std::filesystem::path sourceFile = m_FileBrowser.GetInfos().m_FilePath;
		std::filesystem::path targetParent = "Assets";
		auto target = targetParent / sourceFile.filename();

		try
		{
			std::filesystem::create_directories(targetParent);
			std::filesystem::copy_file(sourceFile, target, std::filesystem::copy_options::overwrite_existing);
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
		}
	}

	void Editor::OptionMenu()
	{
		ImGui::Begin("Preference");
		
		{

			ImGui::Columns(2);
			ImGui::SetColumnOffset(1, 230);

			if (ImGui::Button("Theme", ImVec2(230 - 15, 39)))
				m_optionTab = 0;

			if (ImGui::Button("Close", ImVec2(230 - 15, 39)))
				m_optionMenu = false;
		}

		ImGui::NextColumn();

		{
			if (ImGui::Button("Real Dark", ImVec2(230 - 70, 29)))
				ImGui::StyleColorsRealDark();
			ImGui::SameLine();
			if (ImGui::Button("Dark", ImVec2(230 - 70, 29)))
				ImGui::StyleColorsDark();
			ImGui::SameLine();
			if (ImGui::Button("Classic", ImVec2(230 - 70, 29)))
				ImGui::StyleColorsClassic();
			ImGui::SameLine();
			if (ImGui::Button("Light", ImVec2(230 - 70, 29)))
				ImGui::StyleColorsLight();

			ImGuiStyle* style = &ImGui::GetStyle();

			for (int color : m_ImGuiColor)
			{
				m_ThemeColor[color] = { style->Colors[color].x, style->Colors[color].y, style->Colors[color].z, style->Colors[color].w };
			}

			for (int i = 0; i < m_ImGuiColor.size(); i++)
			{
				std::string temp("##" + std::string(m_ThemeName[i]));
				ImGui::Text(m_ThemeName[i]); ImGui::SameLine(); ImGui::ColorEdit4(temp.c_str(), glm::value_ptr(m_ThemeColor[i]));
			}

			for (int color : m_ImGuiColor)
			{
				style->Colors[color] = ImColor(m_ThemeColor[color].x, m_ThemeColor[color].y, m_ThemeColor[color].z, m_ThemeColor[color].w);
			}
		}

		ImGui::End();
	}
}