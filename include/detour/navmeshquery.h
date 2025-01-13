#ifndef MAGIC_ENGINE_NAVIGATION_NAVMESH_QUERY_H_H
#define MAGIC_ENGINE_NAVIGATION_NAVMESH_QUERY_H_H

#include "navmesh.h"
#include "dstatus.h"
#include "detour.h"

namespace Detour {
    class  QueryFilter {
        float m_areaCost[MAX_AREAS];		///< Cost per area type. (Used by default implementation.)
        unsigned short m_includeFlags;		///< Flags for polygons that can be visited. (Used by default implementation.)
        unsigned short m_excludeFlags;		///< Flags for polygons that should not be visted. (Used by default implementation.)
        
    public:
        QueryFilter();
        
    #ifdef VIRTUAL_QUERYFILTER
        virtual ~QueryFilter() { }
    #endif
        

    /**
     * @brief           如果可以访问多边形，则返回true。（即，是可穿越的。）
     * 
     * @param ref 
     * @param ptrTile   包含多边形的tile
     * @param ptrPoly   要检测的多边形。
     * @return true 
     * @return false 
     */
    #ifdef VIRTUAL_QUERYFILTER
        virtual bool PassFilter(const PolyRef ref,
                                const MeshTile *ptrTile,
                                const Poly     *ptrPoly) const;
    #else
        bool PassFilter(const PolyRef ref,
                        const MeshTile *ptrTile,
                        const Poly     *ptrPoly) const;
    #endif
         /**
         * @brief 返回从线段的起点移动到终点的成本
         *        它完全包含在一个多边形中.
         * @param pa           上一个多边形和当前多边形边缘上的起始位置.[（x，y，z）]
         * @param pb           当前多边形和下一个多边形边缘上的结束位置.[（x，y，z）]
         * @param prevRef      上一个多边形的引用ID.   [opt]
         * @param ptrPrevTile  包含上一个多边形的tile. [opt]
         * @param ptrPrevPoly  上一个多边形. [opt]
         * @param curRef       当前多边形的引用ID.
         * @param ptrCurTile   包含当前多边形的tile.
         * @param ptrCurPoly   当前多边形.
         * @param nextRef      下一个多边形的引用ID.   [opt]
         * @param ptrNextTile  包含下一个多边形的tile. [opt]
         * @param ptrNextPoly  下一个多边形. [opt]
         * @return float 
         */
    #ifdef VIRTUAL_QUERYFILTER
        virtual float GetCost(const float* pa, 
                              const float* pb,
                              const PolyRef prevRef, const MeshTile* ptrPrevTile, const Poly* ptrPrevPoly,
                              const PolyRef curRef,  const MeshTile* ptrCurTile,  const Poly* ptrCurPoly,
                              const PolyRef nextRef, const MeshTile* ptrNextTile, const Poly* ptrNextPoly) const;
    #else
   
        float GetCost(const float* pa, 
                      const float* pb,
                      const PolyRef prevRef, const MeshTile* ptrPrevTile, const Poly* ptrPrevPoly,
                      const PolyRef curRef,  const MeshTile* ptrCurTile,  const Poly* ptrCurPoly,
                      const PolyRef nextRef, const MeshTile* ptrNextTile, const Poly* ptrNextPoly) const;
    #endif

        /**
         * @brief 返回区域的遍历开销.
         * 
         * @param i        该区域的id.
         * @return float   该区域的穿越成本.
         */
        inline float GetAreaCost(const int i) const { return m_areaCost[i]; }


        /**
         * @brief 设置区域的遍历成本。
         * 
         * @param i     该区域的id.
         * @param cost  成本
         */
        inline void SetAreaCost(const int i, const float cost) { m_areaCost[i] = cost; } 

        /**
         * @brief 返回筛选器的包含标志。
         *        任何包含一个或多个这些标志的多边形都将被删除.
         * @return unsigned short 
         */
        inline unsigned short GetIncludeFlags() const { return m_includeFlags; }

        /**
         * @brief  设置过滤器的包含标识.
         * 
         * @param flags 标识.
         */
        inline void SetIncludeFlags(const unsigned short flags) { m_includeFlags = flags; }


        /**
         * @brief 返回筛选器的排除标志.
         *        任何包含一个或多个这些标志的多边形都将被删除
         *        被排除在手术之外.
         * @return unsigned short 
         */
        inline unsigned short GetExcludeFlags() const { return m_excludeFlags; }

        /**
         * @brief 设置过滤器的排除标志.
         * 
         * @param flags 
         */
        inline void SetExcludeFlags(const unsigned short flags) { m_excludeFlags = flags; }

    };



    /// Provides information about raycast hit
    /// filled by NavMeshQuery::raycast
    struct RaycastHit {
        /// The hit parameter. (FLT_MAX if no wall hit.)
        float t; 
        
        /// hitNormal	The normal of the nearest wall hit. [(x, y, z)]
        float hitNormal[3];

        /// The index of the edge on the final polygon where the wall was hit.
        int hitEdgeIndex;
        
        /// Pointer to an array of reference ids of the visited polygons. [opt]
        PolyRef  *path;
        
        /// The number of visited polygons. [opt]
        int pathCount;

        /// The maximum number of polygons the @p path array can hold.
        int maxPath;

        ///  The cost of the path until hit.
        float pathCost;
    };

    /**
     * @brief 提供自定义多边形查询行为.
     *        //由NavMeshQuery:：QueryPolygons使用.
     * 
     */
    class PolyQuery {
    public:
        virtual ~PolyQuery() { }

        ///在NavMeshQuery:：queryPolygons中为搜索区域所触及的每一批唯一多边形调用.
        ///对于一个查询，可以多次调用它.
        virtual void Process(const MeshTile* ptrTile, Poly** ptrPolys, PolyRef* ptrRefs, int count) = 0;
    };

    /// Provides the ability to perform pathfinding related queries against
    /// a navigation mesh.
    class DLLIMPORT_DETOUR NavMeshQuery
    {
    public:
        NavMeshQuery();
        ~NavMeshQuery();
        

        /**
         * @brief 初始化查询对象
         * 
         * @param ptrNav    指向用于所有查询的dtNavMesh对象的指针
         * @param maxNodes  最大搜索节点数。[Limits：0<值<=65535]
         * @return Status   查询的状态标志。
         */
        Status Init(const NavMesh* ptrNav, const int maxNodes);


        /**
         * @brief  查找从起始多边形到结束多边形的路径。
         * 
         * @param startRef       起始多边形的引用id。
         * @param endRef         结束多边形的参考id。
         * @param startPos       起始多边形中的位置。[（x，y，z）]
         * @param endPos         末端多边形中的位置。[（x，y，z）]
         * @param ptrFilter      要应用于查询的多边形过滤器。
         * @param ptrPath        表示路径的多边形参照的有序列表。(从头到尾) [(polyRef) * @p pathCount]
         * @param pathCount      @p路径数组中返回的多边形数。
         * @param maxPath        @p路径数组可以容纳的最大多边形数。[Limit：>=1]
         * @return Status 
         */
        Status FindPath(PolyRef startRef, PolyRef endRef,
                        const float* startPos, const float* endPos,
                        const QueryFilter *ptrFilter,
                        PolyRef *ptrPath, 
                        int* pathCount, 
                        const int maxPath) const;


        /**
         * @brief 在多边形道路中查找从起点到终点位置的直线路径.
         * 
         * @param startPos                      Path start position. [(x, y, z)]
         * @param endPos                        Path end position. [(x, y, z)]
         * @param ptrPath                       表示道路道路的多边形引用数组.
         * @param pathSize                      路径数组中的多边形数.
         * @param straightPath                  描述直线路径的点(straightPathCount)
         * @param straightPathFlags             描述每个点的标.（参见：#StraightPathFlags）[opt]
         * @param ptrStraightPathRefs           在每个点输入的多边形的参考id [opt]
         * @param straightPathCount             直线路径中的点数.
         * @param maxStraightPath               直线路径阵列可以容纳的最大点数。[Limiter：>0]
         * @param options                       查询选项. (see: #StraightPathOptions)
         * @return Status 
         */
        Status FindStraightPath(const float* startPos, 
                                const float* endPos,
                                const PolyRef *ptrPath, 
                                const int pathSize,
                                float* straightPath, 
                                unsigned char* straightPathFlags, 
                                PolyRef* ptrStraightPathRefs,
                                int* straightPathCount, 
                                const int maxStraightPath, 
                                const int options = 0) const;


        /**
         * @brief 初始化切片路径查询。
         * 
         * @param startRef      起始多边形的引用id.
         * @param endRef        结束多边形的引用id.
         * @param startPos      起始多边形中的位置。[（x，y，z）]
         * @param endPos        结束多边形中的位置
         * @param ptrFilter     要应用于查询的多边形过滤器
         * @param options       查询选项FindPathOptions
         * @return Status 
         */
        Status InitSlicedFindPath(PolyRef startRef, 
                                  PolyRef endRef,
                                  const float* startPos, 
                                  const float* endPos,
                                  const QueryFilter* ptrFilter, 
                                  const unsigned int options = 0);

        /**
         * @brief    Updates an in-progress sliced path query.
         * 
         * @param maxIter       The maximum number of iterations to perform.
         * @param doneIters     The actual number of iterations completed. [opt]
         * @return Status The status flags for the query.
         */
        Status UpdateSlicedFindPath(const int maxIter, int* doneIters);


        /**
         * @brief 完成并返回切片路径查询的结果
         * 
         * @param ptrPath          表示路径的多边形参照的有序列表.(从头到尾) [(polyRef) * @p pathCount]
         * @param pathCount        @p路径数组中返回的多边形数.
         * @param maxPath          路径阵列可以容纳的最大多边形数。[Limit：>=1]
         * @return Status          查询的状态标志。
         */
        Status FinalizeSlicedFindPath(PolyRef *ptrPath, int* pathCount, const int maxPath);


        /**
         * @brief 完成并返回不完整的切片路径查询的结果，将路径返回到最远的位置
         *        搜索过程中访问的现有路径上的多边形。
         * 
         * @param ptrExisting          现有路径的多边形引用数组.
         * @param existingSize         现有数组中的多边形数.
         * @param ptrPath              表示路径的多边形参照的有序列表.(从头到尾) [(polyRef) * @p pathCount]
         * @param pathCount            路径数组中返回的多边形数.
         * @param maxPath              路径数组可以容纳的最大多边形数.[Limit：>=1]
         * @return Status              查询的状态标志.
         */
        Status FinalizeSlicedFindPathPartial(const PolyRef *ptrExisting, 
                                             const int existingSize,
                                             PolyRef *ptrPath, 
                                             int* pathCount, 
                                             const int maxPath);

        /**
         * @brief Dijkstra Search Functions
         *        沿导航图查找与指定圆接触的多边形。
         * 
         * @param startRef         开始搜索的多边形的引用id.
         * @param centerPos        搜索圈的中心.[（x，y，z）]
         * @param radius           搜索圆的半径.
         * @param filter           要应用于查询的多边形过滤器
         * @param resultRef        被圆接触的多边形的引用ID. [选择]
         * @param resultParent     每个结果的父多边形的参考ID, 如果结果多边形没有父多边形，则为零.[选择]
         * @param resultCost       从centerPos到多边形的搜索成本. [选择]
         * @param resultCount      找到的多边形数.[选择]
         * @param maxResult        结果数组可以容纳的最大多边形数.
         * @return Status          查询的状态标志.
         */
        Status FindPolysAroundCircle(PolyRef startRef, 
                                    const float* centerPos, 
                                    const float radius,
                                    const QueryFilter* filter,
                                    PolyRef* resultRef, 
                                    PolyRef* resultParent, 
                                    float* resultCost,
                                    int* resultCount, 
                                    const int maxResult) const;


        /**
         * @brief 沿导航图形查找与指定凸多边形接触的多边形.
         * 
         * @param startRef     开始搜索的多边形的引用id.
         * @param verts        描述凸多边形的顶点.(CCW) [(x, y, z) * @p nverts]
         * @param nverts       多边形中的顶点数.
         * @param filter       要应用于查询的多边形过滤器
         * @param resultRef    被圆接触的多边形的引用ID. [选择]
         * @param resultParent 每个结果的父多边形的引用ID, 如果结果多边形没有父多边形，则为零.[选择]
         * @param resultCost   找到的多边形数.[选择]
         * @param resultCount  找到的多边形数.[选择]
         * @param maxResult    结果数组可以容纳的最大多边形数.
         * @return Status      查询的状态标志.
         */
        Status FindPolysAroundShape(PolyRef startRef, 
                                    const float* verts, 
                                    const int nverts,
                                    const QueryFilter* filter,
                                    PolyRef* resultRef, 
                                    PolyRef* resultParent, 
                                    float* resultCost,
                                    int* resultCount, 
                                    const int maxResult) const;
        
        /**
         * @brief 从上一次搜索中浏览的节点获取路径.
         * 
         * @param endRef       结束多边形的引用id.
         * @param path[out]    表示路径的多边形参照的有序列表.(从头到尾) [(polyRef) * @p pathCount]
         * @param pathCount    The number of polygons returned in the @p path array.
         * @param maxPath      The maximum number of polygons the @p path array can hold. [Limit: >= 0]
         * @return Status      状态标志。如果任何参数错误，则返回FAILURE | INVALID_PARAM, 或则如果在上一次搜索中未探索endRef，
         *                     返回SUCCESS | BUFFER_TOO_SMALL,如果path不能包含整个路径。在这种情况下，它被部分路径填满,否则将返回
         *                     SUCCESS
         * 
         * @remarks            此函数的结果取决于查询对象的状态。出于这个原因，它只应该
         *                     在Dijkstra的两次搜索（FindPolysAroundCircle或FindPolysAroundShape）之一后立即使用.
         */
        Status GetPathFromDijkstraSearch(PolyRef endRef, PolyRef* path, int* pathCount, int maxPath) const;

        /**
         * @brief 查找距离指定中心点最近的多边形
         *        Local Query Functions
         * 
         * @param center         搜索框的中心. [（x，y，z）]
         * @param halfExtents    沿每个轴的搜索距离. [（x，y，z）]
         * @param filter         要应用于查询的多边形过滤器.
         * @param nearestRef     最近多边形的引用id.
         * @param nearestPt      多边形上最近的点. [opt][（x，y，z）]
         * @return Status        查询的状态标志.
         */
        Status FindNearestPoly(const float* center, 
                               const float* halfExtents,
                               const QueryFilter* filter,
                               PolyRef* nearestRef, 
                               float* nearestPt) const;
        /**
         * @brief 查找与搜索框重叠的多边形.
         * 
         * @param center             搜索框的中心. [（x，y，z）]
         * @param halfExtents        沿每个轴的搜索距离. [（x，y，z）]
         * @param filter             要应用于查询的多边形过滤器.
         * @param polys[out]         与查询框重叠的多边形的引用ID.
         * @param polyCount[out]     搜索结果中的多边形数.
         * @param maxPolys           搜索结果可以容纳的最大多边形数.
         * @return Status            查询的状态标志.
         */
        Status QueryPolygons(const float* center, 
                            const float* halfExtents,
                            const QueryFilter* filter,
                            PolyRef* polys, 
                            int* polyCount, 
                            const int maxPolys) const;

        /**
         * @brief 查找与搜索框重叠的多边形.
         * 
         * @param center        搜索框的中心. [（x，y，z）]
         * @param halfExtents   沿每个轴的搜索距离. [（x，y，z）]
         * @param filter        要应用于查询的多边形过滤器.
         * @param query[out]    找到的多边形将被批处理在一起并传递给此查询.
         * @return Status       查询的状态标志.
         */
        Status QueryPolygons(const float* center, 
                             const float* halfExtents,
                             const QueryFilter* filter, 
                             PolyQuery* query) const;

        /**
         * @brief  在围绕中心位置的局部邻域中查找不重叠的导航多边形
         * 
         * @param startRef     开始搜索的多边形的引用id.
         * @param centerPos    搜索圈的中心.[（x，y，z）]
         * @param radius       搜索圆的半径.
         * @param filter            要应用于查询的多边形过滤器.
         * @param resultRef[out]    被圆接触的多边形的引用ID. [选择]
         * @param resultParent[out] 每个结果的父多边形的引用ID, 如果结果多边形没有父多边形，则为零.[选择]
         * @param resultCount[out]  找到的多边形数.[选择]
         * @param maxResult         结果数组可以容纳的最大多边形数.
         * @return Status           查询的状态标志.
         */
        Status FindLocalNeighbourhood(PolyRef startRef, const float* centerPos, const float radius,
                                        const QueryFilter* filter,
                                        PolyRef* resultRef, PolyRef* resultParent,
                                        int* resultCount, const int maxResult) const;

        /**
         * @brief 从约束到导航网格的起点位置移动到终点位置.
         * 
         * @param startRef       开始搜索的多边形的引用id.
         * @param startPos       移动器在起始多边形中的位置。[（x，y，x）]
         * @param endPos         移动器所需的末端位置。[（x，y，z）]
         * @param filter         要应用于查询的多边形过滤器.
         * @param resultPos[out]      移动器的结果位置。[（x，y，z）]
         * @param visited[out]        移动期间访问的多边形的引用ID.
         * @param visitedCount[out]   移动期间访问的多边形数.
         * @param maxVisitedSize      多边形的最大数量为 visited 数组可以容纳.
         * @return Status             查询的状态标志.
         */
        Status MoveAlongSurface(PolyRef startRef, 
                                const float* startPos, 
                                const float* endPos,
                                const QueryFilter* filter,
                                float* resultPos, 
                                PolyRef* visited, 
                                int* visitedCount, 
                                const int maxVisitedSize) const;
    
        /**
         * @brief  沿导航网格的曲面从开始位置向结束位置“可行走性”
         * 
         * @note   Raycast（…，RaycastHit*）周围有一个包装, 保留用于向后兼容.
         * @param startRef    开始搜索的多边形的引用id.
         * @param startPos    移动器在起始多边形中的位置.[（x，y，x）]
         * @param endPos      移动器所需的末端位置.[（x，y，z）]
         * @param filter      要应用于查询的多边形过滤器.
         * @param t           他击中了参数.(如果没有撞到墙壁，则FLT_MAX.)
         * @param hitNormal   最近的墙壁撞击的法线.[（x，y，z）]
         * @param path[out]        访问的多边形的引用ID.[选择]
         * @param pathCount[out]   访问的多边形数.[选择]
         * @param maxPath          路径数组可以容纳的最大多边形数 path array 的最大数量
         * @return Status          查询的状态标志.
         */
        Status Raycast(PolyRef startRef, const float* startPos, const float* endPos,
                        const QueryFilter* filter,
                        float* t, 
                        float* hitNormal, 
                        PolyRef* path, int* pathCount, const int maxPath) const;


        /**
         * @brief 沿导航网格的曲面从开始位置向结束位置“可行走性”
         * 
         * @param startRef   开始搜索的多边形的引用id.
         * @param startPos   移动器在起始多边形中的位置.[（x，y，x）]
         * @param endPos     移动器所需的末端位置.[（x，y，z）]
         * @param filter     要应用于查询的多边形过滤器.
         * @param options    选项: See RaycastOptions
         * @param hit[out]   填充RaycastHit结果
         * @param prevRef 
         * @return Status    查询的状态标志.
         */
        Status Raycast(PolyRef startRef, const float* startPos, const float* endPos,
                        const QueryFilter* filter, const unsigned int options,
                        RaycastHit* hit, PolyRef prevRef = 0) const;



        /**
         * @brief 查找从指定位置到最近多边形墙的距离
         * 
         * @param startRef   开始搜索的多边形的引用id.
         * @param centerPos  搜索圈的中心.[（x，y，z）]
         * @param maxRadius  搜索的最大半径.
         * @param filter     要应用于查询的多边形过滤器.
         * @param hitDist    距离最近的墙的距离 @p centerPos.
         * @param hitPos     被击中的墙上最近的位置.[（x，y，z）]
         * @param hitNormal  从墙点到墙点形成的向量源点. [(x, y, z)]
         * @return Status    查询的状态标志.
         */
        Status FindDistanceToWall(PolyRef startRef, const float* centerPos, const float maxRadius,
                                    const QueryFilter* filter,
                                    float* hitDist, float* hitPos, float* hitNormal) const;
        

        /**
         * @brief 返回指定多边形的线段（可选包括入口）
         * 
         * @param ref          多边形的引用id.
         * @param filter       多边形过滤器.
         * @param segmentVerts[out]   片段.  [(ax, ay, az, bx, by, bz) * segmentCount]
         * @param segmentRefs[out]    每个线段的相邻多边形的引用ID,如果线段是墙，则为0 [opt] [(parentRef) * @p segmentCount] 
         * @param segmentCount[out]   返回的段数
         * @param maxSegments         内存储的最大段数  
         * @return Status             查询的状态标志.
         */
        Status GetPolyWallSegments(PolyRef ref, const QueryFilter* filter,
                                    float* segmentVerts, PolyRef* segmentRefs, int* segmentCount,
                                    const int maxSegments) const;


        /**
         * @brief 返回navmesh上的随机位置
         * 
         * @param filter   多边形过滤器.
         * @param frand    函数，返回一个随机数[0..1]。
         * @param randomRef[out]  随机位置的引用id
         * @param randomPt[out]   随机位置。[（x，y，z）]
         * @return Status         查询的状态标志.
         */
        Status FindRandomPoint(const QueryFilter* filter, float (*frand)(),
                                PolyRef* randomRef, float* randomPt) const;


        /**
         * @brief  返回指定位置范围内navmesh上的随机位置。
         * 
         * @param startRef    开始搜索的多边形的引用id.
         * @param centerPos   搜索圈的中心.[（x，y，z）]
         * @param maxRadius   搜索的最大半径.
         * @param filter      多边形过滤器.
         * @param frand       函数返回一个随机数[0..1]。
         * @param randomRef[out]   随机位置的引用id
         * @param randomPt[out]    随机位置。[（x，y，z）]
         * @return Status          查询的状态标志.
         */
        Status FindRandomPointAroundCircle(PolyRef startRef, const float* centerPos, const float maxRadius,
                                            const QueryFilter* filter, float (*frand)(),
                                            PolyRef* randomRef, float* randomPt) const;
        

        /**
         * @brief 查找指定多边形上最近的点
         * 
         * @param ref       多边形引用ID
         * @param pos       要检查的位置.[（x，y，z）]
         * @param closest[out]      多边形上最近的点. [（x，y，z）]
         * @param posOverPoly[out]  位置的真值在多边形上.
         * @return Status           查询的状态标志.
         */
        Status ClosestPointOnPoly(PolyRef ref, const float* pos, float* closest, bool* posOverPoly) const;

        /**
         * @brief 如果源点位于多边形的xz边界之外，则返回距离源点最近的边界上的点
         * 
         * @param ref             多边形引用ID
         * @param pos             要检查的位置.[（x，y，z）]
         * @param closest[out]    最近的点.[（x，y，z）]
         * @return Status         查询的状态标志.
         */
        Status ClosestPointOnPolyBoundary(PolyRef ref, const float* pos, float* closest) const;
        

        /**
         * @brief 使用高度细节获取多边形在提供位置的高度.(最准确.)
         * 
         * @param ref      多边形引用ID
         * @param pos      多边形的xz边界内的位置.[（x，y，z）]
         * @param height   多边形表面的高度.
         * @return Status  查询的状态标志.
         */
        Status GetPolyHeight(PolyRef ref, const float* pos, float* height) const;

        /**
         * @brief 如果多边形引用有效并通过过滤器限制，则返回true.
         * 
         * @param ref        要检查的多边形引用.
         * @param filter     要应用的过滤器.
         * @return true 
         * @return false 
         */
        bool IsValidPolyRef(PolyRef ref, const QueryFilter* filter) const;
        /**
         * @brief    如果多边形引用位于闭合列表中，则返回true。
         * 
         * @param ref   要检查的多边形的引用id.
         * @return true 如果多边形位于闭合列表中，则为True.
         * @return false 
         */
        bool IsInClosedList(PolyRef ref) const;

        /**
         * @brief获取节点池
         * 
         * @return NodePool* 节点池
         */
        class NodePool* GetNodePool() const { return m_nodePool; }

        /**
         * @brief 获取查询对象正在使用的导航网格.
         * 
         * @return const NavMesh* 查询对象正在使用的导航网格.
         */
        const NavMesh* GetAttachedNavMesh() const { return m_ptrNav; }

        /// @}
        
    private:
        // Explicitly disabled copy constructor and copy assignment operator
        NavMeshQuery(const NavMeshQuery&);
        NavMeshQuery& operator=(const NavMeshQuery&);
        
        /// Queries polygons within a tile.
        void queryPolygonsInTile(const MeshTile* tile, const float* qmin, const float* qmax,
                                const QueryFilter* filter, PolyQuery* query) const;

        /// Returns portal points between two polygons.
        Status getPortalPoints(PolyRef from, PolyRef to, float* left, float* right,
                                unsigned char& fromType, unsigned char& toType) const;
        Status getPortalPoints(PolyRef from, const Poly* fromPoly, const MeshTile* fromTile,
                                PolyRef to, const Poly* toPoly, const MeshTile* toTile,
                                float* left, float* right) const;
        
        /// Returns edge mid point between two polygons.
        Status getEdgeMidPoint(PolyRef from, PolyRef to, float* mid) const;
        Status getEdgeMidPoint(PolyRef from, const Poly* fromPoly, const MeshTile* fromTile,
                                PolyRef to, const Poly* toPoly, const MeshTile* toTile,
                                float* mid) const;
        
        // Appends vertex to a straight path
        Status appendVertex(const float* pos, const unsigned char flags, const PolyRef ref,
                            float* straightPath, unsigned char* straightPathFlags, PolyRef* straightPathRefs,
                            int* straightPathCount, const int maxStraightPath) const;

        // Appends intermediate portal points to a straight path.
        Status appendPortals(const int startIdx, const int endIdx, const float* endPos, const PolyRef* path,
                            float* straightPath, unsigned char* straightPathFlags, PolyRef* straightPathRefs,
                            int* straightPathCount, const int maxStraightPath, const int options) const;

        // Gets the path leading to the specified end node.
        Status getPathToNode(struct Node* endNode, PolyRef* path, int* pathCount, int maxPath) const;
        
        const NavMesh* m_ptrNav;	//navmesh 数据对象.

        struct QueryData
        {
            Status status;
            struct Node* lastBestNode;
            float lastBestNodeCost;
            PolyRef startRef, endRef;
            float startPos[3], endPos[3];
            const QueryFilter* filter;
            unsigned int options;
            float raycastLimitSqr;
        };
        QueryData m_query;				///< Sliced query state.

        class NodePool* m_tinyNodePool;	///< Pointer to small node pool.
        class NodePool* m_nodePool;		///< Pointer to node pool.
        class NodeQueue* m_openList;		///< Pointer to open list queue.
    };

    /**
     * @brief 分配器分配查询对象.
     * 
     * @return NavMeshQuery*  已分配的查询对象，失败时为空
     */
    NavMeshQuery* AllocNavMeshQuery();


    /**
     * @brief 分配器释放指定的查询对象。
     * 
     * @param ptrQuery   使用 AllocNavMeshQuery
     */
    void FreeNavMeshQuery(NavMeshQuery* ptrQuery);
}


#endif 