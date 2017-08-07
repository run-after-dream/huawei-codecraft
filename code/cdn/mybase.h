/***********************************************************************
�ļ�����:mybase.h
�ļ�����:�������ݴ���ģ��,�Լ���д�ļ��㣬��ȡ�ַ����ͷ��ڴ棬����ƴ�ӵȹ��ܺ�������
�ļ�����ʱ��:20170409
�ļ�����:������
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

