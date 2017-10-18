/*
 * PointsTree.cpp
 *
 *  Created on: 1 Feb 2012
 *      Author: csharpe
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "GlobalVars.h"

void BuildPointsTree(struct GPNode *node, int level, int col_count, int col,int row)
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

	     	printf("........................................................\n");
	     	printf("\tAdding %d Leaf Node(s) to Add to the CCD Tree\n",node_count);
	     	printf("........................................................\n");

	     	for(int i = 0; i <= node_count; i++)
	     	{
	     		 leaf_nodes->grid_ref = grid_value;
	     		 leaf_nodes->node_tot = 0;
	     		 printf("\t%d. Grid Value Leaf Node (%d), Contribution Index [%d]\n",i,leaf_nodes->grid_ref,leaf_nodes->array_index);

	     		 grid_value++;
	     		 leaf_nodes++;

	     	}

	     	node->node_tot = node_count;//node_count;

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
		struct GPNode *parent_node = node;
		struct GPNode *child_node;

		int next_col_count = current_col_count;
		next_level++;
		next_row++;

		if (current_col_count == 1)
		{
		//Init the child node, assign it the grid value indexed in the Grid Points table
		printf("\tCreate new set of sibling nodes....\n");
		parent_node->nodes  = (struct GPNode *) malloc(LEVEL * sizeof(struct GPNode));
		child_node = parent_node->nodes;
		child_node->grid_ref = current_grid_value;
		child_node->nodes = NULL;
		child_node->node_tot = 0;
		}
		else
		{
						printf("\tGet next sibling node....\n");
						parent_node->node_tot = parent_node->node_tot + 1;
						child_node = parent_node->nodes + parent_node->node_tot;
						child_node->nodes = NULL;
						child_node->node_tot = 0;
						child_node->grid_ref = current_grid_value;

		}

		int reset_col_count = 1;
		BuildPointsTree(child_node, next_level, reset_col_count, current_col, next_row);

		next_col++;
		next_col_count++;

		int old_grid_value = current_grid_value;
		int new_grid_value;

		if(next_col==GridPoints)
		{
						new_grid_value = -1;
					}
					else
					{
						new_grid_value =  *(Gridpoint_Ptr + next_col  + ((level-1) * GridPoints));
					}


		while (old_grid_value == new_grid_value)
					{

							old_grid_value = new_grid_value;
							next_col++;

							printf("\tLevel %d: Traversing Col %d Col Count %d\n",current_level,next_col,next_col_count);

							if(next_col==GridPoints)
							{
							printf("\t End of Grid Points \n");
							new_grid_value = -1;
							}
							else
							{
							new_grid_value =  *(Gridpoint_Ptr + next_col  + ((level-1) * GridPoints));
							printf("\tGrid Points: Current (%d) <->: Right(%d)\n",old_grid_value,new_grid_value);
							}

					}

						if(next_col< GridPoints && new_grid_value > old_grid_value)
							{printf("\tMove to Next Sibling Node.... \n");
							 BuildPointsTree(parent_node, current_level, next_col_count, next_col, current_row);
							}





	}//END (Level < DIM) else

}

void PointsTreeDebug(struct GPNode *parent, int level, int node_count)
{

	struct GPNode *child;
		int i;

		if (level == DIM)
			{


			 int node_tot = parent->node_tot;



			 printf("Parent:(%d)-> Total Nodes {%d}\n",parent->grid_ref,node_tot);

			 for( i = 0; i <=node_tot ; i++)
			 {
				 child = parent->nodes +i;
				 printf("\t%d. Leaf Node (%d)\n",i+1,child->grid_ref);

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
				PointsTreeDebug(child,next_level,0);
				node_count++;
				PointsTreeDebug(parent,level,node_count);
			}

		}
}


