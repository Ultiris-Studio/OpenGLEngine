#include "MeshRendererComponentPanel.h"

#include <imgui.h>

#include <OpenGLEngine/Entity/Entity.h>
#include <OpenGLEngine/Entity/Components/MeshRendererComponent.h>

namespace OpenGLEngine
{
	void MeshRendererComponentPanel::Render(Entity* entity)
	{
		if (entity->HasComponent<MeshRendererComponent>())
		{
			auto& mrc = entity->GetComponent<MeshRendererComponent>();

			if (ImGui::TreeNodeEx("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen, "Mesh Renderer"))
			{
				ImGui::Text("Render: "); ImGui::SameLine();
				ImGui::Checkbox("##Render", &mrc.m_Rendered);

				ImGui::TreePop();
			}
		}
	}
}
