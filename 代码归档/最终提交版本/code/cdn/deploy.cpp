#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//可调参数
#define MAX_ADD_CLUSTER_SERVER_NUM_PRI 6//(初级用例)单个域内可以增加部署的服务器个数
#define SERVER_NUM_LIMIT_PRI 6//(初级用例)最小类标准，单个域至少可部署的服务器个数
#define MAX_ADD_CLUSTER_SERVER_NUM_MID 7//(中级用例)单个域内可以增加部署的服务器个数
#define SERVER_NUM_LIMIT_MID 7//(中级用例)最小类标准，单个域至少可部署的服务器个数
#define MAX_ADD_CLUSTER_SERVER_NUM_HARD 0//(高级用例)单个域内可以增加部署的服务器个数
#define SERVER_NUM_LIMIT_HARD 5//(高级用例)最小类标准，单个域至少可部署的服务器个数

int num_net_dot;//网络节点数量
int num_link;//网络链路数量
int num_consumer;//消费节点数量
int cost_server;//视频内容服务器部署成本
int num_result_link;//输出网络路径数
int num_consumer_remain;//剩余消费者节点数
int num_server;//已部署服务器数量
int max_add_cluster_server_num;//单个子聚类中最大可部署的服务器叠加量，当聚类中服务器数量超出该域内最大可部署量(至少需要服务器个数与该值的和)时，考虑将该聚类与其兄弟聚类合并为母聚类，该参数对初始大类(cluster_id = 1)无效
int solution_cluster_center = 0;//选择聚类中心的策略 0:通过出度-距离获得聚类中心，1:通过出度最大选聚类中心

/************************************************************************
函数功能:字符串截取函数
入参解释:ch(完整字符串),pos_begin(待截取字符串的起始位置),pos_end(待截取字符串的终止位置)
返回值:返回截取后的字符串
函数作者:陈自立
*************************************************************************/
char *sub_string(char *ch, int pos_begin, int pos_end)
{
    char *pch = ch;
    int length = pos_end - pos_begin;
    int i = 0;

    for(i = 0; i < length; i++)
    {
        pch[i] = ch[(pos_begin + i)];
    }
    pch[i] = '\0';

    return pch;
}

/************************************************************************
函数功能:将int类型转化为字符串类型
入参解释:num(int类型数字),str(待转换的字符串)
返回值:无
函数作者:陈自立
*************************************************************************/
void change_int_to_char(int num, char *str)
{
    sprintf(str, "%d", num);
}

/************************************************************************
函数功能:链表内存释放
功能详解:释放指定链表的内存空间,从头结点开始释放起
入参解释:p_head(链表头节点指针)
返回值:无
函数作者:陈自立
*************************************************************************/
void list_release(STNode **p_head)
{
    STNode *p;
    while(*p_head != NULL)
    {
        p = (*p_head)->pnext;
        free(*p_head);
        *p_head = p;
    }
}

/************************************************************************
函数功能:链表拷贝拼接
功能详解:将长度为length头结点为p_source的链表拷贝一份，得到头结点为p_target的链表
入参解释:p_source(源链表的头结点)p_target(目的链表的头结点)length(链表中的链路数)
返回值:目标链表尾节点的指针
函数作者:陈自立
*************************************************************************/
STNode * list_link(STNode *p_source, STNode *p_target, int length)
{
    STNode *p = p_target;
    STNode *q = p_source;
    int i = 0;
    STNode *p_target_end = NULL;
    for(i = 0; i < length; i++)
    {
        p->id_node = q->id_node;
        p->pnext = (STNode *)malloc(sizeof(STNode));
        p = p->pnext;
        q = q->pnext;
    }
    p->id_node = q->id_node;
    p->pnext = NULL;
    p_target_end = p;
    
    return p_target_end;
}

/************************************************************************
函数功能:解析基本输入文件(基础数据)
功能详解:获取输入文件第一行与第三行的信息，为后面提取链路信息和消费节点信息提供参数
入参解释:topo(官方接口读取输入文件，以字符串数组形式体现)
返回值:无
函数作者:陈自立
*************************************************************************/
void input_analyse_base(char * topo[MAX_EDGE_NUM])
{
    int i = 0;
    int j = 0;
    int begin = 0;
    int end = 0;
    char *str = NULL;

    //解析输入文件第一行信息
    do{
            i++;       
            if((*(topo[0] + i) == ' ') || (*(topo[0] + i) == '\n') || (*(topo[0] + i) == '\r'))
            {
                end = i;
                switch(j)
                {
                    case 0:
                        str = sub_string(topo[0],begin,end);
                        num_net_dot = atoi(str);
                        break;
                    case 1:
                        str = sub_string(topo[0],begin,end);
                        num_link = atoi(str);
                        break;
                    case 2:
                        str = sub_string(topo[0],begin,end);
                        num_consumer = atoi(str);
                        break;
                    default:
                        break;
                }
                j++;
                begin = i + 1; 
            }            
    }while((*(topo[0] + i) != '\n') && (*(topo[0] + i) != '\r'));

    //解析输入文件第三行信息
    str = topo[2];
    i = 0;
    do{
        str[i] = *(topo[2] + i);
        i++;
        }while((*(topo[2] + i) != '\n') && (*(topo[2] + i) != '\r'));
    str[i] = '\0';
    cost_server = atoi(str);    

    num_result_link = 0;
    num_consumer_remain = num_consumer;
    num_server = 0;
    
}

/************************************************************************
函数功能:解析其他输入信息(链路信息与消费节点链接信息)
功能详解:读取并解析输入文件中第三行开始的链路信息与消费节点链接信息，存入到对应的内存空间中
入参解释:link_info(链路信息),consumer_link_info(消费节点链接信息),topo(官方接口读取输入文件，以字符串数组形式体现)
返回值:无
函数作者:陈自立
*************************************************************************/
void input_analyse_other(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info, char * topo[MAX_EDGE_NUM])
{
    int i = 0;
    int j = 0;
    int k = 0;
    int m = 0;
    int begin = 0;
    int end = 0;
    char *str = NULL;

    //解析网路链路信息
    for(i = 0; i < num_link; i++)
    {
        k = i + 4;//从第5行开始
        j = 0;
        m = 0;
        begin = 0;
        do{
                j++;
                if((*(topo[k] + j) == ' ') || (*(topo[k] + j) == '\n') || (*(topo[k] + j) == '\r'))
                {
                    end = j;
                    switch(m)
                    {
                        case 0:
                            str = sub_string(topo[k],begin,end);
                            link_info[i].id_link_begin = atoi(str);
                            break;
                        case 1:
                            str = sub_string(topo[k],begin,end);
                            link_info[i].id_link_end = atoi(str);
                            break;
                        case 2:
                            str = sub_string(topo[k],begin,end);
                            link_info[i].link_bandwidth = atoi(str);
                            break;
                        case 3:
                            str = sub_string(topo[k],begin,end);
                            link_info[i].cost_per_bandwidth = atoi(str);
                            break;
                        default:
                            break;
                    }
                    m++;
                    begin = j + 1;
                }
            }while((*(topo[k] + j) != '\n') && (*(topo[k] + j) != '\r'));
    }

    //解析消费者节点信息
    for(i = 0; i < num_consumer; i++)
    {
        consumer_link_info[i].cluster_id = 1;
        k = i + 5 + num_link;//从第6 + num_link行开始
        j = 0;
        m = 0;
        begin = 0;
        do{
                j++;
                if((*(topo[k] + j) == ' ') || (*(topo[k] + j) == '\n') || (*(topo[k] + j) == '\r'))
                {
                    end = j;
                    switch(m)
                    {
                        case 0:
                            str = sub_string(topo[k],begin,end);
                            consumer_link_info[i].id_consumer = atoi(str);
                            break;
                        case 1:
                            str = sub_string(topo[k],begin,end);
                            consumer_link_info[i].id_net_dot = atoi(str);
                            break;
                        case 2:
                            str = sub_string(topo[k],begin,end);
                            consumer_link_info[i].need_bandwidth = atoi(str);
                            consumer_link_info[i].need_bandwidth_history =  consumer_link_info[i].need_bandwidth;
                            break;
                        default:
                            break;
                    }
                    m++;
                    begin = j + 1;
                }
            }while((*(topo[k] + j) != '\n') && (*(topo[k] + j) != '\r'));
    }
}

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
函数功能:输出信息标准化函数(调试用)
功能详解:将int类型的输出信息转化为官方要求的字符串类型
入参解释:link_sum(总的网络路径数量),result_debug_info(输出网络路径信息),topo_file(官方需要的输出字符串)
返回值:无
函数作者:陈自立
*************************************************************************/
void result_standard_debug(int link_sum, STResultDebugInfo *result_debug_info, char *topo_file)
{
    int i = 0;
    char str[7] = {0};//题目规定单个数值最大为100000，加上空字符最多7个字符
    change_int_to_char(link_sum, str);
    strcpy(topo_file,str);
    strcat(topo_file,"\n\n");
    for(i = 0; i < link_sum; i++)
    {
        change_int_to_char(result_debug_info[i].id_server, str);
        strcat(topo_file,str);
        strcat(topo_file," ");
        change_int_to_char(result_debug_info[i].id_consumer, str);
        strcat(topo_file,str);
        strcat(topo_file," ");
        change_int_to_char(result_debug_info[i].cost_bandwidth, str);
        strcat(topo_file,str);
        strcat(topo_file,"\n");
    }
}

/************************************************************************
函数功能:输出信息标准化函数
功能详解:将int类型的输出信息转化为官方要求的字符串类型
入参解释:result_info(输出结果链路信息),topo_file(官方需要的输出字符串)
返回值:无
函数作者:陈自立
*************************************************************************/
void result_standard(STResultInfo *result_info, char *topo_file)
{
    int i = 0;
    int j = 0;
    char str[7] = {0};//题目规定单个数值最大为100000，加上空字符最多7个字符
    STNode *p = NULL;
    change_int_to_char(num_result_link, str);//网络路径数
    strcpy(topo_file,str);
    strcat(topo_file,"\n\n");
    for(i = 0; i < num_result_link; i++)//输出各条链路信息
    {
        change_int_to_char(result_info[i].p_path->id_node, str);//输出单条链路头结点
        strcat(topo_file,str);
        strcat(topo_file," ");
        p = result_info[i].p_path;
        for(j = 0; j < result_info[i].link_num; j++)//输出单条链路中间节点
        {
            p = p->pnext;
            if(p == NULL)
            {
                printf("[ERROR] <result_standard> p == NULL\n");
                break;
            }
            change_int_to_char(p->id_node, str);
            strcat(topo_file,str);
            strcat(topo_file," ");
        }
        change_int_to_char(result_info[i].id_consumer, str);//输出单条链路消费者节点
        strcat(topo_file,str);
        strcat(topo_file," ");
        change_int_to_char(result_info[i].cost_bandwidth, str);//输出单条链路带宽
        strcat(topo_file,str);
        strcat(topo_file,"\n");
    }
}

/************************************************************************
函数功能:dijkstra调试函数:打印全图所有消费者节点两两之间的最短路径
功能详解:先全图遍历，获取所有消费者节点到其余节点的最短路径，再打印全图所有
消费者节点两两之间的最短路径
入参解释:cluster_id(聚类编号)p2p_path_info(点到点的最短路径信息)consumer_link_info(消费节点链接信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void dijkstra_debug_consumer_all(int cluster_id, STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info)
{
    int i = 0;
    int j = 0;
    int m = 0;
    int id_source = 0;
    int id_end = 0;
    STNode *p = NULL;
    
    //打印出所有消费者节点两两之间的最短路径:
    for(i = 0; i < num_consumer; i++)
    {
        for(j = 0; j < num_consumer; j++)
        {
            if((i != j) && (consumer_link_info[i].cluster_id == cluster_id) && (consumer_link_info[j].cluster_id == cluster_id))
            {
                id_source = consumer_link_info[i].id_net_dot;
                id_end = consumer_link_info[j].id_net_dot;
                printf("cluster%d consumer %d to %d : distance = %d bandwidth = %d \n",cluster_id, consumer_link_info[i].id_consumer, consumer_link_info[j].id_consumer, p2p_path_info[id_source][id_end].distance,p2p_path_info[id_source][id_end].bandwidth);
                p = p2p_path_info[id_source][id_end].p_path;
                printf("[%d] ",consumer_link_info[i].id_consumer);
                printf("%d ",p->id_node);
                for(m = 0; m < p2p_path_info[id_source][id_end].link_num; m++)
                {
                    p = p->pnext;
                    printf("%d ",p->id_node);
                }
                printf("[%d] ",consumer_link_info[j].id_consumer);
                printf("\n");
            }
        }
    }
}

/************************************************************************
函数功能:dijkstra调试函数:打印出指定的两个网络节点之间的最短路径
功能详解:打印出两个网络节点之间的路径信息
入参解释:cluster_id(聚类编号)net_node_info(域内网络节点信息)p2p_path_info(点到点的最短路径信息)
id_source(源节点)id_end(目的节点)
返回值:无
函数作者:陈自立
*************************************************************************/
void dijkstra_debug_net_p2p(int cluster_id, STNetNodeInfo *net_node_info, STP2PPathInfo **p2p_path_info)
{
    int m = 0;
    int i = 0;
    int j = 0;
    int id_source = 0;
    int id_end = 0;
    STNode *p = NULL;

    for(i = 0; i < num_net_dot; i++)
    {
        for(j = 0; j < num_net_dot; j++)
        {
            if((net_node_info[i].cluster_id == cluster_id) && (net_node_info[j].cluster_id == cluster_id))
            {
                id_source = i;
                id_end = j;
                printf("The cluster%d distance route: %d to %d\n",cluster_id, id_source, id_end);
                printf("distance:%d bandwidth:%d linknum:%d\n",p2p_path_info[id_source][id_end].distance, p2p_path_info[id_source][id_end].bandwidth, p2p_path_info[id_source][id_end].link_num);
                p = p2p_path_info[id_source][id_end].p_path;
                printf("%d ",p->id_node);
                for(m = 0; m < p2p_path_info[id_source][id_end].link_num; m++)
                {
                    p = p->pnext;
                    printf("%d ",p->id_node);
                }
                printf("\n");
            }
        }
    }
}

/************************************************************************
函数功能:dijkstra调试函数:打印出指定的两个消费者节点之间的最短路径
功能详解:打印出指定的两个消费者节点之间的最远距离
入参解释:cluster_id(聚类编号)p2p_path_info(点到点的最短路径信息)consumer_link_info(消费节点链接信息)
id_consumer_source(最远距离路径的源消费节点)id_consumer_end(最远距离路径的目的消费节点)
返回值:无
函数作者:陈自立
*************************************************************************/
void dijkstra_debug_consumer_p2p(int cluster_id, STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, int id_consumer_source, int id_consumer_end)
{
    int m = 0;
    int id_source = consumer_link_info[id_consumer_source].id_net_dot;
    int id_end = consumer_link_info[id_consumer_end].id_net_dot;
    STNode *p = NULL;
    
    printf("The cluster%d maximum distance route is:consumer %d to %d\n",cluster_id, id_consumer_source, id_consumer_end);
    printf("distance:%d bandwidth:%d linknum:%d\n",p2p_path_info[id_source][id_end].distance, p2p_path_info[id_source][id_end].bandwidth, p2p_path_info[id_source][id_end].link_num);
    p = p2p_path_info[id_source][id_end].p_path;
    printf("[%d] ",id_consumer_source);
    printf("%d ",p->id_node);
    for(m = 0; m < p2p_path_info[id_source][id_end].link_num; m++)
    {
        p = p->pnext;
        printf("%d ",p->id_node);
    }
    printf("[%d] ",id_consumer_end);
    printf("\n");
}

/************************************************************************
函数功能:计算用函数:打印出域内所有网络节点出方向的流量
功能详解:打印出域内所有网络节点出方向的流量
入参解释:cluster_id(聚类编号)net_node_info(域内网络节点信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void calculate_debug_node_outflow(int cluster_id, STNetNodeInfo *net_node_info)
{
    int i = 0;
    for(i = 0; i < num_net_dot; i++)
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            printf("ID:%d OutFlow:%d\n", i, net_node_info[i].out_flow);
        }
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
函数功能:dijkstra处理函数:指定源点到其他所有点的最短距离
功能详解:找出指定点到其他所有点的最短距离，将最短距离和该路径所经过的点存入指定数组和链表中
入参解释:cluster_id(聚类编号)id_source(源节点id)p2p_path_info(点到点的最短路径信息)net_node_info(域内网络节点信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void dijkstra_process_p2mp(int cluster_id, int id_source, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int j = 0;
    int u = 0;
    int v = 0;
    int min = INF;//存放最短距离
    int value = 0;//非孤立节点标志位(0为孤立节点)
    char *book = NULL;//标记当前点到制定源点的距离是否最小
    int *dis = NULL;//存放源点到其他点的最短距离
    STNode *p = NULL;
    STNode *q = NULL;
    dis = (int *)malloc(num_net_dot * sizeof(int));
    book = (char *)malloc(num_net_dot);

    for(i = 0; i < num_net_dot; i++)//初始化dis数组
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            dis[i] = p2p_path_info[id_source][i].distance;
        }
    }
    
    for (i = 0; i < num_net_dot; i++)//初始化book数组
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            book[i] = 0;
        }
        else
        {
            book[i] = 1;//对于非本类的点，直接放入集合P
        }
    }
    book[id_source] = 1;//将源节点自身先放入集合P，避免成环

    for(i = 0; i < (num_net_dot - 1); i++)//一共循环(num_net_dot - 1)次可以找出结果
    {
        //找到离源点最近的网络节点
        min = INF;
        for(j = 0; j < num_net_dot; j++)
        {
            if((book[j] == 0) && (dis[j] < min) && (dis[j] > 0))
            {
                min = dis[j];
                u = j;
                value = 1;
            }
        }
        if(value == 1)
        {
            book[u] = 1;//将该节点存入集合P
            value = 0;
        }
        else
        {
            break;
        }
        
        for(v = 0; v < num_net_dot; v++)
        {
            if(net_node_info[v].cluster_id == cluster_id)//仅对域内的节点进行操作
            {
                if((p2p_path_info[u][v].distance < INF) && (p2p_path_info[u][v].bandwidth > 0) && (v != id_source))
                {
                    if(dis[v] > dis[u] + p2p_path_info[u][v].distance)
                    {
                        if((dis[u] + p2p_path_info[u][v].distance) < 0)
                        {
                            printf("[ERROR] <dijkstra_process_p2mp> p2p_path_info[%d][%d].distance = %d dis[u] = %d\n",u,v,p2p_path_info[u][v].distance,dis[u]);
                            continue;
                        }
                        dis[v] = dis[u] + p2p_path_info[u][v].distance;//获取到点v的新的最小路径权重
                        if(dis[v] > INF)
                        {
                            dis[v] = INF;
                        }
                        
                        //拼接路径，记录新路径的路径信息
                        //第一段拼接
                        p = p2p_path_info[id_source][v].p_path;//原先的路径
                        q = p2p_path_info[id_source][u].p_path;//待拼接的新路径
                        list_release(&(p->pnext));//释放原路径，保留头结点
                        p2p_path_info[id_source][v].p_path_end = list_link(q, p, p2p_path_info[id_source][u].link_num);
                        //第二段拼接
                        p = p2p_path_info[id_source][v].p_path_end;//第一段拼接后路径的尾节点
                        q = p2p_path_info[u][v].p_path;//第二段待拼接路径
                        p2p_path_info[id_source][v].p_path_end= list_link(q, p, p2p_path_info[u][v].link_num);
                        //其余路径信息更新     
                        p2p_path_info[id_source][v].distance = dis[v];//点到点路径权重之和更新
                        p2p_path_info[id_source][v].link_num = p2p_path_info[id_source][u].link_num + p2p_path_info[u][v].link_num;
                        if(p2p_path_info[id_source][u].bandwidth < p2p_path_info[u][v].bandwidth)//两个路径中的最小值作为新合成路径的带宽
                        {
                            p2p_path_info[id_source][v].bandwidth = p2p_path_info[id_source][u].bandwidth;
                        }
                        else
                        {
                            p2p_path_info[id_source][v].bandwidth = p2p_path_info[u][v].bandwidth;
                        }
                    }
                }
            }
        }
    }

    free(dis);
    dis = NULL;
    free(book);
    book = NULL;
    
}


/************************************************************************
函数功能:dijkstra处理函数:全图遍历
功能详解:全图遍历，计算出全图所有网络节点之间的最短路径
入参解释:cluster_id(聚类编号)p2p_path_info(点到点的最短路径信息)net_node_info(域内网络节点信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void dijkstra_process_all_distance(int cluster_id, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    //计算出所有网络节点两两之间的最短路径
    for(i = 0; i < num_net_dot; i++)
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            dijkstra_process_p2mp(cluster_id, i, p2p_path_info, net_node_info);
        }
    }
}


/************************************************************************
函数功能:dijkstra处理函数:寻找距离最远的消费者结点
功能详解:找出域内消费者节点之间的最远路径，距离为第一优先级，距离一样看带宽，带宽越小认为相距越远
入参解释:cluster_id(聚类编号)p2p_path_info(点到点的最短路径信息)consumer_link_info(消费节点链接信息)
id_cluster_A(最远距离路径的源网络节点)id_cluster_B(最远距离路径的目的网络节点)
返回值:无
函数作者:陈自立
*************************************************************************/
void dijkstra_process_consumer_max(int cluster_id, STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, int *id_cluster_A, int *id_cluster_B)
{
    int i = 0;
    int j = 0;
    int distance_max = 0;//记录路径distance最大值
    int bandwidth_min = 0;//记录同等路径下bandwidth最小值
    //int id_consumer_source = 0;
    //int id_consumer_end = 0;
    int id_source = 0;
    int id_end = 0;
    
    //找出全图距离最远的两个消费者结点，当距离一样远时认为链路带宽越小两点距离越远
    distance_max = 0;
    bandwidth_min = INF;
    for(i = 0;i < num_consumer; i++)
    {
        for(j = 0; j < num_consumer; j++)
        {
            //仅对域内消费者节点进行计算
            if((i != j) && (consumer_link_info[i].cluster_id == cluster_id) && (consumer_link_info[j].cluster_id == cluster_id))
            {
                id_source = consumer_link_info[i].id_net_dot;
                id_end = consumer_link_info[j].id_net_dot;
                if(p2p_path_info[id_source][id_end].distance > distance_max)//取距离最远的路径
                {
                    bandwidth_min = p2p_path_info[id_source][id_end].bandwidth;
                    distance_max = p2p_path_info[id_source][id_end].distance;
                    *id_cluster_A = id_source;
                    *id_cluster_B = id_end;
                    //id_consumer_source = i;
                    //id_consumer_end = j;
                }
                if(p2p_path_info[id_source][id_end].distance == distance_max)//距离相等时，取带宽较小的路径
                {
                    if(p2p_path_info[id_source][id_end].bandwidth < bandwidth_min)
                    {
                        distance_max = p2p_path_info[id_source][id_end].distance;
                        *id_cluster_A = id_source;
                        *id_cluster_B = id_end;
                        //id_consumer_source = i;
                        //id_consumer_end = j;
                    }
                }
            }
        }
    }
    //打印出域内消费者节点间的最远路径
    //dijkstra_debug_consumer_p2p(cluster_id, p2p_path_info, consumer_link_info, id_consumer_source, id_consumer_end);
}

/************************************************************************
函数功能:dijkstra处理函数总入口
功能详解:预处理dijkstra算法，调用其他dijkstra处理函数
入参解释:cluster_id(聚类编号)p2p_path_info(点到点的最短路径信息)net_node_info(域内网络节点信息)
consumer_link_info(消费节点链接信息)
返回值:无
函数作者:陈自立
*************************************************************************/

void dijkstra_process_main(int cluster_id, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info)
{ 
    printf("[INFO] <dijkstra_process_main> cluster_id = %d\n",cluster_id);
    
    //域内全图遍历
    dijkstra_process_all_distance(cluster_id, p2p_path_info, net_node_info);
    //打印出所有消费者节点两两间的距离
    //dijkstra_debug_consumer_all(cluster_id, p2p_path_info, consumer_link_info);
}

/************************************************************************
函数功能:计算函数:算出域内所有消费者节点总需求
功能详解:算出域内所有消费者节点总需求
入参解释:cluster_id(聚类编号)consumer_link_info(消费节点链接信息)
返回值:域内所有消费者结点总需求
函数作者:陈自立
*************************************************************************/
int calculate_consumer_need(int cluster_id, STConsumerLinkInfo *consumer_link_info)
{
    int sum = 0;
    int i = 0;

    for(i = 0; i < num_consumer; i++)
    {
        if(consumer_link_info[i].cluster_id == cluster_id)
        {
        sum = sum + consumer_link_info[i].need_bandwidth;
        }
    }

    return sum;
}

/************************************************************************
函数功能:计算函数:算出域内各网络节点的流量出度
功能详解:算出域内各网络节点的流量出度,
入参解释:cluster_id(聚类编号)link_array(有向链路数组)consumer_link_info(域内消费节点链接信息)
net_node_info(域内网络节点信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void calculate_node_outflow(int cluster_id, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int j = 0;
    int id = 0;
    //先初始化一下:
    for(i = 0; i < num_net_dot; i++)
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            net_node_info[i].out_flow = 0;
        }
    }
    //首先找出消费节点相邻的网络节点，加上消费节点所需的带宽
    for(i = 0; i < num_consumer; i++)
    {
        if(consumer_link_info[i].cluster_id == cluster_id)
        {
            id = consumer_link_info[i].id_net_dot;
            for(j = 0; j < num_net_dot; j++)
            {
                if(id == j)
                {
                    net_node_info[j].out_flow = net_node_info[j].out_flow + consumer_link_info[i].need_bandwidth;
                }
            }
        }
    }
    for(i = 0; i < num_net_dot; i++)
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            for(j = 0; j < num_net_dot; j++)
            {
                if(net_node_info[j].cluster_id == cluster_id)
                {
                    if((link_array[i][j].link_bandwidth < INF) && (link_array[i][j].link_bandwidth > 0))
                    {
                        net_node_info[i].out_flow = net_node_info[i].out_flow + link_array[i][j].link_bandwidth;
                    }
                }
            }
        }
    }
}

/************************************************************************
函数功能:计算函数:算出域内需部署视频服务器的最小值
功能详解:算出域内需部署视频服务器的最小值
入参解释:cluster_id(聚类编号);link_array(有向链路数组)consumer_link_info(域内消费节点链接信息)
net_node_info(域内网络节点信息)
返回值:返回域内所需最少服务器台数
函数作者:陈自立
*************************************************************************/
int calculate_server_minimum(int cluster_id, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int max = 0;
    int id_used = 0;
    int num_server = 0;
    int sum_server_flow = 0;
    int sum_consumer_need = 0;
    sum_consumer_need = calculate_consumer_need(cluster_id, consumer_link_info);
    calculate_node_outflow(cluster_id, link_array, consumer_link_info, net_node_info);
    //将域内服务器按供应流量能力大小，将流量由大到小累加，找到能满足域内消费者结点的最小部署数量
    while(sum_server_flow < sum_consumer_need)
    {
        for(i = 0; i < num_net_dot; i++)
        {
            if(net_node_info[i].cluster_id == cluster_id)
            {
                if(net_node_info[i].valid == 1)
                {
                    if(net_node_info[i].out_flow > max)
                    {
                        max = net_node_info[i].out_flow;
                        id_used = i;
                    }
                }
            }
        }
        num_server++;
        sum_server_flow = sum_server_flow + max;
        max = 0;
        net_node_info[id_used].valid = 0;
    }
    //恢复网络节点可用标志位
    for(i = 0; i < num_net_dot; i++)
    {
        net_node_info[i].valid = 1;
    }
    //打印出域内需要的流量大小和至少需要的服务器数量
    printf("In cluster%d,sum_consumer_need = %d num_server_min = %d\n",cluster_id, sum_consumer_need, num_server);
    return num_server;
}

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
    dijkstra_process_main(cluster_id, p2p_path_info, net_node_info, consumer_link_info);
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
    dijkstra_process_main(cluster_id, p2p_path_info, net_node_info, consumer_link_info);

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
        flag = 63;
    }
    //调用dijkstra处理函数，域内遍历最短路径
    dijkstra_process_main(cluster_id, p2p_path_info, net_node_info, consumer_link_info);
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
            dijkstra_process_main(cluster_id, p2p_path_info, net_node_info, consumer_link_info);
            
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
            dijkstra_process_main(cluster_id, p2p_path_info, net_node_info, consumer_link_info);
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
            dijkstra_process_main(cluster_id_origin, p2p_path_info, net_node_info, consumer_link_info);
        }
    }
    printf("[INFO]<cluster_flow_out_main> cluster%d: server num : %d\n",cluster_id,cluster_info[cluster_id].num_server_current);
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
