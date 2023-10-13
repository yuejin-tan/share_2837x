/**
 * main.c
 * 主程序入口
 */

#include "main.h"

int main(void)
{
    // 初始化时钟等基本配置
    initSysCtrl();

    // 板级支持包
    bsp_init_all();

    // 控制算法初始化
    ctrl_init();

    // 开始运行
    bsp_start();

    // 主循环
    infLoopMain();

    return 0;
}
