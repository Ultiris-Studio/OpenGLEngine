#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <OpenGLEngine/Asset/Asset.h>

namespace OpenGLEngine
{
	class Texture : public Asset
	{
	private:
		unsigned int m_id;
		unsigned int m_Format;
		unsigned int m_InternalFormat;

		int m_Width, m_Height;

		std::string m_path;
	public:
		Texture();
		Texture(const std::string& path, bool gamma, unsigned int min_filter_param, unsigned int mag_filter_param);
		Texture(const std::vector<unsigned char>& imageData, bool gamma, unsigned int min_filter_param, unsigned int mag_filter_param);
		~Texture();

		static std::shared_ptr<Texture> CreateTexture(const std::string& path, bool gamma);
		static std::shared_ptr<Texture> CreateTexture(const std::vector<unsigned char>& imageData, bool gamma);

		unsigned int GetID() const { return m_id; }

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

		std::string getPath() const;

		void bind() const;

		static AssetType GetStaticType() { return AssetType::TEXTURE; }
		virtual AssetType GetType() override { return GetStaticType(); }
	};
}