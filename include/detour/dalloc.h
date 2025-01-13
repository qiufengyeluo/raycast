#ifndef MAGIC_ENGINE_NAVIGATION_DETOUR_ALLOC_H_H
#define MAGIC_ENGINE_NAVIGATION_DETOUR_ALLOC_H_H

#include <stddef.h>

namespace Detour {

    /**
     * @brief 内存生存周期
     * 
     */
    enum AllocHit {
        ALLOC_PERM,   //固定内存
        ALLOC_TEMP    //临时内存
    };

    /**
    * @brief 内存分配函数
    * 
    * @param  (size_t)size    分配内存大小
    * @param  (AllocHit)hint  内存使用身存周期
    * @return 返回分配的内存，失败返回NULL
    */
    typedef void* (AllocFunc)(size_t size, AllocHit hint);

    /**
    * @brief 内存释放函数
    * 
    * @param  (void*)ptr   需要释放的内存对象
    * @desc AllocSet 设置自定义内存分配释放函数
    */
    typedef void (FreeFunc)(void* ptr);

    /**
    * @brief 设置自定义内存分配释放函数
    * 
    * @param (AllocFunc*)allocFunc 分配函数
    * @param (FreeFunc*)freeFunc   释放函数
    */
    void AllocSet(AllocFunc *allocFunc, FreeFunc *freeFunc);

    /**
    * @brief 默认内存分配函数
    * 
    * @param (size_t)size   内存大小
    * @param (AllocHit)hint 内存生存周期
    * @return 返回分配的内存
    */
    void* Alloc(size_t size, AllocHit hint);

    /**
     * @brief 
     * 
     * @param ptr 
     */
    void  Free(void* ptr);
}


#endif