#include "_obj/_cgo_export.h"
#include "gonavigatior.h"
#include "navigation.h"
#include "dalloc.h"
#include "unitytempobstacleloader.h"
#include "unitysololoader.h"
struct GLoader *NewLoader(int mode,
                          int platform,
                          const char *filename,
                          int maxnode){
        GLoader *ploader = (GLoader *)Detour::Alloc(sizeof(struct GLoader), Detour::ALLOC_PERM);
        if ((GLoaderMode)mode == GLoaderMode::GLM_SOLO){
            if((GPlatform)platform == GPlatform::GPM_NOMAL){
                ploader->__ptrLoader = new SoloLoader();
            }else if((GPlatform)platform == GPlatform::GPM_UNITY){
                ploader->__ptrLoader = new UnitySoloLoader();
            }else if ((GPlatform)platform == GPlatform ::GPM_UE4){
            }
        }else {
            if((GPlatform)platform == GPlatform::GPM_NOMAL){
                ploader->__ptrLoader = new TempObstacleLoader();
            }else if((GPlatform)platform == GPlatform::GPM_UNITY){
                ploader->__ptrLoader = new UnityTempObstacleLoader();
            }else if ((GPlatform)platform == GPlatform ::GPM_UE4){
            }
        }
        if (!ploader->__ptrLoader || !((ILoader*)ploader->__ptrLoader)->LoadNavMesh(filename,maxnode)){
            FreeLoader(ploader);
            ploader = NULL;
        }
    return ploader;
}

void FreeLoader(struct GLoader *ploader){
    if (ploader->__ptrLoader != NULL){
        Detour::Free(ploader->__ptrLoader);
        ploader->__ptrLoader = NULL;
    }
    Detour::Free(ploader);
}

struct GNavigatior *NewNavigatior(struct GLoader *ploader,int maxPolys = 256,int maxSmootPath = 2048){
    if (ILoader *ptrloader = static_cast<ILoader *>(ploader->__ptrLoader)){
            GNavigatior *pnav = (GNavigatior *)Detour::Alloc(sizeof(struct GNavigatior), Detour::ALLOC_PERM);
            pnav->__ptrNavigation = new Navigation(ptrloader);
            pnav->__ptrPolyBuffer = AllocPolyBuffer(maxPolys);
            pnav->__nPolyBufferSize = maxPolys;
            pnav->__ptrSmothBuffer = AllocSmoothPathBuffer(maxSmootPath);
            pnav->__nSmothBufferSize = maxSmootPath;
            return pnav;
    }
    return NULL;
}

void FreeNavigatior(struct GNavigatior *p){
    if (p->__ptrNavigation != NULL){
        Navigation *pnav = (Navigation *)p->__ptrNavigation;
        delete pnav;
        p->__ptrNavigation = NULL;
    }

    if (p->__ptrPolyBuffer != NULL){
        FreePolyBuffer((PolyRef *)p->__ptrPolyBuffer);
        p->__ptrPolyBuffer = NULL;
    }

    if (p->__ptrSmothBuffer != NULL){
        FreeSmootPathBuffer((float *)p->__ptrSmothBuffer);
        p->__ptrSmothBuffer = NULL;
    }
    Detour::Free(p);
}

int FindPath(struct GNavigatior *pgn, float sp[3], float ep[3]){
    if (Navigation *pnav = static_cast<Navigation *>(pgn->__ptrNavigation)){
        if (pgn->__ptrPolyBuffer != NULL && pgn->__ptrSmothBuffer != NULL){
            return pnav->FilePath(sp, 
                                  ep, 
                                  (PolyRef *)pgn->__ptrPolyBuffer, 
                                  (float *)pgn->__ptrSmothBuffer, 
                                  pgn->__nPolyBufferSize, 
                                  pgn->__nSmothBufferSize);
        }
    }
    return 0;
}

void getPathPoint(struct GNavigatior* pgn,int i,float* pos){
	pos[0] = (((float*)pgn->__ptrSmothBuffer)[i*3+0]);
	pos[1] = (((float*)pgn->__ptrSmothBuffer)[i*3+1]);
	pos[2] = (((float*)pgn->__ptrSmothBuffer)[i*3+2]);
}

void AddObstacle(struct GNavigatior *pgn, float pos[3]){
    Navigation *pnav = static_cast<Navigation *>(pgn->__ptrNavigation);
    pnav->AddObstacle(pos);
}

void RemoveObstacle(struct GNavigatior *pgn, int idx){
    Navigation *pnav = static_cast<Navigation *>(pgn->__ptrNavigation);
    pnav->RemoveObstacle(idx);
}

void RemoveAllObstacle(struct GNavigatior *pgn){
    Navigation *pnav = static_cast<Navigation *>(pgn->__ptrNavigation);
    pnav->RemoveObstacle();
}

void Update(struct GNavigatior *pgn, const float dt, void* upToDate = NULL){
    Navigation *pnav = static_cast<Navigation *>(pgn->__ptrNavigation);
    pnav->Update(dt,(bool*)upToDate);
}