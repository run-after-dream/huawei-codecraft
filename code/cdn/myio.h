/***********************************************************************
文件名称:myio.h
文件功能:基本输入输出处理模块，根据题目要求，编写自己的输出输出数据处理函数，函数声明
文件创建时间:20170409
文件作者:陈自立
***********************************************************************/

#ifndef __MYIO_H__
#define __MYIO_H__

void input_analyse_base(char * topo[MAX_EDGE_NUM]);

void input_analyse_other(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info, char * topo[MAX_EDGE_NUM]);

void result_standard_debug(int link_sum, STResultDebugInfo *result_debug_info, char *topo_file);

void result_standard(STResultInfo *result_info, char *topo_file);

#endif

