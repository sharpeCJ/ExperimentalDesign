//============================================================================
// Name        : Threshold.cpp
// Author      : C J S
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <algorithm>
#include "GlobalVars.h"
#include "mtwist.h"
#include "randistrs.h"

//Utility Functions
void  CreateThresholdSeqeunce(struct GPNode *root, int steps);
void SortDesignPoints(AllPoints *ap);
int Comparef(const void *p1, const void *p2);
void SortThreshold(double *th, int size);
int Compareth(const void *p1, const void *p2);
void ThresholdDebug();
void RunThresholdAccepting(struct GPNode *root,struct GPNode *points, AllPoints *design, int steps, int its);
void setGlobalBest(double CCD, AllPoints *Design);
void generateRandomDesign(AllPoints *design);

int SequenceSize = 100;
int SearchIterations = 100;
double *ThresholdSeq_Ptr = (double *)malloc((SequenceSize)* sizeof(double));

AllPoints *Global_Best_Design = (struct AllPoints *) malloc(sizeof(struct AllPoints));
double GlobalBestCCD = 0;

int main(void)
{

	//Initialise the Flexible Region parameters
	int q = LEVEL;
	int d = DIM;
	double m = M_PARAM;

	//Variables for generating grid points in the FR
	int gp_length;      //Number grid points in FR

	//Determine how many points are in the FR.
	gp_length = GetPointsLength(d,q,m);

	//Create a table of Valid Grid Points coordinates in the FR.
	//Each column is a point coordinate, each row a dimension of a point.
	//The size of this data structure is d * gp_length.
	Gridpoint_Ptr = (int *)malloc((d * gp_length)* sizeof(int));
	GenerateGridPoints(Gridpoint_Ptr,d,q,m,gp_length);

	//Init the variables for indexing the Grid Point data structure while building
	int init_TreeLevel = 1;
	int init_ColCount = 1;
	int init_Col = 0;
	int init_Row  = 0;


	 //Init the Global Variables for the CCD Tree
	 GlobalTraverseCount = 0;
	 GridPoints = gp_length;
	 GridPointSize = gp_length - 1;
	 DesignPoints = DESIGN_POINTS;
	 NodeIndexCounter = 0;
	 Edges = 2 * LEVEL;
	 EdgeTracker_Ptr = (int *)malloc((Edges)* sizeof(int));
	 int *EdgePattern = (int *)malloc((LEVEL)* sizeof(int));
	 initPointer(0,EdgePattern,LEVEL);
	 GenerateEdgeProfile(Gridpoint_Ptr,d,q,gp_length,LEVEL,EdgePattern);

	 //Execution Time Analytics
	 double start = 0;
	 double stop = 0;
	 double cpuTime = 0;

	 //Create the Root Node for the Tree and pointer to the Root
	 CCDTree Root = {9999, NULL, 0,0};
	 CCDTree *Tree_Ptr = &Root;

	 //Create Points Tree
	 CCDTree PointsRoot = {9999, NULL, 0,0};
	 CCDTree *PointsTree_Ptr = &PointsRoot;

	 //#BUILD THE TREE
	 printf("1.\tBuild The Tree\t\n");
	 start = clock();
     BuildTree(Tree_Ptr, init_TreeLevel, init_ColCount,init_Col,init_Row,EdgePattern);
	 BuildPointsTree(PointsTree_Ptr, init_TreeLevel, init_ColCount,init_Col,init_Row);
	 //PointsTreeDebug(PointsTree_Ptr, init_TreeLevel,init_Address_ptr);
	 stop = clock();
	 cpuTime = difftime(stop, start)/(double)CLOCKS_PER_SEC;
	 mt_bestseed();
	 //#ADD CONTRIBUTIONS
	 printf("Completed in %2.4f secs\n",cpuTime);
	 printf("...................\n");
	 printf("2.\tAdd Contributions\t\n");
	 Contributions_Ptr = (int *)malloc((GlobalTraverseCount)* sizeof(int));
	 initPointer(0,Contributions_Ptr,GlobalTraverseCount);
	 start = clock();
	 AddGridContribution(Tree_Ptr,init_TreeLevel,init_Col,init_Row);
	 stop = clock();
	 cpuTime = difftime(stop, start)/(double)CLOCKS_PER_SEC;
	 printf("Completed in %2.4f secs\n",cpuTime);
	 printf("...................\n");
	 //#DEBUG TREES - IF REQUIRED

	 //TreeDebug(Tree_Ptr, init_TreeLevel,init_Address_ptr);
	 EmptyRegionCalc_Debug();

	 //#CREATE THRESHOLD SEQUENCE

	 int stime;
	 long ltime;
	 ltime = time(NULL);
	 stime = (unsigned) ltime/LEVEL;
	 srand(stime);

	 AllPoints *ap = (struct AllPoints *) malloc(sizeof(struct AllPoints));

	 for(int cycles = 0;cycles<1;cycles++)
	 {

	 //#CREATE THRESHOLD
	 printf("3.\tCreate Threshold Sequence\t\n");
	 start = clock();
	 CreateThresholdSeqeunce(Tree_Ptr,SequenceSize);
	 //#DEBUG THRESHOLD - IF REQUIRED
	 //ThresholdDebug();
	 stop = clock();
	 cpuTime = difftime(stop, start)/(double)CLOCKS_PER_SEC;
	 printf("Completed in %2.4f secs\n",cpuTime);
	 printf("...................\n");
	 //#OPTIMISE THE DESIGN - THRESHOLD ACCEPTING
	 printf("4.\tOptimise Design - Run TA\t\n");
	 start = clock();
	 generateRandomDesign(ap);
	 setGlobalBest(9999,ap);
	 RunThresholdAccepting(Tree_Ptr,PointsTree_Ptr,ap,SequenceSize,SearchIterations);
	 stop = clock();
	 cpuTime = difftime(stop, start)/(double)CLOCKS_PER_SEC;
	 printf("Completed in %2.4f secs\n",cpuTime);
	 printf("...................\n");

	 printf("-------------------------------------------------------------------------\n");
	 printf("Empty Region Discrepancy: d = %d, m = %1.2f, q = %d : Empty = %2.4f\n",DIM,M_PARAM,LEVEL,EmptyRegion_Discc);
	 printf("\nPoints Length %d  Leaf Node Count %d, Global Node Count %d\n",gp_length,GlobalTraverseCount, NodeIndexCounter + GlobalTraverseCount);

	 printf("\n-------------------------------------------------------------------------\n");
	 printf("%d Best Design n = %d:\n",cycles,DESIGN_POINTS);
	 printf("[");
		  for(int i = 0; i<DesignPoints;i++)
		  {
			  for(int j = 0; j<DIM; j++)
			  {
				  if(j<DIM-1)
				  printf("%d,",Global_Best_Design->dt[i][j]);
				  else
				  printf("%d;",Global_Best_Design->dt[i][j]);
			  }
		  }
	 printf("]");
	 printf("\n");


	 DesignPoints_Ptr = (int *)Global_Best_Design->dt;
	 //double CCD_Calc =  CalculateCCD(Tree_Ptr);
	 printf("Optimised CCD Best %2.6f\n",GlobalBestCCD);
	 printf("-------------------------------------------------------------------------\n");
	 }
	 return EXIT_SUCCESS;

}

void RunThresholdAccepting(struct GPNode *root,struct GPNode *points, AllPoints *design, int steps, int its)
{
 double Current_Best_CCD = 9999;
 double Global_Best_CCD = 9999;
 double CCD_Calc = 0;
 double CCD_Calc_Ref = 0;
 double Threshold;
 AllPoints *best_design = (struct AllPoints *) malloc(sizeof(struct AllPoints));
 AllPoints *old_design = (struct AllPoints *) malloc(sizeof(struct AllPoints));


 uint32_t rand_level;
 uint32_t rand_col;
 uint32_t rand_row;
 int ud = 3;

 double start = 0;
 double stop = 0;
 double cpuTime;

 generateRandomDesign(old_design);
 SortDesignPoints(design);
 DesignPoints_Ptr = (int *)(design->dt);
 CCD_Calc_Ref = -CalculateCCD(root);

 	 for(int i=0;i<steps;i++)
 	 {
 		Threshold = *(ThresholdSeq_Ptr + i);

 		 start = clock();



 		 for(int j=0;j<its;j++)
 		 {

 			 //Select Point element at random

			 //printf("Old : {%d,%d,%d}\n",design->dt[ran_point][0],design->dt[ran_point][1],design->dt[ran_point][2]);
			 //printf("Design %d Grid %d\n",ran_point,ran);

 			for(int i = 0; i<DESIGN_POINTS; i++)
 			{
 				for(int j = 0; j<DIM; j++){
 					old_design->dt[i][j] = design->dt[i][j];
 				}
 			}
 			for(int i=0;i<ud;i++)
 			{
 				rand_col = mt_lrand() %DIM;
 				rand_row = mt_lrand() %DESIGN_POINTS;
 				 //if(rand_row > DesignPoints-1)
 					 //rand_row = 0;
 				rand_level = mt_lrand() % LEVEL;
 				rand_level++;

 				int *next_point = (int *)Gridpoint_Ptr + mt_lrand() % GridPointSize;

 				for(int j=0;j<DIM;j++)
 					design->dt[rand_row][j] = *(next_point  + j * GridPoints);


 				//for(int k=0;k<DIM;k++)
 				//{
 					//if(k==rand_col)
 					//{
 						//if(old_design->dt[rand_row][k] != rand_level)
 						//design->dt[rand_row][k] = rand_level;
 					//}
 				//}

 				//rand_row++;
 			 //printf("New : {%d,%d,%d}\n",design->dt[ran_point][0],design->dt[ran_point][1],design->dt[ran_point][2]);
 			}
 		SortDesignPoints(design);
	  	DesignPoints_Ptr = (int *)(design->dt);
	  	//Generate CCD
	  	CCD_Calc = CalculateCCD(root);
	  	//Check Against Threshold
	  	//printf("it %d Threshold %2.4f CCD %2.4f\n",j,Threshold,CCD_Calc);
	  	//Accept/Reject
	  	if((CCD_Calc + CCD_Calc_Ref)<Threshold)
	  	{
	  		Current_Best_CCD = CCD_Calc;
	  		//printf("Current Best %2.4f\n",Current_Best_CCD);
	  		for(int i = 0; i<DESIGN_POINTS; i++)
	  		{
	  			for(int j = 0; j<DIM; j++){
	  				best_design->dt[i][j] = design->dt[i][j];
	  			}

	  		}
	  		if(Current_Best_CCD<Global_Best_CCD)
	  		{
			 Global_Best_CCD = Current_Best_CCD;
			 setGlobalBest(Global_Best_CCD, best_design);
	  		}
	   }
	   else
	   {
		   for(int i = 0; i<DESIGN_POINTS; i++)
		   {
			   for(int j = 0; j<DIM; j++){
				   design->dt[i][j] = old_design->dt[i][j];
			   }
		   }

	   }
	 }

 		stop = clock();
 	 	cpuTime = difftime(stop, start)/(double)CLOCKS_PER_SEC;
 		//printf("Completed in %2.4f secs\n",cpuTime);
 		cpuTime = 0;
 	}

}

void generateRandomDesign(AllPoints *design)
{
	int *next_point;

	for(int i=0;i<DESIGN_POINTS;i++)
	{
		next_point = (int *)Gridpoint_Ptr + mt_lrand() % GridPointSize;

		for(int j=0;j<DIM;j++)

		design->dt[i][j] = *(next_point  + j * GridPoints);
	}

	SortDesignPoints(design);
	DesignPoints_Ptr = (int *)(design->dt);


}

void setGlobalBest(double CCD, AllPoints *Design)
{
	//printf("Best Design n = %d:\n",DESIGN_POINTS);
	//printf("[");
	for(int i = 0; i<DESIGN_POINTS; i++)
	{
		for(int j = 0; j<DIM; j++){
			Global_Best_Design->dt[i][j] = Design->dt[i][j];

			//if(j<DIM-1)
			//printf("%d,",Global_Best_Design->dt[i][j]);
			//else
			//printf("%d;",Global_Best_Design->dt[i][j]);
		}
	}

	 //printf("]");
	 //printf("\n");

GlobalBestCCD = CCD;
printf("Global Best %2.4f\n",GlobalBestCCD);
}


void CreateThresholdSeqeunce(struct GPNode *root, int steps)
{
    int quantiles = 50;
	double *quantile_seq = (double *)malloc((steps * quantiles)* sizeof(double));
    int quantile_steps = steps * quantiles;
    int quantile_start = quantile_steps - steps;

    AllPoints *ap = (struct AllPoints *) malloc(sizeof(struct AllPoints));

    int rand_level;
    int rand_col;
    int rand_row;
    int ud = 3;

    double CCD_Calc_Ref = 0;
    double CCD_Calc = 0;

	for(int its=0;its<quantile_steps;its++)
	{
		//Generate Random Point
		generateRandomDesign(ap);
		CCD_Calc_Ref = CalculateCCD(root);

		//for(int i=0;i<DesignPoints;i++)
		for(int i=0;i<ud;i++)
		{
		 //int *start = (int *)Gridpoint_Ptr;
		 //int next_point = rand() % GridPointSize + 1;
		 //int *next = (int *)start + next_point;
			rand_col = mt_lrand() %DIM;
			rand_row = mt_lrand() %DESIGN_POINTS;
			rand_level = mt_lrand() % LEVEL;
			rand_level++;


			for(int j = 0; j<DIM; j++)
			{
				if(j==rand_col)
			    {ap->dt[rand_row][j] = rand_level;}
				//ap->dt[i][j] = *(next  + j * GridPoints);
			}

		 }

		 //Sort point
		 SortDesignPoints(ap);
		 DesignPoints_Ptr = (int *)(ap->dt);

		 //Add to Threshold Sequence
		  CCD_Calc = CalculateCCD(root);
		  double diff =  CCD_Calc_Ref - CCD_Calc;
		  *(quantile_seq + its) = fabs(diff);
		}

	SortThreshold(quantile_seq,quantile_steps);

    if(quantiles > 1)
    {	quantile_start = quantile_start-1;

    }
	for(int its=0;its<steps;its++)
			*(ThresholdSeq_Ptr + its) = *(quantile_seq + quantile_start + its);

}

void ThresholdDebug()
{
	printf("Debug - Print Threshold Sequence\n");
	printf("==============================\n");
	for(int i = 0; i<SequenceSize;i++)
	{
		printf("%d : %2.6f \n",i+1,*(ThresholdSeq_Ptr + i));
	}
}

void initPointer(int init_value, int *ptr,int size)
{

	for(int i=0;i<size;i++){*(ptr + i) = init_value;}

}


void SortDesignPoints(AllPoints *ap)
{

  qsort(ap->dt, DesignPoints, sizeof ap->dt[0], Comparef);

}

int Comparef(const void *p1, const void *p2)
{
	  int* a = (int*)p1;
	  int* b = (int*)p2;

	  if (a[0] < b[0] ) return -1;
	  if (a[0] > b[0] ) return +1;
	  return 0;
}

void SortThreshold(double *th, int size)
{

  qsort(th, size, sizeof th[0], Compareth);

}

int Compareth(const void *x, const void *y)
{
		//Return List in Ascending Order
		double dx, dy;

	    dx = *(double *)x;
	    dy = *(double *)y;

	    if (dx > dy)
	    {
	        return -1;
	    }
	    else if (dx < dy)
	    {
	        return +1;
	    }
	    else return 0;
}


