/***********************************************************************
�ļ�����:deploy.cpp
�ļ�����:�ٷ��ṩ�Ĺ��ܺ�����ڣ���д�����̿�ܺ����ӿڣ�����ȫ�ֱ�������
�ļ�����ʱ��:20170307
�ļ�����:huawei
�޸�ʱ��:20170409
�޸���:������
***********************************************************************/

#include "deploy.h"
#include "dijkstra.h"
#include "cluster.h"
#include "mybase.h"
#include "myio.h"
#include <stdio.h>
#include <stdlib.h>

int num_net_dot;//����ڵ�����
int num_link;//������·����
int num_consumer;//���ѽڵ�����
int cost_server;//��Ƶ���ݷ���������ɱ�
int num_link_actual;//����˫������ʱ��ʵ����·����
int num_result_link;//�������·����
int num_consumer_remain;//ʣ�������߽ڵ���
int num_server;//�Ѳ������������
int max_add_cluster_server_num;//�����Ӿ��������ɲ���ķ��������������������з����������������������ɲ�����(������Ҫ�������������ֵ�ĺ�)ʱ�����ǽ��þ��������ֵܾ���ϲ�Ϊĸ���࣬�ò����Գ�ʼ����(cluster_id = 1)��Ч
int solution_cluster_center = 0;//ѡ��������ĵĲ��� 0:ͨ������-�����þ������ģ�1:ͨ���������ѡ��������

/************************************************************************
��������:�������ܵ��Ժ���(������)
�������:�����ú����������з��������������ѽڵ��϶�
��ν���:consumer_link_info(���ѽڵ�������Ϣ),result_debug_info(�������·����Ϣ)
����ֵ:����·����
��������:������
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
��������:������ϢԤ����ģ��
�������:Ԥ����������Ϣ,��link_info�е����ݴ���link_array��,��ʼ���㵽������·����Ϣ��
������ڵ���Ϣ��ʼ��
��ν���:link_info(��·��Ϣ)link_array(������·����)p2p_path_info(��ŵ㵽������·����Ϣ)
net_node_info(����ڵ���Ϣ)result_info(��������·��Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void process_pre(STLinkInfo *link_info, STLink **link_array, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STResultInfo *result_info)
{
    int i = 0;
    int m = 0;
    int n = 0;

    printf("[INFO] <process_pre> begin\n");

    if(num_consumer <= 100)//��������
    {
        max_add_cluster_server_num = MAX_ADD_CLUSTER_SERVER_NUM_PRI;
    }
    if((num_consumer > 100) && (num_consumer <= 300))//�м�����
    {
        max_add_cluster_server_num = MAX_ADD_CLUSTER_SERVER_NUM_MID;
    }
    if(num_consumer > 300)//�߼�����
    {
        max_add_cluster_server_num = MAX_ADD_CLUSTER_SERVER_NUM_HARD;
    }
    
    //��ʼ��
    for(m = 0; m < num_net_dot; m++)
    {
        for(n = 0; n < num_net_dot; n++)
        {
            if(m != n)
            {
                link_array[m][n].link_bandwidth = 0;//��ʼ��������·����
                link_array[m][n].link_bandwidth_history= link_array[m][n].link_bandwidth;
                link_array[m][n].cost_per_bandwidth = INF;
                link_array[m][n].cost_per_bandwidth_history = link_array[m][n].cost_per_bandwidth;
                p2p_path_info[m][n].distance = INF;//��ʼ���㵽������·����Ϣ
                p2p_path_info[m][n].bandwidth = 0;
            }
            else
            {
                link_array[m][n].link_bandwidth = INF;//��ʼ��������·����
                link_array[m][n].link_bandwidth_history= link_array[m][n].link_bandwidth;
                link_array[m][n].cost_per_bandwidth = 0;
                link_array[m][n].cost_per_bandwidth_history = link_array[m][n].cost_per_bandwidth;
                p2p_path_info[m][n].distance = 0;//��ʼ���㵽������·����Ϣ
                p2p_path_info[m][n].bandwidth = INF;
            }
            p2p_path_info[m][n].link_num = 0;
            p2p_path_info[m][n].p_path = (STNode *)malloc(sizeof(STNode));//Ϊͷ����ȷ���һ�οռ�
            p2p_path_info[m][n].p_path->id_node = m;
            p2p_path_info[m][n].p_path->pnext = NULL;
            p2p_path_info[m][n].p_path_end = p2p_path_info[m][n].p_path;//β�ڵ㼴Ϊ·�������һ�㣬��ʼʱ��ΪԴ�ڵ�
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

    //�����ڽڵ�ĵ㵽��·����Ϣ����p2p_path_info
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
��������:ȫ���������Ż�������
�������:��������֮ǰ�����������ڽ���һ���Ż�
��ν���:net_node_info(��������ڵ���Ϣ)link_array(������·����)result_info(��������·��Ϣ)
����ֵ:��
��������:������
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
    STNode *p_begin = NULL;//���ͷ������·��ͷ���
    STNode *p = NULL;//�ݴ�ָ��
    STNode *p_end = NULL;//���ͷ������·��β�ڵ�
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
}


/************************************************************************
��������:debug����
�������:��������Ч���жϣ�Ȩ�ؼ��㺯��
��ν���:link_array(������·����),consumer_link_info(���ѽڵ�������Ϣ)result_info(��������·��Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void result_check_debug(STLink **link_array, STConsumerLinkInfo *consumer_link_info, STResultInfo *result_info)
{
    int i = 0;
    int j = 0;
    int sum = 0;//�����Ȩ��
    int sum_server = 0;
    int sum_waste_flow = 0;//���㵥����·˫������ʱ���ص���������
    int id_net_A = 0;
    int id_net_B = 0;
    int id_consumer = 0;
    int is_correct = 1;// 1:correct 0:error
    int link_cost = 0;
    STNode *p = NULL;
    STLink **result_array = NULL;
    int *consumer_flow = NULL;
    char *server_flag = NULL;
    //����2����ά������ڴ�ռ䣬���ڴ��������·�����ݺ͵㵽��֮������·����Ϣ
    result_array= (STLink **)malloc(num_net_dot * sizeof(STLink*));
     for(i = 0; i < num_net_dot; i++)
    {
        result_array[i] = (STLink *)malloc(num_net_dot * sizeof(STLink));
    }
    //����һ������ռ����ڴ�Ÿ��������߽ڵ��ȡ����������С:
    consumer_flow = (int *)malloc(num_consumer *sizeof(int));
    //����һ������ռ����ڱ�Ǹ�������ڵ��Ƿ��������:
    server_flag = (char *)malloc(num_net_dot *sizeof(char));
    
    //��ʼ��:
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
    
    //��¼�������еķ���������������·���Ĵ�����������������ڵ��ȡ�������
    for(i = 0; i < num_result_link; i++)
    {
        p = result_info[i].p_path;
        server_flag[p->id_node] += result_info[i].cost_bandwidth;//��Ƿ�����,˳����¼�¸÷������������������
        for(j = 0; j < result_info[i].link_num; j++)
        {
            id_net_A = p->id_node;
            id_net_B = p->pnext->id_node;
            result_array[id_net_A][id_net_B].link_bandwidth += result_info[i].cost_bandwidth;//��¼����������
            p=p->pnext;
        }
        id_consumer = result_info[i].id_consumer;
        consumer_flow[id_consumer] += result_info[i].cost_bandwidth;//��¼�����߽ڵ����Ӧ��
    }

    //�������������ɱ�:
    for(i = 0; i < num_net_dot; i++)
    {
        if(server_flag[i] > 0)
        {
           sum_server++; 
        }
    }
    sum = sum + (sum_server * cost_server);
    //����ȶ�:
    for(i = 0; i < num_net_dot; i++)
    {
        for(j = 0; j < num_net_dot; j++)
        {
            if((result_array[i][j].link_bandwidth > 0) && (result_array[j][i].link_bandwidth > 0))//�жϵ�����·���Ƿ����˫������
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
            //��·������ԭ�д���
            if(result_array[i][j].link_bandwidth > result_array[i][j].link_bandwidth_history)
            {
                printf("[RESULT ERROR] result_array[%d][%d].link_bandwidth_history = %d but result_array[%d][%d].link_bandwidth = %d\n",i,j,result_array[i][j].link_bandwidth_history,i,j,result_array[i][j].link_bandwidth);
                is_correct = 0;
            }
            //������·cost:
            sum = sum + (result_array[i][j].link_bandwidth * result_array[i][j].cost_per_bandwidth);
        }
    }
    //�����߽ڵ�����û�й���
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
        if(sum_waste_flow > 0)//���������˷���������ʾ��Ϣ
        {
            printf("[RESULT INFO] there are some flow waste, waste cost = %d, if we can solve, the cost will below the [%d]\n",sum_waste_flow,(sum - sum_waste_flow));
        }
        else
        {
            printf("[RESULT CORRECT] success!\n");
        }
    }

    //�ͷ��ڴ�ռ�
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
��������:�㷨���������
�������:���ô������ӿڣ��õ�������
��ν���:link_info(��·��Ϣ),consumer_link_info(���ѽڵ�������Ϣ)result_info(��������·��Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void process_main(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info, STResultInfo *result_info)
{
    int i = 0; 
    STLink **link_array = NULL;//���ڴ��������·������
    STP2PPathInfo **p2p_path_info = NULL;//���ڴ�ŵ㵽��֮�����С·���ĳ��ȣ��Լ����·��ͷ����ָ��
    STNetNodeInfo *net_node_info = NULL;//���ڴ������ڵ����Ϣ

    net_node_info = (STNetNodeInfo *)malloc(num_net_dot * sizeof(STNetNodeInfo));
    //����2����ά������ڴ�ռ䣬���ڴ��������·�����ݺ͵㵽��֮������·����Ϣ
    link_array= (STLink **)malloc(num_net_dot * sizeof(STLink*));
    p2p_path_info = (STP2PPathInfo **)malloc(num_net_dot * sizeof(STP2PPathInfo *));
    for(i = 0; i < num_net_dot; i++)
    {
        link_array[i] = (STLink *)malloc(num_net_dot * sizeof(STLink));
        p2p_path_info[i] = (STP2PPathInfo *)malloc(num_net_dot * sizeof(STP2PPathInfo));
    }
   
    //Ԥ����������Ϣ
    process_pre(link_info, link_array, p2p_path_info, net_node_info, result_info);

    //���þ��ദ����
    cluster_main(p2p_path_info, link_array, consumer_link_info, net_node_info, result_info);
    //ȫ���������Ż�����
    result_optimize(net_node_info, link_array, result_info);
    //���������:
    result_check_debug( link_array, consumer_link_info, result_info);
    
    //�ͷ��ڴ�ռ�
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
��������:����ɹ��������
�������:��Ϊ�ٷ��ṩ�Ĺ�����ڣ���main������ֱ�ӵ���
��ν���:topo(�ٷ��ӿڶ�ȡ�����ļ������ַ���������ʽ����),line_num(�����ļ��ܺ��������������һ����)
filename(����ļ�·������)
����ֵ:��
��������:�ٷ��ṩ�Ľӿ�
�޸ĵ�:���ӵ��ý�����Σ������㷨�Ľӿ�
�޸���Ա:������
*************************************************************************/
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    //int link_sum = 0;//�ܵ������·��(������)
    STLinkInfo *link_info = NULL;
    STConsumerLinkInfo *consumer_link_info = NULL;
    STResultDebugInfo *result_debug_info = NULL;
    STResultInfo *result_info = NULL;
    char * topo_file = NULL;// ��Ҫ���������
   
    //��������������Ϣ
    input_analyse_base(topo);

    //������·ֵ�������߽ڵ���������̬���伸���ڴ�ռ䣬���ڴ洢������Ϣ:
    link_info = (STLinkInfo *)malloc(num_link * sizeof(STLinkInfo));
    consumer_link_info = (STConsumerLinkInfo *)malloc(num_consumer * sizeof(STConsumerLinkInfo));
    result_debug_info = (STResultDebugInfo *)malloc(num_consumer * sizeof(STResultDebugInfo));
    result_info = (STResultInfo *)malloc(MAX_LINK_NUM * sizeof(STResultInfo));
    topo_file = (char *)malloc(1000*5000*5*sizeof(char));//���Ǽ������������25M�ڴ�ռ����ڴ��������

    //��һ������������Ϣ����ȡ����������Ϣ
    input_analyse_other(link_info, consumer_link_info, topo);

    //�㷨���������
    process_main(link_info, consumer_link_info, result_info);

    //���Դ�����
    //link_sum = process_debug(consumer_link_info, result_debug_info);
    //�����������׼������
    //result_standard_debug(link_sum, result_debug_info, topo_file);
    
    //�����׼������
    result_standard(result_info, topo_file);
    
    // ֱ�ӵ�������ļ��ķ��������ָ���ļ���(ps��ע���ʽ����ȷ�ԣ�����н⣬��һ��ֻ��һ�����ݣ��ڶ���Ϊ�գ������п�ʼ���Ǿ�������ݣ�����֮����һ���ո�ָ���)
    write_result(topo_file, filename);

    //�ͷ��ڴ�ռ�
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

