/***********************************************************************
�ļ�����:deploy.h
�ļ�����:�ٷ��ṩ�Ĺ��ܺ�����ڣ�������к궨�壬ȫ�ֱ�����������ܺ����������ṹ�����Ͷ���
�ļ�����ʱ��:20170307
�ļ�����:huawei
�޸�ʱ��:20170409
�޸���:������
***********************************************************************/

#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"

#define INF 999999//��ʾ�����
#define MAX_LINK_NUM 50000//��������·�����ó���50000
//�ɵ�����
#define MAX_ADD_CLUSTER_SERVER_NUM_PRI 6//(��������)�������ڿ������Ӳ���ķ���������
#define SERVER_NUM_LIMIT_PRI 6//(��������)��С���׼�����������ٿɲ���ķ���������
#define MAX_ADD_CLUSTER_SERVER_NUM_MID 7//(�м�����)�������ڿ������Ӳ���ķ���������
#define SERVER_NUM_LIMIT_MID 7//(�м�����)��С���׼�����������ٿɲ���ķ���������
#define MAX_ADD_CLUSTER_SERVER_NUM_HARD 5//(�߼�����)�������ڿ������Ӳ���ķ���������
#define SERVER_NUM_LIMIT_HARD 5//(�߼�����)��С���׼�����������ٿɲ���ķ���������
#define HARD_FLAG 3//(�߼�����)ԭʼ��Ƚ�λ

extern int num_net_dot;//����ڵ�����
extern int num_link;//������·����
extern int num_consumer;//���ѽڵ�����
extern int cost_server;//��Ƶ���ݷ���������ɱ�
extern int num_link_actual;//����˫������ʱ��ʵ����·����
extern int num_result_link;//�������·����
extern int num_consumer_remain;//ʣ�������߽ڵ���
extern int num_server;//�Ѳ������������
extern int max_add_cluster_server_num;//�����Ӿ��������ɲ���ķ��������������������з����������������������ɲ�����(������Ҫ�������������ֵ�ĺ�)ʱ�����ǽ��þ��������ֵܾ���ϲ�Ϊĸ���࣬�ò����Գ�ʼ����(cluster_id = 1)��Ч
extern int solution_cluster_center;//ѡ��������ĵĲ��� 0:ͨ������-�����þ������ģ�1:ͨ���������ѡ��������

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

//�ṹ������:�ѽṹ�ڽڵ���Ϣ
typedef struct{
	int id_node;//ʵ������ڵ��id
	int value;//��Դ��ľ���
}STStackNodeInfo;

//�ṹ������:����ڵ����Ϣ
typedef struct{
	int valid;//�ڵ���Ч��־λ��1��ʾ����
	int out_flow;//�ڵ�����������
	int cluster_id;//����ڵ���������ID
	int is_server;//�Ƿ�����������
	int id_stack_node;//��Ӧ���ڵĽڵ���
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


int process_debug(STConsumerLinkInfo *consumer_link_info, STResultDebugInfo *result_debug_info);

void process_pre(STLinkInfo *link_info, STLink **link_array, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STResultInfo *result_info);

void result_optimize(STNetNodeInfo *net_node_info, STLink **link_array, STResultInfo *result_info);

void result_check_debug(STLink **link_array, STConsumerLinkInfo *consumer_link_info, STResultInfo *result_info);

void process_main(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info, STResultInfo *result_info);

void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);

#endif
