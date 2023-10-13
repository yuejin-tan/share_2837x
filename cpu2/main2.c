/*
 * main2.c
 * cpu2 程序入口
 *  Created on: 2023年10月12日
 *      Author: t
 */

#include "F28x_Project.h"

#include "main2.h"

#pragma CODE_SECTION(main2, "ramfuncs2");
int main2()
{

    // 启动外设时钟
    cpu2InitSysCtrl();

    // 初始化CPU2外设
    bsp_init_all2();

    // 控制算法初始化
    ctrl_init2();

    // 开始运行
    bsp_start2();

    // 主循环
    infLoopMain2();

    return 0;
}



