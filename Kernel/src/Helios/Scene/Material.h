#pragma once
#include <any>
#include "Helios/Renderer/RenderCommon.h"

namespace Helios
{
	class DeviceTexture;
	class DeviceShader;

	enum class ParamType : uint8_t
	{
		Texture = 0,
		Int,
		Float,
		Vec2,
		Vec3,
		Vec4,
		Mat4,
	};

	/* 材质参数信息 */
	struct MaterialParamInfo
	{
		ParamType	Type{}; /* 参数类型 */
		std::string Name;   /* 参数名 */
		std::any	Value;  /* 参数值, 对于Texture需特殊处理，其Value类型为std::pair<SharedPtr<DeviceTexture>, uint32_t> */

		MaterialParamInfo() = default;
		MaterialParamInfo(ParamType type, std::string name, std::any value)
			: Type(type), Name(std::move(name)), Value(std::move(value))
		{
		}
	};

	/*
	 * 材质类
	 */
	class Material
	{
		using ParameterMap = std::unordered_map<uint32_t, MaterialParamInfo>;

	public:
		Material() = default;
		~Material();

		/* 设置Shader */
		void SetShader(const SharedPtr<DeviceShader>& shader);
		[[nodiscard]] const SharedPtr<DeviceShader>& GetShader() const { return m_pShader; }
		/* 设置参数 */
		void SetParameters(ParamType type, const std::string& name, const std::any& param);
		[[nodiscard]] const ParameterMap& GetAllParameters() const { return m_Parameters; }
		/* 设置纹理
		 * 绑定点（texture unit / [[texture(N)]]）完全由 Shader 反射决定：
		 * 直接取 Shader 中 `layout(binding = X) uniform sampler2D <name>;` 的 X，
		 * 调用方无需、也不应再人为指定 slot。
		 */
		void SetTexture(const std::string& name, const SharedPtr<DeviceTexture>& texture);
		/* 设置光栅化状态 */
		void SetRasterState(const RenderRasterState& state) { m_RasterState = state; }
		[[nodiscard]] const RenderRasterState& GetRasterState() const { return m_RasterState; }
		RenderRasterState& GetRasterState() { return m_RasterState; }

		/* 绑定材质中的各参数 */
		void Bind();
		/* 解绑材质 */
		void Unbind();

		/* 默认材质 */
		static SharedPtr<Material>& Default();
		/* 错误材质 */
		static SharedPtr<Material>& Error();

		/* 创建材质 */
		static SharedPtr<Material> Create(const SharedPtr<DeviceShader>& shader);

	private:
		/* 从当前Shader反射出sampler的绑定点，未找到返回TextureSlot::Invalid */
		[[nodiscard]] uint32_t ResolveTextureBinding(const std::string& name) const;
		/* Shader变更后，重新解析所有纹理参数的绑定点 */
		void RefreshTextureBindings();

		/* Shader */
		SharedPtr<DeviceShader> m_pShader{ nullptr };
		/* 材质所需的各种参数<ToID(Name), MaterialParamInfo> */
		ParameterMap m_Parameters{};
		/* 光栅化状态配置 */
		RenderRasterState m_RasterState{};

		/* 默认材质和错误材质 */
		static SharedPtr<Material> m_pDefaultMaterial;
		static SharedPtr<Material> m_pErrorMaterial;
	};

	/* 材质组类：
	 * 每个模型对应一个MaterialGroup
	 * 每个模型中的MeshSegment对应MaterialGroup中的一个Material
	 */
	class MaterialGroup final
	{
	public:
		MaterialGroup() = default;
		~MaterialGroup() = default;

		/* 添加一个Material */
		void EmplaceMaterial(const SharedPtr<Material>& material);
		/* 根据索引获取Material */
		const SharedPtr<Material>& GetMaterialByIndex(int idx);
		/* 获取组中所有材质 */
		[[nodiscard]] const std::vector<SharedPtr<Material>>& GetAllMaterials() const { return m_Materials; }
		/* 清空材质 */
		void ClearAllMaterials() { m_Materials.clear(); }

		/* 序列化 */
		void Serializer(const std::string& path);
		/* 反序列化 */
		bool Deserializer(const std::string& path);

	private:

		/* 材质组路径 */
		std::string m_Path{};
		/* 材质列表 */
		std::vector<SharedPtr<Material>> m_Materials;
	};
}
