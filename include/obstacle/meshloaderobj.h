#ifndef MAGIC_ENGINE_NAVIGATION_OBSTACLE_MESHLOADEROBJ_BUILDER_H_H
#define MAGIC_ENGINE_NAVIGATION_OBSTACLE_MESHLOADEROBJ_BUILDER_H_H

#include <string>

namespace Obstacle {
    
    class MeshLoaderObj {
    public:
        MeshLoaderObj();
        ~MeshLoaderObj();
        
        bool Load(const std::string& fileName);

        const float* GetVerts() const { return m_verts; }
        const float* GetNormals() const { return m_normals; }
        const int* GetTris() const { return m_tris; }
        int GetVertCount() const { return m_vertCount; }
        int GetTriCount() const { return m_triCount; }
        const std::string& GetFileName() const { return m_filename; }

    private:
        // Explicitly disabled copy constructor and copy assignment operator.
        MeshLoaderObj(const MeshLoaderObj&);
        MeshLoaderObj& operator=(const MeshLoaderObj&);
        
        void addVertex(float x, float y, float z, int& cap);
        void addTriangle(int a, int b, int c, int& cap);
    private:
        std::string m_filename;
        float       m_scale;	
        float*      m_verts;
        int*        m_tris;
        float*      m_normals;
        int         m_vertCount;
        int         m_triCount;
    };
}


#endif 