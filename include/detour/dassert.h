#ifndef MAGIC_ENGINE_NAVIGATION_DETOUR_ASSERT_H_H
#define MAGIC_ENGINE_NAVIGATION_DETOUR_ASSERT_H_H

#ifdef NDEBUG
        #define Assert(p) do { (void)sizeof(v); } while((void)(__LINE__==-1), false)
#else 
        #include <assert.h>
        #define Assert(e) \
                  {\
                        Detour::AssertFailFunc* failFunc = Detour::AssertFailGetCustom(); \
                        if (failFunc == NULL) { assert(e); } \
                        else if (!(e)) { (*failFunc)(#e, __FILE__, __LINE__);} \
                  }
#endif 

namespace Detour {
  
    #ifndef NDEBUG 
        typedef void AssertFailFunc(const char* expression, const char* file, int line);

        void AssertFailSetCustom(AssertFailFunc *pfunc);

        AssertFailFunc* AssertFailGetCustom();
    #endif
}

    
#endif