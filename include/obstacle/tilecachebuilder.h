#ifndef MAGIC_ENGINE_NAVIGATION_OBSTACLE_TILECACHE_BUILDER_H_H
#define MAGIC_ENGINE_NAVIGATION_OBSTACLE_TILECACHE_BUILDER_H_H

#include "detour/dstatus.h"
#include "detour/dalloc.h"

namespace Obstacle {
    static const int TILECACHE_MAGIC = 'D'<<24 | 'T'<<16 | 'L'<<8 | 'R'; ///< 'DTLR';
    static const int TILECACHE_VERSION = 1;

    static const unsigned char  TILECACHE_NULL_AREA = 0;
    static const unsigned char  TILECACHE_WALKABLE_AREA = 63;
    static const unsigned short TILECACHE_NULL_IDX = 0xFFFF;

    struct TileCacheLayerHeader {
        int magic;								///< Data magic
        int version;							///< Data version
        int tx,ty,tlayer;
        float bmin[3], bmax[3];
        unsigned short hmin, hmax;				///< Height min/max range
        unsigned char width, height;			///< Dimension of the layer.
        unsigned char minx, maxx, miny, maxy;	///< Usable sub-region.
    };

    struct TileCacheLayer {
        TileCacheLayerHeader* header;
        unsigned char regCount;					///< Region count.
        unsigned char* heights;
        unsigned char* areas;
        unsigned char* cons;
        unsigned char* regs;
    };

    struct TileCacheContour {
        int nverts;
        unsigned char* verts;
        unsigned char reg;
        unsigned char area;
    };

    struct TileCacheContourSet {
        int nconts;
        TileCacheContour* conts;
    };

    struct TileCachePolyMesh {
        int nvp;
        int nverts;				///< Number of vertices.
        int npolys;				///< Number of polygons.
        unsigned short* verts;	///< Vertices of the mesh, 3 elements per vertex.
        unsigned short* polys;	///< Polygons of the mesh, nvp*2 elements per polygon.
        unsigned short* flags;	///< Per polygon flags.
        unsigned char* areas;	///< Area ID of polygons.
    };


    struct TileCacheAlloc {
        virtual ~TileCacheAlloc() {}

        virtual void Reset() {}
        
        virtual void* Alloc(const size_t size) {
            return Detour::Alloc(size, Detour::ALLOC_TEMP);
        }
        
        virtual void Free(void* ptr) {
            Detour::Free(ptr);
        }
    };

    struct TileCacheCompressor {
        virtual ~TileCacheCompressor() { }

        virtual int MaxCompressedSize(const int bufferSize) = 0;
        virtual Detour::Status Compress(const unsigned char* buffer, const int bufferSize,
                                unsigned char* compressed, const int maxCompressedSize, int* compressedSize) = 0;
        virtual Detour::Status Decompress(const unsigned char* compressed, const int compressedSize,
                                    unsigned char* buffer, const int maxBufferSize, int* bufferSize) = 0;
    };


    Detour::Status BuildTileCacheLayer(TileCacheCompressor* comp,
                                 TileCacheLayerHeader* header,
                                 const unsigned char* heights,
                                 const unsigned char* areas,
                                 const unsigned char* cons,
                                 unsigned char** outData, int* outDataSize);

    void FreeTileCacheLayer(TileCacheAlloc* alloc, TileCacheLayer* layer);

    Detour::Status DecompressTileCacheLayer(TileCacheAlloc* alloc, TileCacheCompressor* comp,
                                        unsigned char* compressed, const int compressedSize,
                                        TileCacheLayer** layerOut);

    TileCacheContourSet* AllocTileCacheContourSet(TileCacheAlloc* alloc);
    void FreeTileCacheContourSet(TileCacheAlloc* alloc, TileCacheContourSet* cset);

    TileCachePolyMesh* AllocTileCachePolyMesh(TileCacheAlloc* alloc);
    void FreeTileCachePolyMesh(TileCacheAlloc* alloc, TileCachePolyMesh* lmesh);

    Detour::Status MarkCylinderArea(TileCacheLayer& layer, const float* orig, const float cs, const float ch,
                                const float* pos, const float radius, const float height, const unsigned char areaId);

    Detour::Status MarkBoxArea(TileCacheLayer& layer, const float* orig, const float cs, const float ch,
                        const float* bmin, const float* bmax, const unsigned char areaId);

    Detour::Status MarkBoxArea(TileCacheLayer& layer, const float* orig, const float cs, const float ch,
                        const float* center, const float* halfExtents, const float* rotAux, const unsigned char areaId);

    Detour::Status BuildTileCacheRegions(TileCacheAlloc* alloc,
                                   TileCacheLayer& layer,
                                   const int walkableClimb);

    Detour::Status BuildTileCacheContours(TileCacheAlloc* alloc,
                                    TileCacheLayer& layer,
                                    const int walkableClimb, 	const float maxError,
                                    TileCacheContourSet& lcset);

    Detour::Status BuildTileCachePolyMesh(TileCacheAlloc* alloc,
                                      TileCacheContourSet& lcset,
                                      TileCachePolyMesh& mesh);

    /// 交换压缩tile数据头的尾数 (#TileCacheLayerHeader).
    /// Tile 层数据不需要端交换，因为它只包含字节.
    ///  @param[in,out]	data		tile数据数组.
    ///  @param[in]		dataSize	数据组大小
    bool TileCacheHeaderSwapEndian(unsigned char* data, const int dataSize);

}

#endif 
