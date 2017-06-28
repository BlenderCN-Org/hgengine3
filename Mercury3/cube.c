#include <stdint.h>
#include <oglDisplay.h>
#include <vertex.h>
#include <shapes.h>

#include <stdlib.h>
#include <stdio.h>
#include <oglShaders.h>

#include <assert.h>

#include <memory.h>

#include <HgVbo.h>
#include <HgElement.h>

//TestRegistration("test");

static uint8_t VTABLE_INDEX;

//binary dump from a hgmdl model, VNU format.
//probably breaks on big endian
uint32_t raw_cube_data[] = {
	0x3F000000,0xBF000000,0x3F000000,0x0,0xBF800000,0x0,0x7FFF7FFF,0xBF000000,0xBF000000,0xBF000000,0x0,0xBF800000,0x0,0x7FFF7FFF,
	0x3F000000,0xBF000000,0xBF000000,0x0,0xBF800000,0x0,0x7FFF7FFF,0xBF000000,0x3F000000,0xBF000000,0x0,0x3F800000,0x0,0x7FFF7FFF,
	0x3F000000,0x3F000000,0x3F000000,0x0,0x3F800000,0x0,0x7FFF7FFF,0x3F000000,0x3F000000,0xBF000000,0x0,0x3F800000,0x0,0x7FFF7FFF,
	0x3F000000,0x3F000000,0xBF000000,0x3F800000,0x80000000,0x0,0x7FFF7FFF,0x3F000000,0xBF000000,0x3F000000,0x3F800000,0x80000000,
	0x0,0x7FFF7FFF,0x3F000000,0xBF000000,0xBF000000,0x3F800000,0x80000000,0x0,0x7FFF7FFF,0x3F000000,0x3F000000,0x3F000000,0x0,
	0x80000000,0x3F800000,0x7FFF7FFF,0xBF000000,0xBF000000,0x3F000000,0x0,0x80000000,0x3F800000,0x7FFF7FFF,0x3F000000,0xBF000000,
	0x3F000000,0x0,0x80000000,0x3F800000,0x7FFF7FFF,0xBF000000,0xBF000000,0x3F000000,0xBF800000,0x80000000,0x80000000,0x7FFF7FFF,
	0xBF000000,0x3F000000,0xBF000000,0xBF800000,0x80000000,0x80000000,0x7FFF7FFF,0xBF000000,0xBF000000,0xBF000000,0xBF800000,
	0x80000000,0x80000000,0x7FFF7FFF,0x3F000000,0xBF000000,0xBF000000,0x0,0x0,0xBF800000,0x7FFF7FFF,0xBF000000,0x3F000000,
	0xBF000000,0x0,0x0,0xBF800000,0x7FFF7FFF,0x3F000000,0x3F000000,0xBF000000,0x0,0x0,0xBF800000,0x7FFF7FFF,0x3F000000,0xBF000000,
	0x3F000000,0x0,0xBF800000,0x0,0x7FFF7FFF,0xBF000000,0xBF000000,0x3F000000,0x0,0xBF800000,0x0,0x7FFF7FFF,0xBF000000,0xBF000000,
	0xBF000000,0x0,0xBF800000,0x0,0x7FFF7FFF,0xBF000000,0x3F000000,0xBF000000,0x0,0x3F800000,0x0,0x7FFF7FFF,0xBF000000,0x3F000000,
	0x3F000000,0x0,0x3F800000,0x0,0x7FFF7FFF,0x3F000000,0x3F000000,0x3F000000,0x0,0x3F800000,0x0,0x7FFF7FFF,0x3F000000,0x3F000000,
	0xBF000000,0x3F800000,0x80000000,0x0,0x7FFF7FFF,0x3F000000,0x3F000000,0x3F000000,0x3F800000,0x80000000,0x0,0x7FFF7FFF,
	0x3F000000,0xBF000000,0x3F000000,0x3F800000,0x80000000,0x0,0x7FFF7FFF,0x3F000000,0x3F000000,0x3F000000,0x0,0x80000000,0x3F800000,
	0x7FFF7FFF,0xBF000000,0x3F000000,0x3F000000,0x0,0x80000000,0x3F800000,0x7FFF7FFF,0xBF000000,0xBF000000,0x3F000000,0x0,0x80000000,
	0x3F800000,0x7FFF7FFF,0xBF000000,0xBF000000,0x3F000000,0xBF800000,0x80000000,0x80000000,0x7FFF7FFF,0xBF000000,0x3F000000,
	0x3F000000,0xBF800000,0x80000000,0x80000000,0x7FFF7FFF,0xBF000000,0x3F000000,0xBF000000,0xBF800000,0x80000000,0x80000000,
	0x7FFF7FFF,0x3F000000,0xBF000000,0xBF000000,0x0,0x0,0xBF800000,0x7FFF7FFF,0xBF000000,0xBF000000,0xBF000000,0x0,0x0,0xBF800000,
	0x7FFF7FFF,0xBF000000,0x3F000000,0xBF000000,0x0,0x0,0xBF800000,0x7FFF7FFF
};

//instanced render data
static OGLRenderData *crd = NULL;
/*
static void updateClbk(struct HgElement* e, uint32_t tdelta) {
//	printf("cube\n");
}
*/
static void destroy(HgElement* e) {
//	if (e->m_renderData && e->m_renderData->destroy) e->m_renderData->destroy(e->m_renderData);
//	free(e->m_renderData);
	e->m_renderData = NULL;
}

static HgElement_vtable vtable = {
	.destroy = destroy,
//	.updateFunc = updateClbk
	.updateFunc = NULL
};

//Draw vertices directly. We aren't using indices here,
static void cube_render(RenderData* rd) {
	OGLRenderData *d = (OGLRenderData*)rd;

	setBlendMode(rd->blendMode);
	hgvbo_use(d->hgVbo);

	glDrawArrays(GL_TRIANGLES, d->vbo_offset, d->vertex_count);
}

static void SetupRenderData() {
	crd = new_RenderData();

	crd->vertex_count = 36;
	crd->hgVbo = &staticVboVNU;
	crd->vbo_offset = hgvbo_add_data_vnu_raw(crd->hgVbo, (void*)raw_cube_data, crd->vertex_count);

	crd->baseRender.renderFunc = cube_render;
}

void change_to_cube(HgElement* element) {
	element->vptr_idx = VTABLE_INDEX;
	//create an instance of the render data for all triangles to share
	if (crd == NULL) SetupRenderData();

	element->m_renderData = (RenderData*)crd;
}

void shape_create_cube(HgElement* element) {
	element->position.components.x = 0.0f;
	element->position.components.y = 0.0f;
	element->position.components.z = 0.0f;

	element->rotation.w = 1.0f;
	//	element->rotation.z = 0.707f;
	element->scale = 1.0f;

	change_to_cube(element);
}

REGISTER_LINKTIME(cube_thing);