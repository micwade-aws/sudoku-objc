/* main.m
 *
 * Michael J Wade 
 * mike@iammyownorg.org 
 * Copyright (c) 2009
 * 
 */

#include <stdio.h>
#include <GL/freeglut.h>
#include <GL/gl.h>

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------
int    g_window    = 0;
GLuint g_textureID = 0;

float g_fSpinX           = 0.0f;
float g_fSpinY           = 0.0f;
int   g_nLastMousePositX = 0;
int   g_nLastMousePositY = 0;


//-----------------------------------------------------------------------------
// Name: displayFunc()
// Desc: Called when GLUT is ready to render
//-----------------------------------------------------------------------------
void displayFunc( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
	glTranslatef( 0.0f, 0.0f, -5.0f );
    glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

    //glBindTexture( GL_TEXTURE_2D, g_textureID );
    //glInterleavedArrays( GL_T2F_V3F, 0, g_quadVertices );
    glDrawArrays( GL_QUADS, 0, 4 );

    glutSwapBuffers();
}

//-----------------------------------------------------------------------------
// Name: init()
// Desc: 
//-----------------------------------------------------------------------------
void init( void )
{
	glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
	glutInitWindowSize( 640, 480 );
    char title[200];
    sprintf(title, "Sudoku Visualized - %s", glGetString(GL_VERSION));
	g_window = glutCreateWindow( title );

	glutDisplayFunc( displayFunc );
//	glutKeyboardFunc( keyboardFunc );
//	glutMouseFunc( mouseFunc );
//	glutMotionFunc( motionFunc );
//	glutReshapeFunc( reshapeFunc );
//	glutIdleFunc( idleFunc );

//	loadTexture();

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable( GL_TEXTURE_2D );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
}

int main( int argc, char **argv )
{
	glutInit( &argc, argv );
	init();
	printf("%s\n", glGetString(GL_VERSION));
	glutSwapBuffers();
	glutMainLoop();
	
	return 0;
}
