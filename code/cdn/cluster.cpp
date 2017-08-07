/***********************************************************************
�ļ�����:cluster.cpp
�ļ�����:�����㷨����ģ��,��������
�ļ�����ʱ��:20170409
�ļ�����:������
***********************************************************************/

#include "deploy.h"
#include "dijkstra.h"
#include "cluster.h"
#include "mybase.h"
#include <stdio.h>
#include <stdlib.h>

/************************************************************************
��������:���ദ����:�жϾ�����һ����·�Ƿ���Ҫ����
�������:����ԭ��·�е����нڵ��Ƿ��������Լ�ԭ��·���Ƿ�����·����֦�ж���·�Ƿ���Ҫ����
��ν���:cluster_id(������)p2p_path_info(�㵽������·����Ϣ)net_node_info(��������ڵ���Ϣ)
link_array(������·����)id_net_source(��·Դ�ڵ�id)id_net_end(��·Ŀ�Ľڵ�id)
����ֵ:ret(0:�������ã�1:��Ҫ����)
��������:������
*************************************************************************/
int cluster_p2ppath_is_need_rebuild(int cluster_id, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STLink **link_array, int id_net_source, int id_net_end)
{
    int ret = 0;
    int i = 0;
    int id_a = 0;//������ʱ��¼������·��ͷβ�ڵ���
    int id_b = 0;
    STNode* p = p2p_path_info[id_net_source][id_net_end].p_path;
    if((p2p_path_info[id_net_source][id_net_end].distance < INF) && (p2p_path_info[id_net_source][id_net_end].bandwidth> 0))
    {
        for(i = 0; i < p2p_path_info[id_net_source][id_net_end].link_num; i++)
        {
            id_a = p->id_node;
            p = p->pnext;
            id_b = p->id_node;
            if((p->id_node != cluster_id) || (link_array[id_a][id_b].link_bandwidth== INF))//��·�ϴ��ڵ㲻���ڸþ������·�ϴ�������·����֦�����
            {
                ret = 1;
                return ret;
            }
        }
    }

    return ret;

}

/************************************************************************
��������:���ദ����:������ϢԤ����
�������:�Ծ�����Ϣ����Ԥ����,ע��:������Ϣ�����1��ʼ,��������Ԫ����num_consumer+1��
��ν���:cluster_info(������Ϣ)
����ֵ:��
��������:������
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
��������:���ദ����:����Ԥ����
�������:��ָ��������г�ʼ����������������ڵ��ʼ��p2p_path_info���Ա��������������·����
ͬʱ���¾�����Ϣ
��ν���:cluster_id(������)cluster_info(������Ϣ)p2p_path_info(�㵽������·����Ϣ)
link_array(������·����)net_node_info(��������ڵ���Ϣ)consumer_link_info(�������ѽڵ�������Ϣ)
����ֵ:��
��������:������
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
                            list_release(&(p2p_path_info[i][j].p_path->pnext));//���ͷŵ�ԭ�ȵ���·
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
                            list_release(&(p2p_path_info[i][j].p_path->pnext));//���ͷŵ�ԭ�ȵ���·
                            p2p_path_info[i][j].p_path_end = p2p_path_info[i][j].p_path;
                        }
                    }
                }
            }
        }
    }
}

/************************************************************************
��������:���ദ����:���Ѵ�����
�������:�������ڸ��㵽����������������ڵ�ľ��룬��������ڵ�������߽ڵ��Ϊ����
��ν���:cluster_id(������);id_cluster_A(����A����);id_cluster_B(����B����)
p2p_path_info(�㵽������·����Ϣ);net_node_info(��������ڵ���Ϣ)consumer_link_info(�������ѽڵ�������Ϣ)
����ֵ:��
��������:������
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
��������:���ദ����:�����������
�������:����ָ������ľ�������
��ν���:cluster_id(������)cluster_info(������Ϣ)p2p_path_info(�㵽������·����Ϣ)link_array(������·����)
consumer_link_info(�������ѽڵ�������Ϣ)net_node_info(��������ڵ���Ϣ)
����ֵ:id_center(�����������Ľڵ�ID)
��������:������
*************************************************************************/
int cluster_center(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int j = 0;
    int id_center = 0;
    long value = 0;//��¼�ڵ�����Ȩֵ(�ڵ����-�ڵ㵽���������߽ڵ�ľ���֮��)
    long max_value = -999999999;
    int is_server_cluster_remain = 0;//�����ж��Ƿ�������Ѳ����������ѡȡ
    int id_net = 0;
    double dis_sum = 0;
    
    //�������ڽڵ����
    calculate_node_outflow(cluster_id, link_array, consumer_link_info, net_node_info);
    if(cluster_info[cluster_id].num_server_current > 0)//�������Ѿ������з����������ȴ��������з�������ѡ
    {
        //���Ѳ���������ڵ��ϣ���������ڵ����֮�ͼ�ȥ����ڵ㵽���������߽ڵ�ľ���֮�������ѡȡ��������:
        for(i = 0; i < num_net_dot; i++)
        {
            if((net_node_info[i].cluster_id == cluster_id) && (net_node_info[i].is_server == 1))//���ڷ������ڵ�
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
                if(solution_cluster_center == 0)//ѡ·����Ϊ0
                {
                    value = (long)(net_node_info[i].out_flow) - dis_sum;
                    if(value > max_value)
                    {
                        max_value = value;
                        id_center = i;
                    }
                }
                if(solution_cluster_center == 1)//ѡ·����Ϊ1
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
        if(is_server_cluster_remain == 1)//���Ǵ������Ѳ����������ȡ�㣬���ڴ�ֱ�ӷ���
        {
            return id_center;
        }
    }
    //��Ҫ�����µ�����ڵ���:�����ڷ�����������1
    cluster_info[cluster_id].num_server_current++;

    //�ж��Ƿ��������߽ڵ����������������������ڵ��������ȵ�һ�룬�����ڣ�����������ڵ��ϱ��벿�������:
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
    //��������ڵ����֮�ͼ�ȥ����ڵ㵽���������߽ڵ�ľ���֮�������ѡȡ��������:
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
            if(solution_cluster_center == 0)//ѡ·����Ϊ0
            {
                value = (long)(net_node_info[i].out_flow) - dis_sum;
                if(value > max_value)
                {
                    max_value = value;
                    id_center = i;
                }
            }
            if(solution_cluster_center == 1)//ѡ·����Ϊ1
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
��������:���ദ����:��֦��������ڳ�ʼ������
�������:�����ڼ�֦��ɺ��������³�ʼ��p2p_path_info����Ϣ
��ν���:cluster_id(������)cluster_info(������Ϣ)p2p_path_info(�㵽������·����Ϣ)
link_array(������·����)net_node_info(��������ڵ���Ϣ)consumer_link_info(�������ѽڵ�������Ϣ)
����ֵ:ret(1:�����߽ڵ㱻����,���Դ�����һ�������߽��)
��������:������
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
                    p2p_path_info[i][j].distance = INF;//��ʼ���㵽������·����Ϣ
                    p2p_path_info[i][j].bandwidth = 0;
                }
                else
                {
                    p2p_path_info[i][j].distance = 0;//��ʼ���㵽������·����Ϣ
                    p2p_path_info[i][j].bandwidth = INF;
                }
                p2p_path_info[i][j].link_num = 0;
                p2p_path_info[i][j].p_path->id_node = i;
                list_release(&(p2p_path_info[i][j].p_path->pnext));//���ͷŵ�ԭ�ȵ���·
                p2p_path_info[i][j].p_path->pnext = NULL;
                p2p_path_info[i][j].p_path_end = p2p_path_info[i][j].p_path;//β�ڵ㼴Ϊ·�������һ�㣬��ʼʱ��ΪԴ�ڵ�
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
��������:���ദ����:��֦������
�������:�����ڵ���·�������ַ���Ϻ󣬶Ե���·����֦��ͬʱ�Է���·��Ҳ����֦����
��ν���:cluster_id(������)cluster_info(������Ϣ)id_service_net(����������ڵ�)id_consumer(�����߽����)
cost_bandwidth(��������)p2p_path_info(�㵽������·����Ϣ)link_array(������·����)consumer_link_info(�������ѽڵ�������Ϣ)
net_node_info(��������ڵ���Ϣ)
����ֵ:ret(1:�����߽ڵ㱻����,���Դ�����һ�������߽��)
��������:������
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
    //���ȶ������߽ڵ���м�֦
    if(consumer_link_info[id_consumer].need_bandwidth > cost_bandwidth)
    {
        consumer_link_info[id_consumer].need_bandwidth = consumer_link_info[id_consumer].need_bandwidth - cost_bandwidth;
    }
    else
    {
        consumer_link_info[id_consumer].need_bandwidth = 0;
        consumer_link_info[id_consumer].cluster_id = 0;//����������������Ľڵ㣬�������id��Ϊ0������0��Ӧ�Ѿ���ȥ�������߽ڵ�
        ret = 1;
        num_consumer_remain = num_consumer_remain - 1;
    }
    //�ٶ�������·���м�֦
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
            //һ������ѡ·�������·������Ϊ��ͨ:
            //link_array[id_node_B][id_node_A].link_bandwidth = 0;
            if(link_array[id_node_A][id_node_B].link_bandwidth == 0)
            {
                link_array[id_node_A][id_node_B].cost_per_bandwidth = INF;//��·�����Ϊ0,���֦
            }
        }
        else
        {
            printf("[ERROR]:<cluster_link_prune> link_array[%d][%d].cost_per_bandwidth = %d\n",id_node_A,id_node_B,link_array[id_node_A][id_node_B].cost_per_bandwidth);
        }
    }
    //��֦��Ϻ󣬶��������³�ʼ��������·��
    cluster_link_prune_pre(cluster_id, cluster_info, p2p_path_info, link_array, net_node_info, consumer_link_info);
    //����dijkstra�����������ڱ������·��
    dijkstra_process_main(cluster_id, link_array, p2p_path_info, net_node_info, consumer_link_info);

    return ret;
}

/************************************************************************
��������:���ദ����:���������ַ�������
�������:�����ڲ�������������������ַ�
��ν���:cluster_id(������)cluster_info(������Ϣ)id_service_net(����������ڵ�)p2p_path_info(�㵽������·����Ϣ)
link_array(������·����)consumer_link_info(�������ѽڵ�������Ϣ)net_node_info(��������ڵ���Ϣ)result_info(��������·��Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void cluster_flow_out_process(int cluster_id, STClusterInfo *cluster_info,int id_service_net, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info, STResultInfo *result_info)
{
    int i = 0;
    int is_consumer_clear = 0;//�жϵ�ǰ�����߽ڵ��Ƿ����
    int is_can_not_flow = 0;//�жϵ�ǰ�������Դ������߽ڵ��Ƿ�������Ϊ��
    int id_net_consumer = 0;
    int cost_bandwidth = 0;
    int min_distance = INF;
    int min_id_net = 0;
    int min_id_consumer = 0;
    int consumer_remain = 0;
    char *book = NULL;//���ڱ��һ�������߽ڵ��Ƿ���Ҫ������
    STNode *p = NULL;
    book = (char *)malloc(num_consumer);
    printf("[INFO] <cluster_flow_out_process> cluster_id = %d id_service_net = %d\n",cluster_id,id_service_net);
    //��ʼ��
    for(i = 0; i < num_consumer; i++)
    {
        if(consumer_link_info[i].cluster_id == cluster_id)
        {
            book[i] = 1;//���ڽڵ���Ҫ������
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
                    //�����·������ڵ��������߽�����裬�����߽������������·���������ֵ
                    cost_bandwidth = consumer_link_info[min_id_consumer].need_bandwidth; 
                }
                else
                {
                    //�����·�������ڵ��������߽�����裬�����߽������������·���������ֵ
                    cost_bandwidth = p2p_path_info[id_service_net][min_id_net].bandwidth;
                }
                result_info[num_result_link].cost_bandwidth = cost_bandwidth;
                result_info[num_result_link].id_consumer = consumer_link_info[min_id_consumer].id_consumer;
                result_info[num_result_link].link_num = p2p_path_info[id_service_net][min_id_net].link_num;
                result_info[num_result_link].p_path->id_node = id_service_net;
                result_info[num_result_link].cluster_id = cluster_id;
                //·������
                p = list_link(p2p_path_info[id_service_net][min_id_net].p_path, result_info[num_result_link].p_path, result_info[num_result_link].link_num);
                if(p == NULL)
                {
                    printf("[ERROR] <cluster_flow_out_process> p == NULL\n");
                    break;
                }
                //��֦������
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
                book[min_id_consumer] = 0;//���ڸ������߽ڵ�������Ӧ���
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
��������:���ദ����:���������·�Ż�����
�������:��ѡ��һ���������ĺ󣬶��������е������·�����Ż��������������÷������Ķ���֦Ҷ
��ν���:cluster_id(������)id_service_net(ѡ�����µľ�������)link_array(������·����)
result_info(��������·��Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void cluster_result_optimize(int cluster_id, int id_service_net, STLink **link_array, STResultInfo *result_info)
{
    int i  = 0;
    int j = 0;
    int m = 0;
    int num = 0;
    int id_node_A = 0;
    int id_node_B = 0;
    STNode *p_begin = NULL;//���ͷ������·��ͷ���
    STNode *p = NULL;//�ݴ�ָ��
    STNode *p_end = NULL;//���ͷ������·��β�ڵ�
    
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
                if(p->id_node == id_service_net)//��·�д��ڽڵ㾭���˵�ǰ�貿�����Ƶ�������ڵ㣬�����·��Ҫ�Ż�:
                {
                    //ԭ�����Ϣ����������֦Ҷ������·�����ȣ��任·��ͷ���
                    p_end = p;
                    result_info[i].link_num = result_info[i].link_num - num;
                    result_info[i].p_path = p_end;
                    p = p_begin;
                    for(m = 0; m < num; m++)
                    {
                        id_node_A = p->id_node;
                        id_node_B = p->pnext->id_node;
                        //�ͷ���·link_array
                        link_array[id_node_A][id_node_B].link_bandwidth = link_array[id_node_A][id_node_B].link_bandwidth + result_info[i].cost_bandwidth;
                        link_array[id_node_A][id_node_B].cost_per_bandwidth = link_array[id_node_A][id_node_B].cost_per_bandwidth_history;
                        //��ԭ�����·ָ���ͷ�
                        p_begin = p;
                        p = p->pnext;
                        if(p_begin != NULL)
                        {
                            free(p_begin);
                            p_begin = NULL;
                        }
                    }
                    break;//�˳�����·���Ż�ѭ����������һ����·���Ż�������
                }
                p = p->pnext;
            }
        }
    }
}


/************************************************************************
��������:���ദ����:����ϲ�������
�������:�������в�������������ﵽ�������޻������ѿ��Ը�����ʱ����������Դ�ͷŵ���ĸ���У�����������Ϊ�ͷ���
��ν���:cluster_id(������)cluster_info(������Ϣ)consumer_link_info(�������ѽڵ�������Ϣ)
net_node_info(��������ڵ���Ϣ)result_info(��������·��Ϣ)
����ֵ:ret(�ɹ��򷵻�ĸ��id�����򷵻�0)
��������:������
*************************************************************************/
int cluster_merge_sub_release(int cluster_id, STClusterInfo *cluster_info, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info, STResultInfo *result_info)
{
    int i = 0;
    int cluster_id_origin = (int)(cluster_id/2);//ĸ����id
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
    //�ͷ�����ڵ�
    for(i = 0; i < num_net_dot; i++)
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            net_node_info[i].cluster_id = cluster_id_origin;
        }
    }
    //�ͷ������߽ڵ�
    for(i = 0; i < num_consumer; i++)
    {
        if((consumer_link_info[i].cluster_id == cluster_id) && (consumer_link_info[i].need_bandwidth > 0))
        {
            consumer_link_info[i].cluster_id = cluster_id_origin;
        }
    }
    //�ͷ������·��Ϣ
    for(i = 0; i < num_result_link; i++)
    {
        if(result_info[i].cluster_id == cluster_id)
        {
            result_info[i].cluster_id = cluster_id_origin;
        }
    }
    //�ͷ�����Ϣ
    cluster_info[cluster_id].valid = 2;//���ͷ���
    cluster_info[cluster_id_origin].num_server_current += cluster_info[cluster_id].num_server_current;

    if((cluster_id % 2) == 0)//ż�������
    {
        if(cluster_info[(cluster_id + 1)].valid== 2)//�ֵ���Ҳ�Ѿ��ͷ�
        {
            cluster_info[cluster_id_origin].valid = 1;
            cluster_info[cluster_id_origin].num_cluster_consumer = cluster_info[cluster_id].num_cluster_consumer + cluster_info[(cluster_id+1)].num_cluster_consumer;
        }
    }
    else//���������
    {
        if(cluster_info[(cluster_id - 1)].valid== 2)//�ֵ���Ҳ�Ѿ��ͷ�
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
��������:���ദ����:���������ַ����
�������:���������ַ����
��ν���:cluster_id(������)cluster_info(������Ϣ)p2p_path_info(�㵽������·����Ϣ)link_array(������·����)
consumer_link_info(�������ѽڵ�������Ϣ)net_node_info(��������ڵ���Ϣ)result_info(��������·��Ϣ)
����ֵ:id_center(�����������Ľڵ�ID)
��������:������
*************************************************************************/
void cluster_flow_out_main(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info, STResultInfo *result_info)
{
    int i = 0;
    int id_service_net = 0;
    int cluster_id_origin = 0;//����ϲ��ɹ���õ���ĸ����
    int flag = 1;
    if(num_consumer > 300)//�߼�������Ϊ�����м����࣬Ȼ���ٷ�����
    {
        flag = HARD_FLAG;
    }
    //����dijkstra�����������ڱ������·��
    dijkstra_process_main(cluster_id, link_array, p2p_path_info, net_node_info, consumer_link_info);
    //��ӡ�����ڵ�����ڵ�������߽ڵ���:
    //cluster_debug_net_consumer(cluster_id, net_node_info, consumer_link_info);
    printf("[INFO] <cluster_flow_out_main> cluster_id = %d\n",cluster_id);
    if(cluster_id <= flag)//����ԭʼ����ԣ����뱣֤�����������߽ڵ㹩��
    {
        do
        {
            //���Ժ���:��ӡ���ڸ��ڵ�֮������·��
            //dijkstra_debug_net_p2p(cluster_id, net_node_info, p2p_path_info);
            //��ȡ�µľ�������:
            id_service_net = cluster_center(cluster_id, cluster_info, p2p_path_info, link_array, consumer_link_info, net_node_info);
            net_node_info[id_service_net].is_server = 1;//��Ǹ�����ڵ�Ϊ�������ڵ�
            //������ѡ���ľ������ģ�����������������һ���Ż�
            if(num_consumer <= 300)
            {
                result_optimize(net_node_info, link_array, result_info);
            }
            else
            {
                cluster_result_optimize(cluster_id,id_service_net, link_array, result_info);
            }
            printf("[INFO] <cluster_flow_out_main> cluster_id = %d id_service_net = %d num_cluster_consumer = %d\n",cluster_id,id_service_net,cluster_info[cluster_id].num_cluster_consumer);
            //�����ַ�:
            cluster_flow_out_process(cluster_id, cluster_info, id_service_net, p2p_path_info, link_array, consumer_link_info, net_node_info,result_info);
            net_node_info[id_service_net].cluster_id = (int)(cluster_id/2);//��ԭ����������ڵ������ĸ��
            //��֦��Ϻ󣬶��������³�ʼ��������·��
            cluster_link_prune_pre(cluster_id, cluster_info, p2p_path_info, link_array, net_node_info, consumer_link_info);
            //����dijkstra�����������ڱ������·��
            dijkstra_process_main(cluster_id, link_array, p2p_path_info, net_node_info, consumer_link_info);
            
        }while(cluster_info[cluster_id].num_cluster_consumer > 0);//��������δ�����Ը�����ʱ(���������߽ڵ�ȫ�����)
    }
    else//���ڷ�ԭʼ����ԣ������ڷ����������ﵽ��������ʱ�����Ǻϲ��ֵ��࣬�໥��Դ����
    {
        for(i = 0; i < cluster_info[cluster_id].max_num_server; i++)
        {
            //��ȡ�µľ�������:
            id_service_net = cluster_center(cluster_id, cluster_info, p2p_path_info, link_array, consumer_link_info, net_node_info);
            net_node_info[id_service_net].is_server = 1;//��Ǹ�����ڵ�Ϊ�������ڵ�
            //������ѡ���ľ������ģ�����������������һ���Ż�
            if(num_consumer <= 300)
            {
                result_optimize(net_node_info, link_array, result_info);
            }
            else
            {
                cluster_result_optimize(cluster_id,id_service_net, link_array, result_info);
            }
            printf("[INFO] <cluster_flow_out_main> cluster_id = %d id_service_net = %d num_cluster_consumer = %d\n",cluster_id,id_service_net,cluster_info[cluster_id].num_cluster_consumer);
            //�����ַ�:
            cluster_flow_out_process(cluster_id, cluster_info, id_service_net, p2p_path_info, link_array, consumer_link_info, net_node_info,result_info);
            net_node_info[id_service_net].cluster_id = (int)(cluster_id/2);//��ԭ����������ڵ������ĸ��
            //��֦��Ϻ󣬶��������³�ʼ��������·��
            cluster_link_prune_pre(cluster_id, cluster_info, p2p_path_info, link_array, net_node_info, consumer_link_info);
            //����dijkstra�����������ڱ������·��
            dijkstra_process_main(cluster_id, link_array, p2p_path_info, net_node_info, consumer_link_info);
            if(cluster_info[cluster_id].num_cluster_consumer == 0)//���������Ը�����ʱ�Ƴ�ѭ��
            {
                break;
            }
        }
        cluster_id_origin = cluster_merge_sub_release(cluster_id, cluster_info, consumer_link_info, net_node_info, result_info);
        if(cluster_id_origin > 0)//ĸ���ͷųɹ�
        {
            //��ĸ�����³�ʼ��������·��
            cluster_link_prune_pre(cluster_id_origin, cluster_info, p2p_path_info, link_array, net_node_info, consumer_link_info);
            //����dijkstra�����������ڱ������·��
            dijkstra_process_main(cluster_id_origin, link_array, p2p_path_info, net_node_info, consumer_link_info);
        }
    }
    printf("[INFO]<cluster_flow_out_main> cluster%d: server num : %d\n",cluster_id,cluster_info[cluster_id].num_server_current);
}


/************************************************************************
��������:���ദ����:�ݹ����
�������:ȷ��������Ҫ��������ٷ�����̨���������ٷ�����̨������2ʱ�����ǵ�������
��ν���:cluster_id(������)cluster_info(������Ϣ)p2p_path_info(�㵽������·����Ϣ)
link_array(������·����)consumer_link_info(�������ѽڵ�������Ϣ)net_node_info(��������ڵ���Ϣ)
����ֵ:cluster_id_ret(�ϴ��һ��������)
��������:������
*************************************************************************/
int cluster_process(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info)
{
    int cluster_id_ret = 0;
    int id_cluster_A = 0;//����������Զ����ڵ�ı��
    int id_cluster_B = 0;  
    
    //����dijkstra��������ȫͼ�������·��
    dijkstra_process_main(cluster_id, link_array, p2p_path_info, net_node_info, consumer_link_info);
    //�ҳ������߽��֮�����Զ����
    dijkstra_process_consumer_max(cluster_id, p2p_path_info, consumer_link_info, &id_cluster_A, &id_cluster_B);
    if(id_cluster_A == id_cluster_B)
    {
        printf("[ERROR] <cluster_process> id_cluster_A == id_cluster_B = %d\n",id_cluster_A);
        return cluster_id_ret;
    }
    //���÷��Ѻ��������ݾ��뽫��ǰ���Ϊ����
    cluster_split(cluster_id, id_cluster_A, id_cluster_B, p2p_path_info, net_node_info, consumer_link_info);
    cluster_info[cluster_id].valid = 0;//���������ĸ����ʱ��Ϊ��Ч
    //���þ����ʼ����������ʼ�����Ѻ��µľ����е�p2p_path_info��Ϣ
    cluster_pre((2*cluster_id),cluster_info,p2p_path_info,link_array,net_node_info,consumer_link_info);
    cluster_pre((2*cluster_id + 1),cluster_info,p2p_path_info,link_array,net_node_info,consumer_link_info);
    //��������������Ŀ��һ���ģ�ֻ�ܸ�ֵһ�Σ�֮����������,����д�뺯��cluster_pre���ظ�����
    cluster_info[(2*cluster_id)].need_service_num = calculate_server_minimum((2*cluster_id), link_array, consumer_link_info, net_node_info);
    cluster_info[(2*cluster_id + 1)].need_service_num = calculate_server_minimum((2*cluster_id + 1), link_array, consumer_link_info, net_node_info);
    cluster_info[(2*cluster_id)].max_num_server =  cluster_info[(2*cluster_id)].need_service_num + max_add_cluster_server_num;
    cluster_info[(2*cluster_id + 1)].max_num_server =  cluster_info[(2*cluster_id + 1)].need_service_num + max_add_cluster_server_num;
    cluster_id_ret = 2*cluster_id + 1;
    return cluster_id_ret;
}


/************************************************************************
��������:�����㷨�����
�������:���ø������㷨�ӿ�
��ν���:p2p_path_info(�㵽������·����Ϣ)link_array(������·����)consumer_link_info(�������ѽڵ�������Ϣ)
net_node_info(��������ڵ���Ϣ)result_info(��������·��Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void cluster_main(STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info, STResultInfo *result_info)
{
    int i = 1;
    int cluster_id = 1;//��ʼ����IDΪ1
    int cluster_id_max = 1;
    int cluster_id_ret = 0;
    STClusterInfo *cluster_info = NULL;
    cluster_info = (STClusterInfo *)malloc((num_consumer + 2) * sizeof(STClusterInfo));//���������಻�ᳬ�������߽ڵ������������Ϣ�����1��ʼ
    //Ԥ��ʼ��������Ϣ����,ͬʱ��ʼ����ʼ����
    cluster_info_pre(cluster_info);
    cluster_pre(cluster_id, cluster_info, p2p_path_info, link_array, net_node_info, consumer_link_info);
    cluster_info[cluster_id].need_service_num = calculate_server_minimum(cluster_id, link_array, consumer_link_info, net_node_info);
    cluster_info[cluster_id].max_num_server = cluster_info[cluster_id].need_service_num + max_add_cluster_server_num;//��������������Ŀ��һ���ģ�ֻ�ܸ�ֵһ�Σ�֮����������,����д�뺯��cluster_pre���ظ�����
    printf("[INFO] <cluster_main> begin\n");

    for(i = 1; i <= cluster_id_max; i++)
    {
        if(cluster_info[i].valid == 1)
        {
            cluster_id = i;
            if(num_consumer <= 100)//��������
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
            if((num_consumer > 100) && (num_consumer <= 300))//�м�����
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
            if(num_consumer > 300)//�߼�����
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

    //�Դ�С������С��:�Գ�ʼ�������޲�����������������÷���+��Ԫ����+�ϲ���������:
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
��������:������Ժ���:��ӡ��ĳһ�����е�����ڵ��������߽ڵ���
�������:��ӡ��ĳһ�����е�����ڵ��������߽ڵ���
��ν���:cluster_id(������)net_node_info(��������ڵ���Ϣ)consumer_link_info(�������ѽڵ�������Ϣ)
����ֵ:��
��������:������
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


