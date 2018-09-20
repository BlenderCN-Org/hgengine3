#include <RenderBackend.h>
#include <OGLBackend.h>

#include <oglDisplay.h>

namespace Renderer {
	HgMath::mat4f ProjectionMatrix;
	HgMath::mat4f ViewMatrix;

	std::vector<RenderInstance> opaqueElements;
	std::vector<RenderInstance> transparentElements;

	void Init() {
		RENDERER()->Init();
	}
}

RenderBackend* RENDERER() {
	//replace with some kind of configure based factory thing
	static RenderBackend* api = OGLBackend::Create();
	return api;
}

void RenderBackend::setup_viewports(uint16_t width, uint16_t height) {
	uint8_t i = 0;

	view_port[i].x = view_port[i].y = 0;
	view_port[i].width = width;
	view_port[i].height = height;
	++i;

	view_port[i].x = view_port[i].y = 0;
	view_port[i].width = width / 2;
	view_port[i].height = height;
	++i;

	view_port[i].x = width / 2;
	view_port[i].y = 0;
	view_port[i].width = width / 2;
	view_port[i].height = height;
}

static void submit_for_render_serial(uint8_t viewport_idx, HgCamera* camera, RenderData* renderData, const float* worldSpaceMatrix) {
	RENDERER()->Viewport(viewport_idx);

	//load texture data to GPU here. Can this be made to be done right after loading the image data, regardless of thread?
	if (renderData->updateTextures()) {
		renderData->updateGpuTextures();
		renderData->updateTextures(false);
	}

	HgShader* shader = renderData->shader;
	if (shader) {
		shader->enable();
		//perspective and camera probably need to be rebound here as well. (if the shader program changed. uniforms are local to shader programs).
		//we could give each shader program a "needsGlobalUniforms" flag that is reset every frame, to check if uniforms need to be updated
		//shader->setGlobalUniforms(*camera);
		//const auto spacial = e->getSpacialData();
		shader->uploadMatrices(worldSpaceMatrix, Renderer::ProjectionMatrix, Renderer::ViewMatrix);
		shader->setLocalUniforms(*renderData);
	}

	renderData->render();
}

void Renderer::Render(uint8_t viewportIdx, HgCamera* camera, const HgMath::mat4f& projection) {
	ProjectionMatrix = projection;
	ViewMatrix = camera->toMatrix();

	for (auto& renderInstance : opaqueElements) {
		submit_for_render_serial(viewportIdx, camera, renderInstance.renderData.get(), renderInstance.worldSpaceMatrix);
	}

	for (auto& renderInstance : transparentElements) {
		submit_for_render_serial(viewportIdx, camera, renderInstance.renderData.get(), renderInstance.worldSpaceMatrix);
	}
}

void Renderer::Enqueue(HgElement& e) {
	auto worldSpaceMatrix = e.computeWorldSpaceMatrix();
	if (e.flags.transparent) {
		Renderer::transparentElements.emplace_back(worldSpaceMatrix, e.getRenderDataPtr());
	}
	else {
		Renderer::opaqueElements.emplace_back(worldSpaceMatrix, e.getRenderDataPtr());
	}
}
