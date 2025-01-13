#ifndef MAGIC_ENGINE_NAVIGATION_OBSTACLE_CHUNKYTRIMESH_H_H
#define MAGIC_ENGINE_NAVIGATION_OBSTACLE_CHUNKYTRIMESH_H_H

namespace Obstacle {
    
    struct ChunkyTriMeshNode {
        float bmin[2];
        float bmax[2];
        int i;
        int n;
    };

    struct ChunkyTriMesh {
        inline ChunkyTriMesh() : nodes(0), nnodes(0), tris(0), ntris(0), maxTrisPerChunk(0) {};
        inline ~ChunkyTriMesh() { delete [] nodes; delete [] tris; }

        ChunkyTriMeshNode* nodes;
        int nnodes;
        int* tris;
        int ntris;
        int maxTrisPerChunk;

    private:
        // Explicitly disabled copy constructor and copy assignment operator.
        ChunkyTriMesh(const ChunkyTriMesh&);
        ChunkyTriMesh& operator=(const ChunkyTriMesh&);
    };

    /// Creates partitioned triangle mesh (AABB tree),
    /// where each node contains at max trisPerChunk triangles.
    bool CreateChunkyTriMesh(const float* verts,
                             const int*   tris, 
                             int          ntris,
                             int          trisPerChunk, ChunkyTriMesh* cm);

    /// Returns the chunk indices which overlap the input rectable.
    int GetChunksOverlappingRect(const ChunkyTriMesh* cm, float bmin[2], float bmax[2], int* ids, const int maxIds);

    /// Returns the chunk indices which overlap the input segment.
    int GetChunksOverlappingSegment(const ChunkyTriMesh* cm, float p[2], float q[2], int* ids, const int maxIds);

}


#endif