#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int num_net_dot;//网络节点数量
int num_link;//网络链路数量
int num_consumer;//消费节点数量
int cost_server;//视频内容服务器部署成本

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
void list_release(STNode *p_head)
{
    STNode *p;
    while(p_head != NULL)
    {
        p = p_head->pnext;
        free(p_head);
        p_head = p;
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
入参解释:consumer_link_info(消费节点链接信息),result_info(输出网络路径信息)
返回值:网络路径数
函数作者:陈自立
*************************************************************************/
int process_debug(STConsumerLinkInfo *consumer_link_info, STResultInfo *result_info)
{
    int i = 0;
    for(i = 0; i < num_consumer; i++)
    {
        result_info[i].id_server = consumer_link_info[i].id_net_dot;
        result_info[i].id_consumer = consumer_link_info[i].id_consumer;
        result_info[i].cost_bandwidth = consumer_link_info[i].need_bandwidth;
    }
    return i;
}

/************************************************************************
函数功能:输出信息标准化函数(调试用)
功能详解:将int类型的输出信息转化为官方要求的字符串类型
入参解释:link_sum(总的网络路径数量),result_info(输出网络路径信息),topo_file(官方需要的输出字符串)
返回值:无
函数作者:陈自立
*************************************************************************/
void result_standard_debug(int link_sum, STResultInfo *result_info, char *topo_file)
{
    int i = 0;
    char str[5] = {0};//题目规定单个数值最大为5000，加上空字符最多5个字符
    change_int_to_char(link_sum, str);
    strcpy(topo_file,str);
    strcat(topo_file,"\n\n");
    for(i = 0; i < link_sum; i++)
    {
        change_int_to_char(result_info[i].id_server, str);
        strcat(topo_file,str);
        strcat(topo_file," ");
        change_int_to_char(result_info[i].id_consumer, str);
        strcat(topo_file,str);
        strcat(topo_file," ");
        change_int_to_char(result_info[i].cost_bandwidth, str);
        strcat(topo_file,str);
        strcat(topo_file,"\n");
    }
}

/************************************************************************
函数功能:dijkstra调试函数:打印全图所有消费者节点两两之间的最短路径
功能详解:先全图遍历，获取所有消费者节点到其余节点的最短路径，再打印全图所有
消费者节点两两之间的最短路径
入参解释:p2p_path_info(点到点的最短路径信息)consumer_link_info(消费节点链接信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void dijkstra_debug_consumer_all(STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info)
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
            if(i != j)
            {
                id_source = consumer_link_info[i].id_net_dot;
                id_end = consumer_link_info[j].id_net_dot;
                printf("consumer %d to %d : distance = %d bandwidth = %d \n",consumer_link_info[i].id_consumer, consumer_link_info[j].id_consumer, p2p_path_info[id_source][id_end].distance,p2p_path_info[id_source][id_end].bandwidth);
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
函数功能:dijkstra调试函数:打印出指定的两个消费者节点之间的最短路径
功能详解:打印出指定的两个消费者节点之间的最远距离
入参解释:p2p_path_info(点到点的最短路径信息)consumer_link_info(消费节点链接信息)
id_consumer_source(最远距离路径的源消费节点)id_consumer_end(最远距离路径的目的消费节点)
返回值:无
函数作者:陈自立
*************************************************************************/
void dijkstra_debug_consumer_p2p(STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, int *id_consumer_source, int *id_consumer_end)
{
    int m = 0;
    int id_source = consumer_link_info[*id_consumer_source].id_net_dot;
    int id_end = consumer_link_info[*id_consumer_end].id_net_dot;
    STNode *p = NULL;
    
    printf("The maximum distance route is:consumer %d to %d\n",*id_consumer_source, *id_consumer_end);
    printf("distance:%d bandwidth:%d linknum:%d\n",p2p_path_info[id_source][id_end].distance, p2p_path_info[id_source][id_end].bandwidth, p2p_path_info[id_source][id_end].link_num);
    p = p2p_path_info[id_source][id_end].p_path;
    printf("[%d] ",*id_consumer_source);
    printf("%d ",p->id_node);
    for(m = 0; m < p2p_path_info[id_source][id_end].link_num; m++)
    {
        p = p->pnext;
        printf("%d ",p->id_node);
    }
    printf("[%d] ",*id_consumer_end);
    printf("\n");
}

/************************************************************************
函数功能:计算用函数:打印出域内所有网络节点出方向的流量
功能详解:打印出域内所有网络节点出方向的流量
入参解释:net_node_info(域内网络节点信息)num_node_area(域内网络节点个数)
返回值:无
函数作者:陈自立
*************************************************************************/
void calculate_debug_node_outflow(STNetNodeInfo *net_node_info, int num_node_area)
{
    int i = 0;
    for(i = 0; i < num_node_area; i++)
    {
        printf("ID:%d OutFlow:%d\n",net_node_info[i].id_node, net_node_info[i].out_flow);
    }
}

/************************************************************************
函数功能:输入信息预处理模块
功能详解:预处理输入信息,将link_info中的数据存入link_array中,初始化点到点的最短路径信息，
将网络节点信息初始化
入参解释:link_info(链路信息)link_array(有向链路数组)p2p_path_info(存放点到点的最短路径信息)
net_node_info(网络节点信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void process_pre(STLinkInfo *link_info, STLink **link_array, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int m = 0;
    int n = 0;

    //初始化
    for(m = 0; m < num_net_dot; m++)
    {
        for(n = 0; n < num_net_dot; n++)
        {
            link_array[m][n].link_bandwidth = INF;//初始化有向链路数组
            link_array[m][n].cost_per_bandwidth = INF;
            p2p_path_info[m][n].distance = INF;//初始化点到点的最短路径信息
            p2p_path_info[m][n].link_num = 0;
            p2p_path_info[m][n].bandwidth = INF;
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
        link_array[m][n].cost_per_bandwidth = link_info[i].cost_per_bandwidth;
        link_array[n][m].link_bandwidth = link_info[i].link_bandwidth;
        link_array[n][m].cost_per_bandwidth = link_info[i].cost_per_bandwidth;
    }

    //将相邻节点的点到点路径信息存入p2p_path_info
    for(m = 0; m < num_net_dot; m++)
    {
        net_node_info[m].id_node= m;
        net_node_info[m].out_flow = 0;
        net_node_info[m].valid= 1;
        for(n = 0; n < num_net_dot; n++)
        {
            if(link_array[m][n].link_bandwidth < INF)
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
}

/************************************************************************
函数功能:dijkstra处理函数:指定源点到其他所有点的最短距离
功能详解:找出指定点到其他所有点的最短距离，将最短距离和该路径所经过的点存入指定数组和链表中
入参解释:id_source(源节点id)p2p_path_info(点到点的最短路径信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void dijkstra_process_p2mp(int id_source, STP2PPathInfo **p2p_path_info)
{
    int i = 0;
    int j = 0;
    int u = 0;
    int v = 0;
    int min = INF;//存放最短距离
    char *book = NULL;//标记当前点到制定源点的距离是否最小
    int *dis = NULL;//存放源点到其他点的最短距离
    STNode *p = NULL;
    STNode *q = NULL;
    dis = (int *)malloc(num_net_dot * sizeof(int));
    book = (char *)malloc(num_net_dot);

    for(i = 0; i < num_net_dot; i++)//初始化dis数组
    {
        dis[i] = p2p_path_info[id_source][i].distance;
    }
    
    for (i = 0; i < num_net_dot; i++)//初始化book数组
    {
        book[i] = 0;
    }
    book[id_source] = 1;//将源节点自身先放入集合P，避免成环

    for(i = 0; i < (num_net_dot - 1); i++)//一共循环(num_net_dot - 1)次可以找出结果
    {
        //找到离源点最近的网络节点
        min = INF;
        for(j = 0; j < num_net_dot; j++)
        {
            if((book[j] == 0) && (dis[j] < min))
            {
                min = dis[j];
                u = j;
            }
        }
        book[u] = 1;//将该节点存入集合P
        
        for(v = 0; v < num_net_dot; v++)
        {
            if(p2p_path_info[u][v].distance < INF)
            {
                if(dis[v] > dis[u] + p2p_path_info[u][v].distance)
                {
                    dis[v] = dis[u] + p2p_path_info[u][v].distance;//获取到点v的新的最小路径权重
                    
                    //拼接路径，记录新路径的路径信息
                    //第一段拼接
                    p = p2p_path_info[id_source][v].p_path;//原先的路径
                    q = p2p_path_info[id_source][u].p_path;//待拼接的新路径
                    list_release(p->pnext);//释放原路径，保留头结点
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

    free(dis);
    free(book);
    
}


/************************************************************************
函数功能:dijkstra处理函数:全图遍历
功能详解:全图遍历，计算出全图所有网络节点之间的最短路径
入参解释:p2p_path_info(点到点的最短路径信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void dijkstra_process_all_distance(STP2PPathInfo **p2p_path_info)
{
    int i = 0;
    //计算出所有网络节点两两之间的最短路径
    for(i = 0; i < num_net_dot; i++)
    {
        dijkstra_process_p2mp(i, p2p_path_info);
    }
}


/************************************************************************
函数功能:dijkstra处理函数:寻找距离最远的消费者结点
功能详解:找出全图消费者节点之间的最远路径，距离为第一优先级，距离一样看带宽，带宽越小认为相距越远
入参解释:p2p_path_info(点到点的最短路径信息)consumer_link_info(消费节点链接信息)
id_consumer_source(最远距离路径的源消费节点)id_consumer_end(最远距离路径的目的消费节点)
返回值:无
函数作者:陈自立
*************************************************************************/
void dijkstra_process_consumer_max(STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, int *id_consumer_source, int *id_consumer_end)
{
    int i = 0;
    int j = 0;
    int distance_max = 0;//记录路径distance最大值
    int bandwidth_min = 0;//记录同等路径下bandwidth最小值
    int id_source = consumer_link_info[0].id_net_dot;
    int id_end = consumer_link_info[1].id_net_dot;
    
    //找出全图距离最远的两个消费者结点，当距离一样远时认为链路带宽越小两点距离越远
    distance_max = p2p_path_info[id_source][id_end].distance;
    bandwidth_min = p2p_path_info[id_source][id_end].bandwidth;
    for(i = 0;i < num_consumer; i++)
    {
        for(j = 0; j < num_consumer; j++)
        {
            if(i != j)
            {
                id_source = consumer_link_info[i].id_net_dot;
                id_end = consumer_link_info[j].id_net_dot;
                if(p2p_path_info[id_source][id_end].distance > distance_max)//取距离最远的路径
                {
                    bandwidth_min = p2p_path_info[id_source][id_end].bandwidth;
                    distance_max = p2p_path_info[id_source][id_end].distance;
                    *id_consumer_source = i;
                    *id_consumer_end = j;
                }
                if(p2p_path_info[id_source][id_end].distance == distance_max)//距离相等时，取带宽较小的路径
                {
                    if(p2p_path_info[id_source][id_end].bandwidth < bandwidth_min)
                    {
                        distance_max = p2p_path_info[id_source][id_end].distance;
                        *id_consumer_source = i;
                        *id_consumer_end = j;
                    }
                }
            }
        }
    }
    //打印出消费者节点间的最远路径
    dijkstra_debug_consumer_p2p(p2p_path_info, consumer_link_info, id_consumer_source, id_consumer_end);
}

/************************************************************************
函数功能:dijkstra处理函数总入口
功能详解:预处理dijkstra算法，调用其他dijkstra处理函数
入参解释:p2p_path_info(点到点的最短路径信息)link_array(有向链路数组)consumer_link_info(消费节点链接信息)
返回值:无
函数作者:陈自立
*************************************************************************/

void dijkstra_process_main(STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info)
{ 
    int id_consumer_source = 0;
    int id_consumer_end = 0;
    //全图遍历
    dijkstra_process_all_distance(p2p_path_info);
    //打印出所有消费者节点两两间的距离
    dijkstra_debug_consumer_all(p2p_path_info, consumer_link_info);
    //找出消费者结点之间的最远距离
    dijkstra_process_consumer_max(p2p_path_info, consumer_link_info, &id_consumer_source, &id_consumer_end);
    
}

/************************************************************************
函数功能:计算函数:算出域内所有消费者节点总需求
功能详解:算出域内所有消费者节点总需求
入参解释:consumer_link_info(消费节点链接信息)num_consumer_area(域内消费者节点个数)
返回值:域内所有消费者结点总需求
函数作者:陈自立
*************************************************************************/
int calculate_consumer_need(STConsumerLinkInfo *consumer_link_info, int num_consumer_area)
{
    int sum = 0;
    int i = 0;

    for(i = 0; i < num_consumer_area; i++)
    {
        sum = sum + consumer_link_info[i].need_bandwidth;
    }

    return sum;
}

/************************************************************************
函数功能:计算函数:算出域内各网络节点的流量出度
功能详解:算出域内各网络节点的流量出度,
入参解释:link_array(有向链路数组)consumer_link_info(域内消费节点链接信息)num_consumer_area(域内消费者节点个数)
net_node_info(域内网络节点信息)num_node_area(域内网络节点个数)
返回值:无
函数作者:陈自立
*************************************************************************/
void calculate_node_outflow(STLink **link_array, STConsumerLinkInfo *consumer_link_info, int num_consumer_area, STNetNodeInfo *net_node_info, int num_node_area)
{
    int i = 0;
    int j = 0;
    int m = 0;
    int n = 0;
    int id = 0;
    //首先找出消费节点相邻的网络节点，加上消费节点所需的带宽
    for(i = 0; i < num_consumer_area; i++)
    {
        id = consumer_link_info[i].id_net_dot;
        for(j = 0; j < num_node_area; j++)
        {
            if(id == net_node_info[j].id_node)
            {
                net_node_info[j].out_flow = net_node_info[j].out_flow + consumer_link_info[i].need_bandwidth;
            }
        }
    }
    for(i = 0; i < num_node_area; i++)
    {
        m = net_node_info[i].id_node;
        for(j = 0; j < num_node_area; j++)
        {
            n = net_node_info[j].id_node;
            if(link_array[m][n].link_bandwidth < INF)
            {
                net_node_info[i].out_flow = net_node_info[i].out_flow + link_array[m][n].link_bandwidth;
            }
        }
    }
}

/************************************************************************
函数功能:计算函数:算出域内需部署视频服务器的最小值
功能详解:算出域内需部署视频服务器的最小值
入参解释:link_array(有向链路数组)consumer_link_info(域内消费节点链接信息)num_consumer_area(域内消费者节点个数)
net_node_info(域内网络节点信息)num_node_area(域内网络节点个数)
返回值:返回域内所需最少服务器台数
函数作者:陈自立
*************************************************************************/
int calculate_server_minimum(STLink **link_array, STConsumerLinkInfo *consumer_link_info, int num_consumer_area, STNetNodeInfo *net_node_info, int num_node_area)
{
    int i = 0;
    int max = 0;
    int id_used = 0;
    int num_server = 0;
    int sum_server_flow = 0;
    int sum_consumer_need = 0;
    sum_consumer_need = calculate_consumer_need(consumer_link_info, num_consumer_area);
    calculate_node_outflow(link_array, consumer_link_info, num_consumer_area, net_node_info, num_node_area);
    //将域内服务器按供应流量能力大小，将流量由大到小累加，找到能满足域内消费者结点的最小部署数量
    while(sum_server_flow < sum_consumer_need)
    {
        for(i = 0; i < num_node_area; i++)
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
        num_server++;
        sum_server_flow = sum_server_flow + max;
        max = 0;
        net_node_info[id_used].valid = 0;
    }
    //恢复网络节点可用标志位
    for(i = 0; i < num_node_area; i++)
    {
        net_node_info[i].valid = 1;
    }
    //打印出域内需要的流量大小和至少需要的服务器数量
    printf("In area,sum_consumer_need = %d num_server_min = %d\n",sum_consumer_need, num_server);
    return num_server;
}

/************************************************************************
函数功能:聚类处理函数:初始化处理
功能详解:确定全图需要部署的最少服务器台数
入参解释:link_array(有向链路数组)consumer_link_info(域内消费节点链接信息)num_consumer_area(域内消费者节点个数)
net_node_info(域内网络节点信息)num_node_area(域内网络节点个数)
返回值:无
函数作者:陈自立
*************************************************************************/
void cluster_pre(STLink **link_array, STConsumerLinkInfo *consumer_link_info, int num_consumer_area, STNetNodeInfo *net_node_info, int num_node_area)
{
    int need_server_min = 0;
    need_server_min = calculate_server_minimum(link_array, consumer_link_info, num_consumer_area, net_node_info, num_node_area);
    //打印出域内所有网络节点的对外流量
    calculate_debug_node_outflow(net_node_info, num_node_area);
}

/************************************************************************
函数功能:聚类算法总入口
功能详解:调用各聚类算法接口
入参解释:link_array(有向链路数组)consumer_link_info(域内消费节点链接信息)num_consumer_area(域内消费者节点个数)
net_node_info(域内网络节点信息)num_node_area(域内网络节点个数)
返回值:无
函数作者:陈自立
*************************************************************************/
void cluster_main(STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info)
{
    cluster_pre(link_array, consumer_link_info, num_consumer, net_node_info, num_net_dot);
    
}

/************************************************************************
函数功能:算法处理总入口
功能详解:调用处理函数接口，得到输出结果
入参解释:link_info(链路信息),consumer_link_info(消费节点链接信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void process_main(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info)
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
    process_pre(link_info, link_array, p2p_path_info, net_node_info);
    
    //调用dijkstra处理函数，全图遍历最短路径
    dijkstra_process_main(p2p_path_info, link_array, consumer_link_info);

    //调用聚类处理函数
    cluster_main(link_array, consumer_link_info, net_node_info);

    //释放内存空间
    for(i = 0; i < num_net_dot; i++)
    {
        free(link_array[i]);
        free(p2p_path_info[i]);
    }
    free(link_array);
    free(p2p_path_info);
    free(net_node_info);
    
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
    int link_sum = 0;//返回的网络路径总数
    STLinkInfo *link_info = NULL;
    STConsumerLinkInfo *consumer_link_info = NULL;
    STResultInfo *result_info = NULL;
    char * topo_file = NULL;// 需要输出的内容
   
    //解析基础输入信息
    input_analyse_base(topo);

    //依据链路值和消费者节点数量，动态分配几段内存空间，用于存储输入信息:
    link_info = (STLinkInfo *)malloc(num_link * sizeof(STLinkInfo));
    consumer_link_info = (STConsumerLinkInfo *)malloc(num_consumer * sizeof(STConsumerLinkInfo));
    result_info = (STResultInfo *)malloc(num_consumer * sizeof(STResultInfo));
    topo_file = (char *)malloc(2 * num_consumer * sizeof(STResultInfo));//多分配几字节，确保够用

    //进一步解析输入信息，获取其他输入信息
    input_analyse_other(link_info, consumer_link_info, topo);

    //算法处理总入口
    process_main(link_info, consumer_link_info);

    //调试处理函数
    link_sum = process_debug(consumer_link_info, result_info);
    //调试用输出标准化函数
    result_standard_debug(link_sum, result_info, topo_file);
    
    // 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
    write_result(topo_file, filename);

    //释放内存空间
    free(link_info);
    free(consumer_link_info);
    free(result_info);
    free(topo_file);

}
