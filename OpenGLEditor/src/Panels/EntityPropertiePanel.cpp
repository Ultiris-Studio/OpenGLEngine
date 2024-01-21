#include "EntityPropertiePanel.h"

#include "imgui.h"
#include <imgui/imgui_internal.h>
#include "glm/gtc/type_ptr.hpp"

#include <filesystem>
#include <sstream>

#include <OpenGLEngine/Scene/Scene.h>
#include <OpenGLEngine/Entity/Components/TransformComponent.h>
#include <OpenGLEngine/Entity/Components/ModelComponent.h>
#include <OpenGLEngine/Entity/Components/MeshComponent.h>
#include <OpenGLEngine/Entity/Components/MaterialComponent.h>
#include <OpenGLEngine/Entity/Components/CameraComponent.h>
#include <OpenGLEngine/Entity/Components/LightComponent.h>
#include <OpenGLEngine/Entity/Components/TerrainComponent.h>

namespace OpenGLEngine
{
	EntityPropertiePanel::EntityPropertiePanel()
	{
		m_TransformComponentPanel = std::make_unique<TransformComponentPanel>();
		m_ModelComponentPanel = std::make_unique<ModelComponentPanel>();
		m_CameraComponentPanel = std::make_unique<CameraComponentPanel>();
		m_MeshComponentPanel = std::make_unique<MeshComponentPanel>();
		m_TerrainComponentPanel = std::make_unique<TerrainComponentPanel>();
		m_MaterialComponentPanel = std::make_unique<MaterialComponentPanel>();
		m_LightComponentPanel = std::make_unique<LightComponentPanel>();
	}

	void EntityPropertiePanel::OnImGuiRender(SceneManager& sceneManager)
	{
		ImGui::Begin("Inspector");

		if (sceneManager.getActiveScene().m_SelectedEntity)
		{
			if (sceneManager.getActiveScene().isOnRuntime())
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			Entity* entity = sceneManager.getActiveScene().m_SelectedEntity;

			char* uuid = entity->GetId();

			std::string result;

			std::stringstream sstm;
			sstm << "UUID: " << uuid;
			result = sstm.str();
			ImGui::Text(result.c_str());
			ImGui::Separator();

			std::stringstream sstm2;
			sstm2 << "##" << uuid;
			result = sstm2.str();
			ImGui::InputText(result.c_str(), entity->GetName(), 10);

			ImGui::Separator();

			m_TransformComponentPanel->Render(entity);
			m_ModelComponentPanel->Render(entity);
			m_CameraComponentPanel->Render(entity, sceneManager);
			m_MeshComponentPanel->Render(entity);
			m_TerrainComponentPanel->Render(entity);
			m_MaterialComponentPanel->Render(entity);
			m_LightComponentPanel->Render(entity);

			if (ImGui::Button("Add Component")) {
				ImGui::OpenPopup("AddComponent");
			}

			if (ImGui::BeginPopup("AddComponent")) {

				if (!entity->HasComponent<TransformComponent>()) {
					if (ImGui::MenuItem("Transform Component")) {
						entity->AddComponent<TransformComponent>();
					}
				}

				if (!entity->HasComponent<ModelComponent>()) {
					if (ImGui::MenuItem("Model Component")) {
						entity->AddComponent<ModelComponent>();
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
						entity->GetComponent<MaterialComponent>().InitializeMaterial();
					}
				}

				if (!entity->HasComponent<CameraComponent>()) {
					if (ImGui::MenuItem("Camera Component")) {
						entity->AddComponent<CameraComponent>().Init();
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

				ImGui::EndPopup();
			}

			if (sceneManager.getActiveScene().isOnRuntime())
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}

		ImGui::End();
	}
}
