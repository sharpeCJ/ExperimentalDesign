#ifndef GLOBALVARS_H
#define GLOBALVARS_H
 //*GLOBALVARS_H*
#endif

#define INF 5555.0
#define LEVEL 3//159;//75;//43;//35;//31;//
#define DIM 5
#define M_PARAM 0.3//2;//0.3;//0.5;//1;//
#define LEFT 0
#define RIGHT 1
#define DESIGN_POINTS 1
#define ROWS 2
#define COLS DESIGN_POINTS
//Global Variables
extern int GridPointSize;
extern int GridPoints;
extern int GlobalTraverseCount;
extern int NodeIndexCounter;
extern int Edges;


extern double EmptyRegion_Discc;

extern double add;
extern double get;
extern double init;
extern double calc;
extern int Calcc_inc;
extern int *Gridpoint_Ptr;
extern int DesignPointsSize;
extern int *Contributions_Ptr;
extern int *EdgeTracker_Ptr;
extern int *DesignPoints_Ptr;
extern int DesignPoints;

// This is the structure of an individual node in the tree
struct GPNode
{
	int grid_ref;          //The value of the
	struct GPNode *nodes;  //A set of GPNodes (A sub tree)
	int array_index;       //for leaf nodes, its value in array index.
	int node_tot;		   //The node count at that level
};

struct TraverseCard
{
 int LHS_Counter;
 int RHS_Counter;
 int START;
 int END;
};

struct JobCard
{
 double Multiplier;
 bool card_init_l;
 bool card_init_r;
 int dt[COLS][ROWS];
 int stop_l;
 int stop_r;
};

struct AllPoints
{
	int dt[COLS][DIM];
};

typedef struct GPNode CCDTree;


//Re-initialise
void initPointer(int init_value, int *ptr,int size);
//Function signatures for InitGridPoints.cpp
int GetPointsLength(int d,int q, double m);
void GenerateGridPoints(int *gp_ptr, int d,int q, double m, int lg);
void GenerateEdgeProfile(int *gp_ptr, int d,int q, int lg, int level,int *edgepattern);

//Function signatures for CCDTree.cpp
void BuildTree(struct GPNode *node, int level, int col_count, int col,int row,int *edgepattern);
void TreeDebug(struct GPNode *node, int level, int node_count);
void AddGridContribution(struct GPNode *parent, int level, int col, int row);
void EmptyRegionCalc_Debug(void);

//Function signatures for DiscrepancyCalc.cpp
double CalculateCCD(struct GPNode *root);
extern inline double divide(int a, int b) {return (double)a / b;}
extern inline double adding(double a, double b, double c){return a + b + c;}
extern inline double subtract(double a, double b){return a - b;}
//Function Signature for points tree
void BuildPointsTree(struct GPNode *node, int level, int col_count, int col,int row);
void PointsTreeDebug(struct GPNode *parent, int level, int node_count);
