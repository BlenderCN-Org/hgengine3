#include <HgModel.h>
#include <stdio.h>
#include <stdlib.h>
#include <HgVbo.h>

#include "oglDisplay.h"

typedef struct header {
	uint32_t vertex_count, index_count;
} header;


static model_data LoadModel(const char* filename) {
	header head;
	model_data r;
	r.vertices = NULL;
	r.indices = NULL;

	vbo_layout_vnu* buffer1 = NULL;
	uint16_t* buffer2 = NULL;

	FILE* f = NULL;
	errno_t err = fopen_s(&f, filename, "rb");
	if (err != 0) {
		fprintf(stderr, "Unable to open file \"%s\"\n", filename);
		return r;
	}

	int32_t read = fread(&head, sizeof(head), 1, f);
	if (read != 1) {
		fprintf(stderr, "Unable to read file header for \"%s\"\n", filename);
		fclose(f);
		return r;
	}

	if (head.vertex_count > 65535) {
		fprintf(stderr, "Too many vertices for \"%s\"\n", filename);
		fclose(f);
		return r;
	}

	if (head.index_count > 1000000) {
		fprintf(stderr, "Too many indices for \"%s\"\n", filename);
		fclose(f);
		return r;
	}

	buffer1 = (vbo_layout_vnu*)malloc(sizeof(*buffer1)*head.vertex_count);
	buffer2 = (uint16_t*)malloc(sizeof(*buffer2)*head.index_count);

	read = fread(buffer1, sizeof(*buffer1), head.vertex_count, f);
	if (read != head.vertex_count) {
		fprintf(stderr, "Error, %d vertices expected, read %d", head.vertex_count, read);
		free(buffer1);
		free(buffer2);
		fclose(f);
		return r;
	}

	read = fread(buffer2, sizeof(*buffer2), head.index_count, f);
	if (read != head.index_count) {
		fprintf(stderr, "Error, %d indices expected, read %d", head.index_count, read);
		free(buffer1);
		free(buffer2);
		fclose(f);
		return r;
	}

	fclose(f);

	r.vertices = buffer1;
	r.indices = buffer2;
	r.vertex_count = head.vertex_count;
	r.index_count = head.index_count;

	return r;
}

static RenderData* init_render_data() {
	OGLRenderData* rd = OGLRenderData::Create();
//	rd->baseRender.renderFunc = model_render;
	return rd;
}

static void updateClbk(HgElement* e, uint32_t tdelta) {
	//	printf("cube\n");
}

model_data::model_data()
	:vertices(nullptr), indices(nullptr)
{

}
model_data::~model_data() {
}

static void destroy(HgElement* e) {
//	SCALL(e->m_renderData, destroy);
	free(e->m_renderData);
	e->m_renderData = NULL;
}

static void change_to_model(HgElement* element) {
//	element->vptr_idx = VTABLE_INDEX;

	//create an instance of the render data for all triangles to share
	element->m_renderData = init_render_data(); //this needs to be per model instance if the model is animated
}

int8_t model_load(HgElement* element, const char* filename) {
	change_to_model(element);

	OGLRenderData* rd = (OGLRenderData*)element->m_renderData;

	SET_FLAG(element, HGE_DESTROY); //clear when we make it to the end

	model_data mdl = LoadModel(filename);
	if (mdl.vertices == NULL || mdl.indices == NULL) return -1;

	rd->hgVbo = &staticVboVNU;
	rd->vertex_count = mdl.vertex_count;
	rd->index_count = mdl.index_count;
	rd->vbo_offset = staticVboVNU.add_data(mdl.vertices, rd->vertex_count);
	free(mdl.vertices);

//	mrd->index_count = mdl.index_count;
	rd->indices.data = mdl.indices;
	rd->indices.owns_ptr = 1;
	rd->renderFunction = Indice16Render;

	CLEAR_FLAG(element, HGE_DESTROY);

	return 0;
}

/*
void model_create(HgElement* element) {
	element->position.components.x = 0.0f;
	element->position.components.y = 0.0f;
	element->position.components.z = 0.0f;

	element->rotation.w = 1.0f;
	//	element->rotation.z = 0.707f;

	element->scale = 1;

	change_to_model(element);
}
*/

REGISTER_LINKTIME(hgmodel,change_to_model);