#include "CSCIx229.h"

/*
 *  Intraocular Drug Delivery
 *
 *  Shows how a drug expands in the eye over a period of 120 hours.
 *
 *  Key bindings:
 *  t/T        Decrease and increade time step
 *  a/A        Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */

// Array of all x,y,z values and time values.
static float  nodeArr[110][1][121][121];

// Projection
int    ortho=0;  // Projection type (turned off for Perspective)
int    th=-20;   // Azimuth
int    ph=+40;   // Elevation
int    zh=30;    // Light azimuth
double fov=45;   // Field of View
double asp=1;    // Aspect ratio
double dim=1;    // Dimension
int    axes=1;   // Axes
double X0=0;     // X center
double Y0=0;     // Y center
double Z0=0;     // Z center
int    time=0;   // Time stamp
double dt=15;    // Time step


/*
 *  Draw 4 node face
 */
static void DrawFace4(int t,int z,int r,int c)
{
   // Check if the values given are out of bounds
   if ((t >= 110) || (z >= 1) || (r >= 121) || (c >= 121)) Fatal("Out of Bounds");
   
   // Calculate all x,y,z values for all four vertices
   float x0 = c/52.6;        float y0 = r/52.6;        float z0 = nodeArr[t][0][r][c];      // Node 0
   float x1 = (c+1)/52.6;    float y1 = r/52.6;        float z1 = nodeArr[t][0][r][c+1];    // Node 1
   float x2 = (c+1)/52.6;    float y2 = (r+1)/52.6;    float z2 = nodeArr[t][0][r+1][c+1];  // Node 2
   float x3 = c/52.6;        float y3 = (r+1)/52.6;    float z3 = nodeArr[t][0][r+1][c];    // Node 3
   
   // Difference vectors
   float dx0 = x1-x0; float dy0 = y1-y0; float dz0 = z1-z0;
   float dx1 = x2-x1; float dy1 = y2-y1; float dz1 = z2-z1;
   
   // Cross product
   float dx = dy0*dz1 - dy1*dz0;
   float dy = dz0*dx1 - dz1*dx0;
   float dz = dx0*dy1 - dx1*dy0;
   float d = sqrt(dx*dx+dy*dy+dz*dz);
   if (d==0) return;
   
   // Add colors based on z value
   if(((z0 > 0.05) & (z0 <= 0.5)) || ((z1 > 0.05) & (z1 <= 0.5)) || ((z2 > 0.05) & (z2 <= 0.5)) || ((z3 > 0.05) & (z3 <= 0.5)) )   
   {   
      glColor3f(0,0,1);
   }
   else if(((z0 > 0.5) & (z0 <= 1)) || ((z1 > 0.5) & (z1 <= 1)) || ((z2 > 0.5) & (z2 <= 1)) || ((z3 > 0.5) & (z3 <= 1)) )   
   {   
      glColor3f(0,1,0);
   }
   else if(z0 > 1 || z1 > 1 || z2 > 1 || z3 > 1)   
   {   
      glColor3f(1,0,0);
   }
   else   
   {   
      glColor3f(1,1,1);
   }
   
   // Draw the face
   glBegin(GL_QUADS);
   glNormal3f(dx/d,dy/d,dz/d);
   glVertex3f(x0,y0,z0);
   glVertex3f(x1,y1,z1);
   glVertex3f(x2,y2,z2);
   glVertex3f(x3,y3,z3);
   glEnd();
}


/*
 * Function to loop through time
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   time = fmod(dt*t,109);
   glutPostRedisplay();
}


/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=3e5;  //  Length of axes
   
   // Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   
   // Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   
   // Undo previous transformations
   glLoadIdentity();
   
   //  Perspective - set eye position
   double Ex = -4*dim*Sin(th)*Cos(ph);
   double Ey = +4*dim        *Sin(ph);
   double Ez = +4*dim*Cos(th)*Cos(ph);
   gluLookAt(Ex,Ey,Ez ,1.14,1.14,0 , 0,Cos(ph), 0);
   glRotated(-90,1, 0, 0);

   // Enable lighting
   float Ambient[]   = {0.3,0.3,0.3,1.0};
   float Diffuse[]   = {0.5,0.5,0.5,1};
   
   // Light direction
   float Position[]  = {Sin(zh),Cos(zh),0.33,0};
   
   // Enable lighting with normalization
   glEnable(GL_LIGHTING);
   glEnable(GL_NORMALIZE);
   
   // glColor sets ambient and diffuse color materials
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   
   // Enable light 
   glEnable(GL_LIGHT0);
   glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
   glLightfv(GL_LIGHT0,GL_POSITION,Position);
   
   // Draw model at a particular time!
   glPushMatrix();
   if(time < 110 && time >= 0)
   {
        for (int j=0;j<120;j++)
	{
	    for (int i=0;i<120;i++)
	    {
	        DrawFace4(time,0,j,i);
	    }
	}
   }
   glPopMatrix();

   // Sanity check
   if (glGetError()) Fatal("ERROR: %s\n",gluErrorString(glGetError()));

   // Draw axes
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   if (axes)
   {
       glBegin(GL_LINES);
       glVertex3f(0.0,0.0,0.0);
       glVertex3f(len,0.0,0.0);
       glVertex3f(0.0,0.0,0.0);
       glVertex3f(0.0,len,0.0);
       glVertex3f(0.0,0.0,0.0);
       glVertex3f(0.0,0.0,len);
       glEnd();
       // Label axes
       glRasterPos3d(len,0.0,0.0);
       Print("X");
       glRasterPos3d(0.0,len,0.0);
       Print("Y");
       glRasterPos3d(0.0,0.0,len);
       Print("Z");
   }
   
   // Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d Time=%f(hrs) DeltaT=%f (hrs per second)",th,ph,((float)time)*(1.090909),12/(11*dt));
   
   // Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}


/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   // Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   
   // Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   
   // Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   
   // Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   
   // PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_DOWN)
      dim *= 1.05;
   
   // PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_UP && dim>1)
      dim *= 0.95;
   
   // Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   
   // Update projection
   Project(ortho?0:fov,asp,dim);
   
   // Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}


/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   // Exit on ESC
   if (ch == 27)
      exit(0);
   
   // Reset view angle
   else if (ch == '0')
      th = ph = 0;
   
   // Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   
   // Change delta t to be higher
   else if(ch == 't' && dt >= 2)
        dt--;
   
   // Change delta t to be lower
   else if(ch == 'T' && dt <= 60)
        dt++;
    
   // Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}


/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   // Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   
   // Set the viewport to the entire window
   glViewport(0,0, width,height);
   
   // Set projection
   Project(ortho?0:fov,asp,dim);
}


/*
 *  Load model from file
 */
void LoadModel()
{
   FILE* f;
   
   // Open XYZ file to read node locations
   f = fopen("conc3.txt","r");
   for(int r = 0; r < 121; r++)
   {
   	for(int t = 0; t < 110; t++)
   	{
	    for(int c = 0; c < 121; c++)
	    {
	    	float n = 0;
	    	if(fscanf(f,"%f", &n))
	    	nodeArr[t][0][r][c] = n;
	    }
   
   	}
   }
   fclose(f);
}


/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   // Initialize GLUT
   glutInit(&argc,argv);
   
   // Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("Drug Expansion in Eye - William Anderson");
   
   // Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);

   // Load model
   LoadModel();
   
   // Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
