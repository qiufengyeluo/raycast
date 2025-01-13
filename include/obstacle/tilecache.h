#ifndef MAGIC_ENGINE_NAVIGATION_OBSTACLE_TILECACHE_H_H
#define MAGIC_ENGINE_NAVIGATION_OBSTACLE_TILECACHE_H_H

#include "detour/dstatus.h"
#include "detour/navmeshbuilder.h"
#include "detour/navmesh.h"

namespace Obstacle {
    typedef unsigned int ObstacleRef;

    typedef unsigned int CompressedTileRef;

    /// Flags for addTile
    enum CompressedTileFlags {
        COMPRESSEDTILE_FREE_DATA = 0x01,					///Navmesh拥有tile内存，应该可以释放它
    };

    struct CompressedTile {
        unsigned int salt;						///< Counter describing modifications to the tile.
        struct TileCacheLayerHeader* header;
        unsigned char* compressed;
        int compressedSize;
        unsigned char* data;
        int dataSize;
        unsigned int flags;
        CompressedTile* next;
    };

    enum ObstacleState {
        OBSTACLE_EMPTY,
        OBSTACLE_PROCESSING,
        OBSTACLE_PROCESSED,
        OBSTACLE_REMOVING,
    };

    enum ObstacleType {
        OBSTACLE_CYLINDER,
        OBSTACLE_BOX, // AABB
        OBSTACLE_ORIENTED_BOX, // OBB
    };

    struct ObstacleCylinder {
        float pos[ 3 ];
        float radius;
        float height;
    };

    struct ObstacleBox {
        float bmin[ 3 ];
        float bmax[ 3 ];
    };

    struct ObstacleOrientedBox {
        float center[ 3 ];
        float halfExtents[ 3 ];
        float rotAux[ 2 ]; //{ cos(0.5f*angle)*sin(-0.5f*angle); cos(0.5f*angle)*cos(0.5f*angle) - 0.5 }
    };

    static const int MAX_TOUCHED_TILES = 8;
    struct TileCacheObstacle {
        union {
            ObstacleCylinder cylinder;
            ObstacleBox box;
            ObstacleOrientedBox orientedBox;
        };

        CompressedTileRef touched[MAX_TOUCHED_TILES];
        CompressedTileRef pending[MAX_TOUCHED_TILES];
        unsigned short salt;
        unsigned char type;
        unsigned char state;
        unsigned char ntouched;
        unsigned char npending;
         TileCacheObstacle* next;
    };

    struct  TileCacheParams {
        float orig[3];
        float cs, ch;
        int width, height;
        float walkableHeight;
        float walkableRadius;
        float walkableClimb;
        float maxSimplificationError;
        int maxTiles;
        int maxObstacles;
    };

    struct  TileCacheMeshProcess {
        virtual ~ TileCacheMeshProcess() { }

        virtual void Process(struct  Detour::NavMeshCreateParams* params,
                            unsigned char* polyAreas, unsigned short* polyFlags) = 0;
    };


    class  TileCache {
    public:
         TileCache();
        ~ TileCache();
        
        struct  TileCacheAlloc* getAlloc() { return m_talloc; }
        struct  TileCacheCompressor* getCompressor() { return m_tcomp; }
        const  TileCacheParams* getParams() const { return &m_params; }
        
        inline int GetTileCount() const { return m_params.maxTiles; }
        inline const  CompressedTile* getTile(const int i) const { return &m_tiles[i]; }
        
        inline int GetObstacleCount() const { return m_params.maxObstacles; }
        inline const  TileCacheObstacle* GetObstacle(const int i) const { return &m_obstacles[i]; }
        
        const  TileCacheObstacle* GetObstacleByRef( ObstacleRef ref);
        
        ObstacleRef GetObstacleRef(const  TileCacheObstacle* obmin) const;
        
        Detour::Status Init(const  TileCacheParams* params,
                    struct  TileCacheAlloc* talloc,
                    struct  TileCacheCompressor* tcomp,
                    struct  TileCacheMeshProcess* tmproc);
        
        int GetTilesAt(const int tx, const int ty,  CompressedTileRef* tiles, const int maxTiles) const ;
        
        CompressedTile* GetTileAt(const int tx, const int ty, const int tlayer);
        CompressedTileRef GetTileRef(const  CompressedTile* tile) const;
        const  CompressedTile* GetTileByRef( CompressedTileRef ref) const;
        
        Detour::Status AddTile(unsigned char* data, const int dataSize, unsigned char flags,  CompressedTileRef* result);
        
        Detour::Status RemoveTile( CompressedTileRef ref, unsigned char** data, int* dataSize);
        
        // Cylinder obstacle.
        Detour::Status AddObstacle(const float* pos, const float radius, const float height, ObstacleRef* result);

        // Aabb obstacle.
        Detour::Status AddBoxObstacle(const float* bmin, const float* bmax, ObstacleRef* result);

        // Box obstacle: can be rotated in Y.
        Detour::Status AddBoxObstacle(const float* center, const float* halfExtents, const float yRadians, ObstacleRef* result);
        
        Detour::Status RemoveObstacle(const ObstacleRef ref);
        
        Detour::Status QueryTiles(const float* bmin, const float* bmax,
                            CompressedTileRef* results, int* resultCount, const int maxResults) const;
        
        /// Updates the tile cache by rebuilding tiles touched by unfinished obstacle requests.
        ///  @param[in]		dt			The time step size. Currently not used.
        ///  @param[in]		navmesh		The mesh to affect when rebuilding tiles.
        ///  @param[out]	upToDate	Whether the tile cache is fully up to date with obstacle requests and tile rebuilds.
        ///  							If the tile cache is up to date another (immediate) call to update will have no effect;
        ///  							otherwise another call will continue processing obstacle requests and tile rebuilds.
        Detour::Status Update(const float dt,  Detour::NavMesh* navmesh, bool* upToDate = 0);
        
        Detour::Status BuildNavMeshTilesAt(const int tx, const int ty,  Detour::NavMesh* navmesh);
        
        Detour::Status BuildNavMeshTile(const CompressedTileRef ref,  Detour::NavMesh* navmesh);
        
        void CalcTightTileBounds(const struct TileCacheLayerHeader* header, float* bmin, float* bmax) const;
        
        void GetObstacleBounds(const struct TileCacheObstacle* ob, float* bmin, float* bmax) const;
        

        /// Encodes a tile id.
        inline CompressedTileRef EncodeTileId(unsigned int salt, unsigned int it) const {
            return ((CompressedTileRef)salt << m_tileBits) | (CompressedTileRef)it;
        }
        
        /// Decodes a tile salt.
        inline unsigned int DecodeTileIdSalt(CompressedTileRef ref) const  {
            const CompressedTileRef saltMask = ((CompressedTileRef)1<<m_saltBits)-1;
            return (unsigned int)((ref >> m_tileBits) & saltMask);
        }
        
        /// Decodes a tile id.
        inline unsigned int DecodeTileIdTile(CompressedTileRef ref) const {
            const CompressedTileRef tileMask = ((CompressedTileRef)1<<m_tileBits)-1;
            return (unsigned int)(ref & tileMask);
        }

        /// Encodes an obstacle id.
        inline ObstacleRef EncodeObstacleId(unsigned int salt, unsigned int it) const {
            return ((ObstacleRef)salt << 16) | (ObstacleRef)it;
        }
        
        /// Decodes an obstacle salt.
        inline unsigned int DecodeObstacleIdSalt(ObstacleRef ref) const {
            const ObstacleRef saltMask = ((ObstacleRef)1<<16)-1;
            return (unsigned int)((ref >> 16) & saltMask);
        }
        
        /// Decodes an obstacle id.
        inline unsigned int DecodeObstacleIdObstacle(ObstacleRef ref) const {
            const ObstacleRef tileMask = ((ObstacleRef)1<<16)-1;
            return (unsigned int)(ref & tileMask);
        }
        
        
    private:
        // Explicitly disabled copy constructor and copy assignment operator.
        TileCache(const TileCache&);
        TileCache& operator=(const TileCache&);

        enum ObstacleRequestAction {
            REQUEST_ADD,
            REQUEST_REMOVE,
        };
        
        struct ObstacleRequest {
            int action;
            ObstacleRef ref;
        };
        
        int m_tileLutSize;						///< Tile hash lookup size (must be pot).
        int m_tileLutMask;						///< Tile hash lookup mask.
        
        CompressedTile** m_posLookup;			///< Tile hash lookup.
        CompressedTile* m_nextFreeTile;		///< Freelist of tiles.
        CompressedTile* m_tiles;				///< List of tiles.
        
        unsigned int m_saltBits;				///< Number of salt bits in the tile ID.
        unsigned int m_tileBits;				///< Number of tile bits in the tile ID.
        
        TileCacheParams m_params;
        
        TileCacheAlloc* m_talloc;
        TileCacheCompressor* m_tcomp;
        TileCacheMeshProcess* m_tmproc;
        
        TileCacheObstacle* m_obstacles;
        TileCacheObstacle* m_nextFreeObstacle;
        
        static const int MAX_REQUESTS = 64;
        ObstacleRequest m_reqs[MAX_REQUESTS];
        int m_nreqs;
        
        static const int MAX_UPDATE = 64;
        CompressedTileRef m_update[MAX_UPDATE];
        int m_nupdate;
    };

    TileCache* AllocTileCache();
    void FreeTileCache(TileCache* tc);
}



#endif 