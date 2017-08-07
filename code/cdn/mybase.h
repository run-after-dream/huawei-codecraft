/***********************************************************************
文件名称:mybase.h
文件功能:基本数据处理模块,自己编写的计算，截取字符，释放内存，链表拼接等功能函数声明
文件创建时间:20170409
文件作者:陈自立
***********************************************************************/

#ifndef __MYBASE_H__
#define __MYBASE_H__

char *sub_string(char *ch, int pos_begin, int pos_end);

void change_int_to_char(int num, char *str);

void list_release(STNode **p_head);

STNode * list_link(STNode *p_source, STNode *p_target, int length);

void calculate_debug_node_outflow(int cluster_id, STNetNodeInfo *net_node_info);

int calculate_consumer_need(STConsumerLinkInfo *consumer_link_info, int num_consumer_area);

void calculate_node_outflow(int cluster_id, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info);

int calculate_server_minimum(int cluster_id, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info);

#endif

