#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "GlobalVars.h"

//============================================================================
//
// Name       : CCDTree.cpp
// Author      : C J Sharpe
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : This initilaises the grid point array that conatins all the valid grid
// 				 points within a Flexible Region.
//============================================================================

//============================================================================

int first_flag = 1;
int edge_start = 0;
struct GPNode *first_traversal_node;

void addEdgeNodes(struct GPNode *node ,int level, int address);

void BuildTree(struct GPNode *node, int level, int col_count, int col,int row, int *edgepattern)
{

//Get the "current" grid value in the Flexible Region from the Grid Point data structure
int current_grid_value = *(Gridpoint_Ptr + col  + ((level-1) * GridPoints));

//Local variables grid point indexed for the FR traversal values at a part of the sub tree

int current_col = col;
int current_row = row;
int current_level = level;
int current_col_count = col_count;

//printf("========================================================\n");
//printf("Tree Level %d: Col %d Row %d : Grid Value %d\n",current_level,current_col,current_row,current_grid_value);

	/*
	 * Add Leaf nodes to the CCD tree.
	 */
	if (current_level == DIM)
	{	//printf("........................................................\n");
		//printf("\tPreparing Leaf Nodes to Add to the CCD Tree\n");
		//printf("........................................................\n");

		struct GPNode *leaf_nodes;
		int leaf_grid_value = current_grid_value;
		int one_col_advance = 1;
		int node_count = 0;

		//Check the end of the Grid Points table isn't exceeded
		if(current_col + one_col_advance <= GridPointSize)
		{

	     int next_leaf_grid_value = *(Gridpoint_Ptr + current_col  + ((level-1) * GridPoints) + one_col_advance);

	     	 //printf("\tComparing Grid Values\n");

	     	 while (leaf_grid_value < next_leaf_grid_value && current_col + one_col_advance <= GridPointSize)
	 		 {
	 				//printf("\t %d. Current %d  Next %d\n",node_count, leaf_grid_value, next_leaf_grid_value);
	 				one_col_advance++;
	 				//Simply extract the next value and compare
	 				leaf_grid_value = next_leaf_grid_value;
	 				next_leaf_grid_value = *(Gridpoint_Ptr + current_col  + ((level-1) * GridPoints) + one_col_advance);
	 				node_count++;
	 		}


		 }
			int grid_value = current_grid_value;

	     	//Allocate memory based on the number of nodes we have counted; then get the leaf nodes
	     	//if(edge_start==0)
			//node->nodes  = (struct GPNode *) malloc(node_count * sizeof(struct GPNode));
	     	//else
			node->nodes  = (struct GPNode *) malloc(Edges * sizeof(struct GPNode));

	     	leaf_nodes = node->nodes;

	     	//printf("........................................................\n");
	     	//printf("\tAdding %d Leaf Node(s) to Add to the CCD Tree\n",node_count);
	     	//printf("........................................................\n");

	     	for(int i = 0; i <= node_count; i++)
	     	{
	     		 leaf_nodes->grid_ref = grid_value;

	     		 if(M_PARAM==INF)
	     		 {
	     			 leaf_nodes->array_index = GlobalTraverseCount;
	     		 	 GlobalTraverseCount  = GlobalTraverseCount + 1;
	     		 	 //printf("\t%d. Grid Value Leaf Node (%d), Contribution Index [%d]\n",i,leaf_nodes->grid_ref,leaf_nodes->array_index);
	     		}

	     		 //else
	     			 //printf("\t%d. Grid Value Leaf Node (%d), Contribution Index [Edge]\n",i,leaf_nodes->grid_ref);

	     		 grid_value++;
	     		 leaf_nodes++;

	     	}

	     	node->node_tot = node_count;//node_count;


	        //Deallocate excess nodes iff we have no edges in the shape
	        //if(edge_start==0)
	        	//leaf_nodes = NULL;
	     	//Turn off the edge flag;

	}
	/*
	 * Make a sub-tree of the CCD tree.
	*/
	else
	{
		//Move to the next column in the Grid Point, which corresponds to the next subtree
		int next_col = current_col;
		int next_row = current_row;
		int next_level = current_level;
		//int *child_edgepattern = (int *)malloc((LEVEL)* sizeof(int));
		//initPointer(0,child_edgepattern,LEVEL);
		struct GPNode *parent_node = node;
		struct GPNode *child_node;

		bool grid_check = false;

		//Increment the Row count
		int next_col_count = current_col_count;

		//3D


		next_level++;
		next_row++;

			/*
			If we are at the start of a column traversal, create a new set of child nodes for the CCD Tree.
			There is no knowing exactly how many nodes to allocate until a full column traversal,
			therefore the maximum possible number of nodes are pre-allocated. These are de-allocated
			if we have less nodes than than required.
	 	 	 * */
			if (current_col_count == 1)
			{
				//Init the child node, assign it the grid value indexed in the Grid Points table
				//printf("\tCreate new set of sibling nodes....\n");
				parent_node->nodes  = (struct GPNode *) malloc(Edges * sizeof(struct GPNode));
				child_node = parent_node->nodes;
				child_node->grid_ref = current_grid_value;
				child_node->nodes = NULL;
				child_node->node_tot = 0;

				//Put the grid traversal to before the shape edge
				if (current_grid_value>1)
				{
					grid_check = true;
					child_node->grid_ref = current_grid_value - 1;
				}


				NodeIndexCounter = NodeIndexCounter + 1;

			}

			//The nodes counter is incremented to give the correct address for the next node to send down the tree
			//and the parent node assigned the grid value currently indexed in the Grid Points table
			else
			{
				//printf("\tGet next sibling node....\n");
				parent_node->node_tot = parent_node->node_tot + 1;
				child_node = parent_node->nodes + parent_node->node_tot;
				child_node->nodes = NULL;
				child_node->node_tot = 0;
				child_node->grid_ref = current_grid_value;
				NodeIndexCounter = NodeIndexCounter + 1;

				//if(level==1 && current_col_count==)
			}
			//END if (current_col_count == 1) IF

			/*
			 *  Go to the next dimension of the point until the highest dimension is
			 *  reached note that the column count is reset to 1, as they a new set
			 *	of child nodes to the sub tree after created the leaf nodes of the tree.
			 */
			int reset_col_count = 1;

			//Save a reference to the first node in the traversal, we want to add nodes if we find edges
			if(first_flag && next_level== DIM)
			{
					first_flag = 0;
					first_traversal_node = parent_node;
					//printf("The First Node Saved at Level %d of %d Must be %d == ONE\n",next_level-1,next_level,first_traversal_node->grid_ref);
			}

			//Go Down the Tree

			BuildTree(child_node, next_level, reset_col_count, current_col, next_row, edgepattern);

			///Edge Compare As long as we are over the first point in the row
			if (current_col_count > 1)
			{
				int prev_address = parent_node->node_tot -1;
				int child_node_pts = child_node->node_tot;
				struct GPNode *child_node_prev = parent_node->nodes + prev_address;
				int prev_child_node_pts = child_node_prev->node_tot;


				if(child_node_pts!= prev_child_node_pts)// || *(edgepattern+current_grid_value-1)==current_grid_value)
				{
					//printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
					//printf("Edge Point Detected: Current Count %d Previous Count %d Edge Pattern %d\n",child_node_pts,prev_child_node_pts,*(edgepattern+current_grid_value-1));
					//printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");

					parent_node->node_tot = parent_node->node_tot + 1;
					struct GPNode *edge_node = parent_node->nodes + parent_node->node_tot;

					edge_node->grid_ref = current_grid_value;
					edge_node->nodes  = NULL;
					edge_node->node_tot = 0;
					NodeIndexCounter = NodeIndexCounter + 1;

					child_node->grid_ref = child_node->grid_ref -1;

					/*if(*(edgepattern+current_grid_value-1)==0)
					{
						*(edgepattern+current_grid_value-1) = current_grid_value;
						for(int e=0;e<LEVEL;e++)
						printf("edge %d, ",*(edgepattern+e));
					}*/

					if(next_level == DIM)
					{
						//Get the grid value of the edge point for reference,
						//and increment to the next value in the array
						*EdgeTracker_Ptr = current_grid_value;
						//printf("Edge Tracker Coordinate Value %d\n",*(EdgeTracker_Ptr));
						EdgeTracker_Ptr++;
						edge_start++;
					}
                    //printf("EDGE NODE %d %d\n",edge_node->grid_ref, child_node->grid_ref);
					BuildTree(edge_node, next_level, reset_col_count, current_col, next_row,edgepattern);
				}
				else if(*(edgepattern+current_grid_value-1)==current_grid_value && parent_node->node_tot >1)
				{
					parent_node->node_tot = parent_node->node_tot + 1;
										struct GPNode *edge_node = parent_node->nodes + parent_node->node_tot;

										edge_node->grid_ref = current_grid_value;
										edge_node->nodes  = NULL;
										edge_node->node_tot = 0;
										NodeIndexCounter = NodeIndexCounter + 1;

										child_node->grid_ref = child_node->grid_ref -1;

										if(*(edgepattern+current_grid_value-1)==0)
										{
											*(edgepattern+current_grid_value-1) = current_grid_value;
											//for(int e=0;e<LEVEL;e++)
											//printf("edge %d, ",*(edgepattern+e));
										}

										if(next_level == DIM)
										{
											//Get the grid value of the edge point for reference,
											//and increment to the next value in the array
											*EdgeTracker_Ptr = current_grid_value;
											//printf("Edge Tracker Coordinate Value %d\n",*(EdgeTracker_Ptr));
											EdgeTracker_Ptr++;
											edge_start++;
										}
					                    //printf("EDGE NODE %d %d\n",edge_node->grid_ref, child_node->grid_ref);
										BuildTree(edge_node, next_level, reset_col_count, current_col, next_row,edgepattern);

				}

			}//END (current_col_count > 1) IF

			next_col++;
			next_col_count++;


			/*
			 * Compare the current and next grid point values and iterate over the columns in
			 * in the Grid Point Matrix until the values are different.
			 * The CCD sub tree has been built at the already for these points.
			 */
			int old_grid_value = current_grid_value;
			int new_grid_value;//*(Gridpoint_Ptr + next_col  + ((level-1) * GridPoints));


			if(next_col==GridPoints)
			{
				new_grid_value = -1;
			}
			else
			{
				new_grid_value =  *(Gridpoint_Ptr + next_col  + ((level-1) * GridPoints));
			}


			if(grid_check)
			{
						next_col = current_col;
						old_grid_value = current_grid_value-1;
						new_grid_value = current_grid_value;
			}



			/*if((current_level != 1))
			{
				gp_above  = *(Gridpoint_Ptr + current_col  + ((level-2) * GridPoints));
				gp_right =  *(Gridpoint_Ptr + next_col   + ((level-2) * GridPoints));
				printf("?????????????????????????????????????????????????????????\n");
				printf("3D Check - Is Parent Above > Parent Right?\n");
				printf("\tLevel %d  Col Count %d: Checking Parent Nodes: %d: Above(%d) <-> %d: Right(%d)\n",current_level-2,next_col_count,col,gp_above,next_col,gp_right);
				printf("?????????????????????????????????????????????????????????\n");

				if(gp_above<gp_right)
					return;

			}

			if (old_grid_value >= new_grid_value && current_level != 1)
			{

				printf("3D Check - Is Child Here < Child Right? %d\n", next_col_count);
				printf("\tChecking Child Nodes: Current (%d) <->: Right(%d)\n",old_grid_value,new_grid_value);
				printf("?????????????????????????????????????????????????????????\n");


			}*/



			while (old_grid_value == new_grid_value)
			{

					old_grid_value = new_grid_value;
					next_col++;

					//printf("\tLevel %d: Traversing Col %d Col Count %d\n",current_level,next_col,next_col_count);

					if(next_col==GridPoints)
					{
					//printf("\t End of Grid Points \n");
					new_grid_value = -1;
					}
					else
					{
					new_grid_value =  *(Gridpoint_Ptr + next_col  + ((level-1) * GridPoints));
					//printf("\tGrid Points: Current (%d) <->: Right(%d)\n",old_grid_value,new_grid_value);
					}

			}

				if(next_col< GridPoints && new_grid_value > old_grid_value)
					{//printf("\tMove to Next Sibling Node.... \n");
					 BuildTree(parent_node, current_level, next_col_count, next_col, current_row,edgepattern);
					}


				//printf("PARENT NODE TOT %d CURRENT GRID VALUE %d DIM %d\n",parent_node->node_tot,current_col_count,current_level);
				if(current_level == DIM -1 && current_col_count == 1 && edge_start > 0)
				{
					//printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
					//printf("Insert Edge Nodes to Shape\n");
					int first_address = 0;
					addEdgeNodes(first_traversal_node,current_level,first_address);
					//Reset the edge array
					initPointer(99999,EdgeTracker_Ptr - edge_start,Edges);
					EdgeTracker_Ptr = EdgeTracker_Ptr - edge_start;
					edge_start = 0;
					first_flag = 1;
				}
				else if(current_level == DIM -1 && current_col_count == 1 && M_PARAM != INF)
				{
					//printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
					//printf("Insert Edge Nodes to Shape - Single Grid Point\n");
					int first_address = 0;
					addEdgeNodes(first_traversal_node,current_level,first_address);
					//Reset the edge array
					first_flag = 1;
				}


	}//END (Level < DIM) else

}

void addEdgeNodes(struct GPNode *node ,int level, int address)
{
	int addr = address;
    int child_nodes_total = node->node_tot;
	//Insert the leaf edge nodes into the current set of leaf nodes
	if(level == DIM)
	{
		//printf("########################################################\n");
		//printf("\tPreparing Leaf Edge Nodes to Add to the CCD Tree\n");
		//printf("########################################################\n");

		//Move the edge pointer to the first value in the array
		struct GPNode *leaf = node->nodes;
		int node_tot = node->node_tot;
		int *Edge_ptr = EdgeTracker_Ptr - edge_start;
        int leaf_inc = address;
        int grid_ref = leaf->grid_ref;

		//Loop until all the edge nodes are inserted into the correct place
		if(edge_start>0)
		{

			if(grid_ref>1 && grid_ref< *(Edge_ptr))
			{
				node_tot++;
				grid_ref = grid_ref-1;
			}

		for(int i = 0;;)
        //for(int i = 0;i<node_tot;i++)
		{

			///printf("\tCompare: Leaf %d Edge %d\n",grid_ref,*(Edge_ptr + i));



			//Increment the edge pointer if leaf grid value is greater than edge grid value
			if(grid_ref > *(Edge_ptr))
		 		Edge_ptr++;
			//Increment the leaf node pointer to the next leaf node if
		 	else if (grid_ref < *(Edge_ptr))
		 	{
		 		//printf("........................................................\n");
		 		//printf("\tAdding Leaf Nodes to Add to the CCD Tree - Iterate\n");
		 		//printf("........................................................\n");
		 		leaf = node->nodes + leaf_inc;
		 		leaf->grid_ref = grid_ref;
		 		leaf->array_index = GlobalTraverseCount;
		 		GlobalTraverseCount  = GlobalTraverseCount + 1;
		 		grid_ref++;

		 		//printf("\t%d. Grid Value Leaf Node (%d), Contribution Index [%d]\n",leaf_inc,leaf->grid_ref,leaf->array_index);

		 		leaf_inc++;
		 		i++;
		 	}
			//The values match....insert the edge node here and increment to the next node!
		 	else
		 	{
		 		leaf = node->nodes + leaf_inc;
		 		leaf->grid_ref = grid_ref - 1;
		 		leaf->array_index = GlobalTraverseCount;
		 		GlobalTraverseCount  = GlobalTraverseCount + 1;

		 		//printf("........................................................\n");
		 		//printf("\tEdge: Adding Leaf Nodes to Add to the CCD Tree - Insert and Overwrite\n");
		 		//printf("........................................................\n");
		 		//printf("\t%d. Grid Value Leaf Node (%d), Contribution Index [%d]\n",leaf_inc,leaf->grid_ref,leaf->array_index);
		 		leaf_inc++;
			    Edge_ptr++;
		 	}

			//Finish the insertion of edge nodes
		 	if(i==node_tot+1)
		    	 	 break;

			}

		}
        	else
		    {

        		int node_limit;

        		if(grid_ref>1)
        		{
        		  node_limit = node_tot+1;
        		  grid_ref = grid_ref-1;
        		}
        		else
        		 node_limit = node_tot;

        		for(int i = 0;i<=node_limit;i++)
        		  {
        			  leaf = node->nodes + leaf_inc;
        			  leaf->grid_ref = grid_ref;
        			  leaf->array_index = GlobalTraverseCount;
        			  GlobalTraverseCount  = GlobalTraverseCount + 1;

        			  //printf("........................................................\n");
        			  //printf("\tSmooth: Adding Leaf Nodes to Add to the CCD Tree - Insert and Overwrite\n");
        			  //printf("........................................................\n");
        			  //printf("\t%d. Grid Value Leaf Node (%d), Contribution Index [%d]\n",leaf_inc,leaf->grid_ref,leaf->array_index);
        			  leaf_inc++;
        			  grid_ref++;




        		  }



		    }

			//printf("Number of Nodes in Parent: Old %d New %d\n",node->node_tot,leaf_inc);
			node->node_tot = leaf_inc-1;

			//Deallocate memory for excess nodes

		}

		else
		{
				if(addr<=child_nodes_total)
				{
					//Get the next parent node and go down the tree
					struct GPNode *parent = node->nodes + addr;
					//printf("========================================================\n");
					//printf("Traverse Nodes: Node Tot %d Next Address %d  Grid Value %d\n",node->node_tot,addr,parent->grid_ref);
					//printf("========================================================\n");
					int start_addr = 0;
					addEdgeNodes(parent,DIM,start_addr);
					//Goto the next child
					addr++;
					addEdgeNodes(node,DIM-1,addr);
				}

		}


}

/**
 * ============================================================================
 * Description
 *
 * @param  variables
 * @return point_count - This is the length of the array
 *
 * =============================================================================
 **/
void TreeDebug(struct GPNode *parent, int level, int node_count)
{

	struct GPNode *child;
	int i;

	if (level == DIM)
		{


		 int node_tot = parent->node_tot;
		 int array_offset;
		 int contribution;

		 printf("Parent:(%d)-> Total Nodes {%d}\n",parent->grid_ref,node_tot);

		 for( i = 0; i <=node_tot ; i++)
		 {
			 child = parent->nodes +i;

			 array_offset = child->array_index;
			 contribution = *(Contributions_Ptr + array_offset);

			 printf("\t%d. Leaf Node (%d), Contribution Index [%d] -> Contribution {%d}\n",i+1,child->grid_ref,array_offset,contribution);


		 }


		}

	else
	{
		child = parent->nodes + node_count;
		printf("========================================================\n");
		printf("Traverse CCD Tree Level %d: Node Tot %d Next Address %d  Grid Value %d\n",level,parent->node_tot,node_count,parent->grid_ref);
		printf("========================================================\n");

		if(node_count <= parent->node_tot)
		{

			int next_level = level + 1;
			TreeDebug(child,next_level,0);
			node_count++;
			TreeDebug(parent,level,node_count);
		}

	}
}

/**
 * ============================================================================
 * Description
 *
 * @param  variables
 * @return point_count - This is the length of the array
 *
 * =============================================================================
 **/
void AddGridContribution(struct GPNode *parent, int level, int col, int row)
{

	struct GPNode *child;
	int child_grid_ref;
	int current_grid_value = *(Gridpoint_Ptr + col  + ((level-1) * GridPoints));
	int i;

	//printf("========================================================\n");
	//printf("Tree Level %d: Col %d Row %d : Grid Value %d\n",level,col,row,current_grid_value);

	if (level == DIM)
		{

		int array_offset;
		int original_value;
		int node_tot = parent->node_tot;

        for(i = 0; i <=node_tot ; i++)
		 {
			 child = parent->nodes +i;
			 child_grid_ref = child->grid_ref;

			 if(current_grid_value <= child_grid_ref)
			 {
				 array_offset = child->array_index;
				 original_value = *(Contributions_Ptr + array_offset);
				 *(Contributions_Ptr + array_offset)  = original_value + 1;
				 //printf("Value %d %d\n",*(Contributions_Ptr + array_offset),array_offset);
			 }

		 }
        //Contributions_Ptr = start_address;
		}

	else
	{

		int next_level = level + 1;
		int next_col;
		int next_row;

		for(int j = 0; j <= parent->node_tot; j++)
		{
			child = parent->nodes + j;
			child_grid_ref = child->grid_ref;

			if(current_grid_value <= child_grid_ref)
			{
				next_row = row + 1;
				AddGridContribution(child,next_level, col, next_row);

			}
		}

		next_col = col + 1;

		if(next_col <= GridPointSize && level == 1)
		{
			AddGridContribution(parent,level, next_col, row);
		}
	}

}

void EmptyRegionCalc_Debug(void)
{

	double region_conv;
	double region_sum = 0;

	for(int i = 0; i <GlobalTraverseCount; i ++)
	{
		region_conv = (double)*(Contributions_Ptr + i)/(double)GridPoints;
		region_sum = region_sum  + pow(region_conv,2);
		//printf("\t %2.4f",region_conv);

	}
     //MUST CHANGE FOR 3D!!!!
	 //region_sum = region_sum * 1/(double)pow(2,d) * 2;
	 region_sum = region_sum * 1/(double)GridPoints;
	 EmptyRegion_Discc = region_sum;

}


