/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser,
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  push: push a new origin matrix onto the origin stack

  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix
                     based on the provided values, then
                     multiply the current top of the
                     origins stack by it.

  box/sphere/torus: create a solid object based on the
                    provided values. Store that in a
                    temporary matrix, multiply it by the
                    current top of the origins stack, then
                    call draw_polygons.

  line: create a line based on the provided values. Store
        that in a temporary matrix, multiply it by the
        current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the screen
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"
#include "gmath.h"

void my_main() {

  int i;
  struct matrix *tmp;
  struct stack *systems;
  screen t;
  zbuffer zb;
  color g;
  double step_3d = 20;
  double theta;

  //Lighting values here for easy access
  color ambient;
  double light[2][3];
  double view[3];
  double areflect[3];
  double dreflect[3];
  double sreflect[3];

  ambient.red = 50;
  ambient.green = 50;
  ambient.blue = 50;

  light[LOCATION][0] = 0.5;
  light[LOCATION][1] = 0.75;
  light[LOCATION][2] = 1;

  light[COLOR][RED] = 0;
  light[COLOR][GREEN] = 255;
  light[COLOR][BLUE] = 255;

  view[0] = 0;
  view[1] = 0;
  view[2] = 1;

  areflect[RED] = 0.1;
  areflect[GREEN] = 0.1;
  areflect[BLUE] = 0.1;

  dreflect[RED] = 0.5;
  dreflect[GREEN] = 0.5;
  dreflect[BLUE] = 0.5;

  sreflect[RED] = 0.5;
  sreflect[GREEN] = 0.5;
  sreflect[BLUE] = 0.5;

  systems = new_stack();
  tmp = new_matrix(4, 1000);
  clear_screen( t );
  clear_zbuffer(zb);
  g.red = 0;
  g.green = 0;
  g.blue = 0;

  //modified from print_pcode.c
  for (i=0;i<lastop;i++)
    {
      switch (op[i].opcode)
        {
        case LIGHT:
          break;
	  
        case AMBIENT:
          break;
	  
        case CONSTANTS:
          break;
	  
        case SAVE_COORDS:
          break;
	  
        case CAMERA:
          break;
	  
        case SPHERE:
	  add_sphere(tmp,
		     op[i].op.sphere.d[0],
		     op[i].op.sphere.d[1],
		     op[i].op.sphere.d[2],
		     op[i].op.sphere.r,
		     step_3d);
	  matrix_mult(peek(systems), tmp);
	  draw_polygons(tmp,
			t,
			zb,
			view,
			light,
			ambient,
			areflect, dreflect, sreflect);
	  tmp->lastcol = 0;
          break;

        case TORUS:
	  add_torus(tmp,
		     op[i].op.torus.d[0],
		     op[i].op.torus.d[1],
		     op[i].op.torus.d[2],
		     op[i].op.torus.r0,
		     op[i].op.torus.r1,
		     step_3d);
	  matrix_mult(peek(systems), tmp);
	  draw_polygons(tmp,
			t,
			zb,
			view,
			light,
			ambient,
			areflect, dreflect, sreflect);
	  tmp->lastcol = 0;
          break;

        case BOX:
	  add_box(tmp,
		  op[i].op.box.d0[0],
		  op[i].op.box.d0[1],
		  op[i].op.box.d0[2],
		  op[i].op.box.d1[0],
		  op[i].op.box.d1[1],
		  op[i].op.box.d1[2]);
	  matrix_mult(peek(systems), tmp);
	  draw_polygons(tmp,
			t,
			zb,
			view,
			light,
			ambient,
			areflect, dreflect, sreflect);
	  tmp->lastcol = 0;
          break;

        case LINE:
	  add_edge(tmp,
		   op[i].op.line.p0[0],
		   op[i].op.line.p0[1],
		   op[i].op.line.p0[2],
		   op[i].op.line.p1[0],
		   op[i].op.line.p1[1],
		   op[i].op.line.p1[2]);
	  matrix_mult(peek(systems), tmp);
	  draw_lines(tmp, t, zb, g);
	  tmp->lastcol = 0;
          break;

        case MESH:
          break;

        case SET:
          break;

        case MOVE:
	  tmp = make_translate(
                 op[i].op.move.d[0],op[i].op.move.d[1],
                 op[i].op.move.d[2]);
	  matrix_mult(peek(systems), tmp);
	  copy_matrix(tmp, peek(systems));
	  tmp->lastcol = 0;
          break;

        case SCALE:
	  tmp = make_scale(
                 op[i].op.scale.d[0],op[i].op.scale.d[1],
                 op[i].op.scale.d[2]);
	  matrix_mult(peek(systems), tmp);
	  copy_matrix(tmp, peek(systems));
	  tmp->lastcol = 0;
          break;

        case ROTATE:
          theta = op[i].op.rotate.degrees * (M_PI / 180);
          if ( op[i].op.rotate.axis == 0 )
            tmp = make_rotX( theta );
          else if ( op[i].op.rotate.axis == 1 )
            tmp = make_rotY( theta );
          else
            tmp = make_rotZ( theta );
	  matrix_mult(peek(systems), tmp);
	  copy_matrix(tmp, peek(systems));
	  tmp->lastcol = 0;
          break;

        case BASENAME:
          break;

        case SAVE_KNOBS:
          break;

        case TWEEN:
          break;

        case FRAMES:
          break;

        case VARY:
          break;

        case PUSH:
	  push(systems);
          break;

        case POP:
	  pop(systems);
          break;

        case GENERATE_RAYFILES:
          break;

        case SAVE:
	  save_extension(t, op[i].op.save.p->name);
          break;

        case SHADING:
          break;

        case SETKNOBS:
          break;

        case FOCAL:
          break;

        case DISPLAY:
	  display(t);
          break;
        }
    }
}
