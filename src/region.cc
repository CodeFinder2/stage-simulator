/*
  region.cc
  data structures supporting multi-resolution ray tracing in world class.
  Copyright Richard Vaughan 2008
*/

#include "stage-simulator/region.hh"
#include <pthread.h>
using namespace Stg;

Stg::Region::Region() : cells(), count(0), superregion(NULL)
{
}

Stg::Region::~Region()
{
}

void Stg::Region::AddBlock()
{
  ++count;
  superregion->AddBlock();
}

void Stg::Region::RemoveBlock()
{
  --count;
  superregion->RemoveBlock();

  // if there's nothing in this region, we can garbage collect the
  // cells to keep memory usage under control
  if (count == 0)
    cells.clear();
}

SuperRegion::SuperRegion(World *world, point_int_t origin)
    : count(0), origin(origin), regions(), world(world)
{
  for (int32_t c = 0; c < SUPERREGIONSIZE; ++c)
    regions[c].superregion = this;
}

SuperRegion::~SuperRegion()
{
}

void SuperRegion::AddBlock()
{
  ++count;
}

void SuperRegion::RemoveBlock()
{
  --count;
}

void SuperRegion::DrawOccupancy(void) const
{
  // printf( "SR origin (%d,%d) this %p\n", origin.x, origin.y, this );

  glPushMatrix();
  GLfloat scale = 1.0 / world->Resolution();
  glScalef(scale, scale, 1.0); // XX TODO - this seems slightly
  glTranslatef(origin.x << SRBITS, origin.y << SRBITS, 0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // outline superregion
  glColor3f(0, 0, 1);
  glRecti(0, 0, 1 << SRBITS, 1 << SRBITS);

  // outline regions
  const Region *r = &regions[0];

  // XX hack: this means only one window at a time will have its cells
  // drawn properly, but OpenGL needs a non-stack vertex buffer
  static std::vector<GLfloat> rects(1000);
  rects.resize(0);

  //char buf[16];
  
  for (unsigned int y = 0; y < SUPERREGIONWIDTH; ++y)
    for (unsigned int x = 0; x < SUPERREGIONWIDTH; ++x) {
      if (r->count) // region contains some occupied cells
      {
        // outline the region
        glColor3f(0, 1, 0);
        glRecti(x << RBITS, y << RBITS, (x + 1) << RBITS, (y + 1) << RBITS);
	
	// show how many cells are occupied	
	//snprintf( buf, 15, "%lu", r->count );
	//Gl::draw_string( x<<RBITS, y<<RBITS, 0, buf );
	
	// draw a rectangle around each occupied cell
        for (unsigned int p = 0; p < REGIONWIDTH; ++p)
          for (unsigned int q = 0; q < REGIONWIDTH; ++q) {
            const Cell &c = r->cells[p + (q * REGIONWIDTH)];

            if (c.blocks[0].size()) // layer 0
            {
              const GLfloat xx = p + (x << RBITS);
              const GLfloat yy = q + (y << RBITS);

              rects.push_back(xx);
              rects.push_back(yy);
              rects.push_back(xx + 1);
              rects.push_back(yy);
              rects.push_back(xx + 1);
              rects.push_back(yy + 1);
              rects.push_back(xx);
              rects.push_back(yy + 1);
	    }

            if (c.blocks[1].size()) // layer 1
            {
              const GLfloat xx = p + (x << RBITS);
              const GLfloat yy = q + (y << RBITS);
              const double dx = 0.1;

              rects.push_back(xx + dx);
              rects.push_back(yy + dx);
              rects.push_back(xx + 1 - dx);
              rects.push_back(yy + dx);
              rects.push_back(xx + 1 - dx);
              rects.push_back(yy + 1 - dx);
              rects.push_back(xx + dx);
              rects.push_back(yy + 1 - dx);
            }

	  }
      }
      ++r; // next region quickly
    }

  glEnableClientState(GL_VERTEX_ARRAY);

  if (rects.size()) {
    assert(rects.size() % 8 == 0); // should be full of squares
    glVertexPointer(2, GL_FLOAT, 0, &rects[0]);
    glDrawArrays(GL_QUADS, 0, rects.size() / 2);
  }

  // char buf[32];
  // snprintf( buf, 15, "%lu", count );
  // Gl::draw_string( 1<<SBITS, 1<<SBITS, 0, buf );

  glPopMatrix();
}

static inline const std::vector<GLfloat> DrawBlock(GLfloat x, GLfloat y, GLfloat zmin, GLfloat zmax)
{
  std::vector<GLfloat> v(60);

  // TOP
  v[0] = x;
  v[1] = y;
  v[2] = zmax;
  v[3] = 1 + x;
  v[4] = y;
  v[5] = zmax;
  v[6] = 1 + x;
  v[7] = 1 + y;
  v[8] = zmax;
  v[9] = x;
  v[10] = 1 + y;
  v[11] = zmax;

  // sides
  v[12] = x;
  v[13] = y;
  v[14] = zmax;
  v[15] = x;
  v[16] = 1 + y;
  v[17] = zmax;
  v[18] = x;
  v[19] = 1 + y;
  v[20] = zmin;
  v[21] = x;
  v[22] = y;
  v[23] = zmin;
  v[24] = 1 + x;
  v[25] = y;
  v[26] = zmax;
  v[27] = x;
  v[28] = y;
  v[29] = zmax;
  v[30] = x;
  v[31] = y;
  v[32] = zmin;
  v[33] = 1 + x;
  v[34] = y;
  v[35] = zmin;
  v[36] = 1 + x;
  v[37] = 1 + y;
  v[38] = zmax;
  v[39] = 1 + x;
  v[40] = y;
  v[41] = zmax;
  v[42] = 1 + x;
  v[43] = y;
  v[44] = zmin;
  v[45] = 1 + x;
  v[46] = 1 + y;
  v[47] = zmin;
  v[48] = x;
  v[49] = 1 + y;
  v[50] = zmax;
  v[51] = 1 + x;
  v[52] = 1 + y;
  v[53] = zmax;
  v[54] = 1 + x;
  v[55] = 1 + y;
  v[56] = zmin;
  v[57] = x;
  v[58] = 1 + y;
  v[59] = zmin;

  return v;
}

void SuperRegion::DrawVoxels(unsigned int layer) const
{
  glPushMatrix();
  GLfloat scale = 1.0 / world->Resolution();
  glScalef(scale, scale, 1.0); // XX TODO - this seems slightly
  glTranslatef(origin.x << SRBITS, origin.y << SRBITS, 0);

  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  std::vector<GLfloat> verts(1000);
  std::vector<GLfloat> colors(1000);

  const Region *r = &regions[0];

  for (int y = 0; y < SUPERREGIONWIDTH; ++y)
    for (int x = 0; x < SUPERREGIONWIDTH; ++x) {
      if (r->count) // not an empty region
        for (int p = 0; p < REGIONWIDTH; ++p)
          for (int q = 0; q < REGIONWIDTH; ++q) {
            const std::vector<Block *> &blocks = r->cells[p + (q * REGIONWIDTH)].blocks[layer];

            if (blocks.size()) // not an empty cell
            {
              const GLfloat xx(p + (x << RBITS));
              const GLfloat yy(q + (y << RBITS));

              FOR_EACH (it, blocks) {
                Block *block = *it;
                Color c = block->group->mod.GetColor();

                const std::vector<GLfloat> v =
                    DrawBlock(xx, yy, block->global_z.min, block->global_z.max);
                verts.insert(verts.end(), v.begin(), v.end());

                for (unsigned int i = 0; i < 20; i++) {
                  colors.push_back(c.r);
                  colors.push_back(c.g);
                  colors.push_back(c.b);
                }
              }
            }
          }
      ++r;
    }

  if (verts.size()) {
    assert(verts.size() % 60 == 0); // should be full of blocks, each with 20 3D vertices

    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, &verts[0]);
    glColorPointer(3, GL_FLOAT, 0, &colors[0]);

    glDrawArrays(GL_QUADS, 0, verts.size() / 3);

    glDisableClientState(GL_COLOR_ARRAY);
  }

  glPopMatrix();
}

void Stg::Cell::AddBlock(Block *b, unsigned int layer)
{
  assert(b);
  assert(layer < 2);
  blocks[layer].push_back(b);
  b->rendered_cells[layer].push_back(this);
  region->AddBlock();
}

void Stg::Cell::RemoveBlock(Block *b, unsigned int layer)
{
  assert(b);
  assert(layer < 2);

  EraseAll( b, blocks[layer] );  
  region->RemoveBlock();
}
