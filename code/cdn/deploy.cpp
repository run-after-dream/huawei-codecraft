/***********************************************************************
文件名称:deploy.cpp
文件功能:官方提供的功能函数入口，编写主流程框架函数接口，包括全局变量定义
文件创建时间:20170307
文件作者:huawei
修改时间:20170409
修改人:陈自立
***********************************************************************/

#include "deploy.h"
#include "dijkstra.h"
#include "cluster.h"
#include "mybase.h"
#include "myio.h"
#include <stdio.h>
#include <stdlib.h>

int num_net_dot;//网络节点数量
int num_link;//网络链路数量
int num_consumer;//消费节点数量
int cost_server;//视频内容服务器部署成本
int num_link_actual;//考虑双向流量时的实际链路数量
int num_result_link;//输出网络路径数
int num_consumer_remain;//剩余消费者节点数
int num_server;//已部署服务器数量
int max_add_cluster_server_num;//单个子聚类中最大可部署的服务器叠加量，当聚类中服务器数量超出该域内最大可部署量(至少需要服务器个数与该值的和)时，考虑将该聚类与其兄弟聚类合并为母聚类，该参数对初始大类(cluster_id = 1)无效
int solution_cluster_center = 0;//选择聚类中心的策略 0:通过出度-距离获得聚类中心，1:通过出度最大选聚类中心

/************************************************************************
函数功能:基本功能调试函数(调试用)
功能详解:调试用函数，将所有服务器部署在消费节点上端
入参解释:consumer_link_info(消费节点链接信息),result_debug_info(输出网络路径信息)
返回值:网络路径数
函数作者:陈自立
*************************************************************************/
int process_debug(STConsumerLinkInfo *consumer_link_info, STResultDebugInfo *result_debug_info)
{
    int i = 0;
    for(i = 0; i < num_consumer; i++)
    {
        result_debug_info[i].id_server = consumer_link_info[i].id_net_dot;
        result_debug_info[i].id_consumer = consumer_link_info[i].id_consumer;
        result_debug_info[i].cost_bandwidth = consumer_link_info[i].need_bandwidth;
    }
    return i;
}

/************************************************************************
函数功能:输入信息预处理模块
功能详解:预处理输入信息,将link_info中的数据存入link_array中,初始化点到点的最短路径信息，
将网络节点信息初始化
入参解释:link_info(链路信息)link_array(有向链路数组)p2p_path_info(存放点到点的最短路径信息)
net_node_info(网络节点信息)result_info(输出结果链路信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void process_pre(STLinkInfo *link_info, STLink **link_array, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STResultInfo *result_info)
{
    int i = 0;
    int m = 0;
    int n = 0;

    printf("[INFO] <process_pre> begin\n");

    if(num_consumer <= 100)//初级用例
    {
        max_add_cluster_server_num = MAX_ADD_CLUSTER_SERVER_NUM_PRI;
    }
    if((num_consumer > 100) && (num_consumer <= 300))//中级用例
    {
        max_add_cluster_server_num = MAX_ADD_CLUSTER_SERVER_NUM_MID;
    }
    if(num_consumer > 300)//高级用例
    {
        max_add_cluster_server_num = MAX_ADD_CLUSTER_SERVER_NUM_HARD;
    }
    
    //初始化
    for(m = 0; m < num_net_dot; m++)
    {
        for(n = 0; n < num_net_dot; n++)
        {
            if(m != n)
            {
                link_array[m][n].link_bandwidth = 0;//初始化有向链路数组
                link_array[m][n].link_bandwidth_history= link_array[m][n].link_bandwidth;
                link_array[m][n].cost_per_bandwidth = INF;
                link_array[m][n].cost_per_bandwidth_history = link_array[m][n].cost_per_bandwidth;
                p2p_path_info[m][n].distance = INF;//初始化点到点的最短路径信息
                p2p_path_info[m][n].bandwidth = 0;
            }
            else
            {
                link_array[m][n].link_bandwidth = INF;//初始化有向链路数组
                link_array[m][n].link_bandwidth_history= link_array[m][n].link_bandwidth;
                link_array[m][n].cost_per_bandwidth = 0;
                link_array[m][n].cost_per_bandwidth_history = link_array[m][n].cost_per_bandwidth;
                p2p_path_info[m][n].distance = 0;//初始化点到点的最短路径信息
                p2p_path_info[m][n].bandwidth = INF;
            }
            p2p_path_info[m][n].link_num = 0;
            p2p_path_info[m][n].p_path = (STNode *)malloc(sizeof(STNode));//为头结点先分配一段空间
            p2p_path_info[m][n].p_path->id_node = m;
            p2p_path_info[m][n].p_path->pnext = NULL;
            p2p_path_info[m][n].p_path_end = p2p_path_info[m][n].p_path;//尾节点即为路径中最后一点，初始时即为源节点
        }
    }

    for(i = 0; i < num_link; i++)
    {
        m = link_info[i].id_link_begin;
        n = link_info[i].id_link_end;
        link_array[m][n].link_bandwidth = link_info[i].link_bandwidth;
        link_array[m][n].link_bandwidth_history= link_array[m][n].link_bandwidth;
        link_array[m][n].cost_per_bandwidth = link_info[i].cost_per_bandwidth;
        link_array[m][n].cost_per_bandwidth_history = link_array[m][n].cost_per_bandwidth;
        link_array[n][m].link_bandwidth = link_info[i].link_bandwidth;
        link_array[n][m].link_bandwidth_history= link_array[n][m].link_bandwidth;
        link_array[n][m].cost_per_bandwidth = link_info[i].cost_per_bandwidth;
        link_array[n][m].cost_per_bandwidth_history = link_array[n][m].cost_per_bandwidth;
    }

    //将相邻节点的点到点路径信息存入p2p_path_info
    for(m = 0; m < num_net_dot; m++)
    {
        net_node_info[m].out_flow = 0;
        net_node_info[m].valid= 1;
        net_node_info[m].cluster_id = 1;
        net_node_info[m].is_server= 0;
        net_node_info[m].id_stack_node = 0;
        for(n = 0; n < num_net_dot; n++)
        {
            if((link_array[m][n].cost_per_bandwidth < INF) && (m != n))
            {
                p2p_path_info[m][n].distance = link_array[m][n].cost_per_bandwidth;
                p2p_path_info[m][n].link_num = 1;
                p2p_path_info[m][n].bandwidth = link_array[m][n].link_bandwidth;
                p2p_path_info[m][n].p_path->pnext = (STNode *)malloc(sizeof(STNode));
                p2p_path_info[m][n].p_path->pnext->id_node = n;
                p2p_path_info[m][n].p_path->pnext->pnext = NULL;
                p2p_path_info[m][n].p_path_end = p2p_path_info[m][n].p_path->pnext;
             }
        }
    }
    
    for(i = 0; i < MAX_LINK_NUM; i++)
    {
        result_info[i].cluster_id = 0;
        result_info[i].link_num = 0;
        result_info[i].cost_bandwidth = 0;
        result_info[i].id_consumer = 0;
        result_info[i].p_path = (STNode *)malloc(sizeof(STNode));
        result_info[i].p_path->id_node = 0;
        result_info[i].p_path->pnext = NULL;
    }

    printf("[INFO] <process_pre> end\n");
    
}

/************************************************************************
函数功能:全局输出结果优化处理函数
功能详解:在输出结果之前，对输出结果在进行一次优化
入参解释:net_node_info(域内网络节点信息)link_array(有向链路数组)result_info(输出结果链路信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void result_optimize(STNetNodeInfo *net_node_info, STLink **link_array, STResultInfo *result_info)
{
    int k = 0;
    int i  = 0;
    int j = 0;
    int m = 0;
    int num = 0;
    int id_service_net = 0;
    int id_node_A = 0;
    int id_node_B = 0;
    STNode *p_begin = NULL;//待释放输出链路的头结点
    STNode *p = NULL;//暂存指针
    STNode *p_end = NULL;//待释放输出链路的尾节点
    for(k = 0; k < num_net_dot; k++)
    {
        if(net_node_info[k].is_server == 1)
        {
            id_service_net = k;
            for(i = 0; i < num_result_link; i++)
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
}


/************************************************************************
函数功能:debug函数
功能详解:输出结果有效性判断，权重计算函数
入参解释:link_array(有向链路数组),consumer_link_info(消费节点链接信息)result_info(输出结果链路信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void result_check_debug(STLink **link_array, STConsumerLinkInfo *consumer_link_info, STResultInfo *result_info)
{
    int i = 0;
    int j = 0;
    int sum = 0;//输出总权重
    int sum_server = 0;
    int sum_waste_flow = 0;//计算单条链路双向利用时的重叠流量费用
    int id_net_A = 0;
    int id_net_B = 0;
    int id_consumer = 0;
    int is_correct = 1;// 1:correct 0:error
    int link_cost = 0;
    STNode *p = NULL;
    STLink **result_array = NULL;
    int *consumer_flow = NULL;
    char *server_flag = NULL;
    //申请2个二维数组的内存空间，用于存放有向链路的数据和点到点之间的最短路径信息
    result_array= (STLink **)malloc(num_net_dot * sizeof(STLink*));
     for(i = 0; i < num_net_dot; i++)
    {
        result_array[i] = (STLink *)malloc(num_net_dot * sizeof(STLink));
    }
    //申请一个数组空间用于存放各个消费者节点获取到的流量大小:
    consumer_flow = (int *)malloc(num_consumer *sizeof(int));
    //申请一个数组空间用于标记各个网络节点是否部署服务器:
    server_flag = (char *)malloc(num_net_dot *sizeof(char));
    
    //初始化:
    for(i = 0; i < num_consumer; i++)
    {
        consumer_flow[i] = 0;
    }
    for(i = 0; i < num_net_dot; i++)
    {
        server_flag[i] = 0;
        for(j = 0; j < num_net_dot; j++)
        {
            result_array[i][j].link_bandwidth = 0;
            result_array[i][j].link_bandwidth_history = link_array[i][j].link_bandwidth_history;
            result_array[i][j].cost_per_bandwidth = link_array[i][j].cost_per_bandwidth_history;
        }
    }
    
    //记录输出结果中的服务器数量，各链路消耗带宽情况，各服务器节点获取带宽情况
    for(i = 0; i < num_result_link; i++)
    {
        p = result_info[i].p_path;
        server_flag[p->id_node] += result_info[i].cost_bandwidth;//标记服务器,顺带记录下该服务器的流量利用情况
        for(j = 0; j < result_info[i].link_num; j++)
        {
            id_net_A = p->id_node;
            id_net_B = p->pnext->id_node;
            result_array[id_net_A][id_net_B].link_bandwidth += result_info[i].cost_bandwidth;//记录带宽消耗量
            p=p->pnext;
        }
        id_consumer = result_info[i].id_consumer;
        consumer_flow[id_consumer] += result_info[i].cost_bandwidth;//记录消费者节点带宽供应量
    }

    //先算服务器部署成本:
    for(i = 0; i < num_net_dot; i++)
    {
        if(server_flag[i] > 0)
        {
           sum_server++; 
        }
    }
    sum = sum + (sum_server * cost_server);
    //结果比对:
    for(i = 0; i < num_net_dot; i++)
    {
        for(j = 0; j < num_net_dot; j++)
        {
            if((result_array[i][j].link_bandwidth > 0) && (result_array[j][i].link_bandwidth > 0))//判断单条链路上是否存在双向流量
            {
                if(result_array[i][j].link_bandwidth > result_array[j][i].link_bandwidth)
                {
                    sum_waste_flow += result_array[j][i].link_bandwidth * (result_array[i][j].cost_per_bandwidth + result_array[j][i].cost_per_bandwidth);
                }
                else
                {
                    sum_waste_flow += result_array[i][j].link_bandwidth * (result_array[i][j].cost_per_bandwidth + result_array[j][i].cost_per_bandwidth);
                }
            }
            //链路带宽超出原有带宽
            if(result_array[i][j].link_bandwidth > result_array[i][j].link_bandwidth_history)
            {
                printf("[RESULT ERROR] result_array[%d][%d].link_bandwidth_history = %d but result_array[%d][%d].link_bandwidth = %d\n",i,j,result_array[i][j].link_bandwidth_history,i,j,result_array[i][j].link_bandwidth);
                is_correct = 0;
            }
            //计算链路cost:
            sum = sum + (result_array[i][j].link_bandwidth * result_array[i][j].cost_per_bandwidth);
        }
    }
    //消费者节点流量没有供满
    for(i = 0; i < num_consumer; i++)
    {
        if(consumer_flow[i] < consumer_link_info[i].need_bandwidth_history)
        {
            printf("[RESULT ERROR] consumer_link_info[%d].need_bandwidth = %d but consumer_flow[%d] = %d cluster_id = %d\n",i,consumer_link_info[i].need_bandwidth,i,consumer_flow[i],consumer_link_info[i].cluster_id);
            is_correct = 0;
        }
        if(consumer_flow[i] > consumer_link_info[i].need_bandwidth_history)
        {
            printf("[RESULT INFO] consumer_link_info[%d].need_bandwidth = %d and consumer_flow[%d] = %d cluster_id = %d\n",i,consumer_link_info[i].need_bandwidth,i,consumer_flow[i],consumer_link_info[i].cluster_id);
        }
    }

    if(is_correct == 1)
    {
        link_cost =sum - (sum_server * cost_server);
        printf("[RESULT INFO] sum = %d, num_server = %d link_cost = %d\n",sum,sum_server,link_cost);
        for(i = 0; i < num_net_dot; i++)
        {
            if(server_flag[i] > 0)
            {
                printf("server id = %d flow out = %d\n",i,server_flag[i]);
            }
        }
        if(sum_waste_flow > 0)//存在流量浪费情况打出提示信息
        {
            printf("[RESULT INFO] there are some flow waste, waste cost = %d, if we can solve, the cost will below the [%d]\n",sum_waste_flow,(sum - sum_waste_flow));
        }
        else
        {
            printf("[RESULT CORRECT] success!\n");
        }
    }

    //释放内存空间
    for(i = 0; i < num_net_dot; i++)
    {
        free(result_array[i]);
        result_array[i] = NULL;
    }
    free(result_array);
    result_array = NULL;
    free(consumer_flow);
    consumer_flow = NULL;
    free(server_flag);
    server_flag = NULL;
        
}


/************************************************************************
函数功能:算法处理总入口
功能详解:调用处理函数接口，得到输出结果
入参解释:link_info(链路信息),consumer_link_info(消费节点链接信息)result_info(输出结果链路信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void process_main(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info, STResultInfo *result_info)
{
    int i = 0; 
    STLink **link_array = NULL;//用于存放有向链路的数据
    STP2PPathInfo **p2p_path_info = NULL;//用于存放点到点之间的最小路径的长度，以及最短路径头结点的指针
    STNetNodeInfo *net_node_info = NULL;//用于存放网络节点的信息

    net_node_info = (STNetNodeInfo *)malloc(num_net_dot * sizeof(STNetNodeInfo));
    //申请2个二维数组的内存空间，用于存放有向链路的数据和点到点之间的最短路径信息
    link_array= (STLink **)malloc(num_net_dot * sizeof(STLink*));
    p2p_path_info = (STP2PPathInfo **)malloc(num_net_dot * sizeof(STP2PPathInfo *));
    for(i = 0; i < num_net_dot; i++)
    {
        link_array[i] = (STLink *)malloc(num_net_dot * sizeof(STLink));
        p2p_path_info[i] = (STP2PPathInfo *)malloc(num_net_dot * sizeof(STP2PPathInfo));
    }
   
    //预处理输入信息
    process_pre(link_info, link_array, p2p_path_info, net_node_info, result_info);

    //调用聚类处理函数
    cluster_main(p2p_path_info, link_array, consumer_link_info, net_node_info, result_info);
    //全局输出结果优化处理
    result_optimize(net_node_info, link_array, result_info);
    //检查输出结果:
    result_check_debug( link_array, consumer_link_info, result_info);
    
    //释放内存空间
    for(i = 0; i < num_net_dot; i++)
    {
        free(link_array[i]);
        link_array[i] = NULL;
        free(p2p_path_info[i]);
        p2p_path_info[i] = NULL;
    }
    free(link_array);
    link_array = NULL;
    free(p2p_path_info);
    p2p_path_info =NULL;
    free(net_node_info);
    net_node_info = NULL;
    
}

/************************************************************************
函数功能:待完成功能总入口
功能详解:华为官方提供的功能入口，在main函数中直接调用
入参解释:topo(官方接口读取输入文件，以字符串数组形式体现),line_num(输入文件总函数，不包含最后一空行)
filename(输出文件路径名称)
返回值:无
函数作者:官方提供的接口
修改点:增加调用解析入参，处理算法的接口
修改人员:陈自立
*************************************************************************/
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    //int link_sum = 0;//总的输出链路数(调试用)
    STLinkInfo *link_info = NULL;
    STConsumerLinkInfo *consumer_link_info = NULL;
    STResultDebugInfo *result_debug_info = NULL;
    STResultInfo *result_info = NULL;
    char * topo_file = NULL;// 需要输出的内容
   
    //解析基础输入信息
    input_analyse_base(topo);

    //依据链路值和消费者节点数量，动态分配几段内存空间，用于存储输入信息:
    link_info = (STLinkInfo *)malloc(num_link * sizeof(STLinkInfo));
    consumer_link_info = (STConsumerLinkInfo *)malloc(num_consumer * sizeof(STConsumerLinkInfo));
    result_debug_info = (STResultDebugInfo *)malloc(num_consumer * sizeof(STResultDebugInfo));
    result_info = (STResultInfo *)malloc(MAX_LINK_NUM * sizeof(STResultInfo));
    topo_file = (char *)malloc(1000*5000*5*sizeof(char));//考虑极端情况，分配25M内存空间用于存放输出结果

    //进一步解析输入信息，获取其他输入信息
    input_analyse_other(link_info, consumer_link_info, topo);

    //算法处理总入口
    process_main(link_info, consumer_link_info, result_info);

    //调试处理函数
    //link_sum = process_debug(consumer_link_info, result_debug_info);
    //调试用输出标准化函数
    //result_standard_debug(link_sum, result_debug_info, topo_file);
    
    //输出标准化函数
    result_standard(result_info, topo_file);
    
    // 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
    write_result(topo_file, filename);

    //释放内存空间
    free(link_info);
    link_info = NULL;
    free(consumer_link_info);
    consumer_link_info = NULL;
    free(result_debug_info);
    result_debug_info = NULL;
    free(result_info);
    result_info = NULL;
    free(topo_file);
    topo_file = NULL;

}

