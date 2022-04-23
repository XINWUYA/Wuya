#pragma once

namespace Wuya
{
	class ITexture
	{
	public:
		ITexture() = default;
		virtual ~ITexture() = default;

		virtual void Bind(uint32_t slot = 0) = 0;
		virtual void SetData(void* data, uint32_t size) = 0;

		/* 获取纹理原始尺寸 */
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		/* 获取纹理所在路径 */
		virtual const std::string& GetPath() const = 0;

		/* 纹理ID */
		virtual uint32_t GetTextureID() const = 0;
		/* 纹理成功加载 */
		virtual bool IsLoaded() const = 0;

		virtual bool operator==(const ITexture& other) const = 0;

	};

	class Texture2D : public ITexture
	{
	public:
		static SharedPtr<Texture2D> Create(uint32_t width, uint32_t height);
		static SharedPtr<Texture2D> Create(const std::string& path);
	};
}


