#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int axes=1;       //  Display axes
int mode=0;       //  Projection mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
double zh=0;       //  Rotation of teapot
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world

//angle of rotation
float xpos = 0, ypos = 0, zpos = 0, xrot = 0, yrot = 180;

char* mode_arr[] = {"Orthogonal", "Perspective", "First person"};

//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415926/180*(th))
#define Sin(th) sin(3.1415926/180*(th))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
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

/*
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode == 1)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else if (mode == 0)
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   else if (mode == 2)
      gluPerspective (fov, asp, 1.0, 1000.0);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 * Draws a Logarithmic Spiral
 * A and B are constants for the spiral
 * Note it is 2d
 * Source -> https://mathworld.wolfram.com/LogarithmicSpiral.html
 */
void LogSpiral(double a,double b)
{
   double x, y;
 
   double theta = 0.0;
   x = a*pow(2.718281,(double)b*theta)*cos(theta);
   y = a*pow(2.718281,(double)b*theta)*sin(theta);
   for (int i = 0; i < 1000; i++)
   {
     glBegin(GL_LINES);
     theta = 0.025*i;
     glVertex2f((GLfloat)x, (GLfloat)y);
     glVertex2f(a*pow(2.718281, (double)b*theta)*cos(theta), a*pow(2.718281, (double)b*theta)*sin(theta));
 
     //glColor3f(r, g, b);
     x = a*pow(2.718281, (double)b*theta)*cos(theta);
     y = a*pow(2.718281, (double)b*theta)*sin(theta);
  
     glEnd();
   }
}
/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(double th,double ph)
{
   glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}


/*
 *  Draw a sphere (version 2)
 *     at (x,y,z)
 *     radius (r)
 */
static void sphere2(double x,double y,double z,double r)
{
   const int d=5;
   int th,ph;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   //  Latitude bands
   for (ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  Undo transformations
   glPopMatrix();
}

/*
 * Draw a triangular prism
 * at (x,y,z)
 * dimensions (dx,dy,dz)
 * rotated th about the y axis
 */
static void triprism(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   glBegin(GL_QUADS);
   	glColor3f(1,0,0);
        glVertex3f(0.5, 0, 0.5);
        glVertex3f(0.5, 0, -0.5);
        glVertex3f(-0.5, 0, -0.5);
        glVertex3f(-0.5, 0, 0.5);

   	glColor3f(0,0,1);
        glVertex3f(0.5,0,-0.5);
        glVertex3f(0.5,1,-0.5);
        glVertex3f(-0.5,1,-0.5);
        glVertex3f(-0.5,0,-0.5);

   	glColor3f(0,1,1);
        glVertex3f(0.5,1,-0.5);
        glVertex3f(-0.5,1,-0.5);
        glVertex3f(-0.5,0,0.5);
        glVertex3f(0.5,0,0.5);
    glEnd();
    glBegin(GL_TRIANGLES);
   	glColor3f(0,1,0);
        glVertex3f(0.5,0,0.5);
        glVertex3f(0.5,1,-0.5);
        glVertex3f(0.5,0,-0.5);

   	glColor3f(1,1,0);
        glVertex3f(-0.5,0,0.5);
        glVertex3f(-0.5,1,-0.5);
        glVertex3f(-0.5,0,-0.5);
    glEnd();
    glPopMatrix();
}

static void SolidPlane(double x,double y,double z,
                       double dx,double dy,double dz,
                       double ux,double uy, double uz)
{
   // Dimensions used to size airplane
   const double wid=0.05;
   const double nose=+0.50;
   const double cone= 0.20;
   const double wing= 0.00;
   const double strk=-0.20;
   const double tail=-0.50;
   //  Unit vector in direction of flght
   double D0 = sqrt(dx*dx+dy*dy+dz*dz);
   double X0 = dx/D0;
   double Y0 = dy/D0;
   double Z0 = dz/D0;
   //  Unit vector in "up" direction
   double D1 = sqrt(ux*ux+uy*uy+uz*uz);
   double X1 = ux/D1;
   double Y1 = uy/D1;
   double Z1 = uz/D1;
   //  Cross product gives the third vector
   double X2 = Y0*Z1-Y1*Z0;
   double Y2 = Z0*X1-Z1*X0;
   double Z2 = X0*Y1-X1*Y0;
   //  Rotation matrix
   double mat[16];
   mat[0] = X0;   mat[4] = X1;   mat[ 8] = X2;   mat[12] = 0;
   mat[1] = Y0;   mat[5] = Y1;   mat[ 9] = Y2;   mat[13] = 0;
   mat[2] = Z0;   mat[6] = Z1;   mat[10] = Z2;   mat[14] = 0;
   mat[3] =  0;   mat[7] =  0;   mat[11] =  0;   mat[15] = 1;

   //  Save current transforms
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glMultMatrixd(mat);
   //  Nose (4 sided)
   glColor3f(0,0,1);
   glBegin(GL_TRIANGLES);
   glVertex3d(nose, 0.0, 0.0);
   glVertex3d(cone, wid, wid);
   glVertex3d(cone,-wid, wid);

   glVertex3d(nose, 0.0, 0.0);
   glVertex3d(cone, wid,-wid);
   glVertex3d(cone,-wid, wid);

   glVertex3d(nose, 0.0, 0.0);
   glVertex3d(cone, wid,-wid);
   glVertex3d(cone,-wid,-wid);

   glVertex3d(nose, 0.0, 0.0);
   glVertex3d(cone, wid, wid);
   glVertex3d(cone, wid,-wid);

   glVertex3d(nose, 0.0, 0.0);
   glVertex3d(cone,-wid, wid);
   glVertex3d(cone,-wid,-wid);
   glEnd();
   //  Fuselage (square tube)
   glBegin(GL_QUADS);
   glVertex3d(cone, wid, wid);
   glVertex3d(cone,-wid, wid);
   glVertex3d(tail,-wid, wid);
   glVertex3d(tail, wid, wid);

   glVertex3d(cone, wid,-wid);
   glVertex3d(cone,-wid,-wid);
   glVertex3d(tail,-wid,-wid);
   glVertex3d(tail, wid,-wid);

   glVertex3d(cone, wid, wid);
   glVertex3d(cone, wid,-wid);
   glVertex3d(tail, wid,-wid);
   glVertex3d(tail, wid, wid);

   glVertex3d(cone,-wid, wid);
   glVertex3d(cone,-wid,-wid);
   glVertex3d(tail,-wid,-wid);
   glVertex3d(tail,-wid, wid);

   glVertex3d(tail,-wid, wid);
   glVertex3d(tail, wid, wid);
   glVertex3d(tail, wid,-wid);
   glVertex3d(tail,-wid,-wid);
   glEnd();
   //  Wings (plane triangles)
   glColor3f(1,1,0);
   glBegin(GL_TRIANGLES);
   glVertex3d(wing, 0.0, wid);
   glVertex3d(tail, 0.0, wid);
   glVertex3d(tail, 0.0, 0.5);

   glVertex3d(wing, 0.0,-wid);
   glVertex3d(tail, 0.0,-wid);
   glVertex3d(tail, 0.0,-0.5);
   glEnd();
   //  Vertical tail (plane triangle)
   glColor3f(1,0,0);
   glBegin(GL_POLYGON);
   glVertex3d(strk, 0.0, 0.0);
   glVertex3d(tail, 0.3, 0.0);
   glVertex3d(tail, 0.0, 0.0);
   glEnd();
   //  Undo transformations
   glPopMatrix();
}

                   

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3f(1,0,0);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3f(0,0,1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3f(1,1,0);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3f(0,1,0);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glColor3f(0,1,1);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glColor3f(1,0,1);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode == 1)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else if (mode == 0)
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }
   //First Person Persective -> Source: http://www.swiftless.com/tutorials/opengl/camera.html
   else if (mode == 2)
   {
      glRotatef(xrot,1.0,0.0,0.0);  //rotate our camera on teh x-axis (left and right)
      glRotatef(yrot,0.0,1.0,0.0);  //rotate our camera on the y-axis (up and down)
      glTranslated(-xpos,-ypos,-zpos); //translate the screen to the position of our camera
   }


   //OBJECTS DRAWN BELOW:
   //Cube:
   cube(0,0,0 , 0.4,0.4,0.4 , 90);
   sphere2(0,1,0 , 0.2);
   triprism(2,2,2 , 1,3,1, 90);   
   triprism(1,2,1 , 1,6,1, 90);   
   SolidPlane(-1,-1, 0 ,-1,0,0 , 0, 1,0);
   LogSpiral(0.5, 0.1); 
   //  Draw axes
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,mode_arr[mode]);
   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(mode,fov,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
   {
      th = ph = 0;
      xrot = 0;
      yrot = 180;
      xpos = 0;
      ypos = 0;
      zpos = 0;
   }
   //  Toggle axes
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Switch display mode
   else if (ch == 'm' || ch == 'M')
      mode = (mode + 1) % 3;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   
   // FP view
   else if (ch =='k' || ch=='K')
   {
      xrot += 1;
      if (xrot >360) xrot -= 360;
   }
   else if (ch =='i' || ch=='I')
   {
      xrot -= 1;
      if (xrot < -360) xrot += 360;
   }
   if (ch =='l' || ch=='L')
   {
      yrot += 1;
      if (yrot >360) yrot -= 360;
   }
   else if (ch =='j' || ch=='J')
   {
      yrot -= 1;
      if (yrot < -360) yrot += 360;
   }

   // FP movement
   else if (ch =='w' || ch =='W')
   {
      float xrotrad, yrotrad;
      yrotrad = (yrot / 180 * 3.141592654f);
      xrotrad = (xrot / 180 * 3.141592654f);
      xpos += (sin(yrotrad)) ;
      zpos -= (cos(yrotrad)) ;
      ypos -= (sin(xrotrad)) ;
   }

   else if (ch =='s' || ch =='S')
   {
      float xrotrad, yrotrad;
      yrotrad = (yrot / 180 * 3.141592654f);
      xrotrad = (xrot / 180 * 3.141592654f);
      xpos -= (sin(yrotrad));
      zpos += (cos(yrotrad)) ;
      ypos += (sin(xrotrad));
   }

   else if (ch =='d' || ch =='D')
   {
      float yrotrad;
      yrotrad = (yrot / 180 * 3.141592654f);
      xpos += (cos(yrotrad)) * 0.2;
      zpos += (sin(yrotrad)) * 0.2;
   }

   else if (ch=='a' || ch =='A')
   {
      float yrotrad;
      yrotrad = (yrot / 180 * 3.141592654f);
      xpos -= (cos(yrotrad)) * 0.2;
      zpos -= (sin(yrotrad)) * 0.2;
   }
   
   //  Reproject
   Project(mode,fov,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(mode,fov,asp,dim);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("Assignment 2 - William Anderson");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}

