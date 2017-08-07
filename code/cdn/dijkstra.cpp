/***********************************************************************
文件名称:dijkstra.cpp
文件功能:dijkstra单源最短路径算法处理模块,函数定义
文件创建时间:20170409
文件作者:陈自立
***********************************************************************/

#include "deploy.h"
#include "dijkstra.h"
#include "mybase.h"
#include <stdio.h>
#include <stdlib.h>

/************************************************************************
函数功能:dijkstra优化处理函数(构建邻接表):计算邻接表长度即实际链路数量
功能详解:根据最新的link_array计算邻接表长度即实际链路数量
入参解释:link_array(有向链路数组)
返回值:返回邻接表长度
函数作者:陈自立
*************************************************************************/
int dijkstra_adjacency_list_calculate(STLink **link_array)
{
    int i = 0;
    int j = 0;
    int sum = 0;
    for(i = 0; i < num_net_dot; i++)
    {
        for(j = 0; j < num_net_dot; j++)
        {
            if((link_array[i][j].cost_per_bandwidth< INF) && (link_array[i][j].cost_per_bandwidth > 0))
            {
                sum++;
            }
        }
    }
    printf("[INFO] dijkstra_adjacency_list_calculate sum = %d\n",sum);
    return sum;
}

/************************************************************************
函数功能:dijkstra优化处理函数(构建邻接表):构建邻接表
功能详解:构建邻接表
入参解释:link_array(有向链路数组)adj_link_info(邻接链表数组)
first(用于存放邻接链表各个顶点的第一条边id，下标为顶点的编号)next(用于存放邻接链表各边的下一条边:此处的下一条边是指从同一顶点出发的边)
返回值:无
函数作者:陈自立
*************************************************************************/
void dijkstra_adjacency_list_create(STLink **link_array, STLinkInfo *adj_link_info, int *first, int *next)
{
    int i = 0;
    int j = 0;
    int k = 0;

    //初始化
    for(i = 0; i < num_link_actual; i++)
    {
        first[i] = -1;
        next[i] = -1;
    }
    
    for(i = 0; i < num_net_dot; i++)
    {
        for(j = 0; j < num_net_dot; j++)
        {
            if((link_array[i][j].cost_per_bandwidth < INF) && (link_array[i][j].cost_per_bandwidth > 0))
            {
                adj_link_info[k].id_link_begin = i;
                adj_link_info[k].id_link_end = j;
                adj_link_info[k].cost_per_bandwidth = link_array[i][j].cost_per_bandwidth;
                adj_link_info[k].link_bandwidth = link_array[i][j].link_bandwidth;
                next[k] = first[i];//next中存放上一次存入该顶点的编号，记录下当前边的下一条边
                first[i] = k;//first中根据顶点存入该顶点发出的首条边
                k++;
            }
        }
    }

    if(k != num_link_actual)
    {
        printf("[ERROR] dijkstra_adjacency_list_create k = %d num_link_actual = %d\n",k,num_link_actual);
    }
}

/************************************************************************
函数功能:dijkstra优化处理函数:构建最小堆(树中所有母节点均小于其叶子节点)
功能详解:将域内节点加入最小堆，以堆的形式进行排序
入参解释:cluster_id(聚类编号)id_source(源节点id)stack_node(最小堆数组)p2p_path_info(点到点的最短路径信息)
net_node_info(域内网络节点信息)
返回值:stack_length(最小堆长度)
函数作者:陈自立
*************************************************************************/
int dijkstra_stack_create(int cluster_id, int id_source, STStackNodeInfo *stack_node, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int j = 0;
    int k = 0;
    STStackNodeInfo stack_value = {0};//临时变量
    int id_net_node = 0;//堆数组下标
    int stack_length = 0;//堆的长度
    for(i = 0; i < num_net_dot; i++)//将所有域内网络节点加入堆中
    {
        if((net_node_info[i].cluster_id == cluster_id) && (i != id_source))//将域内非源点的所有网络节点加入数组
        {
            stack_length++;
            id_net_node = stack_length;
            stack_node[id_net_node].id_node = i;
            stack_node[id_net_node].value = p2p_path_info[id_source][i].distance;
            if(id_net_node > 1)
            {
                j = id_net_node/2;
                do{
                    if(stack_node[id_net_node].value < stack_node[j].value)//若新加入的值小于其母节点的值，则将两节点交换
                    {
                        stack_value.id_node = stack_node[j].id_node;
                        stack_value.value = stack_node[j].value;
                        stack_node[j].id_node = stack_node[id_net_node].id_node;
                        stack_node[j].value = stack_node[id_net_node].value;
                        stack_node[id_net_node].id_node = stack_value.id_node;
                        stack_node[id_net_node].value = stack_value.value;
                        id_net_node = j;
                        j = id_net_node/2; 
                    }
                    else
                    {
                        break;
                    }
                }while(id_net_node > 1);
            }
        }
    }
    for(i = 1; i <= stack_length; i++)
    {
        k = stack_node[i].id_node;
        net_node_info[k].id_stack_node = i;
    }
    
    return stack_length;
}

/************************************************************************
函数功能:dijkstra优化处理函数:最小堆调整函数
功能详解:从堆顶最小值取出，加入新优化的路径值到堆顶，重新调整最小堆
入参解释:id_node(堆内变化位)stack_length(堆长度)stack_node(最小堆数组)net_node_info(域内网络节点信息)
返回值:ret(返回是否继续迭代标志位1:继续迭代，0:退出迭代)(堆内最小值是否为INF，若是则该堆遍历完毕)
函数作者:陈自立
*************************************************************************/
int dijkstra_stack_optimize(int id_node, int stack_length, STStackNodeInfo *stack_node, STNetNodeInfo *net_node_info)
{
    int ret = 0;
    int i = id_node;
    int j = 0;
    int k = 0;
    int num = 0;//迭代计数
    STStackNodeInfo stack_value = {0};//临时变量
    //printf("[INFO] dijkstra_stack_optimize id_node = %d\n",id_node);
    
    if((id_node < 1) || (id_node > stack_length))
    {
        printf("[ERROR] dijkstra_stack_optimize id_node = %d\n",id_node);
        return ret;
    }
    while(1)
    {
        num++;
        if(i == 1)//id_node = 1
        {
            if((stack_node[(2*i)].value < stack_node[(2*i + 1)].value) || ((2*i + 1) > stack_length))//寻找两个叶子节点中较小的一个
            {
                j = (2*i);
            }
            else
            {
                j = (2*i + 1);
            }
            
            if(stack_node[j].value < stack_node[i].value)//若最小叶子节点小于母节点，则将这两个节点互换
            {
                stack_value.id_node = stack_node[j].id_node;
                stack_value.value = stack_node[j].value;
                stack_node[j].id_node = stack_node[i].id_node;
                stack_node[j].value = stack_node[i].value;
                stack_node[i].id_node = stack_value.id_node;
                stack_node[i].value = stack_value.value;
                /*保存下网络节点对应堆数组中节点的位置*/
                k = stack_node[j].id_node;
                net_node_info[k].id_stack_node = j;
                k = stack_node[i].id_node;
                net_node_info[k].id_stack_node = i;
                i = j;
            }
            else
            {
                break;
            }
        }
        else 
        {
            if(i > stack_length/2)//id_node > stack_length/2
            {
                j = i/2;
                if(stack_node[j].value > stack_node[i].value)//若母节点大于该叶子节点数值，则将这两个节点互换
                {
                    stack_value.id_node = stack_node[j].id_node;
                    stack_value.value = stack_node[j].value;
                    stack_node[j].id_node = stack_node[i].id_node;
                    stack_node[j].value = stack_node[i].value;
                    stack_node[i].id_node = stack_value.id_node;
                    stack_node[i].value = stack_value.value;
                    /*保存下网络节点对应堆数组中节点的位置*/
                    k = stack_node[j].id_node;
                    net_node_info[k].id_stack_node = j;
                    k = stack_node[i].id_node;
                    net_node_info[k].id_stack_node = i;
                    i = j;
                }
                else
                {
                    break;
                }
            }
            else// 1 < id_node <= stack_length/2
            {
                j = i/2;
                if(stack_node[j].value > stack_node[i].value)//若母节点大于该叶子节点数值，则将这两个节点互换
                {
                    stack_value.id_node = stack_node[j].id_node;
                    stack_value.value = stack_node[j].value;
                    stack_node[j].id_node = stack_node[i].id_node;
                    stack_node[j].value = stack_node[i].value;
                    stack_node[i].id_node = stack_value.id_node;
                    stack_node[i].value = stack_value.value;
                    /*保存下网络节点对应堆数组中节点的位置*/
                    k = stack_node[j].id_node;
                    net_node_info[k].id_stack_node = j;
                    k = stack_node[i].id_node;
                    net_node_info[k].id_stack_node = i;
                    i = j;
                }
                else
                {
                    if((stack_node[(2*i)].value < stack_node[(2*i + 1)].value) || ((2*i + 1) > stack_length))//寻找两个叶子节点中较小的一个
                    {
                        j = (2*i);
                    }
                    else
                    {
                        j = (2*i + 1);
                    }
                    if(stack_node[j].value < stack_node[i].value)//若最小叶子节点小于母节点，则将这两个节点互换
                    {
                        stack_value.id_node = stack_node[j].id_node;
                        stack_value.value = stack_node[j].value;
                        stack_node[j].id_node = stack_node[i].id_node;
                        stack_node[j].value = stack_node[i].value;
                        stack_node[i].id_node = stack_value.id_node;
                        stack_node[i].value = stack_value.value;
                        /*保存下网络节点对应堆数组中节点的位置*/
                        k = stack_node[j].id_node;
                        net_node_info[k].id_stack_node = j;
                        k = stack_node[i].id_node;
                        net_node_info[k].id_stack_node = i;
                        i = j;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

    if(stack_node[1].value < INF)
    {
        ret = 1;
    }
    else
    {
        ret = 0;
    }

    return ret;
}

/************************************************************************
函数功能:dijkstra优化处理函数:最小堆检查函数
功能详解:根据最小对规则，检查最小堆有效性，若有效，则打印出最小堆数组，若无效，打印出ERROR信息
入参解释:stack_length(堆长度)stack_node(最小堆数组)
返回值:is_false(0:success,1:fail)
函数作者:陈自立
*************************************************************************/
int dijkstra_stack_check(int stack_length, STStackNodeInfo *stack_node)
{
    int i = 1;
    int j = 0;
    int is_false = 0;
    for(i = 2; i <= stack_length; i++)
    {
        j = i/2;
        if(stack_node[j].value > stack_node[i].value)
        {
            is_false = 1;
            printf("[ERROR] dijkstra_stack_check fail stack_node[%d].value = %d stack_node[%d].value = %d\n",i,stack_node[i].value,j,stack_node[j].value);
            break;
        }
    }
    return is_false;
}

/************************************************************************
函数功能:dijkstra处理函数:指定源点到其他所有点的最短距离
功能详解:找出指定点到其他所有点的最短距离，将最短距离和该路径所经过的点存入指定数组和链表中
入参解释:cluster_id(聚类编号)id_source(源节点id)stack_node(最小堆数组)link_array(有向链路数组)
adj_link_info(邻接链表数组)first(用于存放邻接链表各个顶点的第一条边id，下标为顶点的编号)
next(用于存放邻接链表各边的下一条边:此处的下一条边是指从同一顶点出发的边)p2p_path_info(点到点的最短路径信息)
net_node_info(域内网络节点信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void dijkstra_process_p2mp(int cluster_id, int id_source, STStackNodeInfo *stack_node, STLink **link_array, STLinkInfo *adj_link_info, int *first, int *next, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int u = 0;
    int v = 0;
    int k = 0;
    int id_node = 0;
    int stack_length = 0;
    int is_continue = 1;//最小堆优化返回值
    int is_false = 0;//最小堆校验返回值
    char *book = NULL;//标记当前点到制定源点的距离是否最小
    int *dis = NULL;//存放源点到其他点的最短距离
    STNode *p = NULL;
    STNode *q = NULL;
    dis = (int *)malloc(num_net_dot * sizeof(int));
    book = (char *)malloc(num_net_dot);

    //构建最小堆:
    stack_length = dijkstra_stack_create(cluster_id, id_source, stack_node, p2p_path_info, net_node_info);
    is_false = dijkstra_stack_check(stack_length, stack_node);//若最小堆校验失败，直接退出
    if(is_false == 1)
    {
        printf("[ERROR] dijkstra_stack_check fail\n");
        return;
    }
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

    for(i = 0; i < stack_length; i++)//一共循环stack_length次可以遍历域内所有节点
    {
        if(is_continue == 0)
        {
            break;
        }

        //找到离源点最近的网络节点
        u = stack_node[1].id_node;
        k = first[u];
        /*最小堆头结点优化*/
        stack_node[1].value = INF;
        is_continue = dijkstra_stack_optimize(1, stack_length, stack_node, net_node_info);
        
        while(k != -1)
        {
            v = adj_link_info[k].id_link_end;
            if(net_node_info[v].cluster_id == cluster_id)//仅对域内的节点进行操作
            {
                if(dis[v] > dis[u] + link_array[u][v].cost_per_bandwidth)
                {
                    if((dis[u] + link_array[u][v].cost_per_bandwidth) < 0)
                    {
                        printf("[ERROR] <dijkstra_process_p2mp> p2p_path_info[%d][%d].distance = %d dis[u] = %d\n",u,v,p2p_path_info[u][v].distance,dis[u]);
                        continue;
                    }
                    dis[v] = dis[u] + link_array[u][v].cost_per_bandwidth;//获取到点v的新的最小路径权重
                    if(dis[v] > INF)
                    {
                        dis[v] = INF;
                    }
                    /*最小堆内部结点优化*/
                    id_node = net_node_info[v].id_stack_node;
                    stack_node[id_node].value = dis[v];
                    is_continue = dijkstra_stack_optimize(id_node, stack_length, stack_node, net_node_info);
                    
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
            k = next[k];
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
入参解释:cluster_id(聚类编号)stack_node(最小堆数组)link_array(有向链路数组)adj_link_info(邻接链表数组)
first(用于存放邻接链表各个顶点的第一条边id，下标为顶点的编号)next(用于存放邻接链表各边的下一条边:此处的下一条边是指从同一顶点出发的边)
p2p_path_info(点到点的最短路径信息)net_node_info(域内网络节点信息)
返回值:无
函数作者:陈自立
*************************************************************************/
void dijkstra_process_all_distance(int cluster_id, STStackNodeInfo *stack_node, STLink **link_array, STLinkInfo *adj_link_info, int *first, int *next, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    //计算出所有网络节点两两之间的最短路径
    for(i = 0; i < num_net_dot; i++)
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            dijkstra_process_p2mp(cluster_id, i, stack_node, link_array, adj_link_info, first, next, p2p_path_info, net_node_info);
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
入参解释:cluster_id(聚类编号)link_array(有向链路数组)p2p_path_info(点到点的最短路径信息)net_node_info(域内网络节点信息)
consumer_link_info(消费节点链接信息)
返回值:无
函数作者:陈自立
*************************************************************************/

void dijkstra_process_main(int cluster_id, STLink **link_array, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info)
{ 
    printf("[INFO] <dijkstra_process_main> cluster_id = %d\n",cluster_id);
    //构建邻接表的变量
    STLinkInfo *adj_link_info = NULL;
    int *first = NULL;
    int *next = NULL;
    //构建最小堆的变量
    STStackNodeInfo *stack_node = NULL;
    stack_node = (STStackNodeInfo *)malloc((num_net_dot+1) * sizeof(STStackNodeInfo));//堆结构的数组下标从1开始，此处以空间换时间，申请的内存空间考虑全局最大情况

    num_link_actual = dijkstra_adjacency_list_calculate(link_array);//计算邻接表长度
    adj_link_info = (STLinkInfo *)malloc(num_link_actual * sizeof(STLinkInfo)); 
    first = (int *)malloc(num_link_actual * sizeof(int));
    next = (int *)malloc(num_link_actual * sizeof(int));

    //构建邻接链表
    dijkstra_adjacency_list_create(link_array, adj_link_info, first, next);
    
    //域内全图遍历
    dijkstra_process_all_distance(cluster_id, stack_node, link_array, adj_link_info, first, next, p2p_path_info, net_node_info);
    //打印出所有消费者节点两两间的距离
    //dijkstra_debug_consumer_all(cluster_id, p2p_path_info, consumer_link_info);

    free(adj_link_info);
    adj_link_info = NULL;
    free(first);
    first = NULL;
    free(next);
    next = NULL;
    free(stack_node);
    stack_node = NULL;
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


