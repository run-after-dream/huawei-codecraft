#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"

#define INF 999999//��ʾ�����
#define MAX_LINK_NUM 50000//��������·�����ó���50000

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
    int need_bandwidth_history;
    int cluster_id;//�����߽ڵ���������ID
}STConsumerLinkInfo;

//�ṹ������:��Ӧ��ID�Լ�����һ���ָ��
typedef struct NodeList{
	int id_node;
	struct NodeList *pnext;
}STNode;

//�ṹ������:���������··����Ϣ����(�����õģ�������Ƶ�����������������ѽڵ��϶�)
typedef struct{
    int id_server;
    int id_consumer; 
    int cost_bandwidth;
}STResultDebugInfo;

//�ṹ������:���������··����Ϣ����
typedef struct{
	int cluster_id;//�����·��������
	int link_num;
	int id_consumer;
	int cost_bandwidth;
	STNode *p_path;
}STResultInfo;

//�ṹ������:��Ӧ������·����·��Ϣ�������������·����link_array��
typedef struct{
	int link_bandwidth;
	int link_bandwidth_history;
	int cost_per_bandwidth;
	int cost_per_bandwidth_history;//���ڼ�¼��ԭ������·������ã�������·�ָ�
}STLink;

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
	int cluster_id;//����ڵ���������ID
	int is_server;//�Ƿ�����������
}STNetNodeInfo;

//�ṹ������:���������Ϣ
typedef struct{
	int id_cluster;
	int valid;//0:��Ч�ࣻ1:��Ч�ࣻ2:��������ͷ���
	int need_service_num;//������Ҫ�ķ�������Ŀ
	int num_cluster_consumer;//���������߽ڵ����
	int max_num_server;//���ڷ������ڵ��������
	int num_server_current;//�����Ѳ������������
}STClusterInfo;

char *sub_string(char *ch, int pos_begin, int pos_end);

void change_int_to_char(int num, char *str);

void list_release(STNode **p_head);

STNode * list_link(STNode *p_source, STNode *p_target, int length);

void input_analyse_base(char * topo[MAX_EDGE_NUM]);

void input_analyse_other(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info, char * topo[MAX_EDGE_NUM]);

int process_debug(STConsumerLinkInfo *consumer_link_info, STResultDebugInfo *result_debug_info);

void result_standard_debug(int link_sum, STResultDebugInfo *result_debug_info, char *topo_file);

void result_standard(STResultInfo *result_info, char *topo_file);

void dijkstra_debug_consumer_all(int cluster_id, STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info);

void dijkstra_debug_net_p2p(int cluster_id, STNetNodeInfo *net_node_info, STP2PPathInfo **p2p_path_info);

void dijkstra_debug_consumer_p2p(STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, int *id_consumer_source, int *id_consumer_end);

void calculate_debug_node_outflow(int cluster_id, STNetNodeInfo *net_node_info);

void cluster_debug_net_consumer(int cluster_id, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info);

void process_pre(STLinkInfo *link_info, STLink **link_array, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STResultInfo *result_info);

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

int cluster_center(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info);

void cluster_link_prune_pre(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info);

int cluster_link_prune(int cluster_id, STClusterInfo *cluster_info, int id_service_net, int id_consumer, int cost_bandwidth, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info);

void cluster_flow_out_process(int cluster_id, STClusterInfo *cluster_info,int id_service_net, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info, STResultInfo *result_info);

void cluster_result_optimize(int cluster_id, int id_service_net, STLink **link_array, STResultInfo *result_info);

void result_optimize(STNetNodeInfo *net_node_info, STLink **link_array, STResultInfo *result_info);

int cluster_merge_sub_release(int cluster_id, STClusterInfo *cluster_info, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info, STResultInfo *result_info);

void cluster_flow_out_main(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info, STResultInfo *result_info);

void cluster_main(STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info, STResultInfo *result_info);

void result_check_debug(STLink **link_array, STConsumerLinkInfo *consumer_link_info, STResultInfo *result_info);

void process_main(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info, STResultInfo *result_info);

void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);

	

#endif
