#ifndef MAGIC_ENGINE_NAVIGATION_ILOADER_H_H
#define MAGIC_ENGINE_NAVIGATION_ILOADER_H_H

#include <stdio.h>
#include <string.h>
#include "navmesh.h"
#include "nav.h"

class ILoader {
        friend class Navigation;
    public:
          enum SamplePolyFlags {
            SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk(行走) (地, 草, 路)
            SAMPLE_POLYFLAGS_SWIM = 0x02,		// Ability to swim(游水) (水).
            SAMPLE_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors(门).
            SAMPLE_POLYFLAGS_JUMP = 0x08,		// Ability to jump(跳).
            SAMPLE_POLYFLAGS_DISABLED = 0x10,   // Disabled polygon(禁用多边形).
            SAMPLE_POLYFLAGS_ALL      = 0xFFFF	// 全部开启.
        };
    public:
        ILoader() : m_ptrNavMesh(NULL), m_nmaxNodes(0) {};
        virtual ~ILoader() {if (m_ptrNavMesh != NULL) { Detour::FreeNavMesh(m_ptrNavMesh);m_ptrNavMesh = NULL;}};

        virtual bool LoadNavMesh(const char* filePath, const int maxNodes=2048) = 0;
    protected:
        virtual Detour::NavMesh* loadMeshFile(const char* filePath) = 0;
        virtual void addObstacle(float *p)    = 0;
        virtual void removeObstacle(int idx)  = 0;
        virtual void removeObstacle()         = 0;
        virtual void update(const float dt, bool* upToDate = NULL) = 0;
    protected:
        Detour::NavMesh         *m_ptrNavMesh;
        int                      m_nmaxNodes;
};

#endif 
