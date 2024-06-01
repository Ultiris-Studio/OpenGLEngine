#include "depch.h"
#include "SceneSerializer.h"

#include <yaml-cpp/yaml.h>

#include <OpenGLEngine/Renderer/Renderer.h>
#include <OpenGLEngine/Entity/Components/TransformComponent.h>
#include <OpenGLEngine/Entity/Components/MaterialComponent.h>
#include <OpenGLEngine/Entity/Components/MeshComponent.h>
#include <OpenGLEngine/Entity/Components/ModelComponent.h>
#include <OpenGLEngine/Entity/Components/CameraComponent.h>
#include <OpenGLEngine/Entity/Components/LightComponent.h>
#include <OpenGLEngine/Entity/Components/ScriptComponent.h>
#include <OpenGLEngine/Entity/Components/Physics/RigidBodyComponent.h>
#include <OpenGLEngine/Entity/Components/Physics/MeshColliderComponent.h>
#include <OpenGLEngine/Entity/Components/Physics/BoxColliderComponent.h>
#include <OpenGLEngine/Entity/Components/Physics/CapsuleColliderComponent.h>

#include <OpenGLEngine/Core/Input.h>
#include <OpenGLEngine/Core/KeyCodes.h>

#include "Importer/TextureConfigImporter.h"

#include <fstream>

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
			{
				return false;
			}
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
}

namespace OpenGLEngine
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	static void SerializeEntity(YAML::Emitter& out, Entity* entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity->GetName();
		out << YAML::Key << "ID" << YAML::Value << entity->GetUUID();
		uint64_t parentID = ((entity->m_Parent != UUID::Null()) ? entity->m_Parent : UUID::Null());
		out << YAML::Key << "ParentID" << YAML::Value << parentID;

		out << YAML::Key << "Components" << YAML::Value << YAML::BeginSeq;

		if (entity->HasComponent<TransformComponent>())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "TransformComponent";
			out << YAML::Value << YAML::BeginMap;

			auto& tc = entity->GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << tc.Position;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		if (entity->HasComponent<MaterialComponent>())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "MaterialComponent";
			out << YAML::Value << YAML::BeginMap;

			auto& mc = entity->GetComponent<MaterialComponent>();

			bool hasAlbedo = *mc.GetMaterial().getBoolean("albedo");
			bool hasNormal = *mc.GetMaterial().getBoolean("normal");
			bool hasMetallic = *mc.GetMaterial().getBoolean("metallic");
			bool hasRoughness = *mc.GetMaterial().getBoolean("roughness");
			bool hasAO = *mc.GetMaterial().getBoolean("ao");

			out << YAML::Key << "hasAlbedo" << YAML::Value << hasAlbedo;
			out << YAML::Key << "hasNormal" << YAML::Value << hasNormal;
			out << YAML::Key << "hasMetallic" << YAML::Value << hasMetallic;
			out << YAML::Key << "hasRoughness" << YAML::Value << hasRoughness;
			out << YAML::Key << "hasAO" << YAML::Value << hasAO;

			out << YAML::Key << "albedo" << YAML::Value << *mc.GetMaterial().getVec3("albedo");
			out << YAML::Key << "metallic" << YAML::Value << *mc.GetMaterial().getFloat("metallic");
			out << YAML::Key << "roughness" << YAML::Value << *mc.GetMaterial().getFloat("roughness");
			out << YAML::Key << "ao" << YAML::Value << *mc.GetMaterial().getFloat("ao");

			if (hasAlbedo)
			{
				out << YAML::Key << "albedoMap" << YAML::Value << mc.m_AlbedoTexture;
			}

			if (hasNormal)
			{
				out << YAML::Key << "normalMap" << YAML::Value << mc.m_NormalTexture;
			}

			if (hasMetallic)
			{
				out << YAML::Key << "metallicMap" << YAML::Value << mc.m_MetallicTexture;
			}

			if (hasRoughness)
			{
				out << YAML::Key << "roughnessMap" << YAML::Value << mc.m_RoughnessTexture;
			}

			if (hasAO)
			{
				out << YAML::Key << "aoMap" << YAML::Value << mc.m_AOTexture;
			}

			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		if (entity->HasComponent<MeshComponent>())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "MeshComponent";
			out << YAML::Value << YAML::BeginMap;

			auto& mc = entity->GetComponent<MeshComponent>();

			out << YAML::Key << "Path" << YAML::Value << mc.GetModelPath();
			out << YAML::Key << "Name" << YAML::Value << mc.GetName();

			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		if (entity->HasComponent<LightComponent>())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "LightComponent";
			out << YAML::Value << YAML::BeginMap;

			auto& lc = entity->GetComponent<LightComponent>();

			if (lc.lightType == LightComponent::LightType::DIRECTIONAL)
			{
				out << YAML::Key << "lightType" << YAML::Value << "Directional";

				out << YAML::Key << "color" << YAML::Value << lc.dir_color;
				out << YAML::Key << "power" << YAML::Value << lc.dir_power;
			}
			else if (lc.lightType == LightComponent::LightType::POINT)
			{
				out << YAML::Key << "lightType" << YAML::Value << "Point";

				out << YAML::Key << "color" << YAML::Value << lc.point_color;
				out << YAML::Key << "attenuation" << YAML::Value << lc.point_attenuation;
				out << YAML::Key << "power" << YAML::Value << lc.point_power;
			}

			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		if (entity->HasComponent<CameraComponent>())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "CameraComponent";
			out << YAML::Value << YAML::BeginMap;

			auto& cc = entity->GetComponent<CameraComponent>();
			out << YAML::Key << "fov" << YAML::Value << cc.GetCamera().GetFov();

			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		if (entity->HasComponent<ScriptComponent>())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "ScriptComponent";
			out << YAML::Value << YAML::BeginMap;

			auto& sc = entity->GetComponent<ScriptComponent>();
			out << YAML::Key << "scriptName" << YAML::Value << sc.m_Name;

			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		if (entity->HasComponent<RigidBodyComponent>())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "RigidBodyComponent";
			out << YAML::Value << YAML::BeginMap;

			auto& rbc = entity->GetComponent<RigidBodyComponent>();
			out << YAML::Key << "enableGravity" << YAML::Value << rbc.enableGravity;
			out << YAML::Key << "bodyType" << YAML::Value << rbc.bodyTypeString;
			out << YAML::Key << "linearAxisFactor_X" << YAML::Value << rbc.m_LinearAxisFactorX;
			out << YAML::Key << "linearAxisFactor_Y" << YAML::Value << rbc.m_LinearAxisFactorY;
			out << YAML::Key << "linearAxisFactor_Z" << YAML::Value << rbc.m_LinearAxisFactorZ;
			out << YAML::Key << "angularAxisFactor_X" << YAML::Value << rbc.m_AngularAxisFactorX;
			out << YAML::Key << "angularAxisFactor_Y" << YAML::Value << rbc.m_AngularAxisFactorY;
			out << YAML::Key << "angularAxisFactor_Z" << YAML::Value << rbc.m_AngularAxisFactorZ;

			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		if (entity->HasComponent<MeshColliderComponent>())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "MeshColliderComponent";
			out << YAML::Value << YAML::BeginMap;

			auto& mcc = entity->GetComponent<MeshColliderComponent>();
			out << YAML::Key << "IsConvex" << YAML::Value << mcc.m_IsConvex;

			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		if (entity->HasComponent<BoxColliderComponent>())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "BoxColliderComponent";
			out << YAML::Value << YAML::BeginMap;
			auto& bcc = entity->GetComponent<BoxColliderComponent>();
			out << YAML::Key << "mass" << YAML::Value << bcc.mass;
			out << YAML::Key << "friction" << YAML::Value << bcc.friction;
			out << YAML::Key << "bounciness" << YAML::Value << bcc.bounciness;
			out << YAML::Key << "useEntityScale" << YAML::Value << bcc.m_UseEntityScale;
			out << YAML::Key << "size" << YAML::Value << bcc.m_Size;
			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		if (entity->HasComponent<CapsuleColliderComponent>())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "CapsuleColliderComponent";
			out << YAML::Value << YAML::BeginMap;
			auto& ccc = entity->GetComponent<CapsuleColliderComponent>();
			out << YAML::Key << "mass" << YAML::Value << ccc.mass;
			out << YAML::Key << "friction" << YAML::Value << ccc.friction;
			out << YAML::Key << "bounciness" << YAML::Value << ccc.bounciness;
			out << YAML::Key << "radius" << YAML::Value << ccc.m_Radius;
			out << YAML::Key << "height" << YAML::Value << ccc.m_Height;
			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::vector<UUID> childrens = entity->m_Children;

		for (auto& child : childrens)
		{
			Entity* childEntity = Renderer::m_SceneData.m_Scene->GetEntityByUUID(child);
			SerializeEntity(out, childEntity);
		}
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << m_Scene->getName();
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		for (auto& entity : m_Scene->GetEntityVector())
		{
			if (!entity->m_Parent)
				SerializeEntity(out, entity);
		};

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();

		m_Scene->setPath(filepath);
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
	}

	static void DeserializeEntity(YAML::Node entity, Scene* scene)
	{
		std::string name = entity["Entity"].as<std::string>();
		UUID uuid = entity["ID"].as<uint64_t>();
		UUID parent_uuid = entity["ParentID"].as<uint64_t>();

		Entity* deserializedEntity = scene->CreateEntityWithUUID(uuid, name);
		Entity* deserializedEntityParent = scene->GetEntityByUUID(parent_uuid);

		if (deserializedEntityParent)
		{
			deserializedEntity->m_Parent = deserializedEntityParent->GetUUID();
			deserializedEntityParent->AddChild(deserializedEntity->GetUUID());
		}

		auto components = entity["Components"];
		if (components)
		{
			for (auto component : components)
			{
				auto transformComponent = component["TransformComponent"];
				if (transformComponent)
				{
					auto& tc = deserializedEntity->AddComponent<TransformComponent>();
					tc.Position = transformComponent["Position"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto meshComponent = component["MeshComponent"];
				if (meshComponent)
				{
					std::string path = meshComponent["Path"].as<std::string>();
					std::string name = meshComponent["Name"].as<std::string>();

					Mesh* mesh;

					if (Renderer::m_SceneData.m_ResourceManager.GetModel(path))
					{
						mesh = Renderer::m_SceneData.m_ResourceManager.GetModel(path)->GetMesh(name);
					}
					else
					{
						mesh = Renderer::m_SceneData.m_ResourceManager.CreateModel(path)->GetMesh(name);
					}

					auto& mc = deserializedEntity->AddComponent<MeshComponent>(name, mesh, path);
				}

				auto materialComponent = component["MaterialComponent"];
				if (materialComponent)
				{
					auto& mc = deserializedEntity->AddComponent<MaterialComponent>();

					bool hasAlbedo = materialComponent["hasAlbedo"].as<bool>();
					bool hasNormal = materialComponent["hasNormal"].as<bool>();
					bool hasMetallic = materialComponent["hasMetallic"].as<bool>();
					bool hasRoughness = materialComponent["hasRoughness"].as<bool>();
					bool hasAO = materialComponent["hasAO"].as<bool>();

					*mc.GetMaterial().getBoolean("albedo") = hasAlbedo;
					*mc.GetMaterial().getBoolean("normal") = hasNormal;
					*mc.GetMaterial().getBoolean("metallic") = hasMetallic;
					*mc.GetMaterial().getBoolean("roughness") = hasRoughness;
					*mc.GetMaterial().getBoolean("ao") = hasAO;

					*mc.GetMaterial().getVec3("albedo") = materialComponent["albedo"].as<glm::vec3>();
					*mc.GetMaterial().getFloat("metallic") = materialComponent["metallic"].as<float>();
					*mc.GetMaterial().getFloat("roughness") = materialComponent["roughness"].as<float>();
					*mc.GetMaterial().getFloat("ao") = materialComponent["ao"].as<float>();

					if (hasAlbedo)
					{
						std::string albedoPath = materialComponent["albedoMap"].as<std::string>();

						mc.addTexture("albedo", albedoPath);

						if (!Renderer::m_SceneData.m_ResourceManager.GetTexture(albedoPath))
						{
							Renderer::m_SceneData.m_ResourceManager.CreateTexture(albedoPath, TextureConfigImporter::ImportTextureConfig(albedoPath));
						}
					}

					if (hasNormal)
					{
						std::string normalPath = materialComponent["normalMap"].as<std::string>();

						mc.addTexture("normal", normalPath);

						if (!Renderer::m_SceneData.m_ResourceManager.GetTexture(normalPath))
						{
							Renderer::m_SceneData.m_ResourceManager.CreateTexture(normalPath, TextureConfigImporter::ImportTextureConfig(normalPath));
						}
					}

					if (hasMetallic)
					{
						std::string metallicPath = materialComponent["metallicMap"].as<std::string>();

						mc.addTexture("metallic", metallicPath);

						if (!Renderer::m_SceneData.m_ResourceManager.GetTexture(metallicPath))
						{
							Renderer::m_SceneData.m_ResourceManager.CreateTexture(metallicPath, TextureConfigImporter::ImportTextureConfig(metallicPath));
						}
					}

					if (hasRoughness)
					{
						std::string roughnessPath = materialComponent["roughnessMap"].as<std::string>();

						mc.addTexture("roughness", roughnessPath);

						if (!Renderer::m_SceneData.m_ResourceManager.GetTexture(roughnessPath))
						{
							Renderer::m_SceneData.m_ResourceManager.CreateTexture(roughnessPath, TextureConfigImporter::ImportTextureConfig(roughnessPath));
						}
					}

					if (hasAO)
					{
						std::string aoPath = materialComponent["aoMap"].as<std::string>();

						mc.addTexture("ao", aoPath);

						if (!Renderer::m_SceneData.m_ResourceManager.GetTexture(aoPath))
						{
							Renderer::m_SceneData.m_ResourceManager.CreateTexture(aoPath, TextureConfigImporter::ImportTextureConfig(aoPath));
						}
					}
				}

				auto lightComponent = component["LightComponent"];
				if (lightComponent)
				{
					if (lightComponent["lightType"].as<std::string>() == "Directional")
					{
						auto& lc = deserializedEntity->AddComponent<LightComponent>(LightComponent::LightType::DIRECTIONAL);
						lc.dir_color = lightComponent["color"].as<glm::vec3>();

						if (lightComponent["power"])
							lc.dir_power = lightComponent["power"].as<float>();
					}

					if (lightComponent["lightType"].as<std::string>() == "Point")
					{
						auto& lc = deserializedEntity->AddComponent<LightComponent>(LightComponent::LightType::POINT);
						lc.point_color = lightComponent["color"].as<glm::vec3>();

						if (lightComponent["attenuation"])
							lc.point_attenuation = lightComponent["attenuation"].as<float>();

						if (lightComponent["power"])
							lc.point_power = lightComponent["power"].as<float>();
					}
				}

				auto cameraComponent = component["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity->AddComponent<CameraComponent>();
					cc.GetCamera().Init(&deserializedEntity->GetComponent<TransformComponent>());
					cc.GetCamera().SetFov(cameraComponent["fov"].as<float>());
				}

				auto scriptComponent = component["ScriptComponent"];
				if (scriptComponent)
				{
					auto& sc = deserializedEntity->AddComponent<ScriptComponent>();
					sc.m_Name = scriptComponent["scriptName"].as<std::string>();
				}

				auto rigidBodyComponent = component["RigidBodyComponent"];
				if (rigidBodyComponent)
				{
					auto& rbc = deserializedEntity->AddComponent<RigidBodyComponent>();
					rbc.Init();

					rbc.enableGravity = rigidBodyComponent["enableGravity"].as<bool>();
					rbc.bodyTypeString = rigidBodyComponent["bodyType"].as<std::string>();

					if (rigidBodyComponent["linearAxisFactor_X"])
						rbc.m_LinearAxisFactorX = rigidBodyComponent["linearAxisFactor_X"].as<bool>();

					if (rigidBodyComponent["linearAxisFactor_Y"])
						rbc.m_LinearAxisFactorY = rigidBodyComponent["linearAxisFactor_Y"].as<bool>();

					if (rigidBodyComponent["linearAxisFactor_Z"])
						rbc.m_LinearAxisFactorZ = rigidBodyComponent["linearAxisFactor_Z"].as<bool>();

					if (rigidBodyComponent["angularAxisFactor_X"])
						rbc.m_AngularAxisFactorX = rigidBodyComponent["angularAxisFactor_X"].as<bool>();

					if (rigidBodyComponent["angularAxisFactor_Y"])
						rbc.m_AngularAxisFactorY = rigidBodyComponent["angularAxisFactor_Y"].as<bool>();

					rbc.UpdateEnableGravity();
					rbc.UpdateBodyType();
				}

				auto meshColliderComponent = component["MeshColliderComponent"];
				if (meshColliderComponent)
				{
					auto& mcc = deserializedEntity->AddComponent<MeshColliderComponent>();
					mcc.m_IsConvex = meshColliderComponent["IsConvex"].as<bool>();
					mcc.Generate();
				}

				auto boxColliderComponent = component["BoxColliderComponent"];
				if (boxColliderComponent)
				{
					auto& bcc = deserializedEntity->AddComponent<BoxColliderComponent>();
					bcc.Init();

					if (boxColliderComponent["mass"])
						bcc.mass = boxColliderComponent["mass"].as<float>();

					if (boxColliderComponent["friction"])
						bcc.friction = boxColliderComponent["friction"].as<float>();

					if (boxColliderComponent["bounciness"])
						bcc.bounciness = boxColliderComponent["bounciness"].as<float>();

					if (boxColliderComponent["useEntityScale"])
						bcc.m_UseEntityScale = boxColliderComponent["useEntityScale"].as<bool>();

					if (boxColliderComponent["size"])
						bcc.m_Size = boxColliderComponent["size"].as<glm::vec3>();

					bcc.UpdateColliderMaterial();
					bcc.UpdateColliderSize();
				}

				auto capsuleColliderComponent = component["CapsuleColliderComponent"];
				if (capsuleColliderComponent)
				{
					auto& ccc = deserializedEntity->AddComponent<CapsuleColliderComponent>();
					ccc.Init();

					if (capsuleColliderComponent["mass"])
						ccc.mass = capsuleColliderComponent["mass"].as<float>();

					if (capsuleColliderComponent["friction"])
						ccc.friction = capsuleColliderComponent["friction"].as<float>();

					if (capsuleColliderComponent["bounciness"])
						ccc.bounciness = capsuleColliderComponent["bounciness"].as<float>();

					if (capsuleColliderComponent["radius"])
						ccc.m_Radius = capsuleColliderComponent["radius"].as<float>();

					if (capsuleColliderComponent["height"])
						ccc.m_Height = capsuleColliderComponent["height"].as<float>();

					ccc.UpdateColliderMaterial();
					ccc.UpdateColliderSize();
				}
			}
		}
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
		{
			return false;
		}

		std::string sceneName = data["Scene"].as<std::string>();
		m_Scene->setName(sceneName);
		m_Scene->setPath(filepath);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				//if (entity["ParentID"].as<uint64_t>() != 0)
					DeserializeEntity(entity, m_Scene);
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
		{
			return false;
		}

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				//if (entity["ParentID"].as<uint64_t>() != 0)
					DeserializeEntity(entity, m_Scene);
			}
		}

		return true;
	}
}