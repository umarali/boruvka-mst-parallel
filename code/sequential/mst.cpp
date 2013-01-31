/* mst.cpp
*
*  The algorithm in this file is a sequential implementation of 
*  Boruvka's algorithm to find Minimum Spanning Tree (MST) of a Graph.
*
*/

/* Includes required libraries */
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace std;

/* Preprocessor Directives */
#define MAX_WEIGHT 15
#define NUM_VERTICES 6
#define ZERO 0

/* Global variables */
int NUM_EDGES = ZERO
, 	NUM_EDGES_MST = ZERO;

/* struct(ure) Forest holds information about the edge 
*  
*  value => value of the node
*  rank => rank of the node - used for linking
*  parent => Forest_Node type node for maintaining tree hierarchy
*/

struct Forest_Node {
    int value
    ,	rank;
    struct Forest_Node* parent;
};

/* Creates a list - one per element */
Forest_Node* MakeSet(int value) {
	Forest_Node* node = (Forest_Node *) new Forest_Node;

	node->value = value;
	node->parent = NULL;
	node->rank = 0;
	
	return node;
}

/* Finds the root of the node */
Forest_Node* Find(Forest_Node* node) {
	Forest_Node* temp;
	Forest_Node* root = node;

	while (root->parent != NULL)
		root = root->parent;
	
	/* Updates the parent pointers */
	while (node->parent != NULL) {
		temp = node->parent;
		node->parent = root;
		node = temp;
	}
	
	return root;
}

/* Merges two nodes based on their rank */
void Union(Forest_Node* node1, Forest_Node* node2) {
	Forest_Node* root1 = Find(node1);
	Forest_Node* root2 = Find(node2);

	if (root1->rank > root2->rank) { 
		root2->parent = root1; 
	} else if (root2->rank > root1->rank) { 
		root1->parent = root2; 
	} else {
		root2->parent = root1; 
		root1->rank++; 
	}
}

/* struct(ure) Edge holds information about the edge 
*  
*  v1 => vertex 1
*  v2 => vertex 2
*  w => weight of the edge
*/
struct Edge {
    int v1
    ,	v2
    ,	w;
};

/* Creates an adjacency matrix */
int** createAdjacencyMatrix() {
    int** adjMatrix = new int*[NUM_VERTICES];

    for(int i = 0; i < NUM_VERTICES; i++)
        adjMatrix[i] = new int[NUM_VERTICES];

    /* Initializes all nodes to ZERO */
    for(int i = 0; i < NUM_VERTICES; i++)
        for(int j = 0; j < NUM_VERTICES; j++)
            adjMatrix[i][j] = 0;

    return adjMatrix;
}

/* Generates an adjacency matrix based random graph
*
*  count => total (valid) edge count
*/

int** generateRandomGraph(int &count) {
    int** adjMatrix = createAdjacencyMatrix();

    srand(time(NULL));
    for(int i = 0; i < NUM_VERTICES; i++) {
        for(int j = 0; j < NUM_VERTICES; j++) {
            if (i == j) continue;
            adjMatrix[i][j] = (rand() % MAX_WEIGHT) - 1;
            adjMatrix[j][i] = adjMatrix[i][j];
            
            if(adjMatrix[i][j] > 0)
                count++;
        }
    }

    return adjMatrix;
}

/* Displays edges list */
void displayEdgeList(Edge* E, int size) {
    cout << endl << "Edges [" << endl;
    for(int i = 0; i < size; i++) {
        cout << "\t{" << E[i].v1 << ", " << E[i].v2 << "}" << endl;
    }
    cout << "]" << endl;
}

/* Displays adjacency matrix */
void displayAdjacencyMatrix(int** adjMatrix) {
    cout << endl << "Adjacency Matrix [" << endl;
    for(int i = 0; i < NUM_VERTICES; i++) {
        cout << "\t{";
        for(int j = 0; j < NUM_VERTICES; j++) {
            cout << " " << adjMatrix[i][j] << " ";
        }
        cout << "}" << endl;
    }
    cout << "]" << endl;
}

int main(){
	// Generates a random Graph
	int **adjMatrix = generateRandomGraph(NUM_EDGES)
	,			  c = 0;

	NUM_EDGES--;

	Edge *ES = new Edge[NUM_EDGES]
	,	  *E = new Edge[NUM_EDGES];

	/* Extracts edges' info from Adjacency Matrix */
	for(int i = 0; i < NUM_VERTICES; i++) {
		for(int j = 0; j < NUM_VERTICES; j++) {
			if(i == j) continue;
			if(adjMatrix[i][j] > 0 && c < NUM_EDGES) {
				ES[c].v1 = i;
				ES[c].v2 = j;
				ES[c].w = adjMatrix[i][j];
				c++;
			}
		}
	}

	displayAdjacencyMatrix(adjMatrix);
    //displayEdgeList(ES, NUM_EDGES);

	int min;
	c = 0;

	/* Gets minimum edge for each vertex */
	for(int i = 0; i < NUM_VERTICES; i++) {
		min = 9999;
		int index = -1;
		for(int j = 0; j < NUM_EDGES; j++) {
			if(ES[j].v1 == i && ES[j].w <= min){
				min = ES[j].w;
				index = j;
			}

			if(ES[j].v1 > i) break;
		}

		if(index != -1){
			E[c].v1 = i;
			E[c].v2 = ES[index].v2;
			E[c].w = ES[index].w;
			c++;
		}
	}

	NUM_EDGES_MST = c;
	cout << endl;
	
	/* Displays all min edges */
    //displayEdgeList(E, NUM_EDGES_MST);

	/* Creates the forest */
	Forest_Node** forest = new Forest_Node*[NUM_VERTICES];

	for(int i = 0; i < NUM_VERTICES; i++)
		forest[i] = MakeSet(i);

	int e = 0
	, 	t = 0;

	Edge* mst = new Edge[NUM_EDGES];

	while(e < NUM_EDGES_MST) {
		int v1 = E[e].v1;
		int v2 = E[e].v2;

		if(Find(forest[v1]) != Find(forest[v2])) {
			mst[t] = E[e];
			Union(forest[v1], forest[v2]);
			t += 1;
		}
		e++;
	}

	/* MST Cost */
	int cost = 0;

	cout << endl << "MST [" << endl;
	for(int i = 0; i < t; i++){
        cout << "\t{" << mst[i].v1 << ", " << mst[i].v2 << "}" << endl;
        cost += mst[i].w;
    }
    cout << "]" << endl;
	cout << endl << "MST Cost :: " << cost << endl;

	return 0;
}
