#include "Editor.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "imgui.h"
#include "ImGuizmo.h"

#include "Player.h"

#include "OpenGLEngine/Scene/SceneSerializer.h"

#include <OpenGLEngine/Script/Script.h>

#ifndef WIN32_MEAN_AND_LEAN
#define WIN32_MEAN_AND_LEAN
#endif

#include <Windows.h>

HINSTANCE hinstance{ nullptr };
using _get_script_creator = OpenGLEngine::script_creator(*)(size_t);
_get_script_creator fun_script_creator { nullptr };
using _get_script_names = LPSAFEARRAY(*)(void);
_get_script_names fun_script_names { nullptr };

typedef int (*pGet)();
typedef std::string (*pStr)(size_t index);

namespace OpenGLEngine
{
	Editor::Editor() : Layer("Editor"), m_ContentBrowserPanel(), m_EntityPropertiePanel(), m_SceneHierarchy(), m_Viewport(), m_EditorViewport()
	{
		
	}

	void Editor::OnAttach()
	{
		m_Scene = std::make_unique<Scene>();

		InitImGuiStyle();

		hinstance = LoadLibraryA("ProjectSolution/bin/Release-windows-x86_64/ProjectSolution/ProjectSolution.dll");
		if (hinstance)
		{
			/*pGet obj = (pGet)GetProcAddress(hinstance, "get_script_name_size");
			if (obj)
			{
				int result = obj();
				std::cout << "Size: " << result << std::endl;
			}
			else
			{
				std::cout << "Error" << std::endl;
			}

			std::string result = "";
			pStr str = (pStr)GetProcAddress(hinstance, "get_script_name");
			if (str)
			{
				result = str(0);
				std::cout << result << std::endl;
			}
			else
			{
				std::cout << "Error" << std::endl;
			}*/

			fun_script_creator = (_get_script_creator)GetProcAddress(hinstance, "get_script_creator");
			fun_script_names = (_get_script_names)GetProcAddress(hinstance, "get_script_names");

			// Get the SafeArray of script names
			LPSAFEARRAY scriptNames = fun_script_names();
			
			if (scriptNames == nullptr) {
				std::cerr << "LPSAFEARRAY is null." << std::endl;
			}

			// Obtient les dimensions du tableau
			UINT dimCount = SafeArrayGetDim(scriptNames);

			// V�rifie que la dimension est �gale � 1 (tableau unidimensionnel)
			if (dimCount != 1) {
				std::cerr << "LPSAFEARRAY is not one-dimensional." << std::endl;
			}

			// Obtient la taille du tableau
			LONG lBound, uBound;
			SafeArrayGetLBound(scriptNames, 1, &lBound);
			SafeArrayGetUBound(scriptNames, 1, &uBound);

			// Initialise le vecteur pour contenir les cha�nes
			std::vector<std::wstring> stringVector;

			// Parcourt le tableau et ajoute chaque �l�ment au vecteur
			for (LONG i = lBound; i <= uBound; ++i) {
				BSTR bstr;
				SafeArrayGetElement(scriptNames, &i, &bstr);

				// Convertit BSTR en std::wstring
				std::wstring str(bstr);

				// Ajoute la cha�ne au vecteur
				stringVector.push_back(str);

				// Lib�re la m�moire allou�e pour le BSTR
				SysFreeString(bstr);
			}

			//std::cout << stringVector.at(0).c_str() << std::endl;

			const wchar_t* wideStr = stringVector.at(0).c_str();
			size_t bufferSize = wcslen(wideStr) + 1;  // +1 pour le caract�re nul de fin
			char* narrowStr = new char[bufferSize];

			size_t convertedChars = 0;
			wcstombs_s(&convertedChars, narrowStr, bufferSize, wideStr, _TRUNCATE);

			// Imprime la cha�ne C �troite
			std::cout << "Contenu de la premiere chaine (C etroite) : " << narrowStr << std::endl;

			wideStr = stringVector.at(1).c_str();
			bufferSize = wcslen(wideStr) + 1;
			narrowStr = new char[bufferSize];
			convertedChars = 0;
			wcstombs_s(&convertedChars, narrowStr, bufferSize, wideStr, _TRUNCATE);

			std::cout << "Contenu de la deuxieme chaine (C etroite) : " << narrowStr << std::endl;

			script_creator var_script_creator = fun_script_creator(string_hash()(narrowStr));

			if (var_script_creator)
			{
				std::cout << "Script validate" << std::endl;

				script_ptr entity = var_script_creator();

				entity->OnCreate();
			}

			// N'oubliez pas de lib�rer la m�moire allou�e
			delete[] narrowStr;

			/*script_creator script;
			script = fun_script_creator(string_hash()(result));

			if (script)
			{
				std::cout << "Script validate" << std::endl;
			}*/

			FreeLibrary(hinstance);
		}
	}

	void Editor::OnDetach()
	{
		
	}

	void Editor::OnUpdate()
	{
		m_Scene->Update(1.0f);
		m_EditorViewport.Update(*m_Scene);

		CalculateLatency();

		m_Viewport.Render(*m_Scene);
		m_EditorViewport.Render(*m_Scene);

		if (Input::IsKeyPressed(Key::LeftControl))
		{
			if (Input::IsKeyPressed(Key::S))
			{
				SaveScene();
			}
		}

		if (Input::IsKeyPressed(Key::L))
		{
			std::cout << registery().size() << std::endl;
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
					SaveScene();
				}

				if (ImGui::MenuItem("Load scene"))
				{
					LoadScene();
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Quit"))
					OpenGLEngine::Application::Get().Close();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Start scene"))
				{
					m_Scene->OnScenePlay();
				}
				if (ImGui::MenuItem("Stop scene"))
				{
					m_Scene->OnSceneStop();
				}
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

		m_Viewport.OnImGuiRender(*m_Scene);
		m_EditorViewport.OnImGuiRender(*m_Scene);

		ImGui::Begin("Inspector");

		if (m_Scene->m_SelectedEntity)
			m_EntityPropertiePanel.OnImGuiRender(m_Scene->m_SelectedEntity);

		ImGui::End();

		m_SceneHierarchy.OnImGuiRender(*m_Scene);

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
		m_Scene->getEditorCamera().OnEvent(e);
	}

	void Editor::InitImGuiStyle()
	{
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
		if (m_Scene->getPath() != "")
		{
			SceneSerializer serializer(*m_Scene);
			serializer.Serialize(m_Scene->getPath());
		}
		else
		{
			m_FileBrowser.SaveFile();
			m_Scene->setName(m_FileBrowser.GetInfos().m_FileName);
			std::cout << m_FileBrowser.GetInfos().m_FilePath << std::endl;
			std::cout << m_FileBrowser.GetInfos().m_SelectedFile << std::endl;
			SceneSerializer serializer(*m_Scene);
			serializer.Serialize(m_FileBrowser.GetInfos().m_FilePath);
		}
	}
	
	void Editor::LoadScene()
	{
		m_FileBrowser.OpenFile();
		m_Scene = std::make_unique<Scene>();
		SceneSerializer serializer(*m_Scene);
		serializer.Deserialize(m_FileBrowser.GetInfos().m_FilePath);
	}

	void Editor::LoadScene(std::string filePath)
	{
		m_Scene = std::make_unique<Scene>();
		SceneSerializer serializer(*m_Scene);
		serializer.Deserialize(filePath);
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