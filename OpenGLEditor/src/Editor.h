#pragma once

#include <OpenGLEngine.h>
#include <OpenGLEngine/Events/Event.h>
#include <OpenGLEngine/Events/KeyEvent.h>

#include "Panels/ContentBrowserPanel.h"
#include "Panels/EntityPropertiePanel.h"
#include <map>

namespace OpenGLEngine
{
	enum DEFAULT_OBJECT_TYPE
	{
		CUBE,
		SPHERE,
		PLANE
	};

	class Editor : public Layer
	{
	public:
		Editor();
		virtual ~Editor() = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnImGuiRender() override;
		void OnEvent(Event& e) override;

		void AddGameObject(DEFAULT_OBJECT_TYPE type);
		void AddGameObject(const std::string& file);

		void SaveScene();
		void LoadScene();

		void OptionMenu();

		void CalculateLatency();
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
	private:
		std::unique_ptr<Scene> m_Scene;

		Entity* m_SelectedEntity;

		std::shared_ptr<EditorCamera> m_EditorCamera;
		std::shared_ptr<Framebuffer> m_frameBuffer;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	private:

		FileBrowser m_FileBrowser;
		void OpenExternalFile();

		ContentBrowserPanel m_ContentBrowserPanel;
		EntityPropertiePanel m_EntityPropertiePanel;
	private:
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
	private:
		double last_time = glfwGetTime();
		int nb_frame = 0;
		int fps = 0;
		int latency = 0;
	private:
		bool m_optionMenu = false;
		int m_optionTab = 0;

		int m_GizmoType = -1;

		std::vector<int> m_ImGuiColor;
		std::vector<const char*> m_ThemeName;
		std::map<int, glm::vec4> m_ThemeColor;
	};
}