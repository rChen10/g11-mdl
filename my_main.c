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
      printf("%d: ",i);
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
		     step_3D);
	  matrix_mult(peek(systems), tmp);
	  draw_polygons(tmp,
			t,
			zb,
			view,
			light,
			ambient,
			areflect, dreflect, sreflect,);
	  tmp->lastcol = 0;
          break;
        case TORUS:
	  add_torus(tmp,
		     op[i].op.torus.d[0],
		     op[i].op.torus.d[1],
		     op[i].op.torus.d[2],
		     op[i].op.torus.r,
		     step_3D);
	  matrix_mult(peek(systems), tmp);
	  draw_polygons(tmp,
			t,
			zb,
			view,
			light,
			ambient,
			areflect, dreflect, sreflect,);
	  tmp->lastcol = 0;
          break;
        case BOX:
          printf("Box: d0: %6.2f %6.2f %6.2f d1: %6.2f %6.2f %6.2f",
                 op[i].op.box.d0[0],op[i].op.box.d0[1],
                 op[i].op.box.d0[2],
                 op[i].op.box.d1[0],op[i].op.box.d1[1],
                 op[i].op.box.d1[2]);
          if (op[i].op.box.constants != NULL)
            {
              printf("\tconstants: %s",op[i].op.box.constants->name);
            }
          if (op[i].op.box.cs != NULL)
            {
              printf("\tcs: %s",op[i].op.box.cs->name);
            }

          break;
        case LINE:
          printf("Line: from: %6.2f %6.2f %6.2f to: %6.2f %6.2f %6.2f",
                 op[i].op.line.p0[0],op[i].op.line.p0[1],
                 op[i].op.line.p0[1],
                 op[i].op.line.p1[0],op[i].op.line.p1[1],
                 op[i].op.line.p1[1]);
          if (op[i].op.line.constants != NULL)
            {
              printf("\n\tConstants: %s",op[i].op.line.constants->name);
            }
          if (op[i].op.line.cs0 != NULL)
            {
              printf("\n\tCS0: %s",op[i].op.line.cs0->name);
            }
          if (op[i].op.line.cs1 != NULL)
            {
              printf("\n\tCS1: %s",op[i].op.line.cs1->name);
            }
          break;
        case MESH:
          printf("Mesh: filename: %s",op[i].op.mesh.name);
          if (op[i].op.mesh.constants != NULL)
            {
              printf("\tconstants: %s",op[i].op.mesh.constants->name);
            }
          break;
        case SET:
          printf("Set: %s %6.2f",
                 op[i].op.set.p->name,
                 op[i].op.set.p->s.value);
          break;
        case MOVE:
          printf("Move: %6.2f %6.2f %6.2f",
                 op[i].op.move.d[0],op[i].op.move.d[1],
                 op[i].op.move.d[2]);
          if (op[i].op.move.p != NULL)
            {
              printf("\tknob: %s",op[i].op.move.p->name);
            }
          break;
        case SCALE:
          printf("Scale: %6.2f %6.2f %6.2f",
                 op[i].op.scale.d[0],op[i].op.scale.d[1],
                 op[i].op.scale.d[2]);
          if (op[i].op.scale.p != NULL)
            {
              printf("\tknob: %s",op[i].op.scale.p->name);
            }
          break;
        case ROTATE:
          printf("Rotate: axis: %6.2f degrees: %6.2f",
                 op[i].op.rotate.axis,
                 op[i].op.rotate.degrees);
          if (op[i].op.rotate.p != NULL)
            {
              printf("\tknob: %s",op[i].op.rotate.p->name);
            }
          break;
        case BASENAME:
          printf("Basename: %s",op[i].op.basename.p->name);
          break;
        case SAVE_KNOBS:
          printf("Save knobs: %s",op[i].op.save_knobs.p->name);
          break;
        case TWEEN:
          printf("Tween: %4.0f %4.0f, %s %s",
                 op[i].op.tween.start_frame,
                 op[i].op.tween.end_frame,
                 op[i].op.tween.knob_list0->name,
                 op[i].op.tween.knob_list1->name);
          break;
        case FRAMES:
          printf("Num frames: %4.0f",op[i].op.frames.num_frames);
          break;
        case VARY:
          printf("Vary: %4.0f %4.0f, %4.0f %4.0f",
                 op[i].op.vary.start_frame,
                 op[i].op.vary.end_frame,
                 op[i].op.vary.start_val,
                 op[i].op.vary.end_val);
          break;
        case PUSH:
          printf("Push");
          break;
        case POP:
          printf("Pop");
          break;
        case GENERATE_RAYFILES:
          printf("Generate Ray Files");
          break;
        case SAVE:
          printf("Save: %s",op[i].op.save.p->name);
          break;
        case SHADING:
          printf("Shading: %s",op[i].op.shading.p->name);
          break;
        case SETKNOBS:
          printf("Setknobs: %f",op[i].op.setknobs.value);
          break;
        case FOCAL:
          printf("Focal: %f",op[i].op.focal.value);
          break;
        case DISPLAY:
          printf("Display");
          break;
        }
      printf("\n");
    }
}