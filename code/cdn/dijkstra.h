/***********************************************************************
�ļ�����:dijkstra.h
�ļ�����:dijkstra��Դ���·���㷨����ģ��,��������
�ļ�����ʱ��:20170409
�ļ�����:������
***********************************************************************/

#ifndef __DIJKSTRA_H__
#define __DIJKSTRA_H__

int dijkstra_adjacency_list_calculate(STLink **link_array);

void dijkstra_adjacency_list_create(STLink **link_array, STLinkInfo *adj_link_info, int *first, int *next);

int dijkstra_stack_create(int cluster_id, int id_source, STStackNodeInfo *stack_node, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info);

int dijkstra_stack_optimize(int id_node, int stack_length, STStackNodeInfo *stack_node, STNetNodeInfo *net_node_info);

int dijkstra_stack_check(int stack_length, STStackNodeInfo *stack_node);

void dijkstra_process_p2mp(int cluster_id, int id_source, STStackNodeInfo *stack_node, STLink **link_array, STLinkInfo *adj_link_info, int *first, int *next, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info);

void dijkstra_process_all_distance(int cluster_id, STStackNodeInfo *stack_node, STLink **link_array, STLinkInfo *adj_link_info, int *first, int *next, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info);

void dijkstra_process_consumer_max(int cluster_id, STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, int *id_cluster_A, int *id_cluster_B);

void dijkstra_process_main(int cluster_id, STLink **link_array, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info);

void dijkstra_debug_consumer_all(int cluster_id, STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info);

void dijkstra_debug_net_p2p(int cluster_id, STNetNodeInfo *net_node_info, STP2PPathInfo **p2p_path_info);

void dijkstra_debug_consumer_p2p(int cluster_id, STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, int id_consumer_source, int id_consumer_end);


#endif


