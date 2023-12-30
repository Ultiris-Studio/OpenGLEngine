#include "EntityPropertiePanel.h"
#include "glm/gtc/type_ptr.hpp"

#include "imgui.h"
#include <imgui/imgui_internal.h>

#include <filesystem>
#include <sstream>

#include <OpenGLEngine/Scene/Scene.h>
#include <OpenGLEngine/Entity/Components/TransformComponent.h>
#include <OpenGLEngine/Entity/Components/ModelComponent.h>
#include <OpenGLEngine/Entity/Components/MaterialComponent.h>
#include <OpenGLEngine/Entity/Components/SkyboxComponent.h>
#include <OpenGLEngine/Entity/Components/CameraComponent.h>
#include <OpenGLEngine/Entity/Components/NativeScriptComponent.h>
#include <OpenGLEngine/Entity/Components/LightComponent.h>

namespace OpenGLEngine
{
	EntityPropertiePanel::EntityPropertiePanel()
	{
		m_ModelTexture = Texture::CreateTexture("Icons/texture_obj.png");
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

			if (entity->HasComponent<TransformComponent>())
			{
				auto& tc = entity->GetComponent<TransformComponent>();

				if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
				{
					DrawVec3Control("Translation", tc.Position);
					glm::vec3 rotation = glm::degrees(tc.Rotation);
					DrawVec3Control("Rotation", rotation);
					tc.Rotation = glm::radians(rotation);
					DrawVec3Control("Scale", tc.Scale, 1.0f);

					ImGui::TreePop();
				}
			}

			if (entity->HasComponent<CameraComponent>())
			{
				auto& cc = entity->GetComponent<CameraComponent>();

				if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
				{
					ImGui::Text("Camera parameters: ");

					ImGui::Separator();

					bool isActive = sceneManager.getActiveScene().getActiveCamera() == &cc.GetCamera();
					if (ImGui::Checkbox("Active camera", &isActive))
					{
						sceneManager.getActiveScene().setActiveCamera(&cc.GetCamera());
					}

					ImGui::Separator();

					ImGui::Text("FOV: "); ImGui::SameLine();
					ImGui::DragFloat("##FOV", &cc.GetCamera().m_fov, 0.1f, 0.0f, 180.0f, "%.1f");					

					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Delete Component"))
						{
							entity->RemoveComponent<MaterialComponent>();
						}
						ImGui::EndPopup();
					}
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
							}
							ImGui::EndDragDropTarget();
						}
						ImGui::SameLine();
						if (mc.GetMaterial().hasTexture("diffuse"))
						{
							ImGui::Checkbox("Use Diffuse Texture", mc.GetMaterial().getBoolean("diffuse").get());
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
							}
							ImGui::EndDragDropTarget();
						}
						ImGui::SameLine();
						if (mc.GetMaterial().hasTexture("specular"))
						{
							ImGui::Checkbox("Use Specular Texture", mc.GetMaterial().getBoolean("specular").get());
						}

						ImGui::Text("Specular Color: ");
						ImGui::ColorEdit3("##SpecularColor", glm::value_ptr(*mc.GetMaterial().getVec3("specular")));

						ImGui::Separator();

						ImGui::Text("Shininess: ");
						ImGui::InputFloat("##Shininess", mc.GetMaterial().getFloat("shininess").get());
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

			if (entity->HasComponent<LightComponent>())
			{
				auto& lc = entity->GetComponent<LightComponent>();

				if (ImGui::TreeNodeEx("Light", ImGuiTreeNodeFlags_DefaultOpen, "Light"))
				{
					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Delete Component"))
						{
							entity->RemoveComponent<LightComponent>();
						}
						ImGui::EndPopup();
					}

					const char* items[] = { "Directional Light", "Point Light" };
					const char* current_item = items[0];

					if (ImGui::BeginCombo("##combolighttype", current_item))
					{
						for (int n = 0; n <= 1; n++)
						{
							bool is_selected = (current_item == items[n]);
							if (ImGui::Selectable(items[n], is_selected))
							{
								if (items[n] == "Directional Light")
								{
									lc.SetType(LightComponent::LightType::DIRECTIONAL);
								}
								else if (items[n] == "Point Light")
								{
									lc.SetType(LightComponent::LightType::POINT);
								}
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					if (lc.lightType == LightComponent::LightType::DIRECTIONAL)
					{
						ImGui::Text("Directional light direction: ");
						DrawVec3Control("dirlightdirection", lc.dir_direction);

						ImGui::Text("Directional light ambient: ");
						ImGui::ColorEdit3("##dirlightambient", glm::value_ptr(lc.dir_ambient));

						ImGui::Text("Directional light diffuse: ");
						ImGui::ColorEdit3("##dirlightdiffuse", glm::value_ptr(lc.dir_diffuse));

						ImGui::Text("Directional light specular: ");
						ImGui::ColorEdit3("##dirlightspecular", glm::value_ptr(lc.dir_specular));
					}
					else if (lc.lightType == LightComponent::LightType::POINT)
					{
						ImGui::Text("Point light position: ");
						DrawVec3Control("pointlightposition", lc.point_position);
						ImGui::Text("Point light ambient: ");
						ImGui::ColorEdit3("##pointlightambient", glm::value_ptr(lc.point_ambient));
						ImGui::Text("Point light diffuse: ");
						ImGui::ColorEdit3("##pointlightdiffuse", glm::value_ptr(lc.point_diffuse));
						ImGui::Text("Point light specular: ");
						ImGui::ColorEdit3("##pointlightspecular", glm::value_ptr(lc.point_specular));
						ImGui::Text("Point light constant: ");
						ImGui::InputFloat("##pointlightconstant", &lc.point_constant);
						ImGui::Text("Point light linear: ");
						ImGui::InputFloat("##pointlightlinear", &lc.point_linear);
						ImGui::Text("Point light quadratic: ");
						ImGui::InputFloat("##pointlightquadratic", &lc.point_quadratic);
					}

					ImGui::TreePop();
				}
			}

			if (entity->HasComponent<NativeScriptComponent>())
			{
				auto& nsc = entity->GetComponent<NativeScriptComponent>();

				if (ImGui::TreeNodeEx("Script Component", ImGuiTreeNodeFlags_DefaultOpen, "Script Component"))
				{
					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Delete Component"))
						{
							entity->RemoveComponent<NativeScriptComponent>();
						}
						ImGui::EndPopup();
					}

					/*if (ImGui::InputText("##script_name", nsc.getScriptName(), 10))
					{
						std::cout << nsc.getScriptName() << std::endl;
					}

					if (ImGui::Button("Reload"))
					{
						nsc.Bind();
					}*/

					int size = static_cast<int>(nsc.getLoadedScriptNames().size());
					const char** items = new const char* [size];

					for (int i = 0; i < size; i++)
					{
						items[i] = nsc.getLoadedScriptNames()[i].c_str();
					}

					const char* current_item = nsc.getScriptName();

					if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
					{
						for (int n = 0; n < size; n++)
						{
							bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
							if (ImGui::Selectable(items[n], is_selected))
								nsc.setScriptName(items[n]);
							nsc.Bind();
							if (is_selected)
								ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
						}
						ImGui::EndCombo();
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

				if (!entity->HasComponent<SkyboxComponent>() && !entity->HasComponent<ModelComponent>()) {
					if (ImGui::MenuItem("Skybox Component")) {
						entity->AddComponent<SkyboxComponent>();
					}
				}

				if (!entity->HasComponent<CameraComponent>()) {
					if (ImGui::MenuItem("Camera Component")) {
						entity->AddComponent<CameraComponent>().Init();
					}
				}

				if (!entity->HasComponent<LightComponent>()) {
					if (ImGui::MenuItem("Light Component")) {
						entity->AddComponent<LightComponent>();
					}
				}

				if (!entity->HasComponent<NativeScriptComponent>()) {
					if (ImGui::MenuItem("Script Component")) {
						entity->AddComponent<NativeScriptComponent>();
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

	void EntityPropertiePanel::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}
}
