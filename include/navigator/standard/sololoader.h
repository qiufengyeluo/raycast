#ifndef MAGIC_ENGINE_NAVIGATION_STANDARD_SOLOLOADER_H_H
#define MAGIC_ENGINE_NAVIGATION_STANDARD_SOLOLOADER_H_H


#include "iloader.h"


class SoloLoader : public ILoader{
        static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
        static const int NAVMESHSET_VERSION = 1;

        struct NavMeshSetHeader {
            int magic;
            int version;
            int numTiles;
            Detour::NavMeshParams params;
        };

        struct NavMeshTileHeader {
	        TileRef tileRef;
	        int     dataSize;
        };
    public:
        SoloLoader();
        ~SoloLoader();

        bool LoadNavMesh(const char* filePath, const int maxNodes=2048);
    protected:
        Detour::NavMesh* loadMeshFile(const char* filePath);
        void addObstacle(float *p);
        void removeObstacle(int idx);
        void removeObstacle();
        void update(const float dt, bool* upToDate = NULL);
};


#endif 