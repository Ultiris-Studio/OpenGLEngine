#pragma once

#include "OpenGLEngine/Core/Core.h"
#include "OpenGLEngine/Events/Event.h"
#include "OpenGLEngine/Events/ApplicationEvent.h"
#include "OpenGLEngine/Core/Window.h"
#include "OpenGLEngine/Core/LayerManager.h"
#include "OpenGLEngine/Core/Layer.h"
#include "OpenGLEngine/ImGui/ImGuiLayer.h"
#include "OpenGLEngine/Renderer/Renderer.h"

namespace OpenGLEngine
{
	class Application
	{
	public:
		Application(const std::string& name = "Duck Application");
		virtual ~Application();

		void Run();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void OnEvent(Event& e);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }

		void Close();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnMouseMove(MouseMovedEvent& e);
	private:
		bool m_Running = true;
		bool m_Minimized = false;

		static Application* s_Instance;

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		LayerManager m_LayerManager;
	};

	Application* CreateApplication();
}