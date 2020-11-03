#include <stdlib.h>
#include <stdio.h>
#include <string.h>

float nodeArr[110][1][121][121];

int main(int argc, char* argv[])
{
	//int cX,cY,cZ;
	FILE* f;
	//  Open XYZ file to read node locations
	f = fopen("conc3.txt","r");
	for(int r = 0; r < 121; r++)
	{
		for(int t = 0; t < 110; t++)
		{
			for(int c = 0; c < 121; c++)
			{
				float n;
				if(fscanf(f,"%f", &n))
			//	long before, after;
			//	before = ftell(f);
			//	int returned = fscanf(f,"%f", &n);
			//	after = ftell(f);
			//	if(returned > 1){
			//		printf("Too many scanned, (%f, 0, %f, %f)\n", t, r, c);
			//		printf("Seek: %ld\n", ftell(f));
			//		fclose(f);
			//		return 1;
			//	}
			//	else if(returned <= 0){
			//		printf("None scanned, (%f, 0, %f, %f)\n", t, r, c);
			//		printf("Seek: %ld\n", ftell(f));
			//		fclose(f);
			//		return 2;
			//	}
			    	nodeArr[t][0][r][c] = n;
			}
				
		}	
	}
	fclose(f);

	int t = 60;
	for (int r=0;r<120;r++)
	{
	  for (int c=0;c<120;c++)
	  {
	   float x0 = c/52.6;    float y0 = r/52.6;    float z0 = nodeArr[t][0][r][c]; // Node 0
	   float x1 = (c+1)/52.6;    float y1 = r/52.6;    float z1 = nodeArr[t][0][r][c+1] ; // Node 1
	   float x2 = (c+1)/52.6;    float y2 = (r+1)/52.6;    float z2 = nodeArr[t][0][r+1][c+1] ; // Node 2
	   float x3 = c/52.6;    float y3 = (r+1)/52.6;    float z3 = nodeArr[t][0][r+1][c];// Node 3

	   printf("%f %f %f\n",x0,y0,z0);
	   printf("%f %f %f\n",x1,y1,z1);
	   printf("%f %f %f\n",x2,y2,z2);
	   printf("%f %f %f\n",x3,y3,z3);
	   printf(" --------------------\n");
	  }
	}

	//fout = fopen("outputdebug.txt", "w");
	//printf("NodeArr: \n");
//	for(int r = 0; r < 121; r++)
//	{
//	//	for(int t = 0; t < 110; t++)
//	//	{
//			for(int c = 0; c < 121; c++)
//			{
//			       fprintf(fout, "DrawFace4(0,0,%d,%d);= %f\n",r,c,nodeArr[0][0][r][c]);
//
//         		}
//      	//	}
//   	}

  // fclose(fout);
   return 0;
}
