#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "GlobalVars.h"
#include <time.h>
/*
 * DiscrepancyCalc.cpp
 *
 *  Created on: 19 Sep 2011
 *      Author: Chris
 */

//Internal functuions
void TraverseRegion(struct GPNode *node, JobCard *points, int address,int level);
int GetIndicesExtent(struct JobCard *jobcard, int gridpoint, int start, int direction);
void InitDesignCard(struct JobCard *child_jobcard,int *parent_design, int level, int points_count,int index, int direction);
void AddDesignPoints(struct JobCard *child_jobcard,int *parent_design, int level,int start_read,int start_write, int points_count, int direction);
void InitFirstdDesignCard(struct JobCard *jobcard, int level);
void PointStateDebug(struct JobCard *jobcard);
double DiscrepancyAdjustment(int design_points,int contribution);
//Sort Utility Functions
int Compare(const void *p1, const void *p2);
void SortDesignPoint(struct JobCard *jobcard, int points_total);

double CDD_Adjustment;
double start;
double end;
double power = (1/pow(2,DIM));

double design_conv;
double region_conv;
double region_adjustment = 0;
double calcc = 0;

double CalculateCCD(struct GPNode *root)
{
    //Get the extents of the shape from the tree
	int level_init = 1;
	int start_address = 0;

	struct JobCard *jobcard = (struct JobCard *) malloc(sizeof(struct JobCard));

    jobcard->stop_l = DesignPoints;
    jobcard->card_init_l = 0;
    jobcard->stop_r = DesignPoints;
    jobcard->card_init_r = 0;
    CDD_Adjustment = 0;
    InitFirstdDesignCard(jobcard,level_init);
    TraverseRegion(root,jobcard, start_address,level_init);

    double CCD =  EmptyRegion_Discc + (CDD_Adjustment * (1/(double)GridPoints));

    //printf("Final CCD Calculation %2.4f %2.4f\n",CCDc, CCD);
    //printf("===================================================\n\n");
    //printf("CCD %2.6f = Empty Region %2.6f - Adjustment %2.4f\n-----------\n", CCD, EmptyRegion_Discc, CDD_Adjustment * 1/(double)GridPoints);
    free(jobcard);
	return CCD;
}

double DiscrepancyAdjustment(int design_points,int contribution)
{
	double design_conv;
	double region_conv;
	double region_adjustment = 0;
	double calcc = 0;

	//start = clock();
	region_conv = divide(contribution, GridPoints);//(contribution/(double)GridPoints);
	design_conv = divide(design_points, DesignPoints);//(double)design_points/DesignPoints;
	calcc = pow(region_conv - design_conv,2);
    //printf("Region %2.4f Design %2.4f Calculation %2.4f\n",region_conv,design_conv,calc);
    region_adjustment = (calcc - pow(region_conv,2)) * power;
    //end = clock();
    //calc = calc + difftime(end,start)/(double)CLOCKS_PER_SEC;
    //Calcc_inc++;
    //if(contribution==0)
    //region_adjustment=pow(region_conv,2);

	return region_adjustment;
}


void TraverseRegion(struct GPNode *node, JobCard *jobcard, int address,int level)
{

	GPNode *parent_node = node;


	//printf("========================================================\n");
	//printf("Traverse State Dim %d Parent Value %d Node Total %d\n",level,node->grid_ref,node->node_tot);
	//printf("========================================================\n");

	if(level==DIM)
	{
			//printf("\tProcess Leaf Nodes - Y Dimension Traversal\n");
			//printf("........................................................\n");


			GPNode *child_node_l = parent_node->nodes;
			GPNode *child_node_r = (parent_node->nodes + parent_node->node_tot);

			int L = child_node_l->grid_ref;
			int R = child_node_r->grid_ref;

			struct TraverseCard traverse_card = {L,R,L,R};


		     //Looking LHS Traversal
		     int design_coord_l = jobcard->dt[0][1];
		     int next_point_l = 0;
		     int count_l = 0;


		     //Looking RHS Traversal
		     int design_coord_r = jobcard->dt[jobcard->stop_r-1][1];
		     int next_point_r = jobcard->stop_r;
		     int count_r = 0;

		     if(R<LEVEL)
		    	 traverse_card.RHS_Counter = R + 1;

		     int grid_ref_l = L;
		     int grid_ref_r = R;
		     int nodes = parent_node->node_tot;

		     //discc
		     double discc_adjusment_l = 0;
		     double discc_adjusment_r = 0;
		     int left_dp_counter = 0;
		     int right_dp_counter = 0;

		     //cotribution
		     int cont_val = 0;

			//Traverse The Leaf Nodes
			for(int i=0;i<=nodes;i++)
			{
				child_node_l  = parent_node->nodes + i;
				grid_ref_l = child_node_l->grid_ref;

				child_node_r  = parent_node->nodes + nodes - i;
				grid_ref_r = child_node_r->grid_ref;

				if(next_point_l<jobcard->stop_l)
				{
					design_coord_l = jobcard->dt[next_point_l][1];
				}
				else
					design_coord_l = 9999;

				if(next_point_r>0)
			    {
					design_coord_r = jobcard->dt[next_point_r-1][1];
			    }
				else
					design_coord_r = -9999;

				//printf("Left Grid Value [%d] Right Grid Ref  [%d] -> Left Contribution (%d) Right Contribution (%d)\n",grid_ref_l, grid_ref_r, (int)*(Contributions_Ptr + child_node_l->array_index), (int)*(Contributions_Ptr + child_node_r->array_index));


			  	if(grid_ref_l != traverse_card.LHS_Counter)
			  	{
			       traverse_card.LHS_Counter = traverse_card.LHS_Counter + 1;
			       traverse_card.RHS_Counter = traverse_card.RHS_Counter - 1;
			  	}

			  	//printf("Grid Ref: Left [%d] OR Right [%d] == Design Value Left {%d} AND Right {%d}\n",traverse_card.LHS_Counter,traverse_card.RHS_Counter,design_coord_l,design_coord_r);

			  		if(traverse_card.LHS_Counter>=design_coord_l && design_coord_l != 9999)
			  		{
					//printf("Detect Point LHS\n");
					count_l = 0;
					count_l = GetIndicesExtent(jobcard,grid_ref_l,next_point_l,LEFT);
					next_point_l = next_point_l + count_l;
					left_dp_counter = left_dp_counter + count_l;
			  		}

			  		cont_val = (int)*(Contributions_Ptr + child_node_l->array_index);

			  		if(left_dp_counter>0 && cont_val>0)
			  		{
			  			double dl = DiscrepancyAdjustment(left_dp_counter,cont_val);
			  			//printf("Left %2.4f %d\n",dl,(int)*(Contributions_Ptr + child_node_l->array_index));
			  			discc_adjusment_l =  discc_adjusment_l + dl;
			  			//printf("Accumulate Left Pointer: Left Counter %d Adjustment %2.4f Contribution %d\n",left_dp_counter,dl,(int)*(Contributions_Ptr + child_node_l->array_index));
			  		}
			  		//else
			  			//discc_adjusment_l = discc_adjusment_l + pow(((int)*(Contributions_Ptr + child_node_l->array_index)/(double)GridPoints),2);


			  		if(traverse_card.RHS_Counter<=design_coord_r && design_coord_r != -9999)
			  		{
                	   count_r = 0;
                	   count_r = GetIndicesExtent(jobcard,traverse_card.RHS_Counter,next_point_r-1, RIGHT);
                	   next_point_r = next_point_r - count_r;
                	   right_dp_counter = right_dp_counter + count_r;
			  		}

			  		if(right_dp_counter>0 && cont_val>0)
			  		{
			  		    double dr = DiscrepancyAdjustment(right_dp_counter,cont_val);
			  			//printf("Right %2.4f %d\n",dr,(int)*(Contributions_Ptr + child_node_l->array_index));
			  			discc_adjusment_r =  discc_adjusment_r + dr;
			  			//printf("\nAccumulate Right Pointer: Right Counter %d Adjustment %2.4f Contribution %d\n",right_dp_counter,dr,(int)*(Contributions_Ptr + child_node_r->array_index));

			  			//#printf("h %2.4f dr %2.4f\n",h,dr);
			  		}

			  		//else
			  		//{
			  		//	discc_adjusment_r = discc_adjusment_r + pow(((int)*(Contributions_Ptr + child_node_r->array_index)/(double)GridPoints),2);
			  		//}

			}


			CDD_Adjustment = adding(CDD_Adjustment,discc_adjusment_r,discc_adjusment_l);
	}
 	else
	{

 	 int next_level = level  + 1;
	 GPNode *child_node = parent_node->nodes;
	 int L = child_node->grid_ref;
	 int R = (parent_node->nodes + parent_node->node_tot)->grid_ref;

	 struct TraverseCard traverse_card = {L,R,L,R};

	 if(R<LEVEL)
	 {
		 traverse_card.RHS_Counter = R + 1;
	 }

	 struct JobCard *child_jobcard_l = (struct JobCard *) malloc (sizeof(struct JobCard));
	 struct JobCard *child_jobcard_r = (struct JobCard *) malloc (sizeof(struct JobCard));

	 int grid_ref;
     int nodes = parent_node->node_tot;

     //Looking LHS Traversal
     int design_coord_l = jobcard->dt[0][1];
     int next_point_l = 0;
     int count_l = 0;


     //Looking RHS Traversal
     int design_coord_r = jobcard->dt[jobcard->stop_r-1][1];
     int next_point_r = jobcard->stop_r;
     int count_r = 0;

     /*if(design_coord_l>L && design_coord_r<R)
     {traverse_card.LHS_Counter = design_coord_l;
     traverse_card.RHS_Counter =  (LEVEL+1/2) + design_coord_l;}*/
     //printf("Next POINT r-1 %d \n",next_point_r-1);
     //Traverse The Tree
     for(int i=0;i<=nodes;i++)
	 {

    	 child_node  = parent_node->nodes + i;
    	 grid_ref = child_node->grid_ref;

    	 if(next_point_l<jobcard->stop_l)
    	 design_coord_l = jobcard->dt[next_point_l][1];
    	 else
    	 design_coord_l = 9999;


    	 if(next_point_r>0)
    	 design_coord_r = jobcard->dt[next_point_r-1][1];
    	 else
    	 design_coord_r = -9999;


    	 if(grid_ref != traverse_card.LHS_Counter)
    	 {
    		 traverse_card.LHS_Counter = traverse_card.LHS_Counter + 1;
    	 	 traverse_card.RHS_Counter = traverse_card.RHS_Counter - 1;
    	 }

    	 //printf("........................................................\n");
    	 //printf("Process Tree Node - %d Dimension Traversal\n\n",level);
    	 //printf("Grid Ref: Left [%d] OR Right [%d] == Design Value Left {%d} AND Right {%d}\n",traverse_card.LHS_Counter,traverse_card.RHS_Counter,design_coord_l,design_coord_r);

    	 if(traverse_card.LHS_Counter>=design_coord_l)
         {
    		  count_l = 0;
    		  //start = clock();
    		  count_l = GetIndicesExtent(jobcard,grid_ref,next_point_l,LEFT);
    		  //end = clock();
    		  //get = get + difftime(end,start)/(double)CLOCKS_PER_SEC;

    		next_point_l = next_point_l + count_l;

    		 if(jobcard->card_init_l != 1)
        	 {
        	  //printf("--------------------------------------------------\n");
        	  //printf("\tInitialising New Left Traverse Job Card\n");
        	  //printf("--------------------------------------------------\n");

        	  jobcard->card_init_l = 1;

              //Init Child Card...
        	  child_jobcard_l->stop_l = count_l;
        	  child_jobcard_l->stop_r = count_l;

        	  //printf("Init: Points Count (%d) \n",count_l);
        	  //printf("Init: Stop Index   (%d) \n",child_jobcard_l->stop_l);
        	  //start = clock();
        	  InitDesignCard(child_jobcard_l,&jobcard->dt[0][0],next_level,count_l,0,LEFT);
        	  //end = clock();
        	  //init = init + difftime(end,start)/(double)CLOCKS_PER_SEC;

        	   if(count_l > 1)
        	  	SortDesignPoint(child_jobcard_l,count_l);

        	   /*if(design_coord_l<=L && R<LEVEL)
        	   {

        		   //printf("Left Sup, %d ",L);
        		   surpress_check_l = 1;
        	   }*/


        	  //printf("--------------------------------------------------\n");
        	 }
    		 else
    		 {
    			 //printf("--------------------------------------------------\n");
    		    //printf("\tAdding Points New Left Traverse Job Card\n");
    			//printf("--------------------------------------------------\n");
    			//printf("Add: Points Count (%d) \n",count_l);

    			//printf("Add: Old Stop Index (%d) \n",child_jobcard_l->stop_l);
    			child_jobcard_l->stop_l = child_jobcard_l->stop_l + count_l;
    			child_jobcard_l->stop_r = child_jobcard_l->stop_r + count_l;
    			//printf("Add: New Stop Index (%d) \n",child_jobcard_l->stop_l);

				int next_write = child_jobcard_l->stop_l - count_l;//next_point_r-1;//child_jobcard_r->stop_r-1
        		int next_read = next_write * 2;
        		//start = clock();
    			AddDesignPoints(child_jobcard_l,&jobcard->dt[0][0],next_level,next_read,next_write,count_l,LEFT);
    			//end = clock();
    			//add = add + difftime(end,start)/(double)CLOCKS_PER_SEC;

    			SortDesignPoint(child_jobcard_l,next_point_l);

    			//printf("--------------------------------------------------\n");

    		 }

         }//END (traverse_card.LHS_Counter>=design_coord)

    	 //if( traverse_card.LHS_Counter==R)
    		// surpress_check_r = 1;
    	 //Go down the Tree LHS Traversal....
    	 if(jobcard->card_init_l==1)// && surpress_check_l==0)
    	 {
    		 if(next_level<DIM)
    		 {child_jobcard_l->card_init_l = 0;
    		  child_jobcard_l->card_init_r = 0;}
    	 TraverseRegion(child_node,child_jobcard_l,0,next_level);
    	 }

    	 if(traverse_card.RHS_Counter<=design_coord_r)
         {
        		count_r = 0;
        		//start = clock();
        		count_r = GetIndicesExtent(jobcard,traverse_card.RHS_Counter,next_point_r-1, RIGHT);
        		//end = clock();
        		//get = get + difftime(end,start)/(double)CLOCKS_PER_SEC;

        		next_point_r = next_point_r - count_r;

        		if(jobcard->card_init_r != 1)
        		{
        			//printf("--------------------------------------------------\n");
        			//printf("\tInitialising New Right Traverse Job Card\n");
        			//printf("--------------------------------------------------\n");

        			jobcard->card_init_r = 1;

        			//Init Child Card...
        			child_jobcard_r->stop_l = count_r;
        			child_jobcard_r->stop_r = count_r;

        			//printf("Init: Points Count (%d) \n",count_r);
        			//printf("Init: Stop Index   (%d) \n",child_jobcard_r->stop_r);

        			int  index = (jobcard->stop_r * 2) -2;
        			//start = clock();
        			InitDesignCard(child_jobcard_r,&jobcard->dt[0][0],next_level,count_r,index,RIGHT);
        			//end = clock();
        			//init = init + difftime(end,start)/(double)CLOCKS_PER_SEC;
        			if(count_r > 1)
        				SortDesignPoint(child_jobcard_r,count_r);


        			  /*if(design_coord_r>=R+1 && R<LEVEL)
        			  {
        			        		   //printf("Right Sup, %d ",R+1);
        			        		   surpress_check_r = 1;
        			  }*/
        			//printf("--------------------------------------------------\n");
        		}
        		else
        		{
        			//printf("--------------------------------------------------\n");
        			//printf("\tAdding Points New Right Traverse Job Card\n");
        			//printf("--------------------------------------------------\n");
        			//printf("Add: Points Count (%d) \n",count_r);
        			//printf("Add: Old Stop Index (%d) \n",child_jobcard_r->stop_r);
        			child_jobcard_r->stop_r = child_jobcard_r->stop_r + count_r;
        			child_jobcard_r->stop_l = child_jobcard_r->stop_l + count_r;
        			//printf("Add: New Stop Index (%d) \n",child_jobcard_r->stop_r);

        			int next_write = child_jobcard_r->stop_r - count_r;//next_point_r-1;//child_jobcard_r->stop_r-1
        			int next_read = (next_point_r + (count_r -1)) * 2;
        			//printf("Next Read %d, Next Write %d \n",next_read,next_write);
        		    //start = clock();
        			AddDesignPoints(child_jobcard_r,&jobcard->dt[0][0],next_level,next_read,next_write,count_r,RIGHT);
        		    //end= clock();
        		    //add = add + difftime(end,start)/(double)CLOCKS_PER_SEC;
        		    SortDesignPoint(child_jobcard_r,child_jobcard_r->stop_r);

        			//printf("--------------------------------------------------\n");
        		}

         }//END (traverse_card.RHS_Counter<=design_coord)


    	 //Go down the Tree RHS Traversal....
    	 if(jobcard->card_init_r == 1)// && surpress_check_r==0)
    	 {  if(next_level<DIM)
		 {
		  child_jobcard_r->card_init_l = 0;
		  child_jobcard_r->card_init_r = 0;
		 }
    		TraverseRegion(child_node,child_jobcard_r,0,next_level);
    	 }
	 }//END Traverse Nodes
     free(child_jobcard_r);
	 free(child_jobcard_l);
	}//END (level<DIM)

}

void InitFirstdDesignCard(struct JobCard *jobcard, int level)
{

	//printf("--------------------------------------------------\n");
	//printf("Initialise First Card\n");


    int *dtPtr = (int *) malloc((ROWS * DesignPoints) * sizeof(int));

    int next_r = 0;
    int next_c = 1;
    int next_dp = 0;

    for(int i=0;i<DesignPoints;i++)
    {
    	*(dtPtr + next_r) =  i;
    	*(dtPtr + next_c) =  *(DesignPoints_Ptr + (level-1) + next_dp);


    	//printf("%d | %d |\n",*(dtPtr + next_r),*(dtPtr + next_c));
    	next_dp =  next_dp + DIM;
    	next_r = next_r  + 2;
    	next_c = next_c  + 2;

    }

	InitDesignCard(jobcard,dtPtr,level,DesignPoints,0,LEFT);

	//printf("--------------------------------------------------\n");
}

void InitDesignCard(struct JobCard *child_jobcard,int *parent_design, int level, int points_count,int index, int direction)
{

	int next_r = index;
	int design_index = 0;

	switch(direction)
	{
	 case LEFT:

		 //printf("\nInitialise Design LHS Card\n\n");

		 for (int i = 0 ; i < DesignPoints ; i++)
		 {
			 //Add in all parent points int that plane.
			 if(points_count > 0)
			 {
				 points_count--;
				 design_index = *(parent_design + next_r);
				 child_jobcard->dt[i][0] = *(parent_design + next_r);
				 child_jobcard->dt[i][1] =  *(DesignPoints_Ptr + (level-1) + (design_index * DIM));
			 }
			 else
			 {
				 child_jobcard->dt[i][0] = 0;
				 child_jobcard->dt[i][1] = 0;
			 }

			 //printf("Parent ..%d | %d |", design_index,*(DesignPoints_Ptr + (level-1) + (design_index * DIM)));
			 //printf("Child  ..%d | %d |\n", child_jobcard->dt[i][0],child_jobcard->dt[i][1]);

			 next_r = next_r  + 2;
		 }
		break;

		case RIGHT:

			//printf("\nInitialise Design RHS Card\n\n");

			 for (int i = 0 ; i < DesignPoints ; i++)
			{
						 //Add in all parent points int that plane.
						 if(points_count > 0)
						 {
							 points_count--;
							 design_index = *(parent_design + next_r);
							 child_jobcard->dt[i][0] = *(parent_design + next_r);
							 child_jobcard->dt[i][1] =  *(DesignPoints_Ptr + (level-1) + (design_index * DIM));
						 }
						 else
						 {
							 child_jobcard->dt[i][0] = 0;
							 child_jobcard->dt[i][1] = 0;
						 }

						 //printf("Parent ..%d | %d |",design_index,*(DesignPoints_Ptr + (level-1) + (design_index * DIM)));
						 //printf("Child  ..%d | %d |\n", child_jobcard->dt[i][0],child_jobcard->dt[i][1]);

						 next_r = next_r  - 2;
					 }

		break;


	}

}

void AddDesignPoints(struct JobCard *child_jobcard,int *parent_design, int level,int start_read,int start_write, int points_count,int direction)
{

			int next_r;// = start - points_count;
			int next_w;
			int design_index;

			switch(direction)
			{

				case LEFT:

					next_r = start_read;
					next_w = start_write;
					design_index = 0;

					//printf("\nAdd To LHS Design Card %d %d\n\n",next_r,next_w);

					for (int i = 0 ; i < points_count ; i++)
					{
						//Add in all parent points int that plane.
						design_index = *(parent_design + next_r);
						child_jobcard->dt[next_w+i][0] = *(parent_design + next_r);
						child_jobcard->dt[next_w+i][1] =  *(DesignPoints_Ptr + (level-1) + (design_index * DIM));

						//printf("Parent...%d | %d | ", design_index,*(DesignPoints_Ptr + (level-1) + (design_index * DIM)));
						//printf("Child...%d  | %d | ", child_jobcard->dt[i + next_w][0],child_jobcard->dt[i + next_w][1]);
						//printf( "Pts C %d Index %d\n",points_count,design_index);


						next_r = next_r  + 2;
				 }

					break;

				case RIGHT:

					next_r = start_read;
					next_w = start_write;
					design_index = 0;

					//printf("\nAdd To RHS Design Card %d %d\n\n",next_w,next_r);

					for (int i = 0 ; i < points_count ; i++)
					{
						//Add in all parent points int that plane.
						design_index = *(parent_design + next_r);
					    child_jobcard->dt[next_w+i][0] = *(parent_design + next_r);
						child_jobcard->dt[next_w+i][1] =  *(DesignPoints_Ptr + (level-1) + (design_index * DIM));

						//printf("Parent...%d | %d | ", design_index,*(DesignPoints_Ptr + (level-1) + (design_index * DIM)));
						//printf("Child...%d  | %d | ", child_jobcard->dt[i + next_w][0],child_jobcard->dt[i + next_w][1]);
						//printf( "Pts C %d Index %d\n",points_count,design_index);

						next_r = next_r  - 2;
					}

					break;

			}

}

int GetIndicesExtent(struct JobCard *jobcard,int grid_point, int start, int direction)
{
	//printf("\nInitialise Indices Extent\n\n");

	int points_counter = 0;
	int start_index;
	int stop_index;
	int design_point;


	switch(direction)
	{
	 case LEFT:

		start_index = start;
		stop_index = jobcard->stop_l;
		design_point = jobcard->dt[start_index][1];

		while(design_point  <= grid_point)
		{
			//printf("LEFT: At index (%d) : Design Point %d | Grid Point %d\n",start_index,design_point,grid_point);

			points_counter++;
			start_index++;

			if(start_index<stop_index)
			{
				design_point = jobcard->dt[start_index][1];
			}
			else
				break;
		 }

		//printf("\nLEFT: Start Index %d Stop Index %d Points Total %d\n",start,stop_index,points_counter);

	break;

	case RIGHT:

			start_index = start;
			stop_index = -1;
			design_point = jobcard->dt[start_index][1];

			while(design_point >= grid_point)
			{
				//printf("RIGHT: At index (%d) : Design Point %d | Grid Point %d\n",start_index,design_point,grid_point);

				points_counter++;
				start_index--;

				if(start_index>stop_index)
				{
					design_point = jobcard->dt[start_index][1];
				}
				else
					break;
			 }

			//printf("\nRIGHT Start Index %d Stop Index %d Points Total %d\n",start,stop_index,points_counter);

		break;
	}

    return points_counter;
}



void SortDesignPoint(struct JobCard *jobcard, int points_total)
{

  //printf("\nSort Design Card\n");


  qsort(jobcard->dt, points_total, sizeof jobcard->dt[0], Compare);

  for (int i = 0 ; i < points_total ; i++ )
  {

  	 //printf("%d | %d | \n",jobcard->dt[i][0],jobcard->dt[i][1]);

  }
}

int Compare(const void *p1, const void *p2)
{
	  int* a = (int*)p1;
	  int* b = (int*)p2;

	  //printf("a %d == b %d %d??\n",a[1],b[1],(a[1] < b[1]));

	  if (a[1] < b[1] ) return -1;
	  if (a[1] > b[1] ) return +1;
	  return 0;
}

void PointStateDebug(struct JobCard *jobcard)
{

	//Break if global points reached
	int global_count = 0;//jobcard->points_counter;
	int *design_table = (int *)malloc((DIM * LEVEL)* sizeof(int));
	int next_design_point = 0;

	for(int i=0;i<LEVEL * DIM;i++)
	{

		  *(design_table + i) = 0;
	}


	for(int i=0; i<global_count;i++)
	{

		for(int j=0;j<DIM;j++)
	       {
	    	 int design_index =  *(DesignPoints_Ptr + (j + next_design_point));
	    	 //printf("design Index %d\n",design_index);
	    	 int points = *(design_table + (j * DIM-1) + design_index);
	    	 *(design_table + (j * DIM-1) + design_index) = points + 1;
	       }

	         next_design_point = next_design_point + DIM;

	   }


	 printf("Design Points Hash Table\n");
	 printf("#########################\n");
	 for(int i=0;i<DIM*LEVEL;i++)
	 {

		 printf(" %d ",*(design_table + i));

		 if((i+1)%DIM==0)
		 printf("\n");
	 }
	 printf("#########################\n");


}


