
#include <iostream.h>
#include <windows.h>
#include <mmsystem.h>
#include "hlbsp.h"
#include "pakfile.h"
#include "camera.h"

HLBsp *g_pLevel = NULL;
Camera *g_pCamera = NULL;

char pak[128] = "E:/data/hlfiles/valve/pak0.pak";
char map[64] = "maps/c1a0.bsp";
char wad[128] = "E:/data/hlfiles/valve/halflife.wad";

bool init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	
	if(!bsp.LoadFromPak(pak, map))
	{
		cerr << "Couldn't find " << map << " in " << pak << endl;
		return false;
	}

#ifdef _DEBUG
	//cout << bsp << endl;
#endif
	//bsp.ReadTokens();

	camera.SetPosition(60.0, 220.0, -173.0, 0.0);
    
	glActiveTextureARB = 
        (PFNGLCLIENTACTIVETEXTUREARBPROC) 
        wglGetProcAddress("glActiveTextureARB");
    glMultiTexCoord2fvARB = 
        (PFNGLMULTITEXCOORD2FVARBPROC) 
        wglGetProcAddress("glMultiTexCoord2fvARB");
    glClientActiveTextureARB = 
        (PFNGLACTIVETEXTUREARBPROC) 
        wglGetProcAddress("glClientActiveTextureARB");

	return true;
}

void displayFPS()
{
	static long start = timeGetTime();
	static int FPS = 0;
	static int workingFPS = 0;

	long end = timeGetTime();
	
	++workingFPS;

	if(end - start > 1000)
	{
		FPS = workingFPS;
		workingFPS = 0;
		start = timeGetTime();
	}

	char buffer[80];
	sprintf(buffer, "%d FPS", FPS);
	glutSetWindowTitle(buffer);

	glutPostRedisplay();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	camera.PositionViewer();

	bsp.Render(camera.position);
	
	displayFPS();
	
	glFlush();
	
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, w/h, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case '+':
		bsp.m_texnum++;
		break;
	case '-':
		bsp.m_texnum--;
		break;
	case 27: // escape
		exit(0);
		break;
	case 'w':
		camera.MoveForward();
		break;
	case 's':
		camera.MoveBack();
		break;
	case 'a':
		camera.TurnLeft();
		break;
	case 'd':
		camera.TurnRight();
		break;
	}
	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	switch(key)
	{
	case 101: // up
		camera.MoveForward();
		break;
	case 100: // left
		camera.TurnLeft();
		break;
	case 103: // down
		camera.MoveBack();
		break;
	case 102: // right
		camera.TurnRight();
		break;
	}
	glutPostRedisplay();
}

int ox, oy;

void mouse(int button, int state, int x, int y)
{
	ox = x; oy = y;
}

void motion(int x, int y)
{
	camera.yawAngle += -(x - ox) * 1.0;
	camera.pitchAngle += -(y - oy) * 1.0;

	camera.ChangeDirection();

	ox = x; oy = y;
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(640, 480);
	glutCreateWindow("HalfLife Viewer");

	if(!init())
		return 0;

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();
	return 0;
}