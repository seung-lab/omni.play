
/*
 * Omni Global Graphic Libraries Header File
 *
 * Brett Warne - bwarne@mit.edu - 4/4/09
 */

#ifndef OM_GL_UTILITY_H
#define OM_GL_UTILITY_H

#include "omGl.h"
#include "omStd.h"
#include "system/omDebug.h"

#define SELEC_BUFF_SIZE 256
static GLuint selectBuf[SELEC_BUFF_SIZE];

#pragma mark
#pragma mark Picking Utility Methods
/////////////////////////////////
///////          Picking Utility Methods

/*
 *	Picking Utility Methods
 *	
 *	Adopted from:
 *	http://www.lighthouse3d.com/opengl/picking/
 */

#define PICKING_WINDOW_DIM 5

/* start picking from x,y cursor coordinates */
void startPicking(int x, int y, float perspective[4])
{

	glSelectBuffer(SELEC_BUFF_SIZE, selectBuf);
	glRenderMode(GL_SELECT);

	//push proj matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	//define small rendering area with pick matrix
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	gluPickMatrix(x, viewport[3] - y, PICKING_WINDOW_DIM, PICKING_WINDOW_DIM, viewport);

	//mult perspective matrix by the pick matrix
	gluPerspective(perspective[0], perspective[1], perspective[2], perspective[3]);

	//back to model view
	glMatrixMode(GL_MODELVIEW);
	glInitNames();
}

/* stop picking and get hit results */
int stopPicking()
{
	int hits;

	//restore proj matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	//ensure all picking rendering complete
	glFlush();

	//return to rendering mode and count hits
	hits = glRenderMode(GL_RENDER);

	return hits;
}

/*
 *	Process pickings hits.
 *	
 *	Given number of hits in selection buffer and selection buffer,
 *	return ptr to closest hit in buffer.
 */
void processHits(GLint hits, int **ppNamesRet, int *pNumNamesRet)
{
	GLuint *buffer = selectBuf;
	unsigned int i, j;
	GLuint names, *ptr, minZ, *ptrNames, numberOfNames;

	//printf ("hits = %d\n", hits);
	//find closest hit
	ptr = (GLuint *) buffer;
	minZ = 0xffffffff;
	for (i = 0; i < hits; i++) {
		names = *ptr;
		ptr++;
		if (*ptr < minZ) {
			numberOfNames = names;
			minZ = *ptr;
			ptrNames = ptr + 2;
		}

		ptr += names + 2;
	}
	ptr = ptrNames;

	//store results
	*pNumNamesRet = numberOfNames;
	*ppNamesRet = (int *)ptrNames;

	//unsigned int depth = *(ptrNames-2);
	//debug("FIXME", << "selection buf: " << (float)depth/ (unsigned int)(-1) << endl;
	//unsigned int max = -1;
	//debug("FIXME", << depth/max;

	//PRINT NAMES OF CLOSEST
	/*
	   //debug("FIXME", << "num hits: " << hits << endl;
	   //debug("FIXME", << "num names: " << numberOfNames << endl;
	   for (j = 0; j < numberOfNames; j++,ptr++) {
	   printf ("%d ", *ptr);
	   }
	   printf ("\n");
	 */

}

/*
 *	Uses the depth buffer to unproject a pixel.  Can be given a z_scale factor
 *	to scale the value returned from the depth buffer.
 *
 *	Returns 0 on success
 *	Returns -1 on error (no depth value)
 */
int unprojectPixel(int x, int y, GLdouble point[], GLfloat z_scale)
{

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	//GLdouble posX, posY, posZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels(x, int (winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	//if invalid depth buffer value (1.0f is cleared buffer value)
	if (winZ > 1.0f - 0.0000001)
		return -1;

	//offset
	winZ *= z_scale;

	//valid depth buffer value, so unproject
	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &point[0], &point[1], &point[2]);

	//success
	return 0;
}

/*
 *	From Qt OpenGl examples.
 */
void pushGlState()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
}

void popGlState()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();
}

#pragma mark
#pragma mark Shapes Macros
/////////////////////////////////
///////          Shapes Macros
static void drawBox(GLfloat size, GLenum type)
{
	static GLfloat n[6][3] = {
		{-1.0, 0.0, 0.0},
		{0.0, 1.0, 0.0},
		{1.0, 0.0, 0.0},
		{0.0, -1.0, 0.0},
		{0.0, 0.0, 1.0},
		{0.0, 0.0, -1.0}
	};
	static GLint faces[6][4] = {
		{0, 1, 2, 3},
		{3, 2, 6, 7},
		{7, 6, 5, 4},
		{4, 5, 1, 0},
		{5, 6, 2, 1},
		{7, 4, 0, 3}
	};
	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

	for (i = 5; i >= 0; i--) {
		glBegin(type);
		glNormal3fv(&n[i][0]);
		glVertex3fv(&v[faces[i][0]][0]);
		glVertex3fv(&v[faces[i][1]][0]);
		glVertex3fv(&v[faces[i][2]][0]);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}

/* CENTRY */
void omglWireCube(GLdouble size)
{
	drawBox(size, GL_LINE_LOOP);
}

void omglSolidCube(GLdouble size)
{
	drawBox(size, GL_QUADS);
}

#pragma mark
#pragma mark Draw Generic 3D Axis
/////////////////////////////////
///////          Draw Generic 3D Axis

void glDrawAxis(int size)
{
	glPushMatrix();
	//glTranslated(camera.GetCenter()[0], camera.GetCenter()[1], camera.GetCenter()[2]);

	// Save current state of OpenGL
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	// This is to draw the axes when the mouse button is down
	glDisable(GL_LIGHTING);
	glLineWidth(size);
	glPushMatrix();
	//glScaled(5.0,5.0,5.0);
	glBegin(GL_LINES);
	glColor4f(1, 0.5, 0.5, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(1, 0, 0);
	glColor4f(0.5, 1, 0.5, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 1, 0);
	glColor4f(0.5, 0.5, 1, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 1);

	glColor4f(0.5, 0.5, 0.5, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(-1, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, -1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, -1);

	glEnd();
	glPopMatrix();

	glPopAttrib();
	glPopMatrix();
}

void glDrawPositiveAxis(int size)
{
	glPushMatrix();
	//glTranslated(camera.GetCenter()[0], camera.GetCenter()[1], camera.GetCenter()[2]);

	// Save current state of OpenGL
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	// This is to draw the axes when the mouse button is down
	glDisable(GL_LIGHTING);
	glLineWidth(size);
	glPushMatrix();
	//glScaled(5.0,5.0,5.0);
	glBegin(GL_LINES);
	glColor4f(1, 0.5, 0.5, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(1, 0, 0);
	glColor4f(0.5, 1, 0.5, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 1, 0);
	glColor4f(0.5, 0.5, 1, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 1);

	glEnd();
	glPopMatrix();

	glPopAttrib();
	glPopMatrix();
}

#endif
