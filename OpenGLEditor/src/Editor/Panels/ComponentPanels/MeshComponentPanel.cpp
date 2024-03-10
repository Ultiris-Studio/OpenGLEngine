#include "MeshComponentPanel.h"

#include <imgui.h>

#include <OpenGLEngine/Entity/Entity.h>
#include <OpenGLEngine/Entity/Components/MeshComponent.h>

namespace OpenGLEngine
{
	void MeshComponentPanel::Render(Entity* entity)
	{
		if (entity->HasComponent<MeshComponent>())
		{
			auto& mc = entity->GetComponent<MeshComponent>();

			if (ImGui::TreeNodeEx("Mesh", ImGuiTreeNodeFlags_DefaultOpen, "Mesh"))
			{
				ImGui::Text("Mesh: "); ImGui::SameLine();
				ImGui::Text(mc.GetName().c_str());

				ImGui::TreePop();
			}
		}
	}
}