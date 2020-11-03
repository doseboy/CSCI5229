#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//Params
double s = 10;
double b = 2.6666;
double r = 28;
double dt = 0.001;
double w = 0.03;

//Global Vars
int th = 0;	//Azimuth of view angle
int ph = 0;	//Elevation of view angle
double dim = 2; //Dimension of orthogonal box
int mode = 0;   //Param to tune
const char* text[] = {"","s-","s+","b-","b+","r-","r+","zoom"};  // Dimension display text

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  // Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

void display()
{
   //  Clear screen
   glClear(GL_COLOR_BUFFER_BIT);
   
   //Reset tranformations???
   glLoadIdentity();
   
   // Set view angle
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);

   //X Y Z Values
   double x = 1;
   double y = 1;
   double z = 1;

   // Change in dimensions 
   double dx = 0;
   double dy = 0;
   double dz = 0;

   glBegin(GL_LINE_STRIP);
   for(int i = 0; i < 50000; i++)
   {
	dx = s * (y - x);
	dy = x * (r - z) - y;
	dz = x * y - b * z;

	x += dt * dx;
	y += dt * dy;
	z += dt * dz;

	glColor3f(x * w, y * w, z * w);
	glVertex3d(x * w, y * w, z * w);
   }
   glEnd();

   //  Draw axes in white
   glColor3f(1,1,1);
   glBegin(GL_LINES);
   glVertex3d(0,0,0);
   glVertex3d(1,0,0);
   glVertex3d(0,0,0);
   glVertex3d(0,1,0);
   glVertex3d(0,0,0);
   glVertex3d(0,0,1);
   glEnd();
   
   //  Label axes
   glRasterPos3d(1,0,0);
   Print("X");
   glRasterPos3d(0,1,0);
   Print("Y");
   glRasterPos3d(0,0,1);
   Print("Z");
   
   //  Display parameters
   glWindowPos2i(5,25);
   Print("View Angle=%d,%d  %s",th,ph,text[mode]);
   
   //  Flush and swap
   glWindowPos2i(5,5);
   Print("Parameters: s=%f, b=%f, r=%f", s, b, r);
   glFlush();
   glutSwapBuffers();
}

void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   
   //  Reset view angle
   else if (ch == '0')
   {
      th = ph = 0;
      w = 0.03;
   }
   
   //keyboard interaction
   else if (ch == 's')
   {
      s -= 1;
      mode = 1;
   } 
   else if (ch == 'd')
   {
      s += 1;
      mode = 2;
   } 
   else if (ch == 'b')
   {
      b -= 1;
      mode = 3;
   } 
   else if (ch == 'n')
   {
      b += 1;
      mode = 4;
   } 
   else if (ch == 'r')
   {
      r -= 1;
      mode = 5;
   } 
   else if (ch == 't')
   {
      r += 1;
      mode = 6;
   } 
   else if (ch == '+')
   {
      w += 0.01;
      mode = 7;
   } 
   else if (ch == '-')
   {
      w -= 0.01;
      mode = 7;
   } 
   
   //Reset params
   else if(ch == 'z')
   {
      s = 10;
      b = 2.6666;
      r = 28;
      mode = 0;
   }
   
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

void special(int key,int x,int y)
{
   //  Right arrow key - increase azimuth by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease azimuth by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   glOrtho(-dim*w2h,+dim*w2h, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}


int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   //  Request 500 x 500 pixel window
   glutInitWindowSize(500,500);
   // Create window
   glutCreateWindow("Assignment 1 - William Anderson");
   //  Register function used to display scene
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT for events
   glutMainLoop();
   //  Return to OS
   return 0;
}

