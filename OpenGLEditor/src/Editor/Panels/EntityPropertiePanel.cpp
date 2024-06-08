#include "EntityPropertiePanel.h"

#include "imgui.h"
#include <imgui/imgui_internal.h>
#include "glm/gtc/type_ptr.hpp"

#include <filesystem>
#include <sstream>

#include <OpenGLEngine/Renderer/Renderer.h>
#include <OpenGLEngine/Scene/Scene.h>
#include <OpenGLEngine/Entity/Components/TransformComponent.h>
#include <OpenGLEngine/Entity/Components/MeshComponent.h>
#include <OpenGLEngine/Entity/Components/MaterialComponent.h>
#include <OpenGLEngine/Entity/Components/CameraComponent.h>
#include <OpenGLEngine/Entity/Components/LightComponent.h>
#include <OpenGLEngine/Entity/Components/TerrainComponent.h>
#include <OpenGLEngine/Entity/Components/ScriptComponent.h>
#include <OpenGLEngine/Entity/Components/MeshRendererComponent.h>
#include <OpenGLEngine/Entity/Components/Physics/RigidBodyComponent.h>
#include <OpenGLEngine/Entity/Components/Physics/BoxColliderComponent.h>
#include <OpenGLEngine/Entity/Components/Physics/MeshColliderComponent.h>
#include <OpenGLEngine/Entity/Components/Physics/CapsuleColliderComponent.h>
#include <OpenGLEngine/Entity/Components/Gameplay/CharacterControllerComponent.h>

#include <OpenGLEngine/Core/UUID.h>

namespace OpenGLEngine
{
	EntityPropertiePanel::EntityPropertiePanel()
	{
		m_TransformComponentPanel = std::make_unique<TransformComponentPanel>();
		m_CameraComponentPanel = std::make_unique<CameraComponentPanel>();
		m_MeshComponentPanel = std::make_unique<MeshComponentPanel>();
		m_TerrainComponentPanel = std::make_unique<TerrainComponentPanel>();
		m_MaterialComponentPanel = std::make_unique<MaterialComponentPanel>();
		m_LightComponentPanel = std::make_unique<LightComponentPanel>();
		m_ScriptComponentPanel = std::make_unique<ScriptComponentPanel>();
		m_MeshRendererComponentPanel = std::make_unique<MeshRendererComponentPanel>();
		m_RigidBodyComponentPanel = std::make_unique<RigidBodyComponentPanel>();
		m_BoxColliderComponentPanel = std::make_unique<BoxColliderComponentPanel>();
		m_MeshColliderComponentPanel = std::make_unique<MeshColliderComponentPanel>();
		m_CapsuleColliderComponentPanel = std::make_unique<CapsuleColliderComponentPanel>();
		m_CharacterControllerComponentPanel = std::make_unique<CharacterControllerComponentPanel>();
	}

	void EntityPropertiePanel::OnImGuiRender(SceneManager& sceneManager, SceneHierarchy& sceneHierarchy)
	{
		ImGui::Begin("Inspector");

		if (sceneHierarchy.m_SelectedEntity)
		{
			if (sceneManager.getActiveScene().isOnRuntime())
			{
				//ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				//ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			Entity* entity = sceneHierarchy.m_SelectedEntity;

			UUID uuid = entity->GetUUID();

			std::string result;

			std::stringstream sstm;
			sstm << "UUID: " << uuid;
			result = sstm.str();
			ImGui::Text(result.c_str());
			ImGui::Separator();

			std::stringstream sstm2;
			sstm2 << "##" << uuid;
			result = sstm2.str();
			ImGui::InputText(result.c_str(), entity->GetName(), 20);

			ImGui::Separator();

			m_TransformComponentPanel->Render(entity);
			m_CameraComponentPanel->Render(entity, sceneManager);
			m_MeshRendererComponentPanel->Render(entity);
			m_MeshComponentPanel->Render(entity);
			m_ScriptComponentPanel->Render(entity);
			m_TerrainComponentPanel->Render(entity);
			m_MaterialComponentPanel->Render(entity);
			m_LightComponentPanel->Render(entity);
			m_RigidBodyComponentPanel->Render(entity);
			m_BoxColliderComponentPanel->Render(entity);
			m_MeshColliderComponentPanel->Render(entity);
			m_CapsuleColliderComponentPanel->Render(entity);
			m_CharacterControllerComponentPanel->Render(entity);

			if (ImGui::Button("Add Component")) {
				ImGui::OpenPopup("AddComponent");
			}

			if (ImGui::BeginPopup("AddComponent")) {

				if (!entity->HasComponent<TransformComponent>()) {
					if (ImGui::MenuItem("Transform Component")) {
						entity->AddComponent<TransformComponent>();
					}
				}

				if (!entity->HasComponent<MeshRendererComponent>()) {
					if (ImGui::MenuItem("Mesh Renderer Component")) {
						entity->AddComponent<MeshRendererComponent>();

						//Renderer::m_SceneData.m_Scene->RegisterRenderedEntity(entity);
					}
				}

				if (!entity->HasComponent<MeshComponent>()) {
					if (ImGui::MenuItem("Mesh Component")) {
						if (!entity->HasComponent<MeshRendererComponent>()) {
							entity->AddComponent<MeshRendererComponent>();

							//Renderer::m_SceneData.m_Scene->RegisterRenderedEntity(entity);
						}
						entity->AddComponent<MeshComponent>();
					}
				}

				if (!entity->HasComponent<TerrainComponent>()) {
					if (ImGui::MenuItem("Terrain Component")) {
						entity->AddComponent<TerrainComponent>();
					}
				}

				if (!entity->HasComponent<MaterialComponent>()) {
					if (ImGui::MenuItem("Material Component")) {
						entity->AddComponent<MaterialComponent>();
						entity->GetComponent<MaterialComponent>();
					}
				}

				if (!entity->HasComponent<ScriptComponent>()) {
					if (ImGui::MenuItem("Script Component")) {
						entity->AddComponent<ScriptComponent>();
					}
				}

				if (!entity->HasComponent<CameraComponent>()) {
					if (ImGui::MenuItem("Camera Component")) {
						entity->AddComponent<CameraComponent>().GetCamera().Init(&entity->GetComponent<TransformComponent>());
					}
				}

				if (!entity->HasComponent<LightComponent>()) {
					if (ImGui::MenuItem("Directional Light")) {
						entity->AddComponent<LightComponent>(LightComponent::LightType::DIRECTIONAL);
					}
				}

				if (!entity->HasComponent<LightComponent>()) {
					if (ImGui::MenuItem("Point Light Component")) {
						entity->AddComponent<LightComponent>(LightComponent::LightType::POINT);
					}
				}

				if (!entity->HasComponent<RigidBodyComponent>()) {
					if (ImGui::MenuItem("RigidBody Component")) {
						entity->AddComponent<RigidBodyComponent>().Init();
					}
				}

				if (!entity->HasComponent<BoxColliderComponent>()) {
					if (ImGui::MenuItem("Box Collider Component")) {
						entity->AddComponent<BoxColliderComponent>().Init();
					}
				}

				if (!entity->HasComponent<MeshColliderComponent>()) {
					if (ImGui::MenuItem("Mesh Collider Component")) {
						entity->AddComponent<MeshColliderComponent>();
					}
				}

				if (!entity->HasComponent<CapsuleColliderComponent>()) {
					if (ImGui::MenuItem("Capsule Collider Component")) {
						entity->AddComponent<CapsuleColliderComponent>().Init();
					}
				}

				if (!entity->HasComponent<CharacterControllerComponent>()) {
					if (ImGui::MenuItem("Character Controller Component")) {
						entity->AddComponent<CharacterControllerComponent>().Init();
					}
				}

				ImGui::EndPopup();
			}

			if (sceneManager.getActiveScene().isOnRuntime())
			{
				//ImGui::PopItemFlag();
				//ImGui::PopStyleVar();
			}
		}

		ImGui::End();
	}
}
