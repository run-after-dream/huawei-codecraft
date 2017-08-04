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
    int cluster_id;//�����߽ڵ���������ID
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
	int valid;//�ڵ���Ч��־λ��1��ʾ����
	int out_flow;//�ڵ�����������
	int cluster_id;//�����߽ڵ���������ID
}STNetNodeInfo;

//�ṹ������:���������Ϣ
typedef struct{
	int id_cluster;
	int valid;
	int need_service_num;
}STClusterInfo;

char *sub_string(char *ch, int pos_begin, int pos_end);

void change_int_to_char(int num, char *str);

void list_release(STNode **p_head);

STNode * list_link(STNode *p_source, STNode *p_target, int length);

void input_analyse_base(char * topo[MAX_EDGE_NUM]);

void input_analyse_other(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info, char * topo[MAX_EDGE_NUM]);

int process_debug(STConsumerLinkInfo *consumer_link_info, STResultInfo *result_info);

void result_standard_debug(int link_sum, STResultInfo *result_info, char *topo_file);

void dijkstra_debug_consumer_all(int cluster_id, STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info);

void dijkstra_debug_consumer_p2p(STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, int *id_consumer_source, int *id_consumer_end);

void calculate_debug_node_outflow(int cluster_id, STNetNodeInfo *net_node_info);

void cluster_debug_net_consumer(int cluster_id, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info);

void process_pre(STLinkInfo *link_info, STLink **link_array, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info);

void dijkstra_process_p2mp(int cluster_id, int id_source, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info);

void dijkstra_process_all_distance(int cluster_id, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info);

void dijkstra_process_consumer_max(int cluster_id, STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, int *id_cluster_A, int *id_cluster_B);

void dijkstra_process_main(int cluster_id, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info);

int calculate_consumer_need(STConsumerLinkInfo *consumer_link_info, int num_consumer_area);

void calculate_node_outflow(STLink **link_array, STConsumerLinkInfo *consumer_link_info, int num_consumer_area, STNetNodeInfo *net_node_info, int num_node_area);

int calculate_server_minimum(int cluster_id, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info);

int cluster_p2ppath_is_need_rebuild(int cluster_id, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STLink **link_array, int id_net_source, int id_net_end);

void cluster_info_pre(STClusterInfo *cluster_info);

void cluster_pre(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info);

void cluster_split(int cluster_id, int id_cluster_A, int id_cluster_B, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info);

int cluster_process(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info);

int cluster_center(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info);

void cluster_flow_out_main(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info);

void cluster_main(STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info);

void process_main(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info);

void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);

	

#endif
