#include <RenderData.h>
#include <RenderBackend.h>

//RenderData::newRenderDataCallback RenderData::Create = nullptr;

RenderData::RenderData()
	:blendMode(BlendMode::BLEND_NORMAL), instanceCount(0)
	, gpuBuffer(nullptr)
{
	clearTextureIDs();
	init();
}

RenderData::~RenderData()
{
	destroy();
}

void RenderData::render() {
	RENDERER()->setRenderAttributes(blendMode, renderFlags);
	hgVbo()->use();
	if (colorVbo() != nullptr) colorVbo()->use();
	indexVbo()->use();
	indexVbo()->draw(this);
}

void RenderData::destroy() {
	//FIXME: Do something to clean up hgVbo
	//hgvbo_remove(d->hgvbo, d->vbo_offset, d->vertex_count)

	if (shader) HgShader::release(shader);
	shader = nullptr;
}

void RenderData::init() {
	shader = HgShader::acquire("test_vertex.glsl", "test_frag.glsl");
}


void RenderData::clearTextureIDs() {
	memset(m_gpuTextureHandles, 0, sizeof(m_gpuTextureHandles));
}

void RenderData::setTexture(const HgTexture* t) {
	const HgTexture::TextureType type = t->getType();
	m_gpuTextureHandles[type] = t->getGPUId();
}

void RenderData::updateGpuTextures() {
	clearTextureIDs();

	for (auto& texture : textures)
	{
		if (texture->NeedsGPUUpdate()) {
			texture->sendToGPU();
			HgTexture::TextureType type = texture->getType();
		}

		//FIXME: Share texture pointers can cause a problem here. Texture may be updated by another HgEntity.
		//refactor into HgTexture making callback into renderData to set texture IDs.
		//NOTE: Not sure this applies anymore. HgTexture is pretty immutable after creation.
		setTexture(texture.get());
	}
}