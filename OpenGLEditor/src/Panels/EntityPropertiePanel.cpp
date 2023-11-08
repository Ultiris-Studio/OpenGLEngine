#include "EntityPropertiePanel.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include <filesystem>
#include <OpenGLEngine/Entity/Components/TransformComponent.h>
#include <OpenGLEngine/Entity/Components/ModelComponent.h>
#include <OpenGLEngine/Entity/Components/MaterialComponent.h>
#include <OpenGLEngine/Entity/Components/SkyboxComponent.h>
#include <OpenGLEngine/Entity/Components/RenderComponent.h>

namespace OpenGLEngine
{
	EntityPropertiePanel::EntityPropertiePanel()
	{
		m_ModelTexture = Texture::CreateTexture("Icons/texture_obj.png");
	}

	void EntityPropertiePanel::OnImGuiRender(Entity* entity)
	{
		ImGui::Text("Information:");

		ImGui::Separator();

		std::stringstream sstm;
		sstm << "Object_" << entity->GetId();
		std::string result = sstm.str();
		ImGui::InputText(result.c_str(), entity->GetName(), 10);

		ImGui::Separator();

		if (entity->HasComponent<TransformComponent>())
		{
			auto& tc = entity->GetComponent<TransformComponent>();

			if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				ImGui::Text("Position: "); ImGui::SameLine();
				ImGui::DragFloat3("##Position", glm::value_ptr(tc.Position), 0.1f, -1000.0f, 1000.0f);
				ImGui::Text("Rotation: "); ImGui::SameLine();
				ImGui::DragFloat3("##Rotation", glm::value_ptr(tc.Rotation), 0.1f, 0.0f, 180.0f);
				ImGui::Text("Scale: "); ImGui::SameLine();
				ImGui::DragFloat3("##Scale", glm::value_ptr(tc.Scale), 0.1f, 100.0f);

				ImGui::TreePop();
			}
		}

		if (entity->HasComponent<ModelComponent>())
		{
			auto& mc = entity->GetComponent<ModelComponent>();

			if (ImGui::TreeNodeEx("Model", ImGuiTreeNodeFlags_DefaultOpen, "Model"))
			{
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete Component"))
					{
						entity->RemoveComponent<ModelComponent>();
					}
					ImGui::EndPopup();
				}

				ImGui::Text("Model: "); ImGui::SameLine();
				ImGui::ImageButton((ImTextureID)m_ModelTexture->GetID(), { 24.0f, 24.0f }, { 0, 1 }, { 1, 0 });

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path filesys = path;
						std::string file = filesys.string();

						mc.SetModel(file);
						entity->GetComponent<RenderComponent>().GenerateShader();
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::TreePop();
			}
		}

		if (entity->HasComponent<MaterialComponent>())
		{
			auto& mc = entity->GetComponent<MaterialComponent>();

			if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen, "Material"))
			{
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete Component"))
					{
						entity->RemoveComponent<MaterialComponent>();
					}
					ImGui::EndPopup();
				}

				/*const char* items[] = {"BPhong", "CubeMap"};

				if (ImGui::BeginCombo("##combo", mc.current_item))
				{
					for (int n = 0; n < IM_ARRAYSIZE(items); n++)
					{
						bool is_selected = (mc.current_item == items[n]);
						if (ImGui::Selectable(items[n], is_selected))
							mc.current_item = items[n];
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}*/

				if (entity->HasComponent<ModelComponent>())
				{
					ImGui::Text("Diffuse texture: ");
					ImGui::ImageButton((ImTextureID)mc.GetEditorDiffuseTexture().GetID(), { 64.0f, 64.0f }, { 0, 1 }, { 1, 0 });
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path filesys = path;
							std::string file = filesys.string();
							mc.GetMaterial().addTexture("diffuse", file);
							entity->GetComponent<RenderComponent>().GenerateShader();
						}
						ImGui::EndDragDropTarget();
					}
					ImGui::SameLine();
					if (mc.GetMaterial().hasTexture("diffuse"))
					{
						if (ImGui::Checkbox("Use Diffuse Texture", mc.GetMaterial().getBoolean("diffuse").get()))
						{
							entity->GetComponent<RenderComponent>().GenerateShader();
						}
					}

					ImGui::Text("Diffuse Color: ");
					ImGui::ColorEdit3("##DiffuseColor", glm::value_ptr(*mc.GetMaterial().getVec3("diffuse")));

					ImGui::Separator();

					ImGui::Text("Specular texture: ");
					ImGui::ImageButton((ImTextureID)mc.GetEditorSpecularTexture().GetID(), { 64.0f, 64.0f }, { 0, 1 }, { 1, 0 });
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path filesys = path;
							std::string file = filesys.string();
							mc.GetMaterial().addTexture("specular", file);
							entity->GetComponent<RenderComponent>().GenerateShader();
						}
						ImGui::EndDragDropTarget();
					}
					ImGui::SameLine();
					if (mc.GetMaterial().hasTexture("specular"))
					{
						if (ImGui::Checkbox("Use Specular Texture", mc.GetMaterial().getBoolean("specular").get()))
						{
							entity->GetComponent<RenderComponent>().GenerateShader();
						}
					}

					ImGui::Text("Specular Color: "); //ImGui::SameLine();
					ImGui::ColorEdit3("##SpecularColor", glm::value_ptr(*mc.GetMaterial().getVec3("specular")));
				}

				ImGui::TreePop();
			}
		}

		if (entity->HasComponent<SkyboxComponent>())
		{
			auto& mc = entity->GetComponent<SkyboxComponent>();

			if (ImGui::TreeNodeEx("Skybox", ImGuiTreeNodeFlags_DefaultOpen, "Skybox"))
			{
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete Component"))
					{
						entity->RemoveComponent<SkyboxComponent>();
					}
					ImGui::EndPopup();
				}

				ImGui::TreePop();
			}
		}

		if (entity->HasComponent<RenderComponent>())
		{
			auto& rc = entity->GetComponent<RenderComponent>();

			if (ImGui::TreeNodeEx("Render", ImGuiTreeNodeFlags_DefaultOpen, "Render"))
			{
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete Component"))
					{
						entity->RemoveComponent<RenderComponent>();
					}
					ImGui::EndPopup();
				}

				if (ImGui::Checkbox("Active render", &rc.m_CanDraw))
				{
					entity->GetComponent<RenderComponent>().GenerateShader();
				}

				ImGui::TreePop();
			}
		}

		if (ImGui::Button("Add Component")) {
			ImGui::OpenPopup("AddComponent");
		}

		if (ImGui::BeginPopup("AddComponent")) {

			if (!entity->HasComponent<TransformComponent>()) {
				if (ImGui::MenuItem("Transform Component")) {
					entity->AddComponent<TransformComponent>();
				}
			}

			if (!entity->HasComponent<ModelComponent>() && !entity->HasComponent<SkyboxComponent>()) {
				if (ImGui::MenuItem("Model Component")) {
					entity->AddComponent<ModelComponent>();
				}
			}

			if (!entity->HasComponent<MaterialComponent>()) {
				if (ImGui::MenuItem("Material Component")) {
					entity->AddComponent<MaterialComponent>();
					entity->GetComponent<MaterialComponent>().GetMaterial().addVec3("ambient", glm::vec3(0.1f));
					entity->GetComponent<MaterialComponent>().GetMaterial().addVec3("diffuse", glm::vec3(1.0f));
					entity->GetComponent<MaterialComponent>().GetMaterial().addVec3("specular", glm::vec3(1.0f));
					entity->GetComponent<MaterialComponent>().GetMaterial().addBoolean("diffuse", false);
					entity->GetComponent<MaterialComponent>().GetMaterial().addBoolean("specular", false);
					entity->GetComponent<MaterialComponent>().GetMaterial().addFloat("shininess", 32.0f);
				}
			}

			if (!entity->HasComponent<RenderComponent>()) {
				if (ImGui::MenuItem("Render Component")) {
					entity->AddComponent<RenderComponent>();
					entity->GetComponent<RenderComponent>().GenerateShader();
				}
			}

			if (!entity->HasComponent<SkyboxComponent>() && !entity->HasComponent<ModelComponent>()) {
				if (ImGui::MenuItem("Skybox Component")) {
					entity->AddComponent<SkyboxComponent>("Assets/Models/cube.obj");
				}
			}

			ImGui::EndPopup();
		}
	}
}
