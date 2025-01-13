#ifndef MAGIC_ENGINE_NAVIGATION_NAVMESH_BUILDER_H_H
#define MAGIC_ENGINE_NAVIGATION_NAVMESH_BUILDER_H_H

#include "dalloc.h"

namespace Detour {
    struct NavMeshCreateParams {
        const unsigned short* verts;			//多边形网格顶点.  [(x, y, z) * #vertCount] [Unit: vx]
        int vertCount;							//多边形网格顶点数量. [Limit: >= 3]
        const unsigned short* polys;			//多边形数据. [Size: #polyCount * 2 * #nvp]
        const unsigned short* polyFlags;		//分配给每个多边形的用户定义的标志.   [Size: #polyCount]
        const unsigned char*  polyAreas;	    //分配给每个多边形的用户定义的区域ID. [Size: #polyCount]
        int polyCount;							//多边形数量 [Limit: >= 1]
        int nvp;								//每个多边形的最大顶点数. [Limit: >= 3]


        /**
         * @brief 高度细节属性 (可选项)
         * 
         */
        const unsigned int* detailMeshes;		//高度细节子网格数据. [Size: 4 * #polyCount]
        const float*        detailVerts;		//网格顶点的细节.     [Size: 3 * #detailVertsCount] [Unit: wu]
        int detailVertsCount;					//细节网格中的顶点数.
        const unsigned char* detailTris;		//细节网格三角形. [Size: 4 * #detailTriCount]
        int detailTriCount;						//细节网格中三角形的数量.


        /**
         * @brief off-mesh 连接属性(可选)
         *        用于定义导航图中的自定义点到点边off-mesh连接是用户定义的可遍历连接，由两个顶点组成，
         *        其中至少有一个位于导航网格多边形内.
         */

        /// off-mesh 连接顶点. [(ax, ay, az, bx, by, bz) * #offMeshConCount] [Unit: wu]
        const float* offMeshConVerts;
        /// off-mesh 链接半径. [Size: #offMeshConCount] [Unit: wu]
        const float* offMeshConRad;
        /// 分配给off-mesh连接的用户定义标志.   [Size: #offMeshConCount]
        const unsigned short* offMeshConFlags;
        /// 分配给off-mesh连接的用户定义区域ID. [Size: #offMeshConCount]
        const unsigned char* offMeshConAreas;
        /**
         * @brief off-mesh连接的允许行走的方向.
         * 
         * 0 = 仅从端点A移动到端点B.
         * #OFFMESH_CON_BIDIR = 双向行走.
         */
        const unsigned char* offMeshConDir;	
        /// 用户定义off-mesh connections的ID.  [Size: #offMeshConCount]
        const unsigned int* offMeshConUserID;
        /// off-mesh connections的数量. [Limit: >= 0]
        int offMeshConCount;


        /**
         * @brief tile 属性
         * @note  如果目标是单个tile mesh，网格/图层数据可以保留为零.
         * 
         */
        unsigned int userId;	// 用户为这个tile定义的ID.
        int tileX;				// The tile's x-grid location within the multi-tile destination mesh. (Along the x-axis.)
        int tileY;				// The tile's y-grid location within the multi-tile desitation mesh. (Along the z-axis.)
        int tileLayer;			// The tile's layer within the layered destination mesh. [Limit: >= 0] (Along the y-axis.)
        float bmin[3];			// tile的最小界限. [(x, y, z)] [Unit: wu]
        float bmax[3];			// tile的最大界限. [(x, y, z)] [Unit: wu]



        /**
         * @brief 构建配置属性
         * 
         */
        float walkableHeight;	// 可通过的高度. [Unit: wu]
        float walkableRadius;	// 可通过的半径. [Unit: wu]
        float walkableClimb;	// 可通过的最大坡度. (Up/Down) [Unit: wu]
        float cs;				// 多边形网格的xz平面单元大小. [Limit: > 0] [Unit: wu]
        float ch;				// 多边形网格的y轴单元高度. [Limit: > 0] [Unit: wu]

        /**
         * @brief 如果应为tile构建边界体积树，则为True。
         * @note  分层导航网格通常不需要BVTree.
         * 
         */
        bool buildBvTree;
    };


    /**
     * @brief 根据提供的参数构建导航网格数据.
     * 
     * @param params                NavMesh构建参数
     * @param[out] outData          输出的tile数据.
     * @param[out] outDataSize      输出的tile数据组大小.
     * @return true  如果创建成功，则返回True
     * @return false 
     */
    bool CreateNavMeshData(NavMeshCreateParams* params, unsigned char** outData, int* outDataSize);

    
    /**
     * @brief 变换tile数据头的字节顺序 (#MeshHeader)
     * 
     * @param[in,out] data       tile数据组
     * @param[in]     dataSize   数据组大小
     * @return true 
     * @return false 
     */
    bool NavMeshHeaderSwapEndian(unsigned char* data, const int dataSize);

    /**
     * @brief 变换tile数据字节顺序
     * 
     * @param[in,out] data      tile数据组
     * @param[in]     dataSize  数据组大小
     * @return true 
     * @return false 
     */
    bool NavMeshDataSwapEndian(unsigned char* data, const int dataSize);
}

#endif 