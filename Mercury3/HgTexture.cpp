#include "HgTexture.h"
#include <glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>

HgTexture::gpuUpdateTextureFunction HgTexture::updateTextureFunc;
AssetManager<HgTexture> HgTexture::imageMap;

HgTexture::TexturePtr HgTexture::acquire(const std::string& path, TextureType type) {
	bool isNew = false;
	HgTexture::TexturePtr ptr = imageMap.get(path, &isNew);
	if (ptr == nullptr) {
		fprintf(stderr, "Could not open image \"%s\"", path.c_str());
	}
	if (isNew) {
		ptr->setType(type);
	}
	return std::move( ptr );
}
/*
void HgTexture::release(HgTexture* t) {
	if (imageMap.isValid()) { //make sure map hasn't been destroyed (when program exiting)
		imageMap.remove(t->m_path);
	}
	delete t;
}
*/
HgTexture::HgTexture()
{
	data = nullptr;
	gpuId = 0;
	m_type = DIFFUSE;
}


HgTexture::~HgTexture()
{
	if (gpuId > 0) glDeleteTextures(1,&gpuId); //FIXME abstract this
	SAFE_FREE(data);
}

bool HgTexture::stb_load(FILE* f) {
	int x, y, fileChannels;
//	stbi_set_flip_vertically_on_load(1);
	data = stbi_load_from_file(f, &x, &y, &fileChannels, 0);
	m_properties.format = (HgTexture::format)fileChannels;
	m_properties.width = x;
	m_properties.height = y;
	fclose(f);



	return data != NULL;
}

struct DDS_PIXELFORMAT {
	uint32_t size;
	uint32_t flags;
	uint32_t fourCC;
	uint32_t RGBBitCount;
	uint32_t RBitMask;
	uint32_t GBitMask;
	uint32_t BBitMask;
	uint32_t ABitMask;
};

typedef struct {
	uint32_t size;
	uint32_t flags;
	uint32_t height;
	uint32_t width;
	uint32_t pitchOrLinearSize;
	uint32_t depth;
	uint32_t mipMapCount;
	uint32_t reserved[11];
	DDS_PIXELFORMAT ddspf;
	uint32_t caps;
	uint32_t caps2;
	uint32_t caps3;
	uint32_t caps4;
	uint32_t reserved2;
} DDS_HEADER;

bool HgTexture::dds_load(FILE* f) {
	DDS_HEADER header;

	fread(&header, 124, 1, f);

	Properties p;

	p.height = header.height;
	p.width = header.width;
	p.mipMapCount = header.mipMapCount;
	p.format = (HgTexture::format)header.ddspf.fourCC;
	m_properties = p;

	const auto linearSize = header.pitchOrLinearSize;
	const uint32_t size = p.mipMapCount > 1 ? linearSize * 2 : linearSize;

	data = (unsigned char*)malloc(size);
	fread(data, 1, size, f);
	fclose(f);

	return true;
}

bool HgTexture::load(const std::string& path) {
	bool r = load_internal(path);
	if (r) {
		setNeedsGPUUpdate(true);
	}
	return r;
}

bool HgTexture::load_internal(std::string path) {
	char filecode[4];
	FILE *f = fopen(path.c_str(), "rb");
	if (f == nullptr) {
		fprintf(stderr, "Unable to open file \"%s\"", path.c_str());
		return false;
	}

	m_path = std::move(path);

	fread(filecode, 1, 4, f);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		fseek(f, 0, SEEK_SET);
		return stb_load(f);
	}

	return dds_load(f);
}

void HgTexture::sendToGPU()
{
//	gpuId = updateTextureFunc(m_width, m_height, m_channels, data);
	setNeedsGPUUpdate(false);
	gpuId = updateTextureFunc(this);
	SAFE_FREE(data);
}
