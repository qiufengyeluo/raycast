#ifndef MAGIC_ENGINE_NAVIGATION_DETOUR_MATH_H_H
#define MAGIC_ENGINE_NAVIGATION_DETOUR_MATH_H_H

#include <math.h>

namespace Detour {
    /**
     * @brief 绝对值
     * 
     * @param x 
     * @return float 
     */
    inline float MathFabsf(float x)  {  return fabsf(x); }
    /**
     * @brief 求根
     * 
     * @param x 
     * @return float 
     */
    inline float MathSqrtf(float x)  {  return sqrtf(x); }
    /**
     * @brief 向下去整
     * 
     * @param x 
     * @return float 
     */
    inline float MathFloorf(float x) {  return floorf(x);}
    /**
     * @brief 截取整数，有余数就向上取整
     * 
     * @param x 
     * @return float 
     */
    inline float MathCeilf(float x)  {  return ceilf(x); }
    /**
     * @brief 余玄
     * 
     * @param x 
     * @return float 
     */
    inline float MathCosf(float x)   {  return cosf(x);  }
    /**
     * @brief 正玄
     * 
     * @param x 
     * @return float 
     */
    inline float MathSinf(float x)   {  return sinf(x);  }
    /**
     * @brief 反正切
     * 
     * @param y 
     * @param x 
     * @return float 
     */
    inline float MathAtan2f(float y, float x) { return atan2f(y,x); }
}

#endif 