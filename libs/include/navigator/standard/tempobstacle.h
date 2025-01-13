#ifndef MAGIC_ENGINE_NAVIGATION_STANDARD_TEMPOBSTACLELOADER_H_H
#define MAGIC_ENGINE_NAVIGATION_STANDARD_TEMPOBSTACLELOADER_H_H

#include "iloader.h"
#include "tilecache.h"
#include "tilecachebuilder.h"
#include "fastlz.h"
#include "dcommon.h"

class TempObstacleLoader : public ILoader{
        static const int TILECACHESET_MAGIC = 'T' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'TSET';
        static const int TILECACHESET_VERSION = 1;

        struct TileCacheSetHeader {
            int magic;
            int version;
            int numTiles;
            Detour::NavMeshParams     meshParams;
            Obstacle::TileCacheParams cacheParams;
        };

        struct TileCacheTileHeader {
            Obstacle::CompressedTileRef tileRef;
            int dataSize;
        };

        enum SamplePolyAreas {
            SAMPLE_POLYAREA_GROUND,
            SAMPLE_POLYAREA_WATER,
            SAMPLE_POLYAREA_ROAD,
            SAMPLE_POLYAREA_DOOR,
            SAMPLE_POLYAREA_GRASS,
            SAMPLE_POLYAREA_JUMP,
        };

        struct FastLZCompressor : public Obstacle::TileCacheCompressor {
            virtual int maxCompressedSize(const int bufferSize) {
                return (int)(bufferSize* 1.05f);
            }

            virtual Detour::Status compress(const unsigned char* buffer, const int bufferSize,
                unsigned char* compressed, const int /*maxCompressedSize*/, int* compressedSize) {
                *compressedSize = fastlz_compress((const void *const)buffer, bufferSize, compressed);
                return Detour::SUCCESS;
            }

            virtual Detour::Status decompress(const unsigned char* compressed, const int compressedSize,
                unsigned char* buffer, const int maxBufferSize, int* bufferSize) {
                *bufferSize = fastlz_decompress(compressed, compressedSize, buffer, maxBufferSize);
                return *bufferSize < 0 ? Detour::FAILURE : Detour::SUCCESS;
            }
        };


        struct LinearAllocator : public Obstacle::TileCacheAlloc {
            unsigned char* buffer;
            size_t capacity;
            size_t top;
            size_t high;

            LinearAllocator(const size_t cap) : buffer(0), capacity(0), top(0), high(0) {
                resize(cap);
            }

            ~LinearAllocator() {
                Detour::Free(buffer);
            }

            void resize(const size_t cap) {
                if (buffer) Detour::Free(buffer);
                buffer = (unsigned char*)Detour::Alloc(cap, Detour::ALLOC_PERM);
                capacity = cap;
            }

            virtual void reset() {
                high = Detour::Max(high, top);
                top = 0;
            }

            virtual void* alloc(const size_t size) {
                if (!buffer)
                    return 0;
                if (top + size > capacity)
                    return 0;
                unsigned char* mem = &buffer[top];
                top += size;
                return mem;
            }

            virtual void free(void* /*ptr*/) {
                // Empty
            }
        };

        //InputGeom*
        struct MeshProcess : public Obstacle::TileCacheMeshProcess { void* m_geom;

            inline MeshProcess() : m_geom(0){}

            inline void init(void* geom) {
                m_geom = geom;
            }

	        virtual void process(struct Detour::NavMeshCreateParams* params, 
                                 unsigned char* polyAreas, unsigned short* polyFlags) {
                // Update poly flags from areas.
            for (int i = 0; i < params->polyCount; ++i) {
                    if (polyAreas[i] == Obstacle::TILECACHE_WALKABLE_AREA)
                        polyAreas[i] =  SAMPLE_POLYAREA_GROUND;

                    if (polyAreas[i] == SAMPLE_POLYAREA_GROUND ||
                        polyAreas[i] == SAMPLE_POLYAREA_GRASS ||
                        polyAreas[i] == SAMPLE_POLYAREA_ROAD) {
                        polyFlags[i] = SAMPLE_POLYFLAGS_WALK;
                    } else if (polyAreas[i] == SAMPLE_POLYAREA_WATER) {
                        polyFlags[i] = SAMPLE_POLYFLAGS_SWIM;
                    } else if (polyAreas[i] == SAMPLE_POLYAREA_DOOR) {
                        polyFlags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
                    }
                }

                // Pass in off-mesh connections.
                if (m_geom) {
                    //params->offMeshConVerts = m_geom->getOffMeshConnectionVerts();
                    //params->offMeshConRad = m_geom->getOffMeshConnectionRads();
                    //params->offMeshConDir = m_geom->getOffMeshConnectionDirs();
                    //params->offMeshConAreas = m_geom->getOffMeshConnectionAreas();
                    //params->offMeshConFlags = m_geom->getOffMeshConnectionFlags();
                    //params->offMeshConUserID = m_geom->getOffMeshConnectionId();
                    //params->offMeshConCount = m_geom->getOffMeshConnectionCount();
                }
            }
        };
       
    public:
        TempObstacleLoader();
        ~TempObstacleLoader();

        bool LoadNavMesh(const char* filePath, const int maxNodes=2048);
    protected:
        Detour::NavMesh* loadMeshFile(const char* filePath);
        void addObstacle(float *p);
        void removeObstacle(int idx);
        void removeObstacle();
        void update(const float dt, bool* upToDate = NULL);
    protected:
        struct LinearAllocator*     m_talloc;
        struct FastLZCompressor*    m_tcomp;
        struct MeshProcess*         m_tmproc;
        Obstacle::TileCache*        m_ptrTileCache;
};

#endif