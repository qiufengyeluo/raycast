#ifndef MAGIC_ENGINE_NAVIGATION_DETOUR_STATUS_H_H
#define MAGIC_ENGINE_NAVIGATION_DETOUR_STATUS_H_H

namespace Detour {

    typedef unsigned int Status;

    // 高层状态位 
    /********************************************************************
     *                            0 ~ 32 Bit                            |
     * ******************************************************************
     *     31 bit       |     30 bit      |      29 bit       |  28 bit |
     * ******************************************************************
     *  FAILURE == 1    |  SUCCESS == 1   |  IN PROGRESS == 1 |     ?   |
     * ******************************************************************/
    static const unsigned int FAILURE     = 1u << 31;
    static const unsigned int SUCCESS     = 1u << 30;
    static const unsigned int IN_PROGRESS = 1u << 29;

    //Detail 状态信息 
    /*****************************************************************
     *                           0 ~ 32 Bit                          |
     ***************************************************************** 
    * 28 ~ 32 bits | 24 ~ 28 bits |           0 ~ 24 bits           |
    *****************************************************************
    *  高层状态位   |      ?       |            状态信息             |
    *****************************************************************
    * 0xF0000000   |  0x0F000000  |            0x0FFFFFF            |                                                           
    * ***************************************************************/
    static const unsigned int STATUS_DETAIL_MASK = 0x0FFFFFF;
    static const unsigned int WRONG_MAGIC      = 1 << 0; //无法识别的输入数据
    static const unsigned int WRONG_VERSION    = 1 << 1; //输入数据的版本错误
    static const unsigned int OUT_OF_MEMORY    = 1 << 2; //操作内存不足
    static const unsigned int INVALID_PARAM    = 1 << 3; //输入参数无效
    static const unsigned int BUFFER_TOO_SMALL = 1 << 4; //查询的结果缓冲区太小，无法存储所有结果
    static const unsigned int OUT_OF_NODES     = 1 << 5; //查询在搜索过程中耗尽了节点
    static const unsigned int PARTIAL_RESULT   = 1 << 6; //查询未到达结束位置，返回最佳结构
    static const unsigned int ALREADY_OCCUPIED = 1 << 7; //已将TITLE指定给给定的x，y坐标


    inline bool IsStatusSucceed(Status stat) {
        return (stat & SUCCESS) != 0;
    }

    inline bool IsStatusFailed(Status stat) {
        return (stat & FAILURE) != 0;
    }

    inline bool IsStatusInProgress(Status stat) {
        return (stat & IN_PROGRESS) != 0;
    }

    inline bool IsStatusDetail(Status stat, unsigned int detail) {
        return (stat & detail) != 0;
    }

}

#endif 