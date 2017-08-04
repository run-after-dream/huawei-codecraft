#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"

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
}STConsumerLinkInfo;

//结构体类型:输出数据网路路径信息类型(调试用的，所有视频服务器均部署在消费节点上端)
typedef struct{
    int id_server;
    int id_consumer; 
    int cost_bandwidth;
}STResultInfo;

char *sub_string(char *ch, int pos_begin, int pos_end);

void change_int_to_char(int num, char *str);

void input_analyse_base(char * topo[MAX_EDGE_NUM]);

void input_analyse_other(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info, char * topo[MAX_EDGE_NUM]);

int process_debug(STConsumerLinkInfo *consumer_link_info, STResultInfo *result_info);

void result_standard_debug(int link_sum, STResultInfo *result_info, char *topo_file);

void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);

	

#endif
