/* pmst.cpp
*
*  The algorithm in this file is a (partially) parallel implementation of 
*  Boruvka's algorithm to find Minimum Spanning Tree (MST) of a Graph.
*
*/

/* Including required libraries */
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <CL/cl.h>

using namespace std;

/* Preprocessor Directives */
#define MAX_WEIGHT 50
#define NUM_VERTICES 100
#define ZERO 0

/* Global variables */
int NUM_EDGES = ZERO
,   NUM_EDGES_MST = ZERO;

/* struct(ure) Forest holds information about the edge 
*  
*  value => value of the node
*  rank => rank of the node - used for linking
*  parent => Forest_Node type node for maintaining tree hierarchy
*/

struct Forest_Node {
    int value
    ,   rank;
    struct Forest_Node* parent;
};

/* Creates a list - one per element */
Forest_Node* MakeSet(int value) {
    Forest_Node* node = new Forest_Node;

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
    int v1, v2, w;
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

/*  Creates an OpenCL context on the available platform using
*  either a GPU or CPU depending on what is available
*/
cl_context CreateContext() {
    /* Some variable's declarations */
    cl_int errNum;
    cl_uint numPlatforms;
    cl_platform_id firstPlatformId;
    cl_context context = NULL;

    /*  Selects an (available) OpenCL platform to run on */
    errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
    if (errNum != CL_SUCCESS || numPlatforms <= ZERO) {
        cerr << "Failed to find any OpenCL platforms." << endl;
        return NULL;
    }

    /* Sets context properties */
    cl_context_properties contextProperties[] = {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)firstPlatformId,
        0
    };
    
    /* Creates an OpenCL context on the platform */
    context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU,
                                        NULL, NULL, &errNum);
    if (errNum != CL_SUCCESS) {
        cout << "Could not create GPU context, trying CPU..." << endl;
        context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU,
                                            NULL, NULL, &errNum);
        if (errNum != CL_SUCCESS) {
            cerr << "Failed to create an OpenCL GPU or CPU context." << endl;
            return NULL;
        }
    }

    return context;
}

/* Creates a command queue on the device available on the context */
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device) {
    cl_int errNum;
    cl_device_id *devices;
    cl_command_queue commandQueue = NULL;
    size_t deviceBufferSize = -1;

    /* Gets the size of the devices buffer */
    errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, ZERO, NULL, &deviceBufferSize);
    if (errNum != CL_SUCCESS) {
        cerr << "Failed call to clGetContextInfo(...,GL_CONTEXT_DEVICES,...)";
        return NULL;
    }

    if (deviceBufferSize <= ZERO) {
        cerr << "No devices available.";
        return NULL;
    }

    /* Allocates memory for the devices buffer */
    devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
    errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
    if (errNum != CL_SUCCESS) {
        delete [] devices;
        cerr << "Failed to get device IDs";
        return NULL;
    }

    /* Sets command queue properties */
    cl_command_queue_properties cmdQProperties = {
        CL_QUEUE_PROFILING_ENABLE
    };

    /* Chooses the first available device */
    commandQueue = clCreateCommandQueue(context, devices[0], cmdQProperties, NULL);
    if (commandQueue == NULL) {
        delete [] devices;
        cerr << "Failed to create commandQueue for device 0";
        return NULL;
    }

    *device = devices[0];
    delete [] devices;
    return commandQueue;
}

/* Create an OpenCL program from the Kernel source file */
cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName) {
    cl_int errNum;
    cl_program program;

    ifstream kernelFile(fileName, ios::in);
    if (!kernelFile.is_open()) {
        cerr << "Failed to open file for reading: " << fileName << endl;
        return NULL;
    }

    ostringstream oss;
    oss << kernelFile.rdbuf();

    string srcStdStr = oss.str();
    const char *srcStr = srcStdStr.c_str();

    program = clCreateProgramWithSource(context, 1, (const char**)&srcStr, NULL, NULL);
    if (program == NULL) {
        cerr << "Failed to create CL program from source." << endl;
        return NULL;
    }

    errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (errNum != CL_SUCCESS) {
        /* Gets program's errors */
        char buildLog[16384];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                                sizeof(buildLog), buildLog, NULL);

        cerr << "Error in Kernel: " << endl;
        cerr << buildLog;
        clReleaseProgram(program);
        return NULL;
    }

    return program;
}

/* Create memory objects to be used as arguments to the Kernel */
bool CreateMemObjects(cl_context context, cl_mem memObjects[3], Edge* EI) {
    memObjects[0] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    sizeof(Edge) * NUM_EDGES, EI, NULL);
    memObjects[1] = clCreateBuffer(context, CL_MEM_READ_WRITE,
                                    sizeof(Edge) * NUM_EDGES, NULL, NULL);
    memObjects[2] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    sizeof(int) * 1, &NUM_EDGES, NULL);

    if (memObjects[0] == NULL || memObjects[1] == NULL || memObjects[2] == NULL) {
        cerr << "Error creating memory objects." << endl;
        return false;
    }

    return true;
}

/* Cleans up all (created) OpenCL resources */
void Cleanup(cl_context context, cl_command_queue commandQueue,
             cl_program program, cl_kernel kernel, cl_mem memObjects[3]) {
    for (int i = 0; i < 3; i++) {
        if (memObjects[i] != 0)
            clReleaseMemObject(memObjects[i]);
    }
    if (commandQueue != 0)
        clReleaseCommandQueue(commandQueue);

    if (kernel != 0)
        clReleaseKernel(kernel);

    if (program != 0)
        clReleaseProgram(program);

    if (context != 0)
        clReleaseContext(context);
}

/* Main function */
int main(int argc, char** argv) {
    /* Some variables' declarations and initializations */
    cl_context context = 0;
    cl_command_queue commandQueue = 0;
    cl_program program = 0;
    cl_device_id device = 0;
    cl_kernel kernel = 0;
    cl_mem memObjects[3] = { 0, 0, 0 };
    cl_int errNum;                                                                                                                                                                                                                                                                              

    /* Creates an OpenCL context on first available platform */
    context = CreateContext();
    if (context == NULL) {
        cerr << "Failed to create OpenCL context." << endl;
        return 1;
    }

    /* Creates a command queue on the device available on the context */
    commandQueue = CreateCommandQueue(context, &device);
    if (commandQueue == NULL) {
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    // Create OpenCL program from 0.cl kernel source
    program = CreateProgram(context, device, "_kernel.cl");                                                                                                              
    if (program == NULL) {
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    /* Create OpenCL Kernel */
    kernel = clCreateKernel(program, "findMinEdge", NULL);
    if (kernel == NULL) {
        cerr << "Failed to create Kernel" << endl;
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    /* Generates a random Graph */
    int** adjMatrix = generateRandomGraph(NUM_EDGES);
    int c = 0;
    NUM_EDGES--;

    Edge* ES = new Edge[NUM_EDGES];
    Edge* E = new Edge[NUM_EDGES];

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

    if (NUM_VERTICES <= 30)
        displayAdjacencyMatrix(adjMatrix);
    //displayEdgeList(ES, NUM_EDGES);

    if (!CreateMemObjects(context, memObjects, ES)) {
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    /* Set the Kernel arguments */
    errNum = clSetKernelArg(kernel, 0, sizeof(cl_mem), &memObjects[0]);
    errNum |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &memObjects[1]);
    errNum |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &memObjects[2]);
    if (errNum != CL_SUCCESS) {
        cerr << "Error setting Kernel arguments." << endl;
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    /* Ensures to have executed all enqueued tasks */
    clFinish(commandQueue);

    size_t globalWorkSize[1] = { NUM_VERTICES };
    size_t localWorkSize[1] = { 1 };
    cl_event event;

    /* Queues the Kernel up for execution across the array */
    errNum = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL,
                                    globalWorkSize, localWorkSize,
                                    0, NULL, &event);

    /* Ensures Kernel execution is finished */
    clWaitForEvents(1 , &event);

    if (errNum != CL_SUCCESS) {
        cerr << "Error queuing Kernel for execution." << endl;
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    /* Reads the output buffer back to the Host */
    errNum = clEnqueueReadBuffer(commandQueue, memObjects[1], CL_TRUE,
                                 0, NUM_EDGES * sizeof(Edge), E,
                                 0, NULL, NULL);
    if (errNum != CL_SUCCESS) {
        cerr << "Error reading result buffer." << endl;
        Cleanup(context, commandQueue, program, kernel, memObjects);
        return 1;
    }

    /* Displays all min edges */
    //displayEdgeList(E, NUM_VERTICES);

    /* Creates the forest */
    Forest_Node** forest = new Forest_Node*[NUM_VERTICES];

    for(int i = 0; i < NUM_VERTICES; i++)
        forest[i] = MakeSet(i);

    int e = 0
    ,   t = 0;

    Edge* mst = new Edge[NUM_EDGES];
    NUM_EDGES_MST = NUM_VERTICES;

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

    cout << endl << "MST [" << endl << "\t";
    for(int i = 0; i < t; i++){
        if(i % 10 == 0 && i != 0) cout << endl << "\t";

        cout << "{" << mst[i].v1 << ", " << mst[i].v2 << "}, ";
        cost += mst[i].w;
    }
    cout << endl << "]" << endl;
    cout << endl << "MST Cost :: " << cost;

                                            /* Gets the profiling data */
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        cl_ulong time_start, time_end;
        double total_time;

        clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
        clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
        total_time = time_end - time_start;
        printf("\nExecution time in milliseconds = %0.3f ms\n\n", (total_time/1000000.0));
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    /* Releases the allocated resources */
    Cleanup(context, commandQueue, program, kernel, memObjects);

    return 0;
}
