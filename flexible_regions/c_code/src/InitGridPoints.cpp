//============================================================================
//
// Name       : InitGridPoints.cpp
// Author      : C J Sharpe
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : This initilaises the grid point array that conatins all the valid grid
// 				 points within a Flexible Region.
//============================================================================
#include <math.h>
#include <stdio.h>
//============================================================================
/**
 * This enables the grid point array to accurately allocate the size of the array
 *
 * @param  d - dimensions in the Flexible Region
 * @param  q - number of grid point intersections
 * @param  m - Flexible Region shape definition
 * @return point_count - This is the length of the array
 */
//============================================================================

int GetPointsLength(int d,int q, double m)
{

	int i,j,k,l;
	int point_count = 0;
	double centre_p = 0.5;
	double multi = 2.0;
	double d1,d2,d3,d4;
	double FR_Test;

	switch(d)
	{

	case 2:

	for(i = 1; i <= q; i++)
	{
		d1 = (multi * i-1)/(multi * q);

			for(j = 1; j<=q; j++)
			{
				d2 = (multi * j-1)/(multi * q);

	                FR_Test = pow(fabs(multi * (d1 - centre_p)),m) + pow(fabs(multi * (d2 - centre_p)),m);

	                if(FR_Test < 1)
	                {
	                   point_count = point_count + 1;
	                   //printf("Inside %2.4f\n",FR_Test);

	                 }
	            }

	}

	break;

	case 3:

	for(i = 1; i <= q; i++)
	{
		d1 = (multi * i-1)/(multi * q);

			for(j = 1; j<=q; j++)
			{
				d2 = (multi * j-1)/(multi * q);

	                for(k = 1; k<=q; k++)
	                {

	                	d3 = (multi * k-1)/(multi * q);

	                	FR_Test = pow(fabs(multi * (d1 - centre_p)),m) + pow(fabs(multi * (d2 - centre_p)),m) + pow(fabs(multi * (d3 - centre_p)),m);
	                	if(FR_Test < 1)
	                	{
	                		point_count = point_count + 1;
	                		//printf("Inside %2.4f\n",FR_Test);
	                	}
	                }
	            }

	}

	break;

	case 4:

	for(i = 1; i <= q; i++)
	{
		d1 = (multi * i-1)/(multi * q);

			for(j = 1; j<=q; j++)
			{
				d2 = (multi * j-1)/(multi * q);

	                for(k = 1; k<=q; k++)
	                {

	                	d3 = (multi * k-1)/(multi * q);

	                	for(l = 1; l<=q; l++)
	                	{

	                		d4 = (multi * l-1)/(multi * q);

	                		FR_Test = pow(fabs(multi * (d1 - centre_p)),m) + pow(fabs(multi * (d2 - centre_p)),m) + pow(fabs(multi * (d3 - centre_p)),m) + pow(fabs(multi * (d4 - centre_p)),m);

	                		if(FR_Test < 1)
	                		{
	                			point_count = point_count + 1;
	                			//printf("Inside %2.4f\n",FR_Test);
	                		}
	                	}
	                }
	            }

	}

	break;


	}
	return point_count;
}

//============================================================================
/**
 * This writes the valid grid points in the Flexible Region to grid point array
 *
 * @param  *gp - dimensions in the Flexible Region
 * @param  d - dimensions in the Flexible Region
 * @param  q - number of grid point intersections
 * @param  m - Flexible Region shape definition
 * @return point_count - This is the length of the array
 */
//============================================================================

void GenerateGridPoints(int *gp_ptr, int d,int q, double m, int lg)
{

 int *gridpoint_ptr = gp_ptr;
 int length = lg;
 int i,j,k,l;
 int point_count = 0;
 double centre_p = 0.5;
 double multi = 2.0;
 double d1,d2,d3,d4;
 double FR_Test;


 switch(d)
 {

 	case 2:

 		for(i = 1; i <= q; i++)
 		{
 			d1 = (multi * i-1)/(multi * q);

 			for(j = 1; j<=q; j++)
			{
				d2 = (multi * j-1)/(multi * q);

	                FR_Test = pow(fabs(multi * (d1 - centre_p)),m) + pow(fabs(multi * (d2 - centre_p)),m);

	                if(FR_Test < 1)
	                {

	                   *(gridpoint_ptr + point_count) = i;
	                   *(gridpoint_ptr + (point_count + (length * (d -1)))) = j;
	                   //printf("At %d %d are coordinates %d %d\n",i,j,*(gridpoint_ptr + point_count),*(gridpoint_ptr + (point_count + (length * (d -1)))));
	                   point_count = point_count + 1;
	                }
	         }

 		}
 		break;

 		case 3:

 			for(i = 1; i <= q; i++)
 			{
 				d1 = (multi * i-1)/(multi * q);

 				for(j = 1; j<=q; j++)
 				{
 					d2 = (multi * j-1)/(multi * q);

 		                for(k = 1; k<=q; k++)
 		                {

 		                	d3 = (multi * k-1)/(multi * q);

 		                	FR_Test = pow(fabs(multi * (d1 - centre_p)),m) + pow(fabs(multi * (d2 - centre_p)),m) + pow(fabs(multi * (d3 - centre_p)),m);

 		                	if(FR_Test < 1)
 		                	{
 		                	   *(gridpoint_ptr + point_count) = i;
 		                	   *(gridpoint_ptr + (point_count + (length * (d-2)))) = j;
 		                	   *(gridpoint_ptr + (point_count + (length * (d-1)))) = k;

 		                	    //printf("At %d %d %d are coordinates %d %d %d\n",i,j,k,*(gridpoint_ptr + point_count),*(gridpoint_ptr + (point_count + (length * (d -1)))), *(gridpoint_ptr + (point_count + (length * (d-2)))));
 		                	    point_count = point_count + 1;
 		                	}
 		                }
 				 }
 			}

 		break;

 		case  4:

 			for(i = 1; i <= q; i++)
 			 			{
 			 				d1 = (multi * i-1)/(multi * q);

 			 				for(j = 1; j<=q; j++)
 			 				{
 			 					d2 = (multi * j-1)/(multi * q);

 			 		                for(k = 1; k<=q; k++)
 			 		                {

 			 		                	d3 = (multi * k-1)/(multi * q);


 			 		                	for(l = 1; l<=q; l++)
 			 		                	{

 			 		                		d4 = (multi * l-1)/(multi * q);

 			 		                		FR_Test = pow(fabs(multi * (d1 - centre_p)),m) + pow(fabs(multi * (d2 - centre_p)),m) + pow(fabs(multi * (d3 - centre_p)),m) + pow(fabs(multi * (d4 - centre_p)),m);

 			 		                		if(FR_Test < 1)
 			 		                		{
 			 		                			 *(gridpoint_ptr + point_count) = i;
 			 		                			 *(gridpoint_ptr + (point_count + (length * (d-3)))) = j;
 			 		                			 *(gridpoint_ptr + (point_count + (length * (d-2)))) = k;
 			 		                			 *(gridpoint_ptr + (point_count + (length * (d-1)))) = l;

 			 		                			 printf("At %d %d %d %d are coordinates %d %d %d %d\n",i,j,k,l,*(gridpoint_ptr + point_count),*(gridpoint_ptr + (point_count + (length * (d -1)))), *(gridpoint_ptr + (point_count + (length * (d-2)))), *(gridpoint_ptr + (point_count + (length * (d-3)))));
 			 		                			 point_count = point_count + 1;
 			 		                		}
 			 		                	}
 			 		                }
 			 				 }
 			 			}

 		break;

 	}


}


void GenerateEdgeProfile(int *gp_ptr, int d,int q, int lg, int level,int *edgepattern)
{

 int *gridpoint_ptr = gp_ptr;
 int length = lg;
 int counter = 0;
 int current_grid = *(gridpoint_ptr);
 int edges [level];
 int index = 1;
  for(int i = 0;i<length;i++)
  {

 //printf("%d",*(gridpoint_ptr + i));

  if(current_grid == *(gridpoint_ptr+i))
  counter++;
  else
  {
  //printf("\n");
  //printf("%d Edge Between is %d %d\n",counter,current_grid,*(gridpoint_ptr+i));

  edges[index] = counter;
  //printf("%d Edges counter %d\n",index,edges[index]);

  if(index>1)
  {
  if(edges[index-1]!=edges[index])
  *(edgepattern + current_grid-1) = current_grid;
  }

  index++;
  counter=1;
  current_grid = *(gridpoint_ptr+i);
  }

  }

  //for(int i = 0;i<level;i++)
  //{

  //printf("%i Edge Pattern %d\n",i,*(edgepattern + i));

  //}


}
//============================================================================
