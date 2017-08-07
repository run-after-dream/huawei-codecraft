/***********************************************************************
文件名称:mybase.cpp
文件功能:基本数据处理模块,自己编写的计算，截取字符，释放内存，链表拼接等功能函数定义
文件创建时间:20170409
文件作者:陈自立
***********************************************************************/

#include "deploy.h"
#include "mybase.h"
#include <stdio.h>
#include <stdlib.h>


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

