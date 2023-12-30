#include "depch.h"
#include <OpenGLEngine/Scene/Scene.h>

#include "OpenGLEngine/Core/Application.h"
#include <GLFW/glfw3.h>

#include <OpenGLEngine/Tools/UUID.h>
#include <OpenGLEngine/Entity/Components/LightComponent.h>
#include <OpenGLEngine/Entity/Components/TransformComponent.h>
#include <OpenGLEngine/Entity/Components/NativeScriptComponent.h>
#include <OpenGLEngine/Entity/Components/CameraComponent.h>

#include <OpenGLEngine/Core/Input.h>

namespace OpenGLEngine
{
	Scene::Scene() :
		m_Name("Untitled"),
		m_Path(""),
		m_SelectedEntity(nullptr),
		m_EditorCamera(std::make_unique<EditorCamera>(glm::vec3(0.0f, 0.0f, 6.0f))),
		m_ActiveCamera(nullptr),
		m_OnRuntime(false),
		m_LightsCount(0)
	{
		Init();
	}

	Scene::Scene(const std::string& name) : 
		m_Name(name),
		m_Path(""),
		m_SelectedEntity(nullptr),
		m_EditorCamera(std::make_unique<EditorCamera>(glm::vec3(0.0f, 0.0f, 6.0f))),
		m_ActiveCamera(nullptr),
		m_OnRuntime(false),
		m_LightsCount(0)
	{
		Init();
	}

	Scene::~Scene()
	{
		m_PhysicsEngine->Shutdown();
	}

	void Scene::Init()
	{
		m_PhysicsEngine = std::make_unique<PhysicsEngine>();
		m_PhysicsEngine->Init();
	}

	Entity* Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(uuid::generate_uuid_v4(), name);
	}

	Entity* Scene::CreateEntityWithUUID(const std::string& uuid, const std::string& name)
	{
		Entity entity = Entity(name, uuid);
		entity.AddComponent<TransformComponent>();
		m_EntityMap[entity.GetId()] = entity;
		return &m_EntityMap[entity.GetId()];
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_EntityMap.erase(entity.GetId());
	}

	Entity* Scene::FindEntityByName(std::string name)
	{
		for (auto it = m_EntityMap.begin(); it != m_EntityMap.end(); it++)
		{
			if (it->second.GetName() == name)
				return &it->second;
		}

		return nullptr;
	}

	Entity* Scene::GetEntityByUUID(std::string uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return &m_EntityMap.at(uuid);

		return nullptr;
	}

	std::vector<Entity*> Scene::GetEntityVector()
	{
		std::vector<Entity*> entities;
		for (auto it = m_EntityMap.begin(); it != m_EntityMap.end(); it++)
		{
			entities.push_back(&it->second);
		}

		return entities;
	}

	void Scene::Update(double deltaTime)
	{
		m_EditorCamera->Update();

		m_PhysicsEngine->Update(deltaTime);

		if (m_ActiveCamera)
			m_ActiveCamera->Update();

		if (m_OnRuntime)
			UpdateRuntime(deltaTime);
	}

	void Scene::UpdateRuntime(double deltaTime)
	{
		for (Entity* entity : View<NativeScriptComponent>()) {
			NativeScriptComponent& nsc = entity->GetComponent<NativeScriptComponent>();

			if (nsc.Instance == nullptr) {
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = *entity;
				nsc.Instance->m_Window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

				nsc.Instance->OnCreate();
			}

			nsc.Instance->OnUpdate(deltaTime);
		}
	}

	void Scene::OnScenePlay()
	{
		if (m_ActiveCamera)
			m_OnRuntime = true;
	}

	void Scene::OnSceneStop()
	{
		m_OnRuntime = false;

		for (Entity* entity : View<NativeScriptComponent>()) {
			NativeScriptComponent& nsc = entity->GetComponent<NativeScriptComponent>();
			if (nsc.Instance != nullptr) {
				nsc.Instance->OnDestroy();
				nsc.Instance = nullptr;
			}
		}
	}

	void Scene::ResizeEditorCamera(float width, float height)
	{
		m_EditorCamera->OnResize(width, height);
	}

	void Scene::ResizeActiveCamera(float width, float height)
	{
		if (m_ActiveCamera)
			m_ActiveCamera->OnResize(width, height);
	}

	EntityMap* Scene::getEntities()
	{
		return &m_EntityMap;
	}
}