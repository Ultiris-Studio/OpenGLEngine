#pragma once

#include <vector>

#include <OpenGLEngine/Resources/Vertex.h>
#include <OpenGLEngine/Asset/Asset.h>
#include <OpenGLEngine/Resources/Materials/Material.h>

namespace OpenGLEngine
{
	enum class DrawMode
	{
		TRIANGLES = 0,
		TRIANGLE_STRIP,
		TRIANGLE_FAN,
		LINES,
		LINE_STRIP,
		LINE_LOOP,
		POINTS
	};

	class Mesh : public Asset
	{
	private:
		unsigned int m_vao;
		unsigned int m_vbo;
		unsigned int m_ebo;

		DrawMode m_drawMode;

		std::vector<Vertex> m_vertices;
		std::vector<unsigned int> m_indices;

		MaterialSpecification m_material;

	public:
		Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, DrawMode drawMode = DrawMode::TRIANGLES);
		Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const MaterialSpecification& material, DrawMode drawMode = DrawMode::TRIANGLES);
		~Mesh();

		void draw() const;

		const MaterialSpecification& GetMaterial() const { return m_material; }

		void GenerateMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

		const size_t& GetVerticesCount() { return m_vertices.size(); }
		const size_t& GetIndicesCount() { return m_indices.size(); }

		const std::vector<Vertex>& GetVertices() const { return m_vertices; }
		const std::vector<unsigned int>& GetIndices() const { return m_indices; }

		static AssetType GetStaticType() { return AssetType::MESH; }
		virtual AssetType GetType() override { return GetStaticType(); }
	};
}