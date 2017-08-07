/***********************************************************************
文件名称:cluster.cpp
文件功能:聚类算法处理模块,函数定义
文件创建时间:20170409
文件作者:陈自立
***********************************************************************/

#include "deploy.h"
#include "dijkstra.h"
#include "cluster.h"
#include "mybase.h"
#include <stdio.h>
#include <stdlib.h>

/************************************************************************
函数功能:聚类处理函数:判断聚类中一条链路是否需要重置
功能详解:根据原链路中的所有节点是否都在域内以及原链路中是否有链路被剪枝判断链路是否需要重置
入参解释:cluster_id(聚类编号)p2p_path_info(点到点的最短路径信息)net_node_info(域内网络节点信息)
link_array(有向链路数组)id_net_source(链路源节点id)id_net_end(链路目的节点id)
返回值:ret(0:无须重置；1:需要重置)
函数作者:陈自立
*************************************************************************/
int cluster_p2ppath_is_need_rebuild(int cluster_id, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STLink **link_array, int id_net_source, int id_net_end)
{
    int ret = 0;
    int i = 0;
    int id_a = 0;//用于临时记录单条链路的头尾节点编号
    int id_b = 0;
    STNode* p = p2p_path_info[id_net_source][id_net_end].p_path;
    if((p2p_path_info[id_net_source][id_net_end].distance < INF) && (p2p_path_info[id_net_source][id_net_end].bandwidth> 0))
    {
        for(i = 0; i < p2p_path_info[id_net_source][id_net_end].link_num; i++)
        {
            id_a = p->id_node;
            p = p->pnext;
            id_b = p->id_node;
            if((p->id_node != cluster_id) || (link_array[id_a][id_b].link_bandwidth== INF))//链路上存在点不属于该聚类或链路上存在子链路被剪枝的情况
            {
                ret = 1;
                return ret;
            }
        }
    }

    return ret;

}

/************************************************************************
函数功能:聚类处理函数:聚类信息预处理
功能详解:对聚类信息进行预处理,注意:聚类信息数组从1开始,所以数组元素有num_consumer+1个
入参解释:cluster_info(聚类信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void cluster_info_pre(STClusterInfo *cluster_info)
{
    int i = 0;
    for(i = 0; i <= num_consumer; i++)
    {
        cluster_info[i].id_cluster = i;
        cluster_info[i].need_service_num = 0;
        cluster_info[i].valid = 0;
        cluster_info[i].num_cluster_consumer = 0;
        cluster_info[i].max_num_server = 0;
        cluster_info[i].num_server_current = 0;
    }

}

/************************************************************************
函数功能:聚类处理函数:聚类预处理
功能详解:对指定聚类进行初始化，根据域内网络节点初始化p2p_path_info，以便后面计算域内最短路径，
同时更新聚类信息
入参解释:cluster_id(聚类编号)cluster_info(聚类信息)p2p_path_info(点到点的最短路径信息)
link_array(有向链路数组)net_node_info(域内网络节点信息)consumer_link_info(域内消费节点链接信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void cluster_pre(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info)
{
    int i = 0;
    int j = 0;
    int is_need_rebuild = 0;

    cluster_info[cluster_id].valid = 1;
    cluster_info[cluster_id].num_cluster_consumer = 0;

    for(i = 0; i <= num_consumer; i++)
    {
        if(consumer_link_info[i].cluster_id == cluster_id)
        {
            cluster_info[cluster_id].num_cluster_consumer++;
        }
    }
    
    for(i = 0; i < num_net_dot; i++)
    {
        for(j = 0; j < num_net_dot; j++)
        {
            if((p2p_path_info[i][j].distance < INF) && (p2p_path_info[i][j].bandwidth> 0))
            {
                if((net_node_info[i].cluster_id == cluster_id) && (net_node_info[j].cluster_id == cluster_id))
                {
                    is_need_rebuild = cluster_p2ppath_is_need_rebuild(cluster_id, p2p_path_info, net_node_info, link_array, i, j);
                    if(is_need_rebuild == 1)
                    {
                        if((link_array[i][j].cost_per_bandwidth< INF) && (link_array[i][j].link_bandwidth > 0))
                        {
                            p2p_path_info[i][j].distance = link_array[i][j].cost_per_bandwidth;
                            p2p_path_info[i][j].link_num = 1;
                            p2p_path_info[i][j].bandwidth = link_array[i][j].link_bandwidth;
                            list_release(&(p2p_path_info[i][j].p_path->pnext));//先释放掉原先的链路
                            p2p_path_info[i][j].p_path->pnext = (STNode *)malloc(sizeof(STNode));
                            p2p_path_info[i][j].p_path->pnext->id_node = j;
                            p2p_path_info[i][j].p_path->pnext->pnext = NULL;
                            p2p_path_info[i][j].p_path_end = p2p_path_info[i][j].p_path->pnext;
                        }
                        else
                        {
                            if(i != j)
                            {
                                p2p_path_info[i][j].distance = INF;
                                p2p_path_info[i][j].bandwidth = 0;
                            }
                            else
                            {
                                p2p_path_info[i][j].distance = 0;
                                p2p_path_info[i][j].bandwidth = INF;
                            }
                            p2p_path_info[i][j].link_num = 0;
                            list_release(&(p2p_path_info[i][j].p_path->pnext));//先释放掉原先的链路
                            p2p_path_info[i][j].p_path_end = p2p_path_info[i][j].p_path;
                        }
                    }
                }
            }
        }
    }
}

/************************************************************************
函数功能:聚类处理函数:分裂处理函数
功能详解:根据域内各点到所给定的两个网络节点的距离，将各网络节点和消费者节点分为两类
入参解释:cluster_id(聚类编号);id_cluster_A(聚类A中心);id_cluster_B(聚类B中心)
p2p_path_info(点到点的最短路径信息);net_node_info(域内网络节点信息)consumer_link_info(域内消费节点链接信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void cluster_split(int cluster_id, int id_cluster_A, int id_cluster_B, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info)
{
    int i = 0;
    int id_net_consumer = 0;
    for(i = 0; i < num_net_dot; i++)
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            if(p2p_path_info[i][id_cluster_A].distance < p2p_path_info[i][id_cluster_B].distance)
            {
                net_node_info[i].cluster_id = 2 * cluster_id;
            }
            else
            {
                net_node_info[i].cluster_id = 2 * cluster_id + 1;
            }
        }
    }

    for(i = 0; i < num_consumer; i++)
    {
        id_net_consumer = consumer_link_info[i].id_net_dot;
        consumer_link_info[i].cluster_id = net_node_info[id_net_consumer].cluster_id;
    }
}

/************************************************************************
函数功能:聚类处理函数:计算聚类中心
功能详解:计算指定聚类的聚类中心
入参解释:cluster_id(聚类编号)cluster_info(聚类信息)p2p_path_info(点到点的最短路径信息)link_array(有向链路数组)
consumer_link_info(域内消费节点链接信息)net_node_info(域内网络节点信息)
返回值:id_center(返回域内中心节点ID)
函数作者:陈自立
*************************************************************************/
int cluster_center(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int j = 0;
    int id_center = 0;
    long value = 0;//记录节点域内权值(节点出度-节点到其他消费者节点的距离之和)
    long max_value = -999999999;
    int is_server_cluster_remain = 0;//用于判断是否从域内已部署服务器中选取
    int id_net = 0;
    double dis_sum = 0;
    
    //计算域内节点出度
    calculate_node_outflow(cluster_id, link_array, consumer_link_info, net_node_info);
    if(cluster_info[cluster_id].num_server_current > 0)//若域内已经部署有服务器，则先从域内已有服务器中选
    {
        //在已部署服务器节点上，根据网络节点出度之和减去网络节点到域内消费者节点的距离之和最大来选取聚类中心:
        for(i = 0; i < num_net_dot; i++)
        {
            if((net_node_info[i].cluster_id == cluster_id) && (net_node_info[i].is_server == 1))//域内服务器节点
            {
                is_server_cluster_remain = 1;
                for(j = 0; j < num_consumer; j++)
                {
                    if(consumer_link_info[j].cluster_id == cluster_id)
                    {
                        id_net = consumer_link_info[j].id_net_dot;
                        dis_sum = dis_sum + p2p_path_info[i][id_net].distance;
                    }
                }
                if(solution_cluster_center == 0)//选路策略为0
                {
                    value = (long)(net_node_info[i].out_flow) - dis_sum;
                    if(value > max_value)
                    {
                        max_value = value;
                        id_center = i;
                    }
                }
                if(solution_cluster_center == 1)//选路策略为1
                {
                    value = (long)(net_node_info[i].out_flow);
                    if(value > max_value)
                    {
                        max_value = value;
                        id_center = i;
                    }
                }
                dis_sum = 0;
            }
        }
        if(is_server_cluster_remain == 1)//若是从域内已部署服务器中取点，则在此直接返回
        {
            return id_center;
        }
    }
    //需要部署到新的网络节点上:将域内服务器数量加1
    cluster_info[cluster_id].num_server_current++;

    //判断是否有消费者节点流量需求大于其相邻网络节点流量出度的一半，若存在，则相邻网络节点上必须部署服务器:
    for(i = 0; i < num_consumer; i++)
    {
        if(consumer_link_info[i].cluster_id == cluster_id)
        {
            id_net = consumer_link_info[i].id_net_dot;
            if(consumer_link_info[i].need_bandwidth > ((net_node_info[id_net].out_flow)/2))
            {
                id_center = id_net;
                return id_center;
            }
        }
    }
    //根据网络节点出度之和减去网络节点到域内消费者节点的距离之和最大来选取聚类中心:
    for(i = 0; i < num_net_dot; i++)
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            for(j = 0; j < num_consumer; j++)
            {
                if(consumer_link_info[j].cluster_id == cluster_id)
                {
                    id_net = consumer_link_info[j].id_net_dot;
                    dis_sum = dis_sum + p2p_path_info[i][id_net].distance;
                }
            }
            if(solution_cluster_center == 0)//选路策略为0
            {
                value = (long)(net_node_info[i].out_flow) - dis_sum;
                if(value > max_value)
                {
                    max_value = value;
                    id_center = i;
                }
            }
            if(solution_cluster_center == 1)//选路策略为1
            {
                value = (long)(net_node_info[i].out_flow);
                if(value > max_value)
                {
                    max_value = value;
                    id_center = i;
                }
            }
            dis_sum = 0;
        }
    }
    printf("[INFO] <cluster_center> id_center = %d\n",id_center);
    return id_center;
}

/************************************************************************
函数功能:聚类处理函数:剪枝处理后域内初始化函数
功能详解:在域内剪枝完成后，域内重新初始化p2p_path_info的信息
入参解释:cluster_id(聚类编号)cluster_info(聚类信息)p2p_path_info(点到点的最短路径信息)
link_array(有向链路数组)net_node_info(域内网络节点信息)consumer_link_info(域内消费节点链接信息)
返回值:ret(1:消费者节点被消除,可以处理下一个消费者结点)
函数作者:陈自立
*************************************************************************/
void cluster_link_prune_pre(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info)
{
    int i = 0;
    int j = 0;

    cluster_info[cluster_id].num_cluster_consumer = 0;

    for(i = 0; i < num_consumer; i++)
    {
        if(consumer_link_info[i].cluster_id == cluster_id)
        {
            cluster_info[cluster_id].num_cluster_consumer++;
        }
    }
    
    for(i = 0; i < num_net_dot; i++)
    {
        for(j = 0; j < num_net_dot; j++)
        {
            if((net_node_info[i].cluster_id == cluster_id) && (net_node_info[j].cluster_id == cluster_id))
            {
                if(i != j)
                {
                    p2p_path_info[i][j].distance = INF;//初始化点到点的最短路径信息
                    p2p_path_info[i][j].bandwidth = 0;
                }
                else
                {
                    p2p_path_info[i][j].distance = 0;//初始化点到点的最短路径信息
                    p2p_path_info[i][j].bandwidth = INF;
                }
                p2p_path_info[i][j].link_num = 0;
                p2p_path_info[i][j].p_path->id_node = i;
                list_release(&(p2p_path_info[i][j].p_path->pnext));//先释放掉原先的链路
                p2p_path_info[i][j].p_path->pnext = NULL;
                p2p_path_info[i][j].p_path_end = p2p_path_info[i][j].p_path;//尾节点即为路径中最后一点，初始时即为源节点
                if((link_array[i][j].cost_per_bandwidth < INF) && (i != j))
                {
                    p2p_path_info[i][j].distance = link_array[i][j].cost_per_bandwidth;
                    p2p_path_info[i][j].link_num = 1;
                    p2p_path_info[i][j].bandwidth = link_array[i][j].link_bandwidth;
                    p2p_path_info[i][j].p_path->pnext = (STNode *)malloc(sizeof(STNode));
                    p2p_path_info[i][j].p_path->pnext->id_node = j;
                    p2p_path_info[i][j].p_path->pnext->pnext = NULL;
                    p2p_path_info[i][j].p_path_end = p2p_path_info[i][j].p_path->pnext;
                 }
            }
        }
    }
}

/************************************************************************
函数功能:聚类处理函数:剪枝处理函数
功能详解:在域内单条路径流量分发完毕后，对单条路径剪枝，同时对反向路径也做剪枝处理
入参解释:cluster_id(聚类编号)cluster_info(聚类信息)id_service_net(服务器部署节点)id_consumer(消费者结点编号)
cost_bandwidth(流量消耗)p2p_path_info(点到点的最短路径信息)link_array(有向链路数组)consumer_link_info(域内消费节点链接信息)
net_node_info(域内网络节点信息)
返回值:ret(1:消费者节点被消除,可以处理下一个消费者结点)
函数作者:陈自立
*************************************************************************/
int cluster_link_prune(int cluster_id, STClusterInfo *cluster_info, int id_service_net, int id_consumer, int cost_bandwidth, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int ret = 0;
    int id_net_consumer = 0;
    id_net_consumer = consumer_link_info[id_consumer].id_net_dot;
    STNode *p = p2p_path_info[id_service_net][id_net_consumer].p_path;
    int id_node_A = 0;
    int id_node_B = 0;
    printf("[INFO] <cluster_link_prune> id_service_net = %d id_consumer = %d cost_bandwidth = %d\n",id_service_net,id_consumer,cost_bandwidth);
    //首先对消费者节点进行剪枝
    if(consumer_link_info[id_consumer].need_bandwidth > cost_bandwidth)
    {
        consumer_link_info[id_consumer].need_bandwidth = consumer_link_info[id_consumer].need_bandwidth - cost_bandwidth;
    }
    else
    {
        consumer_link_info[id_consumer].need_bandwidth = 0;
        consumer_link_info[id_consumer].cluster_id = 0;//对于流量需求满足的节点，将其聚类id变为0，聚类0对应已经消去的消费者节点
        ret = 1;
        num_consumer_remain = num_consumer_remain - 1;
    }
    //再对网络链路进行剪枝
    for(i = 0; i < p2p_path_info[id_service_net][id_net_consumer].link_num; i++)
    {
        if(p->pnext == NULL)
        {
            printf("[ERROR]:<cluster_link_prune> p->pnext == NULL\n");
            break;
        }
        id_node_A = p->id_node;
        p = p->pnext;
        id_node_B = p->id_node;
        if(link_array[id_node_A][id_node_B].cost_per_bandwidth < INF)
        {
            link_array[id_node_A][id_node_B].link_bandwidth = link_array[id_node_A][id_node_B].link_bandwidth - cost_bandwidth;
            //一旦正向选路，则该链路反向设为不通:
            //link_array[id_node_B][id_node_A].link_bandwidth = 0;
            if(link_array[id_node_A][id_node_B].link_bandwidth == 0)
            {
                link_array[id_node_A][id_node_B].cost_per_bandwidth = INF;//链路带宽减为0,则剪枝
            }
        }
        else
        {
            printf("[ERROR]:<cluster_link_prune> link_array[%d][%d].cost_per_bandwidth = %d\n",id_node_A,id_node_B,link_array[id_node_A][id_node_B].cost_per_bandwidth);
        }
    }
    //剪枝完毕后，对域内重新初始化，更新路径
    cluster_link_prune_pre(cluster_id, cluster_info, p2p_path_info, link_array, net_node_info, consumer_link_info);
    //调用dijkstra处理函数，域内遍历最短路径
    dijkstra_process_main(cluster_id, link_array, p2p_path_info, net_node_info, consumer_link_info);

    return ret;
}

/************************************************************************
函数功能:聚类处理函数:域内流量分发处理函数
功能详解:在域内部署服务器，进行流量分发
入参解释:cluster_id(聚类编号)cluster_info(聚类信息)id_service_net(服务器部署节点)p2p_path_info(点到点的最短路径信息)
link_array(有向链路数组)consumer_link_info(域内消费节点链接信息)net_node_info(域内网络节点信息)result_info(输出结果链路信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void cluster_flow_out_process(int cluster_id, STClusterInfo *cluster_info,int id_service_net, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info, STResultInfo *result_info)
{
    int i = 0;
    int is_consumer_clear = 0;//判断当前消费者节点是否被清除
    int is_can_not_flow = 0;//判断当前服务器对此消费者节点是否已无能为力
    int id_net_consumer = 0;
    int cost_bandwidth = 0;
    int min_distance = INF;
    int min_id_net = 0;
    int min_id_consumer = 0;
    int consumer_remain = 0;
    char *book = NULL;//用于标记一个消费者节点是否需要供流量
    STNode *p = NULL;
    book = (char *)malloc(num_consumer);
    printf("[INFO] <cluster_flow_out_process> cluster_id = %d id_service_net = %d\n",cluster_id,id_service_net);
    //初始化
    for(i = 0; i < num_consumer; i++)
    {
        if(consumer_link_info[i].cluster_id == cluster_id)
        {
            book[i] = 1;//域内节点需要供流量
            consumer_remain++;
        }
        else
        {
            book[i] = 0;
        }
    }
    
    while(consumer_remain > 0)
    {
        consumer_remain = 0;
        min_distance = INF;
        for(i = 0; i < num_consumer; i++)
        {
            if(book[i] == 1)
            {
                id_net_consumer = consumer_link_info[i].id_net_dot;
                if(min_distance >= p2p_path_info[id_service_net][id_net_consumer].distance)
                {
                    min_distance = p2p_path_info[id_service_net][id_net_consumer].distance;
                    min_id_consumer = i;
                }
            }
        }

        if(book[min_id_consumer] == 1)
        {
            min_id_net = consumer_link_info[min_id_consumer].id_net_dot;
            is_consumer_clear = 0;
            is_can_not_flow = 0;
            if(p2p_path_info[id_service_net][min_id_net].bandwidth > 0)
            {
                if(p2p_path_info[id_service_net][min_id_net].bandwidth >= consumer_link_info[min_id_consumer].need_bandwidth)
                {
                    //如果链路带宽大于等于消费者结点所需，消费者结点的需求便是链路带宽的消耗值
                    cost_bandwidth = consumer_link_info[min_id_consumer].need_bandwidth; 
                }
                else
                {
                    //如果链路带宽不大于等于消费者结点所需，消费者结点的需求便是链路带宽的消耗值
                    cost_bandwidth = p2p_path_info[id_service_net][min_id_net].bandwidth;
                }
                result_info[num_result_link].cost_bandwidth = cost_bandwidth;
                result_info[num_result_link].id_consumer = consumer_link_info[min_id_consumer].id_consumer;
                result_info[num_result_link].link_num = p2p_path_info[id_service_net][min_id_net].link_num;
                result_info[num_result_link].p_path->id_node = id_service_net;
                result_info[num_result_link].cluster_id = cluster_id;
                //路径拷贝
                p = list_link(p2p_path_info[id_service_net][min_id_net].p_path, result_info[num_result_link].p_path, result_info[num_result_link].link_num);
                if(p == NULL)
                {
                    printf("[ERROR] <cluster_flow_out_process> p == NULL\n");
                    break;
                }
                //剪枝处理函数
                is_consumer_clear = cluster_link_prune(cluster_id, cluster_info, id_service_net, result_info[num_result_link].id_consumer, cost_bandwidth, p2p_path_info, link_array, consumer_link_info, net_node_info);
                num_result_link++;
                if(is_consumer_clear == 1)
                {
                    printf("[INFO] <cluster_flow_out_process> id_consumer = %d is clear\n",min_id_consumer);
                }
                if(p2p_path_info[id_service_net][min_id_net].distance < INF)
                {
                    is_can_not_flow = 0;
                }
                else
                {
                    is_can_not_flow = 1;
                    printf("[INFO] <cluster_flow_out_process> id_consumer = %d is can not flow\n",min_id_consumer);
                }
            }
            else
            {
                is_can_not_flow = 1;
                printf("[INFO] <cluster_flow_out_process> id_consumer = %d is can not flow\n",min_id_consumer);
            }
            if(is_consumer_clear || is_can_not_flow)
            {
                book[min_id_consumer] = 0;//域内该消费者节点流量供应完毕
            }
        }

        for(i = 0; i < num_consumer; i++)
        {
            if(book[i] == 1)
            {
                consumer_remain++;
            }
        }
        //dijkstra_debug_net_p2p(cluster_id, net_node_info, p2p_path_info);
    }

    free(book);
    book = NULL;
}

/************************************************************************
函数功能:聚类处理函数:域内输出链路优化函数
功能详解:在选定一个聚类中心后，对域内已有的输出链路进行优化处理，减掉经过该服务器的多余枝叶
入参解释:cluster_id(聚类编号)id_service_net(选定的新的聚类中心)link_array(有向链路数组)
result_info(输出结果链路信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void cluster_result_optimize(int cluster_id, int id_service_net, STLink **link_array, STResultInfo *result_info)
{
    int i  = 0;
    int j = 0;
    int m = 0;
    int num = 0;
    int id_node_A = 0;
    int id_node_B = 0;
    STNode *p_begin = NULL;//待释放输出链路的头结点
    STNode *p = NULL;//暂存指针
    STNode *p_end = NULL;//待释放输出链路的尾节点
    
    for(i = 0; i < num_result_link; i++)
    {
        if(result_info[i].cluster_id == cluster_id)
        {
            p_begin = result_info[i].p_path;
            p = p_begin->pnext;
            num = 0;
            for(j = 0; j < result_info[i].link_num; j++)
            {
                num++;
                if(p->id_node == id_service_net)//链路中存在节点经过了当前需部署的视频服务器节点，则该链路需要优化:
                {
                    //原输出信息调整，减掉枝叶，更新路径长度，变换路径头结点
                    p_end = p;
                    result_info[i].link_num = result_info[i].link_num - num;
                    result_info[i].p_path = p_end;
                    p = p_begin;
                    for(m = 0; m < num; m++)
                    {
                        id_node_A = p->id_node;
                        id_node_B = p->pnext->id_node;
                        //释放链路link_array
                        link_array[id_node_A][id_node_B].link_bandwidth = link_array[id_node_A][id_node_B].link_bandwidth + result_info[i].cost_bandwidth;
                        link_array[id_node_A][id_node_B].cost_per_bandwidth = link_array[id_node_A][id_node_B].cost_per_bandwidth_history;
                        //将原输出链路指针释放
                        p_begin = p;
                        p = p->pnext;
                        if(p_begin != NULL)
                        {
                            free(p_begin);
                            p_begin = NULL;
                        }
                    }
                    break;//退出本链路的优化循环，进入下一条链路的优化处理中
                }
                p = p->pnext;
            }
        }
    }
}


/************************************************************************
函数功能:聚类处理函数:子类合并处理函数
功能详解:当子类中部署服务器数量达到部署上限或子类已可自给自足时，将子类资源释放到其母类中，并将子类置为释放类
入参解释:cluster_id(聚类编号)cluster_info(聚类信息)consumer_link_info(域内消费节点链接信息)
net_node_info(域内网络节点信息)result_info(输出结果链路信息)
返回值:ret(成功则返回母类id，否则返回0)
函数作者:陈自立
*************************************************************************/
int cluster_merge_sub_release(int cluster_id, STClusterInfo *cluster_info, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info, STResultInfo *result_info)
{
    int i = 0;
    int cluster_id_origin = (int)(cluster_id/2);//母类类id
    int ret = 0;
    printf("[INFO] <cluster_merge_sub_release> cluster_id = %d cluster_id_origin = %d\n", cluster_id, cluster_id_origin);
    if(cluster_id_origin == 0)
    {
        printf("[ERROR] <cluster_merge_sub_release> want to release cluster 1\n");
        return ret;
    }
    if(cluster_info[cluster_id_origin].valid != 0)
    {
        printf("[ERROR] <cluster_merge_sub_release> cluster_info[%d].valid != 0\n",cluster_id_origin);
    }
    //释放网络节点
    for(i = 0; i < num_net_dot; i++)
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            net_node_info[i].cluster_id = cluster_id_origin;
        }
    }
    //释放消费者节点
    for(i = 0; i < num_consumer; i++)
    {
        if((consumer_link_info[i].cluster_id == cluster_id) && (consumer_link_info[i].need_bandwidth > 0))
        {
            consumer_link_info[i].cluster_id = cluster_id_origin;
        }
    }
    //释放输出链路信息
    for(i = 0; i < num_result_link; i++)
    {
        if(result_info[i].cluster_id == cluster_id)
        {
            result_info[i].cluster_id = cluster_id_origin;
        }
    }
    //释放类信息
    cluster_info[cluster_id].valid = 2;//已释放类
    cluster_info[cluster_id_origin].num_server_current += cluster_info[cluster_id].num_server_current;

    if((cluster_id % 2) == 0)//偶数编号类
    {
        if(cluster_info[(cluster_id + 1)].valid== 2)//兄弟类也已经释放
        {
            cluster_info[cluster_id_origin].valid = 1;
            cluster_info[cluster_id_origin].num_cluster_consumer = cluster_info[cluster_id].num_cluster_consumer + cluster_info[(cluster_id+1)].num_cluster_consumer;
        }
    }
    else//奇数编号类
    {
        if(cluster_info[(cluster_id - 1)].valid== 2)//兄弟类也已经释放
        {
            cluster_info[cluster_id_origin].valid = 1;
            cluster_info[cluster_id_origin].num_cluster_consumer = cluster_info[cluster_id].num_cluster_consumer + cluster_info[(cluster_id-1)].num_cluster_consumer;
        }
    }

    if(cluster_info[cluster_id_origin].valid == 1)
    {
        printf("[INFO] <cluster_merge_sub_release> cluster_id_origin = %d merge success num_cluster_consumer = %d num_server_current = %d\n",cluster_id_origin,cluster_info[cluster_id_origin].num_cluster_consumer,cluster_info[cluster_id_origin].num_server_current);
        ret = cluster_id_origin;
    }

    return ret;
}


/************************************************************************
函数功能:聚类处理函数:域内流量分发入口
功能详解:域内流量分发入口
入参解释:cluster_id(聚类编号)cluster_info(聚类信息)p2p_path_info(点到点的最短路径信息)link_array(有向链路数组)
consumer_link_info(域内消费节点链接信息)net_node_info(域内网络节点信息)result_info(输出结果链路信息)
返回值:id_center(返回域内中心节点ID)
函数作者:陈自立
*************************************************************************/
void cluster_flow_out_main(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info, STResultInfo *result_info)
{
    int i = 0;
    int id_service_net = 0;
    int cluster_id_origin = 0;//子类合并成功后得到的母类编号
    int flag = 1;
    if(num_consumer > 300)//高级用例分为两个中级子类，然后再发流量
    {
        flag = HARD_FLAG;
    }
    //调用dijkstra处理函数，域内遍历最短路径
    dijkstra_process_main(cluster_id, link_array, p2p_path_info, net_node_info, consumer_link_info);
    //打印出域内的网络节点和消费者节点编号:
    //cluster_debug_net_consumer(cluster_id, net_node_info, consumer_link_info);
    printf("[INFO] <cluster_flow_out_main> cluster_id = %d\n",cluster_id);
    if(cluster_id <= flag)//对于原始类而言，必须保证将所有消费者节点供满
    {
        do
        {
            //调试函数:打印域内各节点之间的最短路径
            //dijkstra_debug_net_p2p(cluster_id, net_node_info, p2p_path_info);
            //获取新的聚类中心:
            id_service_net = cluster_center(cluster_id, cluster_info, p2p_path_info, link_array, consumer_link_info, net_node_info);
            net_node_info[id_service_net].is_server = 1;//标记该网络节点为服务器节点
            //根据新选定的聚类中心，对已有输出结果进行一次优化
            if(num_consumer <= 300)
            {
                result_optimize(net_node_info, link_array, result_info);
            }
            else
            {
                cluster_result_optimize(cluster_id,id_service_net, link_array, result_info);
            }
            printf("[INFO] <cluster_flow_out_main> cluster_id = %d id_service_net = %d num_cluster_consumer = %d\n",cluster_id,id_service_net,cluster_info[cluster_id].num_cluster_consumer);
            //流量分发:
            cluster_flow_out_process(cluster_id, cluster_info, id_service_net, p2p_path_info, link_array, consumer_link_info, net_node_info,result_info);
            net_node_info[id_service_net].cluster_id = (int)(cluster_id/2);//将原服务器部署节点存入其母类
            //剪枝完毕后，对域内重新初始化，更新路径
            cluster_link_prune_pre(cluster_id, cluster_info, p2p_path_info, link_array, net_node_info, consumer_link_info);
            //调用dijkstra处理函数，域内遍历最短路径
            dijkstra_process_main(cluster_id, link_array, p2p_path_info, net_node_info, consumer_link_info);
            
        }while(cluster_info[cluster_id].num_cluster_consumer > 0);//当域内尚未满足自给自足时(域内消费者节点全部清除)
    }
    else//对于非原始类而言，当域内服务器数量达到部署上限时，考虑合并兄弟类，相互资源共享
    {
        for(i = 0; i < cluster_info[cluster_id].max_num_server; i++)
        {
            //获取新的聚类中心:
            id_service_net = cluster_center(cluster_id, cluster_info, p2p_path_info, link_array, consumer_link_info, net_node_info);
            net_node_info[id_service_net].is_server = 1;//标记该网络节点为服务器节点
            //根据新选定的聚类中心，对已有输出结果进行一次优化
            if(num_consumer <= 300)
            {
                result_optimize(net_node_info, link_array, result_info);
            }
            else
            {
                cluster_result_optimize(cluster_id,id_service_net, link_array, result_info);
            }
            printf("[INFO] <cluster_flow_out_main> cluster_id = %d id_service_net = %d num_cluster_consumer = %d\n",cluster_id,id_service_net,cluster_info[cluster_id].num_cluster_consumer);
            //流量分发:
            cluster_flow_out_process(cluster_id, cluster_info, id_service_net, p2p_path_info, link_array, consumer_link_info, net_node_info,result_info);
            net_node_info[id_service_net].cluster_id = (int)(cluster_id/2);//将原服务器部署节点存入其母类
            //剪枝完毕后，对域内重新初始化，更新路径
            cluster_link_prune_pre(cluster_id, cluster_info, p2p_path_info, link_array, net_node_info, consumer_link_info);
            //调用dijkstra处理函数，域内遍历最短路径
            dijkstra_process_main(cluster_id, link_array, p2p_path_info, net_node_info, consumer_link_info);
            if(cluster_info[cluster_id].num_cluster_consumer == 0)//域内满足自给自足时推出循环
            {
                break;
            }
        }
        cluster_id_origin = cluster_merge_sub_release(cluster_id, cluster_info, consumer_link_info, net_node_info, result_info);
        if(cluster_id_origin > 0)//母类释放成功
        {
            //对母类重新初始化，更新路径
            cluster_link_prune_pre(cluster_id_origin, cluster_info, p2p_path_info, link_array, net_node_info, consumer_link_info);
            //调用dijkstra处理函数，域内遍历最短路径
            dijkstra_process_main(cluster_id_origin, link_array, p2p_path_info, net_node_info, consumer_link_info);
        }
    }
    printf("[INFO]<cluster_flow_out_main> cluster%d: server num : %d\n",cluster_id,cluster_info[cluster_id].num_server_current);
}


/************************************************************************
函数功能:聚类处理函数:递归调用
功能详解:确定域内需要部署的最少服务器台数，当最少服务器台数大于2时，考虑迭代分裂
入参解释:cluster_id(聚类编号)cluster_info(聚类信息)p2p_path_info(点到点的最短路径信息)
link_array(有向链路数组)consumer_link_info(域内消费节点链接信息)net_node_info(域内网络节点信息)
返回值:cluster_id_ret(较大的一个聚类编号)
函数作者:陈自立
*************************************************************************/
int cluster_process(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info)
{
    int cluster_id_ret = 0;
    int id_cluster_A = 0;//域内两个最远网络节点的编号
    int id_cluster_B = 0;  
    
    //调用dijkstra处理函数，全图遍历最短路径
    dijkstra_process_main(cluster_id, link_array, p2p_path_info, net_node_info, consumer_link_info);
    //找出消费者结点之间的最远距离
    dijkstra_process_consumer_max(cluster_id, p2p_path_info, consumer_link_info, &id_cluster_A, &id_cluster_B);
    if(id_cluster_A == id_cluster_B)
    {
        printf("[ERROR] <cluster_process> id_cluster_A == id_cluster_B = %d\n",id_cluster_A);
        return cluster_id_ret;
    }
    //调用分裂函数，依据距离将当前类分为两类
    cluster_split(cluster_id, id_cluster_A, id_cluster_B, p2p_path_info, net_node_info, consumer_link_info);
    cluster_info[cluster_id].valid = 0;//生成子类后将母类暂时置为无效
    //调用聚类初始化函数，初始化分裂后新的聚类中的p2p_path_info信息
    cluster_pre((2*cluster_id),cluster_info,p2p_path_info,link_array,net_node_info,consumer_link_info);
    cluster_pre((2*cluster_id + 1),cluster_info,p2p_path_info,link_array,net_node_info,consumer_link_info);
    //域内最大服务器数目是一定的，只能赋值一次，之后不能做更改,不能写入函数cluster_pre中重复调用
    cluster_info[(2*cluster_id)].need_service_num = calculate_server_minimum((2*cluster_id), link_array, consumer_link_info, net_node_info);
    cluster_info[(2*cluster_id + 1)].need_service_num = calculate_server_minimum((2*cluster_id + 1), link_array, consumer_link_info, net_node_info);
    cluster_info[(2*cluster_id)].max_num_server =  cluster_info[(2*cluster_id)].need_service_num + max_add_cluster_server_num;
    cluster_info[(2*cluster_id + 1)].max_num_server =  cluster_info[(2*cluster_id + 1)].need_service_num + max_add_cluster_server_num;
    cluster_id_ret = 2*cluster_id + 1;
    return cluster_id_ret;
}


/************************************************************************
函数功能:聚类算法总入口
功能详解:调用各聚类算法接口
入参解释:p2p_path_info(点到点的最短路径信息)link_array(有向链路数组)consumer_link_info(域内消费节点链接信息)
net_node_info(域内网络节点信息)result_info(输出结果链路信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void cluster_main(STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info, STResultInfo *result_info)
{
    int i = 1;
    int cluster_id = 1;//初始聚类ID为1
    int cluster_id_max = 1;
    int cluster_id_ret = 0;
    STClusterInfo *cluster_info = NULL;
    cluster_info = (STClusterInfo *)malloc((num_consumer + 2) * sizeof(STClusterInfo));//聚类个数最多不会超出消费者节点个数，聚类信息数组从1开始
    //预初始化聚类信息数组,同时初始化初始聚类
    cluster_info_pre(cluster_info);
    cluster_pre(cluster_id, cluster_info, p2p_path_info, link_array, net_node_info, consumer_link_info);
    cluster_info[cluster_id].need_service_num = calculate_server_minimum(cluster_id, link_array, consumer_link_info, net_node_info);
    cluster_info[cluster_id].max_num_server = cluster_info[cluster_id].need_service_num + max_add_cluster_server_num;//域内最大服务器数目是一定的，只能赋值一次，之后不能做更改,不能写入函数cluster_pre中重复调用
    printf("[INFO] <cluster_main> begin\n");

    for(i = 1; i <= cluster_id_max; i++)
    {
        if(cluster_info[i].valid == 1)
        {
            cluster_id = i;
            if(num_consumer <= 100)//初级用例
            {
                if((cluster_info[i].need_service_num > SERVER_NUM_LIMIT_PRI) && (i <= (num_consumer/2)))
                {
                    printf("[INFO] <cluster_main> cluster_id = %d, num_cluster_consumer = %d, need_service_num = %d, max_num_server = %d\n",i,cluster_info[i].num_cluster_consumer,cluster_info[i].need_service_num, cluster_info[i].max_num_server);
                    cluster_id_ret = cluster_process(i, cluster_info, p2p_path_info, link_array, consumer_link_info, net_node_info);
                    if(cluster_id_ret > cluster_id_max)
                    {
                        cluster_id_max = cluster_id_ret;
                    }
                }
                else
                {
                    printf("[INFO] <cluster_main> this is smallest cluster ,the cluster_id = %d need_service_num = %d\n",i,cluster_info[i].need_service_num);
                }
            }
            if((num_consumer > 100) && (num_consumer <= 300))//中级用例
            {
                if((cluster_info[i].need_service_num > SERVER_NUM_LIMIT_MID) && (i <= (num_consumer/2)))
                {
                    printf("[INFO] <cluster_main> cluster_id = %d, num_cluster_consumer = %d, need_service_num = %d, max_num_server = %d\n",i,cluster_info[i].num_cluster_consumer,cluster_info[i].need_service_num, cluster_info[i].max_num_server);
                    cluster_id_ret = cluster_process(i, cluster_info, p2p_path_info, link_array, consumer_link_info, net_node_info);
                    if(cluster_id_ret > cluster_id_max)
                    {
                        cluster_id_max = cluster_id_ret;
                    }
                }
                else
                {
                    printf("[INFO] <cluster_main> this is smallest cluster ,the cluster_id = %d need_service_num = %d\n",i,cluster_info[i].need_service_num);
                }
            }
            if(num_consumer > 300)//高级用例
            {
                if((cluster_info[i].need_service_num > SERVER_NUM_LIMIT_HARD) && (i <= (num_consumer/2)))
                {
                    printf("[INFO] <cluster_main> cluster_id = %d, num_cluster_consumer = %d, need_service_num = %d, max_num_server = %d\n",i,cluster_info[i].num_cluster_consumer,cluster_info[i].need_service_num, cluster_info[i].max_num_server);
                    cluster_id_ret = cluster_process(i, cluster_info, p2p_path_info, link_array, consumer_link_info, net_node_info);
                    if(cluster_id_ret > cluster_id_max)
                    {
                        cluster_id_max = cluster_id_ret;
                    }
                }
                else
                {
                    printf("[INFO] <cluster_main> this is smallest cluster ,the cluster_id = %d need_service_num = %d\n",i,cluster_info[i].need_service_num);
                }
            }
        }
    }

    //自大到小处理最小类:对初始类无上限部服务器，其他类采用分裂+单元部署+合并迭代策略:
    for(i = cluster_id_max; i > 0; i--)
    {
        if(cluster_info[i].valid == 1)
        {
            cluster_flow_out_main(i, cluster_info, p2p_path_info, link_array, consumer_link_info, net_node_info, result_info);
        }
    }
    
    printf("[INFO]<cluster_main> num_result_link = %d num_consumer_remain = %d\n",num_result_link,num_consumer_remain);

    if(cluster_info == NULL)
    {
        printf("[ERROR] <cluster_main> cluster_info == NULL\n");
    }
    else
    {
        free(cluster_info);
        cluster_info = NULL;
    }
    
}


/************************************************************************
函数功能:聚类调试函数:打印出某一聚类中的网络节点与消费者节点编号
功能详解:打印出某一聚类中的网络节点与消费者节点编号
入参解释:cluster_id(聚类编号)net_node_info(域内网络节点信息)consumer_link_info(域内消费节点链接信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void cluster_debug_net_consumer(int cluster_id, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info)
{
    int i = 0;
    printf("cluster%d info:\n",cluster_id);
    printf("net node info:\n");
    for(i = 0; i < num_net_dot; i++)
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            printf("net_node_id: %d\n",i);
        }
    }
    printf("consumer node info:\n");
    for(i = 0; i < num_consumer; i++)
    {
        if(consumer_link_info[i].cluster_id == cluster_id)
        {
            printf("consumer_node_id: %d\n",i);
        }
    }
}


