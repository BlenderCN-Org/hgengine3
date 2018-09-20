#pragma once

#include <stdint.h>
#include <memory>

#include <HgShader.h>
//#include <HgTexture.h>
#include <HgVbo.h>

#include <HgGPUBuffer.h>
#include <HgTexture.h>
#include <vector>

class HgShader;
//class IHgVbo;
class HgTexture;

enum BlendMode : uint8_t {
	BLEND_NORMAL = 0,
	BLEND_ADDITIVE,
	BLEND_ALPHA,
	BLEND_INVALID = 0xFF
};

//enum RenderFlags : uint8_t {
//	NONE = 0,
//	FACE_CULLING = 1,
//	DEPTH_WRITE = 2
//};

class RenderData {
private:
	//We need to be able to support multiple VBOs without hardcoding more here.
	VboIndex m_vertexVbo;
	VboIndex m_indexVbo;
	VboIndex m_colorVbo;

public:
	typedef std::shared_ptr<RenderData>(*newRenderDataCallback)();
	//typedef void(*indiceRenderFunc)(RenderData* rd);
	static newRenderDataCallback Create;
	
	struct Flags {
		Flags() : FACE_CULLING(true), DEPTH_WRITE(true), updateTextures(false)
		{}

		bool FACE_CULLING : 1;
		bool DEPTH_WRITE : 1;
		bool updateTextures : 1;
	};

	RenderData();
	virtual ~RenderData();

	void render();

	void destroy();
	void init();
	virtual void clearTextureIDs() = 0;
	virtual void setTexture(const HgTexture* t) = 0;

	IHgVbo* hgVbo() { return m_vertexVbo.VboRec().Vbo().get(); }
	IHgVbo* indexVbo() { return m_indexVbo.VboRec().Vbo().get(); }
	IHgVbo* colorVbo() { return m_colorVbo.VboRec().Vbo().get(); }

	void VertexVboRecord(const VboIndex& vbo_index) { m_vertexVbo = vbo_index; }
	const HgVboRecord& VertexVboRecord() const { return m_vertexVbo.VboRec(); }

	void indexVboRecord(const VboIndex& vbo_index) { m_indexVbo = vbo_index; }
	const HgVboRecord& indexVboRecord() const { return m_indexVbo.VboRec(); }

	void colorVbo(const VboIndex& vbo_index) { m_colorVbo = vbo_index; }
	//inline void colorVbo(std::unique_ptr<IHgVbo>& vbo) { m_colorVbo = std::move(vbo); }

	void updateGpuTextures();
	bool updateTextures() const { return renderFlags.updateTextures; }
	void updateTextures(bool t) { renderFlags.updateTextures = t; }

	uint32_t instanceCount;

	HgShader* shader;

	//RenderFlags renderFlags;

	//std::shared_ptr<IHgGPUBuffer> gpuBuffer;
	IHgGPUBuffer* gpuBuffer;

	std::vector< HgTexture::TexturePtr > textures;

	BlendMode blendMode;
	Flags renderFlags;
};

typedef std::shared_ptr<RenderData> RenderDataPtr;