#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"

#define INF 99999999//��ʾ�����

//�ṹ������:������·��Ϣ����
typedef struct {
    int id_link_begin;
    int id_link_end;
    int link_bandwidth;
    int cost_per_bandwidth;
}STLinkInfo;

//�ṹ������:���ѽڵ���·��Ϣ����
typedef struct{
    int id_consumer;
    int id_net_dot; 
    int need_bandwidth;
}STConsumerLinkInfo;

//�ṹ������:���������··����Ϣ����(�����õģ�������Ƶ�����������������ѽڵ��϶�)
typedef struct{
    int id_server;
    int id_consumer; 
    int cost_bandwidth;
}STResultInfo;

//�ṹ������:��Ӧ������·����·��Ϣ�������������·����link_array��
typedef struct{
	int link_bandwidth;
	int cost_per_bandwidth;
}STLink;

//�ṹ������:��Ӧ��ID�Լ�����һ���ָ��
typedef struct NodeList{
	int id_node;
	struct NodeList *pnext;
}STNode;

//�ṹ������:��������洢�㵽������·�����Լ�·��ͷ����ָ��
typedef struct{
	int distance;
	int bandwidth;
	int link_num;
	STNode *p_path;
	STNode *p_path_end;//ָ��·��β�ڵ��ָ��
}STP2PPathInfo;

//�ṹ������:����ڵ����Ϣ
typedef struct{
	int id_node;
	int valid;//�ڵ���Ч��־λ��1��ʾ����
	int out_flow;//�ڵ�����������
}STNetNodeInfo;

char *sub_string(char *ch, int pos_begin, int pos_end);

void change_int_to_char(int num, char *str);

void list_release(STNode*p_head);

STNode * list_link(STNode *p_source, STNode *p_target, int length);

void input_analyse_base(char * topo[MAX_EDGE_NUM]);

void input_analyse_other(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info, char * topo[MAX_EDGE_NUM]);

int process_debug(STConsumerLinkInfo *consumer_link_info, STResultInfo *result_info);

void result_standard_debug(int link_sum, STResultInfo *result_info, char *topo_file);

void dijkstra_debug_consumer_all(STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info);

void dijkstra_debug_consumer_p2p(STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, int *id_consumer_source, int *id_consumer_end);

void calculate_debug_node_outflow(STNetNodeInfo *net_node_info, int num_node_area);

void process_pre(STLinkInfo *link_info, STLink **link_array, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info);

void dijkstra_process_p2mp(int id_source, STP2PPathInfo **p2p_path_info);

void dijkstra_process_all_distance(STP2PPathInfo **p2p_path_info);

void dijkstra_process_consumer_max(STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, int *id_consumer_source, int *id_consumer_end);

void dijkstra_process_main(STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info);

int calculate_consumer_need(STConsumerLinkInfo *consumer_link_info, int num_consumer_area);

void calculate_node_outflow(STLink **link_array, STConsumerLinkInfo *consumer_link_info, int num_consumer_area, STNetNodeInfo *net_node_info, int num_node_area);

int calculate_server_minimum(STLink **link_array, STConsumerLinkInfo *consumer_link_info, int num_consumer_area, STNetNodeInfo *net_node_info, int num_node_area);

void cluster_pre(STLink **link_array, STConsumerLinkInfo *consumer_link_info, int num_consumer_area, STNetNodeInfo *net_node_info, int num_node_area);

void cluster_main(STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info);

void process_main(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info);

void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);

	

#endif
