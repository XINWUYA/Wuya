#pragma once
#include <any>
#include "Wuya/Renderer/RenderCommon.h"

namespace Wuya
{
	class Texture;
	class Shader;

	/* PBR材质纹理贴图Slot */
	namespace TextureSlot
	{
		constexpr uint8_t Albedo = 0;
		constexpr uint8_t Specular = 1;
		constexpr uint8_t Normal = 2;
		constexpr uint8_t Bump = 3;
		constexpr uint8_t Displacement = 4;
		constexpr uint8_t Roughness = 5;
		constexpr uint8_t Metallic = 6;
		constexpr uint8_t Emissive = 7;
		constexpr uint8_t Ambient = 8;
		constexpr uint8_t ValidSlotCnt = 9;

		constexpr uint8_t Invalid = 255;

		inline std::string GetSlotName(uint8_t slot)
		{
			switch (slot)
			{
			case Albedo:		return "Albedo";
			case Specular:		return "Specular";
			case Normal:		return "Normal";
			case Bump:			return "Bump";
			case Displacement:	return "Displacement";
			case Roughness:		return "Roughness";
			case Metallic:		return "Metallic";
			case Emissive:		return "Emissive";
			case Ambient:		return "Ambient";
			default:			return "Invalid";
			}
		}
	}

	enum class ParamType : uint8_t
	{
		Texture = 0,
		Int,
		Float,
		Vec2,
		Vec3,
		Vec4,
	};

	/* 材质参数信息 */
	struct MaterialParamInfo
	{
		ParamType	Type{}; /* 参数类型 */
		std::string Name;   /* 参数名 */
		std::any	Value;  /* 参数值, 对于Texture需特殊处理，其Value类型为std::pair<SharedPtr<Texture>, uint32_t> */

		MaterialParamInfo() = default;
		MaterialParamInfo(ParamType type, std::string name, std::any value)
			: Type(type), Name(std::move(name)), Value(std::move(value))
		{}
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
		void SetShader(const SharedPtr<Shader>& shader) { m_pShader = shader; }
		[[nodiscard]] const SharedPtr<Shader>& GetShader() const { return m_pShader; }
		/* 设置参数 */
		void SetParameters(ParamType type, const std::string& name, const std::any& param);
		[[nodiscard]] const ParameterMap& GetAllParameters() const { return m_Parameters; }
		/* 设置纹理 */
		void SetTexture(const std::string& name, const SharedPtr<Texture>& texture, int slot = -1);
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
		static SharedPtr<Material> Create(const SharedPtr<Shader>& shader);

	private:
		/* Shader */
		SharedPtr<Shader> m_pShader{ nullptr };
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
