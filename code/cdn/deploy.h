/***********************************************************************
文件名称:deploy.h
文件功能:官方提供的功能函数入口，存放所有宏定义，全局变量声明，框架函数声明，结构体类型定义
文件创建时间:20170307
文件作者:huawei
修改时间:20170409
修改人:陈自立
***********************************************************************/

#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"

#define INF 999999//表示无穷大
#define MAX_LINK_NUM 50000//输出结果链路数不得超过50000
//可调参数
#define MAX_ADD_CLUSTER_SERVER_NUM_PRI 6//(初级用例)单个域内可以增加部署的服务器个数
#define SERVER_NUM_LIMIT_PRI 6//(初级用例)最小类标准，单个域至少可部署的服务器个数
#define MAX_ADD_CLUSTER_SERVER_NUM_MID 7//(中级用例)单个域内可以增加部署的服务器个数
#define SERVER_NUM_LIMIT_MID 7//(中级用例)最小类标准，单个域至少可部署的服务器个数
#define MAX_ADD_CLUSTER_SERVER_NUM_HARD 5//(高级用例)单个域内可以增加部署的服务器个数
#define SERVER_NUM_LIMIT_HARD 5//(高级用例)最小类标准，单个域至少可部署的服务器个数
#define HARD_FLAG 3//(高级用例)原始类比较位

extern int num_net_dot;//网络节点数量
extern int num_link;//网络链路数量
extern int num_consumer;//消费节点数量
extern int cost_server;//视频内容服务器部署成本
extern int num_link_actual;//考虑双向流量时的实际链路数量
extern int num_result_link;//输出网络路径数
extern int num_consumer_remain;//剩余消费者节点数
extern int num_server;//已部署服务器数量
extern int max_add_cluster_server_num;//单个子聚类中最大可部署的服务器叠加量，当聚类中服务器数量超出该域内最大可部署量(至少需要服务器个数与该值的和)时，考虑将该聚类与其兄弟聚类合并为母聚类，该参数对初始大类(cluster_id = 1)无效
extern int solution_cluster_center;//选择聚类中心的策略 0:通过出度-距离获得聚类中心，1:通过出度最大选聚类中心

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
    int need_bandwidth_history;
    int cluster_id;//消费者节点所属聚类ID
}STConsumerLinkInfo;

//结构体类型:对应点ID以及到下一点的指针
typedef struct NodeList{
	int id_node;
	struct NodeList *pnext;
}STNode;

//结构体类型:输出数据网路路径信息类型(调试用的，所有视频服务器均部署在消费节点上端)
typedef struct{
    int id_server;
    int id_consumer; 
    int cost_bandwidth;
}STResultDebugInfo;

//结构体类型:输出数据网路路径信息类型
typedef struct{
	int cluster_id;//输出链路所属聚类
	int link_num;
	int id_consumer;
	int cost_bandwidth;
	STNode *p_path;
}STResultInfo;

//结构体类型:对应有向链路的链路信息，存放在有向链路数组link_array中
typedef struct{
	int link_bandwidth;
	int link_bandwidth_history;
	int cost_per_bandwidth;
	int cost_per_bandwidth_history;//用于记录下原来的链路带宽费用，便于链路恢复
}STLink;

//结构体类型:用于链表存储点到点的最短路径，以及路径头结点的指针
typedef struct{
	int distance;
	int bandwidth;
	int link_num;
	STNode *p_path;
	STNode *p_path_end;//指向路径尾节点的指针
}STP2PPathInfo;

//结构体类型:堆结构内节点信息
typedef struct{
	int id_node;//实际网络节点的id
	int value;//到源点的距离
}STStackNodeInfo;

//结构体类型:网络节点的信息
typedef struct{
	int valid;//节点有效标志位，1表示可用
	int out_flow;//节点出方向的流量
	int cluster_id;//网络节点所属聚类ID
	int is_server;//是否服务器部署点
	int id_stack_node;//对应堆内的节点编号
}STNetNodeInfo;

//结构体类型:聚类相关信息
typedef struct{
	int id_cluster;
	int valid;//0:无效类；1:有效类；2:处理完毕释放类
	int need_service_num;//至少需要的服务器数目
	int num_cluster_consumer;//域内消费者节点个数
	int max_num_server;//域内服务器节点最大部署量
	int num_server_current;//域内已部署服务器数量
}STClusterInfo;


int process_debug(STConsumerLinkInfo *consumer_link_info, STResultDebugInfo *result_debug_info);

void process_pre(STLinkInfo *link_info, STLink **link_array, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STResultInfo *result_info);

void result_optimize(STNetNodeInfo *net_node_info, STLink **link_array, STResultInfo *result_info);

void result_check_debug(STLink **link_array, STConsumerLinkInfo *consumer_link_info, STResultInfo *result_info);

void process_main(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info, STResultInfo *result_info);

void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);

#endif
