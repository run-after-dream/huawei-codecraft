/***********************************************************************
�ļ�����:myio.h
�ļ�����:���������������ģ�飬������ĿҪ�󣬱�д�Լ������������ݴ���������������
�ļ�����ʱ��:20170409
�ļ�����:������
***********************************************************************/

#ifndef __MYIO_H__
#define __MYIO_H__

void input_analyse_base(char * topo[MAX_EDGE_NUM]);

void input_analyse_other(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info, char * topo[MAX_EDGE_NUM]);

void result_standard_debug(int link_sum, STResultDebugInfo *result_debug_info, char *topo_file);

void result_standard(STResultInfo *result_info, char *topo_file);

#endif

