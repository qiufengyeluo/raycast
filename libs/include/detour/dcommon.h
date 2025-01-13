#ifndef MAGIC_ENGINE_NAVIGATION_DETOUR_COMMON_H_H
#define MAGIC_ENGINE_NAVIGATION_DETOUR_COMMON_H_H

#include "dmath.h"

namespace Detour {
    /// 用于忽略函数参数。VS抱怨未使用的参数
    /// and this silences the warning.
    ///  @param [in] _ Unused parameter
    template<class T> void IgnoreUnused(const T&) { }

    /// 交换两个参数的值.
    ///  @param[in,out]	a	Value A
    ///  @param[in,out]	b	Value B
    template<class T> inline void Swap(T& a, T& b) { T t = a; a = b; b = t; }

    /**
     * @brief 返回最小值
     * 
     * @tparam T 
     * @param a  Value A
     * @param b  Value b
     * @return T 返回最小值
     */
    template<class T> inline T Min(T a, T b) { return a < b ? a : b; }

    /**
     * @brief 返回最大值
     * 
     * @tparam T 
     * @param a  Value A
     * @param b  Value A
     * @return T 返回最大值
     */
    template<class T> inline T Max(T a, T b) { return a > b ? a : b; }

    /**
     * @brief 取绝对值
     * 
     * @tparam T 
     * @param a 
     * @return T 
     */
    template<class T> inline T Abs(T a) { return a < 0 ? -a : a; }

    /**
     * @brief 2次幂
     * 
     * @tparam T 
     * @param a 
     * @return T 
     */
    template<class T> inline T Sqr(T a) { return a*a; }

    /**
     * @brief 将该值钳制到指定的范围
     * 
     * @tparam T 
     * @param v 
     * @param mn 
     * @param mx 
     * @return T 
     */
    template<class T> inline T Clamp(T v, T mn, T mx) { return v < mn ? mn : (v > mx ? mx : v); }

    /**
     * @brief 导出两个向量的叉积
     * 
     * @param dest 
     * @param v1 
     * @param v2 
     */
    inline void Vcross(float* dest, const float* v1, const float* v2) {
        dest[0] = v1[1]*v2[2] - v1[2]*v2[1];
        dest[1] = v1[2]*v2[0] - v1[0]*v2[2];
        dest[2] = v1[0]*v2[1] - v1[1]*v2[0]; 
    }


    /**
     * @brief  导出两个向量的点积.
     * 
     * @param v1 
     * @param v2 
     * @return float 
     */
    inline float Vdot(const float* v1, const float* v2) {
        return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
    }

    /// 执行缩放向量加法. (@p v1 + (@p v2 * @p s))
    ///  @param[out]	dest	The result vector. [(x, y, z)]
    ///  @param[in]		v1		The base vector. [(x, y, z)]
    ///  @param[in]		v2		The vector to scale and add to @p v1. [(x, y, z)]
    ///  @param[in]		s		The amount to scale @p v2 by before adding to @p v1.
    inline void Vmad(float* dest, const float* v1, const float* v2, const float s) {
        dest[0] = v1[0]+v2[0]*s;
        dest[1] = v1[1]+v2[1]*s;
        dest[2] = v1[2]+v2[2]*s;
    }

    /// 在两个向量之间执行线性插值. (@p v1 toward @p v2)
    ///  @param[out]	dest	The result vector. [(x, y, x)]
    ///  @param[in]		v1		The starting vector.
    ///  @param[in]		v2		The destination vector.
    ///	 @param[in]		t		The interpolation factor. [Limits: 0 <= value <= 1.0]
    inline void Vlerp(float* dest, const float* v1, const float* v2, const float t) {
        dest[0] = v1[0]+(v2[0]-v1[0])*t;
        dest[1] = v1[1]+(v2[1]-v1[1])*t;
        dest[2] = v1[2]+(v2[2]-v1[2])*t;
    }

    /// 执行向量加法. (@p v1 + @p v2)
    ///  @param[out]	dest	The result vector. [(x, y, z)]
    ///  @param[in]		v1		The base vector. [(x, y, z)]
    ///  @param[in]		v2		The vector to add to @p v1. [(x, y, z)]
    inline void Vadd(float* dest, const float* v1, const float* v2) {
        dest[0] = v1[0]+v2[0];
        dest[1] = v1[1]+v2[1];
        dest[2] = v1[2]+v2[2];
    }

    ///执行向量减法. (@p v1 - @p v2)
    ///  @param[out]	dest	The result vector. [(x, y, z)]
    ///  @param[in]		v1		The base vector. [(x, y, z)]
    ///  @param[in]		v2		The vector to subtract from @p v1. [(x, y, z)]
    inline void Vsub(float* dest, const float* v1, const float* v2) {
        dest[0] = v1[0]-v2[0];
        dest[1] = v1[1]-v2[1];
        dest[2] = v1[2]-v2[2];
    }

    /// 按指定值缩放向量. (@p v * @p t)
    ///  @param[out]	dest	The result vector. [(x, y, z)]
    ///  @param[in]		v		The vector to scale. [(x, y, z)]
    ///  @param[in]		t		The scaling factor.
    inline void Vscale(float* dest, const float* v, const float t)
    {
        dest[0] = v[0]*t;
        dest[1] = v[1]*t;
        dest[2] = v[2]*t;
    }

    /// 从指定向量中选择每个元素的最小值.
    ///  @param[in,out]	mn	A vector.  (Will be updated with the result.) [(x, y, z)]
    ///  @param[in]	v	A vector. [(x, y, z)]
    inline void Vmin(float* mn, const float* v) {
        mn[0] = Min(mn[0], v[0]);
        mn[1] = Min(mn[1], v[1]);
        mn[2] = Min(mn[2], v[2]);
    }

    /// 从指定向量中选择每个元素的最大值.
    ///  @param[in,out]	mx	A vector.  (Will be updated with the result.) [(x, y, z)]
    ///  @param[in]		v	A vector. [(x, y, z)]
    inline void Vmax(float* mx, const float* v) {
        mx[0] = Max(mx[0], v[0]);
        mx[1] = Max(mx[1], v[1]);
        mx[2] = Max(mx[2], v[2]);
    }

    /// 将向量元素设置为指定的值.
    ///  @param[out]	dest	The result vector. [(x, y, z)]
    ///  @param[in]		x		The x-value of the vector.
    ///  @param[in]		y		The y-value of the vector.
    ///  @param[in]		z		The z-value of the vector.
    inline void Vset(float* dest, const float x, const float y, const float z) {
        dest[0] = x; dest[1] = y; dest[2] = z;
    }

    inline void Vcopy(float* dest, const float* a) {
	    dest[0] = a[0];
	    dest[1] = a[1];
	    dest[2] = a[2];
    }

    /**
     * @brief 计算向量的标量长度
     * 
     * @param v 
     * @return float 
     */
    inline float Vlen(const float* v) {
        return MathSqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    }


    /**
     * @brief 连点间距离
     * 
     * @param v1 
     * @param v2 
     * @return float 
     */
    inline float Vdist(const float* v1, const float* v2) {
        const float dx = v2[0] - v1[0];
        const float dy = v2[1] - v1[1];
        const float dz = v2[2] - v1[2];
        return MathSqrtf(dx*dx + dy*dy + dz*dz);
    }

    /**
     * @brief   返回两点之间距离的平方.
     * 
     * @param v1    	A point. [(x, y, z)]
     * @param v2       	A point. [(x, y, z)]
     * @return float    两点之间距离的平方。
     */
    inline float VdistSqr(const float* v1, const float* v2) {
        const float dx = v2[0] - v1[0];
        const float dy = v2[1] - v1[1];
        const float dz = v2[2] - v1[2];
        return dx*dx + dy*dy + dz*dz;
    }



    /**
     * @brief 
     * 
     * @param v 
     * @return float 
     */
    inline float VlenSqr(const float* v) {
	    return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    }

    /// 
    ///  @param[in]		v1	A point. [(x, y, z)]
    ///  @param[in]		v2	A point. [(x, y, z)]
    /// @return The distance between the point on the xz-plane.
    ///
    /// The vectors are projected onto the xz-plane, so the y-values are ignored.


    /**
     * @brief 导出xz平面上指定点之间的距离.
     *        向量投影到xz平面上，因此y值被忽略
     * @param v1 A point. [(x, y, z)]
     * @param v2 A point. [(x, y, z)]
     * @return float  xz平面上点之间的距离。
     */
    inline float Vdist2D(const float* v1, const float* v2) {
        const float dx = v2[0] - v1[0];
        const float dz = v2[2] - v1[2];
        return MathSqrtf(dx*dx + dz*dz);
    }

    /**
     * @brief 导出xz平面上指定点之间距离的平方。
     * 
     * @param v1  A point. [(x, y, z)]
     * @param v2  A point. [(x, y, z)]
     * @return float xz平面上点之间距离的平方。
     */
    inline float Vdist2DSqr(const float* v1, const float* v2) {
        const float dx = v2[0] - v1[0];
        const float dz = v2[2] - v1[2];
        return dx*dx + dz*dz;
    }
    /**
     * @brief 规范化向量
     * 
     * @param v[in,out] 要规格化的向量。[（x，y，z）]
     */
    inline void Vnormalize(float* v) {
        float d = 1.0f / MathSqrtf(Sqr(v[0]) + Sqr(v[1]) + Sqr(v[2]));
        v[0] *= d;
        v[1] *= d;
        v[2] *= d;
    }

    /**
     * @brief 对指定的点执行 'sloppy' 的托管检查
     * 
     * @param p0      A point. [(x, y, z)]
     * @param p1      A point. [(x, y, z)]
     * @return true   如果认为这些点位于同一位置，则为True
     * @return false 
     */
    inline bool Vequal(const float* p0, const float* p1) {
        static const float thr = Sqr(1.0f/16384.0f);
        const float d = VdistSqr(p0, p1);
        return d < thr;
    }


    /**
     * @brief 导出xz平面上两个向量的点积.
     *        向量投影到xz平面上，因此y值被忽略.
     * @param u  A vector [(x, y, z)].
     * @param v  A vector [(x, y, z)].
     * @return float xz平面上的点积.
     */
    inline float Vdot2D(const float* u, const float* v) {
        return u[0]*v[0] + u[2]*v[2];
    }

    /**
     * @brief  导出两个向量的xz平面2D perp乘积。（uz*vx-ux*vz）
     *         向量投影到xz平面上，因此y值被忽略.
     * @param u  The LHV vector [(x, y, z)]
     * @param v  The RHV vector [(x, y, z)]
     * @return float xz平面上的点积。
     */
    inline float Vperp2D(const float* u, const float* v) {
        return u[2]*v[0] - u[0]*v[2];
    }


    /**
     * @brief 导出三角形ABC的有符号xz平面面积，或直线AB与点C的关系。
     * 
     * @param a       Vertex A. [(x, y, z)]
     * @param b       Vertex B. [(x, y, z)]
     * @param c       Vertex C. [(x, y, z)]
     * @return float  三角形的有符号xz平面面积.
     */
    inline float TriArea2D(const float* a, const float* b, const float* c) {
        const float abx = b[0] - a[0];
        const float abz = b[2] - a[2];
        const float acx = c[0] - a[0];
        const float acz = c[2] - a[2];
        return acx*abz - abx*acz;
    }

    /**
     * @brief 确定两个轴对齐的边界框是否重叠.
     * 
     * @param amin      Minimum bounds of box A. [(x, y, z)]
     * @param amax      Maximum bounds of box A. [(x, y, z)]
     * @param bmin      Minimum bounds of box B. [(x, y, z)] 
     * @param bmax      Maximum bounds of box B. [(x, y, z)]
     * @return true     如果两个AABB重叠，则为True
     * @return false 
     * @see OverlapBounds
     */
    inline bool OverlapQuantBounds(const unsigned short amin[3], const unsigned short amax[3],
                                    const unsigned short bmin[3], const unsigned short bmax[3]) {
        bool overlap = true;
        overlap = (amin[0] > bmax[0] || amax[0] < bmin[0]) ? false : overlap;
        overlap = (amin[1] > bmax[1] || amax[1] < bmin[1]) ? false : overlap;
        overlap = (amin[2] > bmax[2] || amax[2] < bmin[2]) ? false : overlap;
        return overlap;
    }


    /**
     * @brief 确定两个轴对齐的边界框是否重叠.
     * 
     * @param amin        Minimum bounds of box A. [(x, y, z)]
     * @param amax        Maximum bounds of box A. [(x, y, z)]
     * @param bmin        Minimum bounds of box B. [(x, y, z)]
     * @param bmax        Maximum bounds of box B. [(x, y, z)]
     * @return true        如果两个AABB重叠，则为True
     * @return false 
     * @see OverlapQuantBounds
     */
    inline bool OverlapBounds(const float* amin, const float* amax,
                                const float* bmin, const float* bmax) {
        bool overlap = true;
        overlap = (amin[0] > bmax[0] || amax[0] < bmin[0]) ? false : overlap;
        overlap = (amin[1] > bmax[1] || amax[1] < bmin[1]) ? false : overlap;
        overlap = (amin[2] > bmax[2] || amax[2] < bmin[2]) ? false : overlap;
        return overlap;
    }

    /**
     * @brief 从指定的参考点导出三角形上最近的点.
     * 
     * @param closest          三角形上最近的点
     * @param p                要测试的参考点。[（x，y，z）]
     * @param a                Vertex A of triangle ABC. [(x, y, z)]
     * @param b                Vertex B of triangle ABC. [(x, y, z)]
     * @param c                Vertex C of triangle ABC. [(x, y, z)]
     */
    void ClosestPtPointTriangle(float* closest, const float* p,
                                const float* a, const float* b, const float* c);


    /**
     * @brief             指定的参考点导出三角形上最近点的y轴高度。
     * 
     * @param p           要测试的参考点。[（x，y，z）]
     * @param a           Vertex A of triangle ABC. [(x, y, z)]
     * @param b           Vertex B of triangle ABC. [(x, y, z)]
     * @param c           Vertex C of triangle ABC. [(x, y, z)]
     * @param h[out]      由此产生的高度.
     * @return true 
     * @return false 
     */
    bool ClosestHeightPointTriangle(const float* p, const float* a, const float* b, const float* c, float& h);

    bool IntersectSegmentPoly2D(const float* p0, const float* p1,
                                const float* verts, int nverts,
                                float& tmin, float& tmax,
                                int& segMin, int& segMax);

    bool IntersectSegSeg2D(const float* ap, const float* aq,
                            const float* bp, const float* bq,
                            float& s, float& t);

    /// Determines if the specified point is inside the convex polygon on the xz-plane.
    ///  @param[in]		pt		The point to check. [(x, y, z)]
    ///  @param[in]		verts	The polygon vertices. [(x, y, z) * @p nverts]
    ///  @param[in]		nverts	The number of vertices. [Limit: >= 3]
    /// @return True if the point is inside the polygon.

    /**
     * @brief 确定指定点是否位于xz平面上的凸多边形内.
     * 
     * @param pt     The point to check. [(x, y, z)]
     * @param verts  The polygon vertices. [(x, y, z) * @p nverts]
     * @param nverts The number of vertices. [Limit: >= 3]
     * @return true  如果点位于多边形内，则为True。
     * @return false 
     */
    bool PointInPolygon(const float* pt, const float* verts, const int nverts);

    bool DistancePtPolyEdgesSqr(const float* pt, const float* verts, const int nverts,
                                float* ed, float* et);

    float DistancePtSegSqr2D(const float* pt, const float* p, const float* q, float& t);

    /**
     * @brief  导出凸多边形的质心.
     * 
     * @param tc         polgyon的质心. [（x，y，z）]
     * @param idx        多边形索引. [（vertIndex）*@p nidx]
     * @param nidx       多边形中的索引数。[Limit：>=3]
     * @param verts      多边形的顶点。[（x，y，z）*vertCount]
     */
    void CalcPolyCenter(float* tc, const unsigned short* idx, int nidx, const float* verts);

    /**
     * @brief         确定两个凸多边形是否在xz平面上重叠.
     * 
     * @param polya   多边形是一个顶点 A. [（x，y，z）*@p npolya]
     * @param npolya  多边形A中的顶点数.
     * @param polyb   多边形是一个顶点 B. [（x，y，z）*@p npolya]
     * @param npolyb  多边形B中的顶点数.
     * @return true   如果两个多边形重叠，则为True.
     * @return false 
     */
    bool OverlapPolyPoly2D(const float* polya, const int npolya,
                            const float* polyb, const int npolyb);


    inline unsigned int NextPow2(unsigned int v) {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return v;
    }

    inline unsigned int Ilog2(unsigned int v) {
        unsigned int r;
        unsigned int shift;
        r = (v > 0xffff) << 4; v >>= r;
        shift = (v > 0xff) << 3; v >>= shift; r |= shift;
        shift = (v > 0xf) << 2; v >>= shift; r |= shift;
        shift = (v > 0x3) << 1; v >>= shift; r |= shift;
        r |= (v >> 1);
        return r;
    }

    inline int Align4(int x) { return (x+3) & ~3; }

    inline int OppositeTile(int side) { return (side+4) & 0x7; }

    inline void SwapByte(unsigned char* a, unsigned char* b) {
        unsigned char tmp = *a;
        *a = *b;
        *b = tmp;
    }

    inline void SwapEndian(unsigned short* v) {
        unsigned char* x = (unsigned char*)v;
        SwapByte(x+0, x+1);
    }

    inline void SwapEndian(short* v) {
        unsigned char* x = (unsigned char*)v;
        SwapByte(x+0, x+1);
    }

    inline void SwapEndian(unsigned int* v) {
        unsigned char* x = (unsigned char*)v;
        SwapByte(x+0, x+3); SwapByte(x+1, x+2);
    }

    inline void SwapEndian(int* v){
        unsigned char* x = (unsigned char*)v;
        SwapByte(x+0, x+3); SwapByte(x+1, x+2);
    }

    inline void SwapEndian(float* v) {
        unsigned char* x = (unsigned char*)v;
        SwapByte(x+0, x+3); SwapByte(x+1, x+2);
    }

    void RandomPointInConvexPoly(const float* pts, const int npts, float* areas,
							   const float s, const float t, float* out);

    template<typename TypeToRetrieveAs>
    TypeToRetrieveAs* GetThenAdvanceBufferPointer(const unsigned char*& buffer, const size_t distanceToAdvance) {
        TypeToRetrieveAs* returnPointer = reinterpret_cast<TypeToRetrieveAs*>(buffer);
        buffer += distanceToAdvance;
        return returnPointer;
    }

    template<typename TypeToRetrieveAs>
    TypeToRetrieveAs* GetThenAdvanceBufferPointer(unsigned char*& buffer, const size_t distanceToAdvance) {
        TypeToRetrieveAs* returnPointer = reinterpret_cast<TypeToRetrieveAs*>(buffer);
        buffer += distanceToAdvance;
        return returnPointer;
    }
}

#endif