#pragma once
#include <string>
#include <glad/glad.h>
#include "Wuya/Renderer/Texture.h"
#include "Wuya/Renderer/RenderCommon.h"

namespace Wuya
{
	/**
	 * \brief 根据描述创建纹理，可能是普通纹理，也可能是RenderBuffer
	 */
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const std::string& name, const TextureDesc& texture_desc);
		OpenGLTexture(const std::string& path, const TextureLoadConfig& load_config);
		~OpenGLTexture() override;

		/* 绑定当前纹理 */
		void Bind(uint32_t slot = 0) override;
		/* 解除纹理绑定 */
		void Unbind() override;
		/* 填充纹理数据 */
		void SetData(void* data, const PixelDesc& pixel_desc, uint32_t level = 0,
			uint32_t offset_x = 0, uint32_t offset_y = 0, uint32_t offset_z = 0) override;
		
		/* 获取纹理所在路径 */
		const std::string& GetPath() const override { return m_Path; }

		/* 纹理ID */
		uint32_t GetTextureID() const override { return m_TextureId; }
		/* 纹理是否成功加载 */
		bool IsLoaded() const override { return m_IsLoaded; }

		/* 重载operators */
		bool operator==(const Texture& other) const override
		{
			return ((OpenGLTexture&)other).GetTextureID() == m_TextureId;
		}

	private:
		/* 硬件层TextureId */
		uint32_t m_TextureId{ 0 };
		/* 硬件层TextureFormat */
		GLenum m_InternalFormat{};
		/* 硬件层Target */
		GLenum m_TextureTarget{};
		/* 纹理所在路径，仅在由文件加载时有用 */
		std::string m_Path{};
		/* 纹理是否加载完成，仅在由文件加载时有用 */
		bool m_IsLoaded{ false };

		friend class OpenGLFrameBuffer;
	};
}
