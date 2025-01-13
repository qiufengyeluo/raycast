package raycast

/*
#cgo CFLAGS: -I ${SRCDIR} -I ${SRCDIR}/libs/include
#cgo CXXFLAGS: -I ${SRCDIR} -I ${SRCDIR}/libs/include -I ${SRCDIR}/libs/include/navigator -I ${SRCDIR}/libs/include/detour -I ${SRCDIR}/libs/include/obstacle -I ${SRCDIR}/libs/include/navigator/unity  -I ${SRCDIR}/libs/include/navigator/standard
#cgo lnavigation LDFLAGS: -lnavigation
#cgo navigationa LDFLAGS: -lnavigation -lm -ldl
#cgo linux,!lnavigation,!navigationa LDFLAGS: -L${SRCDIR}/libs/linux -lnavigation
#cgo darwin,!lnavigation,!navigationa LDFLAGS: -lnavigation
#cgo freebsd,!navigationa LDFLAGS: -lnavigation
#cgo windows,!lnavigation LDFLAGS: -L${SRCDIR}/libs/win64 -lnavigation -lmingwex -lmingw32


#include <stdlib.h>
#include "gonavigatior.h"
*/
import "C"
import _ "unsafe"

type GOGLoaderMode int

const (
	GO_GLM_SOLO         = GOGLoaderMode(C.GLM_SOLO)
	GO_GLM_TEMPOBSTACLE = GOGLoaderMode(C.GLM_TEMPOBSTACLE)
)

type GOGPlatform int

const (
	GO_GPM_NOMAL = GOGPlatform(C.GPM_NOMAL)
	GO_GPM_UNITY = GOGPlatform(C.GPM_UNITY)
	GO_GPM_UE4   = GOGPlatform(C.GPM_UE4)
)

var (
	_allocer func() *Path  = nil
	_release func(p *Path) = nil
)

func SetAlloc(allocer func() *Path, release func(p *Path)) {
	_allocer = allocer
	_release = release
}

/**
 * @description:生成非托管内存
 * @param {GOGLoaderMode} mode 静态对象节点  动态对象节点
 * @param {GOGPlatform} platform 地图数据导出平台 Unity UE4
 * @param {string} filename  地图文件路径
 * @param {int} maxnode 最大节点数 默认2048
 * @return {*}
 */
func NewLoader(mode GOGLoaderMode, platform GOGPlatform, filename string, maxnode int) *Loader {
	Cstr := C.CString(filename)
	defer C.free(unsafe.Pointer(Cstr))
	ptr := &Loader{
		_ptr: C.NewLoader(C.int(mode), C.int(platform), Cstr, C.int(maxnode)),
	}
	return ptr
}

/**
 * @description: 生成非托管内存
 * @param {*Loader} ploader 读取器
 * @param {*} maxpoly 最大邻居数
 * @param {int} maxsmoot 最大路径节点数
 * @return {*}
 */
func NewNavigatior(ploader *Loader, maxpoly, maxsmoot int) *Navigatior {
	ptr := &Navigatior{
		_ptr: C.NewNavigatior(ploader._ptr, C.int(maxpoly), C.int(maxsmoot)),
	}
	return ptr
}

func RemoveNavigatior(nav *Navigatior) {
	if nav != nil {
		C.FreeNavigatior(nav._ptr)
	}
}

type Vector3 struct {
	X float32
	Y float32
	Z float32
}

type Path struct {
	_path     []Vector3
	_pathSize int
	_free     func(p *Path)
}

func (p *Path) Path() []Vector3 {
	return p._path
}
func (p *Path) PathSize() int {
	return p._pathSize
}
func (p *Path) Release() {
	if p._free == nil {
		return
	}
	p._free(p)
}

type Loader struct {
	_ptr *C.struct_GLoader
}

func (l *Loader) Release() {
	C.FreeLoader(l._ptr)
	l._ptr = nil
}

type Navigatior struct {
	_ptr         *C.struct_GNavigatior
	_nSmothIndex int
}

/**
 * @description:
 * @param {Vec3} sp 起始位置
 * @param {Vec3} ep 终点位置
 * @return {*} 路径数组
 */
func (nav *Navigatior) FindPath(sp Vector3, ep Vector3) *Path {
	var (
		tsp [3]float32
		tep [3]float32
	)

	tsp[0] = sp.X
	tsp[1] = sp.Y
	tsp[2] = sp.Z

	tep[0] = ep.X
	tep[1] = ep.Y
	tep[2] = ep.Z

	npath := int(C.FindPath(nav._ptr, (*C.float)(unsafe.Pointer(&tsp[0])), (*C.float)(unsafe.Pointer(&tep[0]))))
	if npath == 0 {
		return nil
	}
	nav._nSmothIndex = 0
	var result *Path
	if _allocer == nil {
		result = &Path{
			_path:     make([]Vector3, npath),
			_pathSize: npath,
		}
	} else {
		result = _allocer()
	}
	var pos [3]float32
	for ; nav._nSmothIndex < npath; nav._nSmothIndex++ {
		C.getPathPoint(nav._ptr, C.int(nav._nSmothIndex), (*C.float)(unsafe.Pointer(&pos[0])))
		result._path[nav._nSmothIndex].X = pos[0]
		result._path[nav._nSmothIndex].Y = pos[1]
		result._path[nav._nSmothIndex].Z = pos[2]
	}
	result._free = _release
	return result
}

/**
 * @description:
 * @param {Vec3} pos 动态对象位置
 * @return {*}
 */
func (nav *Navigatior) AddObstacle(pos Vector3) {
	var tpos [3]float32
	tpos[0] = pos.X
	tpos[1] = pos.Y
	tpos[2] = pos.Z
	C.AddObstacle(nav._ptr, (*C.float)(unsafe.Pointer(&tpos[0])))
}

/**
 * @description:
 * @param {int} idx 移除某个动态对象
 * @return {*}
 */
func (nav *Navigatior) RemoveObstacle(idx int) {
	C.RemoveObstacle(nav._ptr, C.int(idx))
}

/**
 * @description: 移除全部动态对象
 * @param {*}
 * @return {*}
 */
func (nav *Navigatior) RemoveAllObstacle() {
	C.RemoveAllObstacle(nav._ptr)
}

/**
 * @description: 更新动态对象信息
 * @param {float32} dt 时间频率
 * @param {interface{}} upToDate 更新数据
 * @return {*}
 */
func (nav *Navigatior) Update(dt float32, upToDate interface{}) {
	C.Update(nav._ptr, C.float(dt), unsafe.Pointer(&upToDate))
}

/**
 * @description: 释放内存
 * @param {*}
 * @return {*}
 */
func (nav *Navigatior) Release() {
	C.FreeNavigatior(nav._ptr)
	nav._ptr = nil
}
