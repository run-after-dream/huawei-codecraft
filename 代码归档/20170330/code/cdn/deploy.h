#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"

#define INF 99999999//表示无穷大

//结构体类型:网络链路信息类型
typedef struct {
    int id_link_begin;
    int id_link_end;
    int link_bandwidth;
    int cost_per_bandwidth;
}STLinkInfo;

//结构体类型:消费节点链路信息类型
typedef struct{
    int id_consumer;
    int id_net_dot; 
    int need_bandwidth;
    int cluster_id;//消费者节点所属聚类ID
}STConsumerLinkInfo;

//结构体类型:输出数据网路路径信息类型(调试用的，所有视频服务器均部署在消费节点上端)
typedef struct{
    int id_server;
    int id_consumer; 
    int cost_bandwidth;
}STResultInfo;

//结构体类型:对应有向链路的链路信息，存放在有向链路数组link_array中
typedef struct{
	int link_bandwidth;
	int cost_per_bandwidth;
}STLink;

//结构体类型:对应点ID以及到下一点的指针
typedef struct NodeList{
	int id_node;
	struct NodeList *pnext;
}STNode;

//结构体类型:用于链表存储点到点的最短路径，以及路径头结点的指针
typedef struct{
	int distance;
	int bandwidth;
	int link_num;
	STNode *p_path;
	STNode *p_path_end;//指向路径尾节点的指针
}STP2PPathInfo;

//结构体类型:网络节点的信息
typedef struct{
	int valid;//节点有效标志位，1表示可用
	int out_flow;//节点出方向的流量
	int cluster_id;//消费者节点所属聚类ID
}STNetNodeInfo;

//结构体类型:聚类相关信息
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
