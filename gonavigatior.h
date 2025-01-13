
#include <stdint.h>

#if defined (__cplusplus)
extern "C" {
#endif

enum GLoaderMode {
    GLM_SOLO = 0,
    GLM_TEMPOBSTACLE,
};

enum GPlatform {
    GPM_NOMAL = 0,
    GPM_UNITY,
    GPM_UE4,
};

struct GLoader {
    void *__ptrLoader;
};

struct GNavigatior {
    void *__ptrNavigation;
    void *__ptrPolyBuffer;
    void *__ptrSmothBuffer;
    int   __nPolyBufferSize;
    int   __nSmothBufferSize;
};


struct GLoader* NewLoader(int mode, int platform, const char *filename, int maxnode);

void FreeLoader(struct GLoader *ploader);

struct GNavigatior* NewNavigatior(struct GLoader *ploader,int maxPolys,int maxSmootPath);

void  FreeNavigatior(struct GNavigatior* p);

//find //add
int FindPath(struct GNavigatior* pgn, float sp[3], float ep[3]);

void AddObstacle(struct GNavigatior* pgn,float pos[3]);

void RemoveObstacle(struct GNavigatior* pgn,int idx);

void RemoveAllObstacle(struct GNavigatior* pgn);

void Update(struct GNavigatior* pgn,const float dt, void* upToDate);


void getPathPoint(struct GNavigatior* pgn,int i,float* pos);

#if defined (__cplusplus)
}
#endif