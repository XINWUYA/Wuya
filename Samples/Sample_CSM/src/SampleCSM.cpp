#include "SampleCSM.h"
#include <imgui.h>

#include <Wuya/Scene/Material.h>
#include "SampleCamera.h"

namespace Wuya
{

void SampleCSM::OnAttached()
{
	Renderer::Init();

	m_pScene = CreateSharedPtr<Scene>();

	/* 向场景中添加SkyBox */
	{
		constexpr TextureLoadConfig load_config{
			true,
			false,
			true,
			false,
			SamplerType::Sampler2D,
			SamplerWrapMode::ClampToEdge,
			SamplerMinFilter::Linear,
			SamplerMagFilter::Linear
		};
		const auto sky_texture = Texture::Create(ABSOLUTE_PATH("Textures/drakensberg_solitary_mountain_4k.hdr"), load_config);

		RenderRasterState raster_state;
		raster_state.EnableDepthWrite = true;
		raster_state.DepthCompareFunc = CompareFunc::LessEqual;
		raster_state.CullMode = CullMode::Cull_None;

		const auto shader = ShaderAssetManager::Instance().GetOrLoad(ABSOLUTE_PATH("Shaders/SkyBox.glsl"));
		const auto material = Material::Create(shader);
		material->SetTexture("u_SkyTex", sky_texture);
		material->SetRasterState(raster_state);

		auto entity = m_pScene->CreateEntity("SkyBox");
		auto& model_component = entity.AddComponent<ModelComponent>();
		model_component.Model = Model::Create(BuiltinModelType::Sphere, material);
	}

	/* 向场景中添加房屋模型 */
	{
		auto entity = m_pScene->CreateEntity("Cottage");
		auto& model_component = entity.AddComponent<ModelComponent>();
		model_component.Model = Model::Create(ABSOLUTE_PATH("Models/big_cottage/Cottage.mesh"));
		auto& transform_component = entity.GetComponent<TransformComponent>();
		transform_component.Scale = glm::vec3(0.6f);
		transform_component.Position = glm::vec3(0.0f, -2.0f, 0.0f);
	}

	/* 向场景中添加方向光 */
	{
		m_pDirectionLight = std::dynamic_pointer_cast<DirectionalLight>(Light::Create(LightType::Directional));

		auto entity = m_pScene->CreateEntity("DirectionalLight");
		auto& light_component = entity.AddComponent<LightComponent>();
		light_component.Light = m_pDirectionLight;
	}

	m_pCamera = CreateSharedPtr<SampleCamera>();
	auto& window = Application::Instance()->GetWindow();
	m_pCamera->SetViewportRegion({ 0,0, window.GetWidth(), window.GetHeight() });

}

void SampleCSM::OnDetached()
{
	ILayer::OnDetached(
	);

}

void SampleCSM::OnUpdate(float delta_time)
{
	Renderer::SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
	Renderer::Clear();

	m_pCamera->OnUpdate(delta_time);
	m_pScene->OnUpdateEditor(m_pCamera.get(), delta_time);
}

void SampleCSM::OnImGuiRender()
{
	ImGui::Begin("Stats");
	ImGui::Text("Sample CSM:");
	ImGui::Text("- Alt + Mouse Left: Rotate.");
	auto light_dir = m_pDirectionLight->GetDirection();
	if (ImGuiExt::DrawDirectionIndicator("Light Dir", light_dir, 0))
		m_pDirectionLight->SetDirection(light_dir);
	ImGui::End();
}
}