#pragma once

namespace Wuya
{
	enum class MGNodeOperatorType : uint8_t
	{
		Add,
		Subtract,
		Multiply,
		Divide
	};

	/* 节点类型 */
	enum class MaterialGraphNodeType : uint8_t
	{
		PBRMaterial,
		Texture2D,
		SamplerState,
		Float,
		Float2,
		Float3,
		Float4,
		Color,
		Operator,
	};

	/* 节点属性 */
	struct MaterialGraphNode
	{
		/* Node名，用于显示在节点上方，表述该节点的类型，一般不会改动 */
		std::string Name;
		/* Node类型 */
		MaterialGraphNodeType NodeType;
		/* Node所使用的样式模板，或者属于s_Templates中一个，或者也可以自定义 */
		GraphEditor::TemplateIndex TemplateIndex;
		/* Node在屏幕空间的位置 */
		float ScreenPosX, ScreenPosY;
		/* Node的尺寸 */
		float Width = 200.0f, Height = 200.0f;
		/* 当前Node属于被选中状态，可能同时存在多个被选中的节点 */
		bool IsSelected;
		/* 每个Node包含一个Entity， 用于挂载各自的属性组件 */
		entt::entity EntityHandle{ entt::null };
	};

	template <typename T, std::size_t N>
	struct Array
	{
		T data[N];
		const size_t size() const { return N; }

		const T operator [] (size_t index) const { return data[index]; }
		operator T* () {
			T* p = new T[N];
			memcpy(p, data, sizeof(data));
			return p;
		}
	};

	template <typename T, typename ... U>
	Array(T, U...)->Array<T, 1 + sizeof...(U)>;

	/* Constants */
	constexpr size_t TemplateIndex_PBRMaterial = 0;
	constexpr size_t TemplateIndex_Texture2D = 1;
	constexpr size_t TemplateIndex_SamplerState = 2;
	constexpr size_t TemplateIndex_Float = 3;
	constexpr size_t TemplateIndex_Float2 = 4;
	constexpr size_t TemplateIndex_Float3 = 5;
	constexpr size_t TemplateIndex_Float4 = 6;
	constexpr size_t TemplateIndex_Color = 7;
	constexpr size_t TemplateIndex_Operator2 = 8;
	constexpr size_t TemplateIndex_Operator3 = 9;
	constexpr size_t TemplateIndex_Operator4 = 10;

	/* All Default Templates */
	static std::vector<GraphEditor::Template> s_Templates = {
		/* 0: PBRMaterial */
		{
			IM_COL32(160, 160, 180, 255),
			IM_COL32(100, 100, 140, 255),
			IM_COL32(110, 110, 150, 255),
			12,
			Array{"Albedo", "Normal", "Roughness", "Metallic", "Specular", "Anisotropy", "Emission", "Ambient", "AO", "IOR", "ClearCoatThickness", "ClearCoatRoughness"},
			nullptr,
			0,
			nullptr,
			nullptr
		},
		/* 1: Texture2D */
		{
			IM_COL32(180, 160, 160, 255),
			IM_COL32(140, 100, 100, 255),
			IM_COL32(150, 110, 110, 255),
			0,
			nullptr,
			nullptr,
			1,
			Array{"Texture"},
			Array{ IM_COL32(200,200,200,255)}
		},
		/* 2: SamplerState */
		{
			IM_COL32(160, 160, 160, 255),
			IM_COL32(100, 100, 100, 255),
			IM_COL32(110, 110, 110, 255),
			1,
			nullptr,
			nullptr,
			6,
			Array{"RGBA", "RGB", "R", "G", "B", "A"},
			Array{ IM_COL32(200,200,200,255), IM_COL32(200,200,200,255), IM_COL32(255,0,0,255), IM_COL32(0,255,0,255), IM_COL32(0,0,255,255), IM_COL32(200,200,200,255)}
		},
		/* 3: Float */
		{
			IM_COL32(180, 160, 160, 255),
			IM_COL32(140, 100, 100, 255),
			IM_COL32(150, 110, 110, 255),
			0,
			nullptr,
			nullptr,
			1,
			Array{"Float"},
			Array{ IM_COL32(200,200,200,255)}
		},
		/* 4: Float2 */
		{
			IM_COL32(180, 160, 160, 255),
			IM_COL32(140, 100, 100, 255),
			IM_COL32(150, 110, 110, 255),
			0,
			nullptr,
			nullptr,
			1,
			Array{"Float2"},
			Array{ IM_COL32(200,200,200,255)}
		},
		/* 5: Float3 */
		{
			IM_COL32(180, 160, 160, 255),
			IM_COL32(140, 100, 100, 255),
			IM_COL32(150, 110, 110, 255),
			0,
			nullptr,
			nullptr,
			1,
			Array{"Float3"},
			Array{ IM_COL32(200,200,200,255)}
		},
		/* 6: Float4 */
		{
			IM_COL32(180, 160, 160, 255),
			IM_COL32(140, 100, 100, 255),
			IM_COL32(150, 110, 110, 255),
			0,
			nullptr,
			nullptr,
			1,
			Array{"Float4"},
			Array{ IM_COL32(200,200,200,255)}
		},
		/* 7: Color */
		{
			IM_COL32(180, 160, 160, 255),
			IM_COL32(140, 100, 100, 255),
			IM_COL32(150, 110, 110, 255),
			0,
			nullptr,
			nullptr,
			1,
			Array{"Color"},
			Array{ IM_COL32(200,200,200,255)}
		},
		/* 8: Operator2 */
		{
			IM_COL32(160, 160, 160, 255),
			IM_COL32(100, 100, 100, 255),
			IM_COL32(110, 110, 110, 255),
			2,
			Array{"Input0", "Input1"},
			nullptr,
			1,
			Array{"Result"},
			Array{ IM_COL32(200,200,200,255)}
		},
		/* 8: Operator3 */
		{
			IM_COL32(160, 160, 160, 255),
			IM_COL32(100, 100, 100, 255),
			IM_COL32(110, 110, 110, 255),
			3,
			Array{"Input0", "Input1", "Input2"},
			nullptr,
			1,
			Array{"Result"},
			Array{ IM_COL32(200,200,200,255)}
		},
		/* 8: Operator4 */
		{
			IM_COL32(160, 160, 160, 255),
			IM_COL32(100, 100, 100, 255),
			IM_COL32(110, 110, 110, 255),
			4,
			Array{"Input0", "Input1", "Input2", "Input3"},
			nullptr,
			1,
			Array{"Result"},
			Array{ IM_COL32(200,200,200,255)}
		},
		/* Test */
		{
		   IM_COL32(180, 160, 160, 255),
		   IM_COL32(140, 100, 100, 255),
		   IM_COL32(150, 110, 110, 255),
		   3,
		   nullptr,
		   Array{ IM_COL32(200,100,100,255), IM_COL32(100,200,100,255), IM_COL32(100,100,200,255) },
		   1,
		   Array{"MyOutput0"},
		   Array{ IM_COL32(200,200,200,255)}
		},
	};

	/* 节点可能挂载的组件 */
	struct MGTexture2DComponent
	{
		SharedPtr<Texture> Texture{ nullptr };
		glm::vec4 BaseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		float TilingFactor{ 1.0f };
		bool IsGenMipmap{ true };
	};

	struct MGSamplerStateComponent
	{
		SamplerWrapMode WrapMode = SamplerWrapMode::ClampToEdge;
		SamplerMinFilter MinFilter = SamplerMinFilter::Linear;
		SamplerMagFilter MagFilter = SamplerMagFilter::Linear;
	};

	struct MGFloatComponent
	{
		float Value = 0.0f;
	};

	struct MGFloat2Component
	{
		glm::vec2 Value = glm::vec2(0.0f);
	};

	struct MGFloat3Component
	{
		glm::vec3 Value = glm::vec3(0.0f);
	};

	struct MGFloat4Component
	{
		glm::vec4 Value = glm::vec4(0.0f);
	};

	struct MGColorComponent
	{
		glm::vec4 Color = glm::vec4(1.0f);
	};

	/* 多元的加减乘除， ElementCnt应>=2 */
	struct MGOperatorComponent
	{
		MGNodeOperatorType OperatorType = MGNodeOperatorType::Add;
		int ElementCnt = 2;
	};

}
