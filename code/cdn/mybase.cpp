/***********************************************************************
�ļ�����:mybase.cpp
�ļ�����:�������ݴ���ģ��,�Լ���д�ļ��㣬��ȡ�ַ����ͷ��ڴ棬����ƴ�ӵȹ��ܺ�������
�ļ�����ʱ��:20170409
�ļ�����:������
***********************************************************************/

#include "deploy.h"
#include "mybase.h"
#include <stdio.h>
#include <stdlib.h>


/************************************************************************
��������:�ַ�����ȡ����
��ν���:ch(�����ַ���),pos_begin(����ȡ�ַ�������ʼλ��),pos_end(����ȡ�ַ�������ֹλ��)
����ֵ:���ؽ�ȡ����ַ���
��������:������
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
��������:��int����ת��Ϊ�ַ�������
��ν���:num(int��������),str(��ת�����ַ���)
����ֵ:��
��������:������
*************************************************************************/
void change_int_to_char(int num, char *str)
{
    sprintf(str, "%d", num);
}

/************************************************************************
��������:�����ڴ��ͷ�
�������:�ͷ�ָ��������ڴ�ռ�,��ͷ��㿪ʼ�ͷ���
��ν���:p_head(����ͷ�ڵ�ָ��)
����ֵ:��
��������:������
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
��������:������ƴ��
�������:������Ϊlengthͷ���Ϊp_source��������һ�ݣ��õ�ͷ���Ϊp_target������
��ν���:p_source(Դ�����ͷ���)p_target(Ŀ�������ͷ���)length(�����е���·��)
����ֵ:Ŀ������β�ڵ��ָ��
��������:������
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
��������:�����ú���:��ӡ��������������ڵ�����������
�������:��ӡ��������������ڵ�����������
��ν���:cluster_id(������)net_node_info(��������ڵ���Ϣ)
����ֵ:��
��������:������
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
��������:���㺯��:����������������߽ڵ�������
�������:����������������߽ڵ�������
��ν���:cluster_id(������)consumer_link_info(���ѽڵ�������Ϣ)
����ֵ:�������������߽��������
��������:������
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
��������:���㺯��:������ڸ�����ڵ����������
�������:������ڸ�����ڵ����������,
��ν���:cluster_id(������)link_array(������·����)consumer_link_info(�������ѽڵ�������Ϣ)
net_node_info(��������ڵ���Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void calculate_node_outflow(int cluster_id, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int j = 0;
    int id = 0;
    //�ȳ�ʼ��һ��:
    for(i = 0; i < num_net_dot; i++)
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            net_node_info[i].out_flow = 0;
        }
    }
    //�����ҳ����ѽڵ����ڵ�����ڵ㣬�������ѽڵ�����Ĵ���
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
��������:���㺯��:��������貿����Ƶ����������Сֵ
�������:��������貿����Ƶ����������Сֵ
��ν���:cluster_id(������);link_array(������·����)consumer_link_info(�������ѽڵ�������Ϣ)
net_node_info(��������ڵ���Ϣ)
����ֵ:���������������ٷ�����̨��
��������:������
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
    //�����ڷ���������Ӧ����������С���������ɴ�С�ۼӣ��ҵ����������������߽�����С��������
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
    //�ָ�����ڵ���ñ�־λ
    for(i = 0; i < num_net_dot; i++)
    {
        net_node_info[i].valid = 1;
    }
    //��ӡ��������Ҫ��������С��������Ҫ�ķ���������
    printf("In cluster%d,sum_consumer_need = %d num_server_min = %d\n",cluster_id, sum_consumer_need, num_server);
    return num_server;
}

