#ifndef MAGIC_ENGINE_NAVIGATION_NAVMESH_H_H
#define MAGIC_ENGINE_NAVIGATION_NAVMESH_H_H

#include "dalloc.h"
#include "dstatus.h"
#include "detour.h"

#ifdef POLYREF64
#include <stdint.h>
#endif

#ifdef POLYREF64
#define SALT_BITS  16
#define TILE_BITS  28
#define POLY_BITS  20
typedef uint64_t PolyRef;
#else
typedef unsigned int PolyRef;
#endif 

#ifdef POLYREF64
typedef uint64_t TileRef;
#else 
typedef unsigned int TileRef;
#endif                          
 

namespace Detour {
    static const int VERTS_PER_POLYGON = 6;                                   //每个多边形的顶点数
    static const int NAVMESH_MAGIC = 'D' << 24 | 'N' << 16 | 'A' << 8 | 'V';  //用于检测导航数据的兼容性
    static const int NAVMESH_VERSION  = 7;                                    //用于检测导航数据的兼容性的版本信息
    static const int NAVMESH_STATE_MAGIC = 'D'<<24 | 'N'<<16 | 'M'<<8 | 'S';  //用于检测导航状态的兼容性
    static const int NAVMESH_STATE_VERSION = 1;                               //用于检测导航状态的兼容性的版本信息

    //指示实体链接到外部实体的标志
    //(例如，多边形边是链接到另一个多边形的入口。)
    static const unsigned short EXT_LINK  = 0x8000;
    //表示实体未链接到任何内容的值。
    static const unsigned int   NULL_LINK = 0xFFFFFFFF;
    //一种标志，指示可以在两个方向上遍历脱离网格的连接。(是双向的。)
    static const unsigned int   OFFMESH_CON_BIDIR = 1;
    //用户定义的区域ID的最大数量。
    static const int MAX_AREAS = 64; 

    enum TileFlags {
        //导航网格，(拥有者负责释放,待定?)
        TILE_FREE_DATA = 0x01,
    };

    //NavMeshQuery::findStraightPath, 返回顶点的标记
    enum StraightPathFlags {
        STRAIGHTPATH_START  = 0x01,             //顶点是路径的起始位置
        STRAIGHTPATH_END    = 0x02,             //顶点是路径的结束位置
        STRAIGHTPATH_OFFMESH_CONNECTION = 0x04, //顶点是脱离网格连接的起点(多层连接的起点)
    };

    //NavMeshQuery::findStraightPath的参数选项
    enum StraightPathOptions {
        STRAIGHTPATH_AREA_CROSSINGS = 0x01,     //在面积变化的每个多边形边交叉处添加顶点
        STRAIGHTPATH_ALL_CROSSINGS  = 0x02,     //在每个多边形边交叉处添加顶点
    };

    //NavMeshQuery::initSlicedFindPath 和 updateSlicedFindPat的参数选项
    enum FindPathOptions {
        FINDPATH_ANY_ANGLE	= 0x02,   //在路径查找中使用RayCasts,最近路径
    };

    //NavMeshQuery::raycast 的参数选项
    enum RaycastOptions {
        RAYCAST_USE_COSTS = 0x01,    //Raycast应该计算沿光线的移动成本，并填充RaycastHit:：cost
    };

    //在任何角度搜索期间限制光线投射
    //限制以字符半径的倍数给出
    static const float RAY_CAST_LIMIT_PROPORTIONS = 50.0f;

    //表示导航网格多边形类型的标志
    enum PolyTypes {
        POLYTYPE_GROUND = 0,             //多边形是标准的凸多边形，是网格曲面的一部分
        POLYTYPE_OFFMESH_CONNECTION = 1, //多边形是由两个顶点组成的非网格连接
    };

    /**
     * @brief 定义MeshTile对象中的多边形。
     * 
     */
    struct Poly {
        /**
         * @brief 索引到链接列表中的第一个链接. (如果没有链接，则为#NULL_LINK)
         * 
         */
        unsigned int firstLink;
        /**
         * @brief 多边形顶点的索引。
         *        实际顶点位于MeshTile::verts中。
         */
        unsigned short verts[VERTS_PER_POLYGON];
        /**
         * @brief 表示每个边的相邻多边形引用和标志的压缩数据
         * 
         */
        unsigned short neis[VERTS_PER_POLYGON];
        /**
         * @brief 用户定义的多边形标志。
         * 
         */
        unsigned short flags;
        /**
         * @brief 多边形中的顶点数。
         * 
         */
        unsigned char  vertCount;
        /**
         * @brief 位压缩区域id和多边形类型。
         * @note  使用结构的set和get方法访问此值。
         * 
         */
        unsigned char  areaAndtype;
        /**
         * @brief Set the Area ID (Limit: < #MAX_AREAS)
         * 
         * @param a 
         */
        inline void SetArea(unsigned char a) { areaAndtype = (areaAndtype & 0xc0) | (a & 0x3f); }

        /**
         * @brief Set the Type (See: #PolyTypes.)
         * 
         * @param t 
         */
        inline void SetType(unsigned char t) { areaAndtype = (areaAndtype & 0x3f) | (t << 6); }
        /**
         * @brief Get the Area ID
         * 
         * @return unsigned char 
         */
        inline unsigned char GetArea() const { return areaAndtype & (unsigned char)0x3F; }
        /**
         * @brief Get the Type (See: #PolyTypes.)
         * 
         * @return unsigned char 
         */
        inline unsigned char GetType() const { return areaAndtype >> 6; }
    };

    /**
     * @brief 定义细节子网格数据在MeshTile中的位置
     * 
     */
    struct PolyDetail {
        unsigned int vertBase;    //MeshTile::detailVerts数组中顶点的偏移量
        unsigned int triBase;     //MeshTile::detailTris数组中三角形的偏移量
        unsigned char vertCount;  //顶点数
        unsigned char triCount;   //三角形数
    };

    /**
     * @brief 定义多边形之间的链接。
     * @note 使用者很少使用到此结构。
     * @see  MeshTile
     * 
     */
    struct Link {
        PolyRef  ref;       //邻居引用计数。(链接到的邻居。)
        unsigned int  next; //下一个链接的索引
        unsigned char edge; //拥有此链接的多边形边的索引
        unsigned char side; //如果是边界连接,链接的是哪一边。
        unsigned char bmin; //如果是边界连接，则表示最小，子边缘区域。
        unsigned char bmax; //如果是边界链接，则表示最大，子边缘区域。
    };

    /**
     * @brief 边界体积节点
     * @note  只用者韩少使用到此结构
     * @see   MeshTile
     */
    struct BVNode {
        unsigned short bmin[3];  //节点的AABB的最小界限。[（x，y，z）]
        unsigned short bmax[3];  //节点的AABB的最大界限。[（x，y，z）]
        int i;
    };

    /**
     * @brief 在MeshTile对象内定义导航网格脱离网格连接。
     *        脱离网格连接是由两个顶点组成的用户定义的可遍历连接。
     */
    struct OffMeshConnection {
        float pos[6];         //连接的端点。[（ax，ay，az，bx，by，bz）]
        float rad;            //端点的半径。[Limit: >= 0]
        unsigned short poly;  //多边形在tile中的引用 (The polygon reference of the connection within the tile)
        /**
         * @brief 链接标识
         * @note  这些不是连接的用户定义标志。这些都是通过连接的Poly定义。
         *        这些是用于内部目的的链接标志
         */
        unsigned char flags; 
        unsigned char side;   //重点边
        /**
         * @brief offmesh连接的id。（构建导航网格时由用户指定。）
         * 
         */
        unsigned int  userId; //
    };

    struct MeshHeader {
        int magic;				//Tile magic number. (用于标识数据格式.)
        int version;			//Tile 数据格式版本号.
        int x;					//x以NavMesh中tile格子为单位.   (x, y, layer)
        int y;					//y以NavMesh中tile格子为单位.   (x, y, layer)
        int layer;				//layer以NavMesh中tile的层数学. (x, y, layer)
        unsigned int userId;    //用户定义的ID
        int polyCount;          //多边形数
        int vertCount;          //顶点数
        int maxLinkCount;       //分配的最大链接数

        int detailMeshCount;    //detail网格中的子网格数.
        int detailVertCount;    //detail网格中的顶点的数目. (除了多边形顶点)
        int detailTriCount;     //detail网格中三角形数目.

        int bvNodeCount;            //边界体积节点的数量. (如果禁用了边界体积，则为零.)
        int offMeshConCount;		//off-mesh的链接数
        int offMeshBase;			//off-mesh第一个多边形的索引
        float walkableHeight;		//作用者使用Tile的高度.
        float walkableRadius;		//作用者使用Tile的半径.
        float walkableClimb;		//作用者使用Tile的最大通过的坡度.
        float bmin[3];				//tile最小边界 AABB. [(x, y, z)]
        float bmax[3];				//tile最大边界 AABB. [(x, y, z)]
        
        /// The bounding volume quantization factor. 
        float bvQuantFactor;        //边界体积因数
    };


    /**
     * @brief  定义导航 mesh tile
     * 
     */
    struct MeshTile {
        unsigned int salt;					//tile修改计数器.

        unsigned int linksFreeList;			//下一个需要释放的链接.
        MeshHeader  *header;				//这个tile的头.
        Poly        *polys;				//这个tile的多边形数据. [Size: MeshHeader::polyCount]
        float       *verts;			    //这个tile的顶点数据.   [Size: MeshHeader::vertCount]
        Link        *links;			    //这个tile的链接数据.   [Size: MeshHeader::maxLinkCount]
        PolyDetail  *detailMeshes;	    //这个tile的detail sub-mesh数据. [Size: MeshHeader::detailMeshCount]

        /**
         * @brief detail mesh 的顶点数据. [(x, y, z) * MeshHeader::detailVertCount]
         * 
         */
        float       *detailVerts;	

        /**
         * @brief detail mesh 的三角型数据. [(vertA, vertB, vertC) * MeshHeader::detailTriCount]
         * 
         */
        unsigned char* detailTris;	

        /// The tile bounding volume nodes. [Size: MeshHeader::bvNodeCount]
        /// (Will be null if bounding volumes are disabled.)

        /**
         * @brief 这个tile边界的节点卷. [Size: MeshHeader::bvNodeCount]
         *        (如果是空,则已禁用边界卷)
         */
        BVNode* bvTree;

        /**
         * @brief 这个tile off-mesh(非网格) 链接数据. [Size: MeshHeader::offMeshConCount]
         * 
         */
        OffMeshConnection *offMeshCons;
        
        /**
         * @brief 这个tile 的数据
         * @note 在正常情况下不能直接访问
         */
        unsigned char* data;	
        int dataSize;		     //这个tile数据的大小(ptrData的大小)
        /**
         * @brief tile的标识 
         * @see   TileFlags
         */
        int flags;								///< Tile flags. (See: #dtTileFlags)
        
        /**
         * @brief  下一个 tile
         * 
         */
        MeshTile* next;						
    private:
        MeshTile(const MeshTile&);
        MeshTile& operator=(const MeshTile&);
    };

    /**
     * @brief 配置参数用于定义多tile导航网格.
     *        这些值用于在导航网格初始化期间分配空间.
     * @see   NavMesh::Init()
     */
    struct NavMeshParams {
        float orig[3];					//导航网格tile空间对应世界空间的原点. [(x, y, z)]
        float tileWidth;				//tile的宽度. (Along the x-axis.)
        float tileHeight;				//tile的高度. (Along the z-axis.)
        int maxTiles;					//最大tile数量.
        int maxPolys;					//最大多边形数量.
    };


    class DLLIMPORT_DETOUR NavMesh {
        public:
            NavMesh();
            ~NavMesh();

            /**
             * @brief 初始化.
             * 
             * @param ptrParams  初始化参数.
             * @return Status 返回操作的状态标志.
             */
            Status Init(const NavMeshParams *ptrParams);

            /**
             * @brief 初始化
             * 
             * @param ptrData  tile数据. (See: #CreateNavMeshData)
             * @param dataSize tile数据大小.
             * @param flags    tile标识. (See: #TileFlags)
             * @return Status 
             * @see CreateNavMeshData
             */
            Status Init(unsigned char* ptrData, const int dataSize, const int flags);

            /**
             * @brief 返回参数.
             * 
             * @return const NavMeshParams* 
             */
            const NavMeshParams* GetParams() const;

            /**
             * @brief 增加一个Tile到导航网格.
             * 
             * @param ptrData    tile数据.
             * @param dataSize   tile数据大小.
             * @param flags      tile标识. (See: #TileFlags).
             * @param lastRef    tile的引用(当重新加载tile时)[opt.默认值: 0]
             * @param result[out]   这个tile的引用(如果这个tile增加成功).
             * @return Status       返回操作的状态标志.
             */
            Status AddTile(unsigned char* ptrData, 
                            int dataSize, 
                            int flags, 
                            TileRef  lastRef, 
                            TileRef* result);

            /**
             * @brief 删除一个Tile.
             * 
             * @param ref         需要删除tile的引用对象.
             * @param ptrData2[out]    被删除tile的数据.     (成功删除的返回)
             * @param ptrDataSize[out] 被删除tile的数据大小. (成功删除的返回)
             * @return Status 
             */
            Status RemoveTile(TileRef ref, unsigned char **ptrData2, int *ptrDataSize);

            /**
             * @brief 计算指定世界位置的tile网格位置.
             * 
             * @param pos 查询的世界位置  [x, y, z].
             * @param tx[out]  tile的x位置.
             * @param ty[out]  tile的y位置.
             */
            void CalcTileLoc(const float* pos, int* tx, int* ty) const;

            /**
             * @brief获取指定的tile,通过 (x,y,layer).
             * 
             * @param x   tile的网格x位置.
             * @param y   tile的网格y位置.
             * @param layer tile的网格层数.
             * @return const MeshTile* 如果不存在返回NULL(invalid).
             */
            const MeshTile* GetTileAt(const int x, const int y, const int layer) const;

            /**
             * @brief 获取指定的tile的数组,通过x,y.
             * 
             * @param x tile的网格x位置.
             * @param y tile的网格y位置.
             * @param ptrTiles[out] 保存查询到的tile结果数据.
             * @param maxTiles 能够保存tile数据的最大数量.
             * @return int 返回tile数组的大小.
             */
            int GetTilesAt(const int x, const int y, MeshTile const **ptrTiles, const int maxTiles) const;

            /**
             * @brief 获取指定的Tile的引用对象.
             * 
             * @param x      tile的网格x位置.
             * @param y      tile的网格y位置.
             * @param layer  tile的网格层数.
             * @return TileRef 返回引用,如果不存在返回0.
             */
            TileRef GetTileRefAt(int x, int y, int layer) const;

            /**
             * @brief 获取指定的Tile的引用对象
             * 
             * @param ptrTile 要查询的tile对象.
             * @return TileRef 返回引用,如果不存在返回0.
             */
            TileRef GetTileRef(const MeshTile* ptrTile) const;

            /**
             * @brief 获取指定的tile.
             * 
             * @param ref 要查询tile的应用对象.
             * @return const MeshTile*   如果不存在返回NULL(invalid)
             */
            const MeshTile* GetTileByRef(TileRef ref) const;

            /**
             * @brief 获取Tile的最大数量.
             * 
             * @return int 最大数量.
             */
            int GetMaxTiles() const;

            /**
             * @brief 查询某个tile对象通过索引.
             * 
             * @param i     tile索引. [Limit: 0 >= index < #GetMaxTiles()]
             * @return MeshTile* 如果不存在返回NULL(invalid)
             */
            const MeshTile* GetTile(int i) const;
            
            /**
             * @brief 获取指定多边形引用的tile和多边形数据.
             * 
             * @param ref      多边形引用对象.
             * @param ptrtTile[out] 返回的tile数据.
             * @param ptrPoly[out]  返回的多边形数据.
             * @return Status  返回操作的状态标志.
             */
            Status GetTileAndPolyByRef(const PolyRef ref, 
                                       const MeshTile** ptrtTile, 
                                       const Poly** ptrPoly) const;


            /**
             * @brief 获取指定多边形引用的tile和多边形数据.
             * 
             * @param ref     多边形引用对象.
             * @param ptrTile[out] 返回的tile数据.
             * @param ptrPoly[out] 返回的多边形数据. 
             */
            void GetTileAndPolyByRefUnsafe(const PolyRef ref, 
                                           const MeshTile** ptrTile, 
                                           const Poly** ptrPoly) const;


            /**
             * @brief 检测这个多边形引用是否有效.
             * 
             * @param ref 多边形引用对象.
             * @return true 有效.
             * @return false 无效.
             */
            bool IsValidPolyRef(PolyRef ref) const;

            /**
             * @brief 返回这个tile的多边形引用.
             * 
             * @param ptrTile tile对象.
             * @return PolyRef 多边形引用对象.
             */
            PolyRef GetPolyRefBase(const MeshTile* ptrTile) const;

            /**
             * @brief 获取off-mesh(非网格)链接的端点，按照移动方向排序.
             * 
             * @param prevRef   上一个多边形引用对象.
             * @param polyRef   off-mesh(非网格)链接的多边形引用对象.
             * @param ptrStartPos[out]  off-mesh链接的起始位置.
             * @param ptrEndPos[out]    off-mesh链接的结束位置.
             * @return Status 返回操作的状态标志.
             */
            Status  GetOffMeshConnectionPolyEndPoints(PolyRef prevRef, 
                                                      PolyRef polyRef, 
                                                      float*  ptrStartPos, 
                                                      float* ptrEndPos) const;

            /**
             * @brief 获取指定多边形引用对象的off-mesh链接
             * 
             * @param ref 多边形引用对象.
             * @return const OffMeshConnection* 返回off-mesh链接对象，如果不存在返回NULL.
             */
            const OffMeshConnection* GetOffMeshConnectionByRef(PolyRef ref) const;

            /**
             * @brief 设置指定多边形的标识
             * 
             * @param ref 多边形引用对象.
             * @param flags 标识.
             * @return Status 返回操作的状态标志.
             */
            Status SetPolyFlags(PolyRef ref, unsigned short flags);

            /**
             * @brief 获取指定多边形的标识
             * 
             * @param ref 多边形引用对象.
             * @param ptrResultFlags[out] 返回标识.
             * @return Status 返回操作的状态标志.
             */
            Status GetPolyFlags(PolyRef ref, unsigned short *ptrResultFlags) const;

            /**
             * @brief 设置多边形区域
             * 
             * @param ref 多边形引用对象.
             * @param area 区域.
             * @return Status 返回操作的状态标志.
             */
            Status SetPolyArea(PolyRef ref, unsigned char area);

            /**
             * @brief 获取指定多边形的区域.
             * 
             * @param ref 多边形引用对象.
             * @param ptrResultArea[out] 返回区域.
             * @return Status 返回操作的状态标志.
             */
            Status GetPolyArea(PolyRef ref, unsigned char *ptrResultArea) const;

            /**
             * @brief 获取这个tile的状态大小.
             * 
             * @param ptrTile 这个tile.
             * @return int 返回存储这个tile状态的缓冲区大小.
             */
            int GetTileStateSize(const MeshTile* ptrTile) const;

            /**
             * @brief  将tile的非结构状态数据存储到指定的缓冲区.
             * 
             * @param ptrTile 这个tile.
             * @param ptrData 存储缓冲区.
             * @param maxDataSize 存储缓冲区的大小.
             * @return Status 返回操作的状态标志.
             */
            Status StoreTileState(const MeshTile* ptrTile, 
                                  unsigned char*  ptrData, 
                                  const int maxDataSize) const;

            /**
             * @brief 恢复这个tile的状态.
             * 
             * @param ptrTile 这个tile.
             * @param ptrData 新的状态.
             * @param maxDataSize 新状态数据大小.
             * @return Status 返回操作的状态标志.
             */
            Status RestoreTileState(MeshTile* ptrTile, 
                                    const unsigned char* ptrData, 
                                    const int maxDataSize);

            /**
             * @brief 导出标准多边形引用对象.(编码)
             * 
             * @note  此功能通常仅供内部使用.
             * @param salt tile的形状值.
             * @param it   tile的索引.
             * @param ip   tile中多边形的索引.
             * @return PolyRef 
             */
            inline PolyRef EncodePolyId(unsigned int salt,
                                        unsigned int it,
                                        unsigned int ip) const {
#ifdef POLYREF64
		        return ((PolyRef)salt << (POLY_BITS+TILE_BITS)) | ((PolyRef)it << POLY_BITS) | (PolyRef)ip;
#else
		        return ((PolyRef)salt << (m_polyBits+m_tileBits)) | ((PolyRef)it << m_polyBits) | (PolyRef)ip;
#endif
            }

            /**
             * @brief 解码标准多边形引用对象.
             * 
             * @note  此功能通常仅供内部使用.
             * @param ref  多边形引用对象.
             * @param salt tile的形状值.
             * @param it   tile的索引.
             * @param ip   tile中多边形的索引.
             */
            inline void DecodePolyId(PolyRef ref, unsigned int& salt, unsigned int& it, unsigned int& ip) const {
#ifdef POLYREF64
                const PolyRef saltMask = ((PolyRef)1<<SALT_BITS)-1;
                const PolyRef tileMask = ((PolyRef)1<<TILE_BITS)-1;
                const PolyRef polyMask = ((PolyRef)1<<POLY_BITS)-1;
                salt = (unsigned int)((ref >> (POLY_BITS+TILE_BITS)) & saltMask);
                it = (unsigned int)((ref >> POLY_BITS) & tileMask);
                ip = (unsigned int)(ref & polyMask);
#else
                const PolyRef saltMask = ((PolyRef)1<<m_saltBits)-1;
                const PolyRef tileMask = ((PolyRef)1<<m_tileBits)-1;
                const PolyRef polyMask = ((PolyRef)1<<m_polyBits)-1;
                salt = (unsigned int)((ref >> (m_polyBits+m_tileBits)) & saltMask);
                it = (unsigned int)((ref >> m_polyBits) & tileMask);
                ip = (unsigned int)(ref & polyMask);
#endif
            }

            /**
             * @brief 从指定的多边形引用中提取tile的salt值.
             * 
             * @note  此功能通常仅供内部使用.
             * @param ref  多边形引用对象.
             * @return unsigned int 
             * @see #EncodePolyId
             */
            inline unsigned int DecodePolyIdSalt(PolyRef ref) const {
#ifdef POLYREF64
		        const PolyRef saltMask = ((PolyRef)1<<SALT_BITS)-1;
		        return (unsigned int)((ref >> (POLY_BITS+TILE_BITS)) & saltMask);
#else
		        const PolyRef saltMask = ((PolyRef)1<<m_saltBits)-1;
		        return (unsigned int)((ref >> (m_polyBits+m_tileBits)) & saltMask);
#endif
            }

            /**
             * @brief 从指定的多边形引用中提取tile的索引.
             * 
             * @param ref 多边形引用对象.
             * @return unsigned int tile索引.
             * @see #EncodePolyId
             */
            inline unsigned int DecodePolyIdTile(PolyRef ref) const {
#ifdef POLYREF64
                const PolyRef tileMask = ((PolyRef)1<<TILE_BITS)-1;
                return (unsigned int)((ref >> POLY_BITS) & tileMask);
#else
                const PolyRef tileMask = ((PolyRef)1<<m_tileBits)-1;
                return (unsigned int)((ref >> m_polyBits) & tileMask);
#endif
            }

            /**
             * @brief 从指定的多边形引用中提取多边形的索引.
             * 
             * @param ref 多边形引用对象.
             * @return unsigned int 多边形索引.
             * @see #EncodePolyId
             */
            inline unsigned int DecodePolyIdPoly(PolyRef ref) const {
#ifdef POLYREF64
		        const PolyRef polyMask = ((PolyRef)1<<POLY_BITS)-1;
		        return (unsigned int)(ref & polyMask);
#else
		        const PolyRef polyMask = ((PolyRef)1<<m_polyBits)-1;
		        return (unsigned int)(ref & polyMask);
#endif                
            }
    private:
        NavMesh(const NavMesh&);
        NavMesh& operator=(const NavMesh&);

        /**
         * @brief 返回指定tile数量的tile数组.
         * 
         * @param i tile数量.
         * @return MeshTile* tile数组.
         */
        MeshTile *getTile(int i);

        /**
         * @brief 返回基于边的相邻tile
         * 
         * @param x tile?/多边形?/坐标?(x).
         * @param y tile?/多边形?/坐标?(y).
         * @param ptrTiles[out] tile缓冲区.
         * @param maxTiles      tile缓冲区最大数量.
         * @return int tile的数量或大小.
         */
        int getTilesAt(const int x, 
                       const int y,
				       MeshTile** ptrTiles, 
                       const int maxTiles) const;

        /**
         * @brief 返回基于边的相邻tile
         * 
         * @param x tile?/多边形?/坐标?(x).
         * @param y tile?/多边形?/坐标?(y).
         * @param side 边.
         * @param ptrTiles[out]  tile缓冲区.
         * @param maxTiles       tile缓冲区最大数量.
         * @return int int tile的数量或大小.
         */
        int getNeighbourTilesAt(const int x, 
                                const int y, 
                                const int side,
						    	MeshTile** ptrTiles, 
                                const int maxTiles) const;
        
        /**
         * @brief 基于段定义的入口返回相邻tile中的所有多边形.
         * 
         * @param ptrVa             段Va
         * @param ptrVb             段Vb
         * @param ptrTile           tile对象.
         * @param side              边
         * @param ptrCon[out]       链接对象.
         * @param ptrConArea[out]   链接区域.
         * @param maxcon            最大链接数.
         * @return int 
         */
        int findConnectingPolys(const float    *ptrVa, 
                                const float    *ptrVb,
							    const MeshTile *ptrTile, 
                                int      side,
							    PolyRef *ptrCon, 
                                float   *ptrConarea, 
                                int      maxcon) const;
        /**
         * @brief 为tile构建内部多边形链接.
         * 
         * @param ptrTile tile对象.
         */
        void connectIntLinks(MeshTile* ptrTile);

        /**
         * @brief 为tile构建内部多边形链接.
         * 
         * @param ptrTile tile对象.
         */
        void baseOffMeshLinks(MeshTile* ptrTile);

        /**
         * @brief 为tile构建外部多边形链接.
         * 
         * @param ptrTile    tile对象.
         * @param ptrTarget  目标tile对象.
         * @param side       边.
         */
        void connectExtLinks(MeshTile*  ptrTile, 
                             MeshTile*  ptrTarget, 
                             int side);

        /**
         * @brief 为tile构建外部多边形链接.
         * 
         * @param ptrTile   tile对象.
         * @param ptrTarget 目标tile对象.
         * @param side      边.
         */
        void connectExtOffMeshLinks(MeshTile *ptrTile, 
                                    MeshTile *ptrTarget, 
                                    int side);

        /**
         * @brief 删除指定侧的外部链接.
         * 
         * @param ptrTile   tile对象.
         * @param ptrTarget 目标tile对象.
         */
        void unconnectLinks(MeshTile *ptrTile, 
                            MeshTile *ptrTarget);


        /**
         * @brief 查询tile中的多边形.
         * 
         * @param ptrTile       这个tile对象.
         * @param qmin          范围最小值.
         * @param qmax          范围最大值.
         * @param ptrPolys[out] 多边形缓冲区. 
         * @param maxPolys      多边形缓冲区大小.
         * @return int          查询到多边形的数量(大小).
         */
        int queryPolygonsInTile(const MeshTile *ptrTile, 
                                const float    *qmin, 
                                const float    *qmax,
							    PolyRef        *ptrPolys, 
                                const int maxPolys) const;
        /**
         * @brief 查询tile中最近的多边形.
         * 
         * @param ptrTile 
         * @param center 
         * @param halfExtents 
         * @param nearestPt 
         * @return PolyRef 
         */
        PolyRef findNearestPolyInTile(const MeshTile *ptrTile, 
                                      const float    *center,
									  const float    *halfExtents, 
                                      float* nearestPt) const;

        /**
         * @brief 返回多边形上最近的点
         * 
         * @param ref 这个多边形引用对象.
         * @param pos 位置.
         * @param closest 
         * @param posOverPoly 
         */
        void closestPointOnPoly(PolyRef      ref, 
                                const float *pos, 
                                float       *closest, 
                                bool        *posOverPoly) const;
    private:
        NavMeshParams   m_params;		 //初始化参数.
        float           m_orig[3];		 //tile原点 (0, 0).
        float           m_tileWidth;     //tile宽度.
        float           m_tileHeight;	 //tile高度.  
        int             m_maxTiles;		 //tile最大数量.				
        int             m_tileLutSize;	 //tile哈希散列大小.
        int             m_tileLutMask;	 //tile哈希散列掩码.

        MeshTile** m_ptrPosLookup;			///tile哈希散列
	    MeshTile*  m_ptrNextFree;		    // tile释放列表
	    MeshTile*  m_ptrTiles;				// tile数据列表

#ifndef POLYREF64
        unsigned int m_saltBits;			// Number of salt bits in the tile ID.
        unsigned int m_tileBits;			// Number of tile bits in the tile ID.
        unsigned int m_polyBits;			// Number of poly bits in the tile ID.
#endif
    };

    /**
     * @brief 网格对象分配器.
     * 
     * @return NavMesh* 网格对象.
     */
    NavMesh* AllocNavMesh();

    /**
     * @brief 网格对象释放器.
     * 
     * @param navmesh 网格对象.
     */
    void FreeNavMesh(NavMesh* navmesh);
}

#endif