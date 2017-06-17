#include <triangle.h>
#include <oglDisplay.h>
#include <vertex.h>
#include <shapes.h>

#include <stdlib.h>
#include <stdio.h>
#include <oglShaders.h>

static uint8_t VTABLE_INDEX;

static float vv[9] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f
};

static color colors[] = {
	255, 0, 0, 255,
	0, 255, 0, 255,
	0, 0, 255, 255
};

static uint8_t indices[] = {
	0,1,2
};

static void updateClbk(struct HgElement* e, uint32_t tdelta) {
	//	printf("cube\n");
}

static void destroy(struct HgElement* e) {

	//	printf("cube\n");
}

static HgElement_vtable vtable = {
	.destroy = NULL,
	.updateFunc = NULL
};

//instanced render data
static OGLRenderData* trd = NULL;

static void SetupRenderData() {
	trd = calloc(1, sizeof(*trd));
	trd->baseRender.renderFunc = ogl_render_renderData;
	trd->baseRender.shader = HGShader_acquire("test_vertex.glsl", "test_frag.glsl");

	vertices points;
	points.points.array = vv;
	points.size = 3;

	trd->hgVbo = &staticVbo;
	trd->vertex_count = points.size;
	trd->index_count = 3;
	trd->indices = indices;
	trd->vbo_offset = hgvbo_add_data_vc(&staticVbo, points.points.v, colors, trd->vertex_count);
}

OGLRenderData* triangle_init_render_data() {
	if (trd == NULL) SetupRenderData();
	return trd;
}

void change_to_triangle(HgElement* element) {
	//create an instance of the render data for all triangles to share
	element->m_renderData = (RenderData*)triangle_init_render_data();
}

void shape_create_triangle(HgElement* element) {
	element->position.components.x = 0.0f;
	element->position.components.y = 0.0f;
	element->position.components.z = 0.0f;

	element->rotation.w = 1.0f;
//	element->rotation.z = 0.707f;

	element->scale = 1;

	change_to_triangle(element);
}

REGISTER_LINKTIME(triangle)