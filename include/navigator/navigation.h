#ifndef MAGIC_ENGINE_NAVIGATION_INAVIGATION_H_H
#define MAGIC_ENGINE_NAVIGATION_INAVIGATION_H_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "navmesh.h"
#include "navmeshquery.h"
#include "dcommon.h"
#include "iloader.h"

class Navigation {
    protected:
      

        static inline  int rcMin(int a, int b) { return a < b ? a : b; }

        static inline  int rcMax(int a, int b) { return a > b ? a : b; }

        static inline bool inRange(const float* v1, const float* v2, const float r, const float h) {
            const float dx = v2[0] - v1[0];
            const float dy = v2[1] - v1[1];
            const float dz = v2[2] - v1[2];
            return (dx*dx + dz*dz) < r*r && fabsf(dy) < h;
        }
        

        static bool getSteerTarget(Detour::NavMeshQuery* ptrNavQuery, 
                                   const float*   startPos, 
                                   const float*   endPos,
                                   const float    minTargetDist,
                                   const PolyRef* path, 
                                   const int      pathSize,
                                   float*         steerPos, 
                                   unsigned char& steerPosFlag, 
                                   PolyRef&       steerPosRef,
                                   float*         outPoints = 0, 
                                   int*           outPointCount = 0) {
            // Find steer target.
            static const int MAX_STEER_POINTS = 3;
            float steerPath[MAX_STEER_POINTS * 3];
            unsigned char steerPathFlags[MAX_STEER_POINTS];
            PolyRef steerPathPolys[MAX_STEER_POINTS];
            int nsteerPath = 0;
            ptrNavQuery->FindStraightPath(startPos, endPos, path, pathSize,
                steerPath, steerPathFlags, steerPathPolys, &nsteerPath, MAX_STEER_POINTS);
            if (!nsteerPath)
                return false;

            if (outPoints && outPointCount) {
                *outPointCount = nsteerPath;
                for (int i = 0; i < nsteerPath; ++i)
                    Detour::Vcopy(&outPoints[i * 3], &steerPath[i * 3]);
            }


            // Find vertex far enough to steer to.
            int ns = 0;
            while (ns < nsteerPath) {
                // Stop at Off-Mesh link or when point is further than slop away.
                if ((steerPathFlags[ns] & Detour::STRAIGHTPATH_OFFMESH_CONNECTION) ||
                    !inRange(&steerPath[ns * 3], startPos, minTargetDist, 1000.0f))
                    break;
                ns++;
            }
            // Failed to find good point to steer to.
            if (ns >= nsteerPath)
                return false;

            Detour::Vcopy(steerPos, &steerPath[ns * 3]);
            steerPos[1] = startPos[1];
            steerPosFlag = steerPathFlags[ns];
            steerPosRef = steerPathPolys[ns];

            return true;
        }

        static int fixupCorridor(PolyRef* path, const int npath, const int maxPath,
            const PolyRef* visited, const int nvisited) {
            int furthestPath = -1;
            int furthestVisited = -1;

            // Find furthest common polygon.
            for (int i = npath - 1; i >= 0; --i) {
                bool found = false;
                for (int j = nvisited - 1; j >= 0; --j) {
                    if (path[i] == visited[j]) {
                        furthestPath = i;
                        furthestVisited = j;
                        found = true;
                    }
                }
                if (found)
                    break;
            }

            // If no intersection found just return current path. 
            if (furthestPath == -1 || furthestVisited == -1)
                return npath;

            // Concatenate paths.	

            // Adjust beginning of the buffer to include the visited.
            const int req = nvisited - furthestVisited;
            const int orig = rcMin(furthestPath + 1, npath);
            int size = rcMax(0, npath - orig);
            if (req + size > maxPath)
                size = maxPath - req;
            if (size)
                memmove(path + req, path + orig, size * sizeof(PolyRef));

            // Store visited
            for (int i = 0; i < req; ++i)
                path[i] = visited[(nvisited - 1) - i];

            return req + size;
        }

        // This function checks if the path has a small U-turn, that is,
        // a polygon further in the path is adjacent to the first polygon
        // in the path. If that happens, a shortcut is taken.
        // This can happen if the target (T) location is at tile boundary,
        // and we're (S) approaching it parallel to the tile edge.
        // The choice at the vertex can be arbitrary, 
        //  +---+---+
        //  |:::|:::|
        //  +-S-+-T-+
        //  |:::|   | <-- the step can end up in here, resulting U-turn path.
        //  +---+---+
        static int fixupShortcuts(PolyRef* path, int npath, Detour::NavMeshQuery* ptrNavQuery) {
            if (npath < 3)
                return npath;

            // Get connected polygons
            static const int maxNeis = 16;
            PolyRef neis[maxNeis];
            int nneis = 0;

            const Detour::MeshTile* tile = 0;
            const Detour::Poly* poly = 0;
            if (Detour::IsStatusFailed(ptrNavQuery->GetAttachedNavMesh()->GetTileAndPolyByRef(path[0], &tile, &poly)))
                return npath;

            for (unsigned int k = poly->firstLink; k != Detour::NULL_LINK; k = tile->links[k].next) {
                const Detour::Link* link = &tile->links[k];
                if (link->ref != 0) {
                    if (nneis < maxNeis)
                        neis[nneis++] = link->ref;
                }
            }

            // If any of the neighbour polygons is within the next few polygons
            // in the path, short cut to that polygon directly.
            static const int maxLookAhead = 6;
            int cut = 0;
            for (int i = Detour::Min(maxLookAhead, npath) - 1; i > 1 && cut == 0; i--) {
                for (int j = 0; j < nneis; j++) {
                    if (path[i] == neis[j]) {
                        cut = i;
                        break;
                    }
                }
            }

            if (cut > 1) {
                int offset = cut - 1;
                npath -= offset;
                for (int i = 1; i < npath; i++)
                    path[i] = path[i + offset];
            }

            return npath;
        }
    public:
        Navigation(ILoader *ptrLoader);
        virtual ~Navigation();

        /**
         * @brief 查找路径
         * 
         * @param sp 起始位置             [x, y, z]
         * @param ep 结束位置             [x, y, z]
         * @param polyBuffer              多边形缓冲区
         * @param smoothPathBuffer        平滑路径缓冲区(最终结果)[float * smoothPathBufferMax * 3]
         * @param polyBufferMax           多边形缓冲区大小(数量)
         * @param smoothPathBufferMax     平滑路径缓冲区大小(数量)
         * @return int 
         */
        int FilePath(float    sp[3], 
                     float    ep[3],
                     PolyRef *polyBuffer,
                     float   *smoothPathBuffer,
                     int      polyBufferMax = 256, 
                     int      smoothPathBufferMax = 2048);
        /**
         * @brief 增加一个动态对象,只在TempObstacle下有效
         * 
         * @param p 
         */
        void AddObstacle(float *p);
        /**
         * @brief 删除一个动态对象,只在TempObstacle下有效
         * 
         * @param idx 
         */
        void RemoveObstacle(int idx);
        /**
         * @brief 删除所有动态对象,只在TempObstacle下有效
         * 
         */
        void RemoveObstacle();
        /**
         * @brief 更行状态
         * @note  建议设定更新频率或帧更新,只在TempObstacle下有效
         * @param dt 
         * @param upToDate 
         */
        void Update(const float dt, bool* upToDate = NULL);
    protected:
        ILoader                 *m_ptrLoader;
        Detour::NavMeshQuery    *m_ptrNavQuery;
        Detour::QueryFilter      m_filter;
};


PolyRef *AllocPolyBuffer(int MaxPolys = 256);
void     FreePolyBuffer(PolyRef* ptr);

float   *AllocSmoothPathBuffer(int MaxSmootPath = 2048);
void     FreeSmootPathBuffer(float *ptr);


#endif 