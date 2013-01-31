struct Edge {
    int v1, v2, w;  // vertex 1, vertex 2, weight makes a complete connection
};

__kernel void findMinEdge(__global struct Edge *EI, __global struct Edge *EO, __global int* numEdges)
{
    int gid = get_global_id(0);
    int c = gid, min = 9999, index = -1;

    for(int j = 0; j < numEdges; j++) {
        if(EI[j].v1 == gid && EI[j].w <= min){
            min = EI[j].w;
            index = j;
        }

        if(EI[j].v1 > gid) break;
    }

    if(index != -1){
        EO[c].v1 = gid;
        EO[c].v2 = EI[index].v2;
        EO[c].w = EI[index].w;
        c++;
    }
}