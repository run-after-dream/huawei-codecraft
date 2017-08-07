/***********************************************************************
�ļ�����:dijkstra.cpp
�ļ�����:dijkstra��Դ���·���㷨����ģ��,��������
�ļ�����ʱ��:20170409
�ļ�����:������
***********************************************************************/

#include "deploy.h"
#include "dijkstra.h"
#include "mybase.h"
#include <stdio.h>
#include <stdlib.h>

/************************************************************************
��������:dijkstra�Ż�������(�����ڽӱ�):�����ڽӱ��ȼ�ʵ����·����
�������:�������µ�link_array�����ڽӱ��ȼ�ʵ����·����
��ν���:link_array(������·����)
����ֵ:�����ڽӱ���
��������:������
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
��������:dijkstra�Ż�������(�����ڽӱ�):�����ڽӱ�
�������:�����ڽӱ�
��ν���:link_array(������·����)adj_link_info(�ڽ���������)
first(���ڴ���ڽ������������ĵ�һ����id���±�Ϊ����ı��)next(���ڴ���ڽ�������ߵ���һ����:�˴�����һ������ָ��ͬһ��������ı�)
����ֵ:��
��������:������
*************************************************************************/
void dijkstra_adjacency_list_create(STLink **link_array, STLinkInfo *adj_link_info, int *first, int *next)
{
    int i = 0;
    int j = 0;
    int k = 0;

    //��ʼ��
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
                next[k] = first[i];//next�д����һ�δ���ö���ı�ţ���¼�µ�ǰ�ߵ���һ����
                first[i] = k;//first�и��ݶ������ö��㷢����������
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
��������:dijkstra�Ż�������:������С��(��������ĸ�ڵ��С����Ҷ�ӽڵ�)
�������:�����ڽڵ������С�ѣ��Զѵ���ʽ��������
��ν���:cluster_id(������)id_source(Դ�ڵ�id)stack_node(��С������)p2p_path_info(�㵽������·����Ϣ)
net_node_info(��������ڵ���Ϣ)
����ֵ:stack_length(��С�ѳ���)
��������:������
*************************************************************************/
int dijkstra_stack_create(int cluster_id, int id_source, STStackNodeInfo *stack_node, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int j = 0;
    int k = 0;
    STStackNodeInfo stack_value = {0};//��ʱ����
    int id_net_node = 0;//�������±�
    int stack_length = 0;//�ѵĳ���
    for(i = 0; i < num_net_dot; i++)//��������������ڵ�������
    {
        if((net_node_info[i].cluster_id == cluster_id) && (i != id_source))//�����ڷ�Դ�����������ڵ��������
        {
            stack_length++;
            id_net_node = stack_length;
            stack_node[id_net_node].id_node = i;
            stack_node[id_net_node].value = p2p_path_info[id_source][i].distance;
            if(id_net_node > 1)
            {
                j = id_net_node/2;
                do{
                    if(stack_node[id_net_node].value < stack_node[j].value)//���¼����ֵС����ĸ�ڵ��ֵ�������ڵ㽻��
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
��������:dijkstra�Ż�������:��С�ѵ�������
�������:�ӶѶ���Сֵȡ�����������Ż���·��ֵ���Ѷ������µ�����С��
��ν���:id_node(���ڱ仯λ)stack_length(�ѳ���)stack_node(��С������)net_node_info(��������ڵ���Ϣ)
����ֵ:ret(�����Ƿ����������־λ1:����������0:�˳�����)(������Сֵ�Ƿ�ΪINF��������öѱ������)
��������:������
*************************************************************************/
int dijkstra_stack_optimize(int id_node, int stack_length, STStackNodeInfo *stack_node, STNetNodeInfo *net_node_info)
{
    int ret = 0;
    int i = id_node;
    int j = 0;
    int k = 0;
    int num = 0;//��������
    STStackNodeInfo stack_value = {0};//��ʱ����
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
            if((stack_node[(2*i)].value < stack_node[(2*i + 1)].value) || ((2*i + 1) > stack_length))//Ѱ������Ҷ�ӽڵ��н�С��һ��
            {
                j = (2*i);
            }
            else
            {
                j = (2*i + 1);
            }
            
            if(stack_node[j].value < stack_node[i].value)//����СҶ�ӽڵ�С��ĸ�ڵ㣬���������ڵ㻥��
            {
                stack_value.id_node = stack_node[j].id_node;
                stack_value.value = stack_node[j].value;
                stack_node[j].id_node = stack_node[i].id_node;
                stack_node[j].value = stack_node[i].value;
                stack_node[i].id_node = stack_value.id_node;
                stack_node[i].value = stack_value.value;
                /*����������ڵ��Ӧ�������нڵ��λ��*/
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
                if(stack_node[j].value > stack_node[i].value)//��ĸ�ڵ���ڸ�Ҷ�ӽڵ���ֵ�����������ڵ㻥��
                {
                    stack_value.id_node = stack_node[j].id_node;
                    stack_value.value = stack_node[j].value;
                    stack_node[j].id_node = stack_node[i].id_node;
                    stack_node[j].value = stack_node[i].value;
                    stack_node[i].id_node = stack_value.id_node;
                    stack_node[i].value = stack_value.value;
                    /*����������ڵ��Ӧ�������нڵ��λ��*/
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
                if(stack_node[j].value > stack_node[i].value)//��ĸ�ڵ���ڸ�Ҷ�ӽڵ���ֵ�����������ڵ㻥��
                {
                    stack_value.id_node = stack_node[j].id_node;
                    stack_value.value = stack_node[j].value;
                    stack_node[j].id_node = stack_node[i].id_node;
                    stack_node[j].value = stack_node[i].value;
                    stack_node[i].id_node = stack_value.id_node;
                    stack_node[i].value = stack_value.value;
                    /*����������ڵ��Ӧ�������нڵ��λ��*/
                    k = stack_node[j].id_node;
                    net_node_info[k].id_stack_node = j;
                    k = stack_node[i].id_node;
                    net_node_info[k].id_stack_node = i;
                    i = j;
                }
                else
                {
                    if((stack_node[(2*i)].value < stack_node[(2*i + 1)].value) || ((2*i + 1) > stack_length))//Ѱ������Ҷ�ӽڵ��н�С��һ��
                    {
                        j = (2*i);
                    }
                    else
                    {
                        j = (2*i + 1);
                    }
                    if(stack_node[j].value < stack_node[i].value)//����СҶ�ӽڵ�С��ĸ�ڵ㣬���������ڵ㻥��
                    {
                        stack_value.id_node = stack_node[j].id_node;
                        stack_value.value = stack_node[j].value;
                        stack_node[j].id_node = stack_node[i].id_node;
                        stack_node[j].value = stack_node[i].value;
                        stack_node[i].id_node = stack_value.id_node;
                        stack_node[i].value = stack_value.value;
                        /*����������ڵ��Ӧ�������нڵ��λ��*/
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
��������:dijkstra�Ż�������:��С�Ѽ�麯��
�������:������С�Թ��򣬼����С����Ч�ԣ�����Ч�����ӡ����С�����飬����Ч����ӡ��ERROR��Ϣ
��ν���:stack_length(�ѳ���)stack_node(��С������)
����ֵ:is_false(0:success,1:fail)
��������:������
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
��������:dijkstra������:ָ��Դ�㵽�������е����̾���
�������:�ҳ�ָ���㵽�������е����̾��룬����̾���͸�·���������ĵ����ָ�������������
��ν���:cluster_id(������)id_source(Դ�ڵ�id)stack_node(��С������)link_array(������·����)
adj_link_info(�ڽ���������)first(���ڴ���ڽ������������ĵ�һ����id���±�Ϊ����ı��)
next(���ڴ���ڽ�������ߵ���һ����:�˴�����һ������ָ��ͬһ��������ı�)p2p_path_info(�㵽������·����Ϣ)
net_node_info(��������ڵ���Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void dijkstra_process_p2mp(int cluster_id, int id_source, STStackNodeInfo *stack_node, STLink **link_array, STLinkInfo *adj_link_info, int *first, int *next, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int u = 0;
    int v = 0;
    int k = 0;
    int id_node = 0;
    int stack_length = 0;
    int is_continue = 1;//��С���Ż�����ֵ
    int is_false = 0;//��С��У�鷵��ֵ
    char *book = NULL;//��ǵ�ǰ�㵽�ƶ�Դ��ľ����Ƿ���С
    int *dis = NULL;//���Դ�㵽���������̾���
    STNode *p = NULL;
    STNode *q = NULL;
    dis = (int *)malloc(num_net_dot * sizeof(int));
    book = (char *)malloc(num_net_dot);

    //������С��:
    stack_length = dijkstra_stack_create(cluster_id, id_source, stack_node, p2p_path_info, net_node_info);
    is_false = dijkstra_stack_check(stack_length, stack_node);//����С��У��ʧ�ܣ�ֱ���˳�
    if(is_false == 1)
    {
        printf("[ERROR] dijkstra_stack_check fail\n");
        return;
    }
    for(i = 0; i < num_net_dot; i++)//��ʼ��dis����
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            dis[i] = p2p_path_info[id_source][i].distance;
        }
    }
    
    for (i = 0; i < num_net_dot; i++)//��ʼ��book����
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            book[i] = 0;
        }
        else
        {
            book[i] = 1;//���ڷǱ���ĵ㣬ֱ�ӷ��뼯��P
        }
    }
    book[id_source] = 1;//��Դ�ڵ������ȷ��뼯��P������ɻ�

    for(i = 0; i < stack_length; i++)//һ��ѭ��stack_length�ο��Ա����������нڵ�
    {
        if(is_continue == 0)
        {
            break;
        }

        //�ҵ���Դ�����������ڵ�
        u = stack_node[1].id_node;
        k = first[u];
        /*��С��ͷ����Ż�*/
        stack_node[1].value = INF;
        is_continue = dijkstra_stack_optimize(1, stack_length, stack_node, net_node_info);
        
        while(k != -1)
        {
            v = adj_link_info[k].id_link_end;
            if(net_node_info[v].cluster_id == cluster_id)//�������ڵĽڵ���в���
            {
                if(dis[v] > dis[u] + link_array[u][v].cost_per_bandwidth)
                {
                    if((dis[u] + link_array[u][v].cost_per_bandwidth) < 0)
                    {
                        printf("[ERROR] <dijkstra_process_p2mp> p2p_path_info[%d][%d].distance = %d dis[u] = %d\n",u,v,p2p_path_info[u][v].distance,dis[u]);
                        continue;
                    }
                    dis[v] = dis[u] + link_array[u][v].cost_per_bandwidth;//��ȡ����v���µ���С·��Ȩ��
                    if(dis[v] > INF)
                    {
                        dis[v] = INF;
                    }
                    /*��С���ڲ�����Ż�*/
                    id_node = net_node_info[v].id_stack_node;
                    stack_node[id_node].value = dis[v];
                    is_continue = dijkstra_stack_optimize(id_node, stack_length, stack_node, net_node_info);
                    
                    //ƴ��·������¼��·����·����Ϣ
                    //��һ��ƴ��
                    p = p2p_path_info[id_source][v].p_path;//ԭ�ȵ�·��
                    q = p2p_path_info[id_source][u].p_path;//��ƴ�ӵ���·��
                    list_release(&(p->pnext));//�ͷ�ԭ·��������ͷ���
                    p2p_path_info[id_source][v].p_path_end = list_link(q, p, p2p_path_info[id_source][u].link_num);
                    //�ڶ���ƴ��
                    p = p2p_path_info[id_source][v].p_path_end;//��һ��ƴ�Ӻ�·����β�ڵ�
                    q = p2p_path_info[u][v].p_path;//�ڶ��δ�ƴ��·��
                    p2p_path_info[id_source][v].p_path_end= list_link(q, p, p2p_path_info[u][v].link_num);
                    //����·����Ϣ����     
                    p2p_path_info[id_source][v].distance = dis[v];//�㵽��·��Ȩ��֮�͸���
                    p2p_path_info[id_source][v].link_num = p2p_path_info[id_source][u].link_num + p2p_path_info[u][v].link_num;
                    if(p2p_path_info[id_source][u].bandwidth < p2p_path_info[u][v].bandwidth)//����·���е���Сֵ��Ϊ�ºϳ�·���Ĵ���
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
��������:dijkstra������:ȫͼ����
�������:ȫͼ�����������ȫͼ��������ڵ�֮������·��
��ν���:cluster_id(������)stack_node(��С������)link_array(������·����)adj_link_info(�ڽ���������)
first(���ڴ���ڽ������������ĵ�һ����id���±�Ϊ����ı��)next(���ڴ���ڽ�������ߵ���һ����:�˴�����һ������ָ��ͬһ��������ı�)
p2p_path_info(�㵽������·����Ϣ)net_node_info(��������ڵ���Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void dijkstra_process_all_distance(int cluster_id, STStackNodeInfo *stack_node, STLink **link_array, STLinkInfo *adj_link_info, int *first, int *next, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    //�������������ڵ�����֮������·��
    for(i = 0; i < num_net_dot; i++)
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            dijkstra_process_p2mp(cluster_id, i, stack_node, link_array, adj_link_info, first, next, p2p_path_info, net_node_info);
        }
    }
}

/************************************************************************
��������:dijkstra������:Ѱ�Ҿ�����Զ�������߽��
�������:�ҳ����������߽ڵ�֮�����Զ·��������Ϊ��һ���ȼ�������һ������������ԽС��Ϊ���ԽԶ
��ν���:cluster_id(������)p2p_path_info(�㵽������·����Ϣ)consumer_link_info(���ѽڵ�������Ϣ)
id_cluster_A(��Զ����·����Դ����ڵ�)id_cluster_B(��Զ����·����Ŀ������ڵ�)
����ֵ:��
��������:������
*************************************************************************/
void dijkstra_process_consumer_max(int cluster_id, STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, int *id_cluster_A, int *id_cluster_B)
{
    int i = 0;
    int j = 0;
    int distance_max = 0;//��¼·��distance���ֵ
    int bandwidth_min = 0;//��¼ͬ��·����bandwidth��Сֵ
    //int id_consumer_source = 0;
    //int id_consumer_end = 0;
    int id_source = 0;
    int id_end = 0;
    
    //�ҳ�ȫͼ������Զ�����������߽�㣬������һ��Զʱ��Ϊ��·����ԽС�������ԽԶ
    distance_max = 0;
    bandwidth_min = INF;
    for(i = 0;i < num_consumer; i++)
    {
        for(j = 0; j < num_consumer; j++)
        {
            //�������������߽ڵ���м���
            if((i != j) && (consumer_link_info[i].cluster_id == cluster_id) && (consumer_link_info[j].cluster_id == cluster_id))
            {
                id_source = consumer_link_info[i].id_net_dot;
                id_end = consumer_link_info[j].id_net_dot;
                if(p2p_path_info[id_source][id_end].distance > distance_max)//ȡ������Զ��·��
                {
                    bandwidth_min = p2p_path_info[id_source][id_end].bandwidth;
                    distance_max = p2p_path_info[id_source][id_end].distance;
                    *id_cluster_A = id_source;
                    *id_cluster_B = id_end;
                    //id_consumer_source = i;
                    //id_consumer_end = j;
                }
                if(p2p_path_info[id_source][id_end].distance == distance_max)//�������ʱ��ȡ�����С��·��
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
    //��ӡ�����������߽ڵ�����Զ·��
    //dijkstra_debug_consumer_p2p(cluster_id, p2p_path_info, consumer_link_info, id_consumer_source, id_consumer_end);
}


/************************************************************************
��������:dijkstra�����������
�������:Ԥ����dijkstra�㷨����������dijkstra������
��ν���:cluster_id(������)link_array(������·����)p2p_path_info(�㵽������·����Ϣ)net_node_info(��������ڵ���Ϣ)
consumer_link_info(���ѽڵ�������Ϣ)
����ֵ:��
��������:������
*************************************************************************/

void dijkstra_process_main(int cluster_id, STLink **link_array, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info)
{ 
    printf("[INFO] <dijkstra_process_main> cluster_id = %d\n",cluster_id);
    //�����ڽӱ�ı���
    STLinkInfo *adj_link_info = NULL;
    int *first = NULL;
    int *next = NULL;
    //������С�ѵı���
    STStackNodeInfo *stack_node = NULL;
    stack_node = (STStackNodeInfo *)malloc((num_net_dot+1) * sizeof(STStackNodeInfo));//�ѽṹ�������±��1��ʼ���˴��Կռ任ʱ�䣬������ڴ�ռ俼��ȫ��������

    num_link_actual = dijkstra_adjacency_list_calculate(link_array);//�����ڽӱ���
    adj_link_info = (STLinkInfo *)malloc(num_link_actual * sizeof(STLinkInfo)); 
    first = (int *)malloc(num_link_actual * sizeof(int));
    next = (int *)malloc(num_link_actual * sizeof(int));

    //�����ڽ�����
    dijkstra_adjacency_list_create(link_array, adj_link_info, first, next);
    
    //����ȫͼ����
    dijkstra_process_all_distance(cluster_id, stack_node, link_array, adj_link_info, first, next, p2p_path_info, net_node_info);
    //��ӡ�����������߽ڵ�������ľ���
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
��������:dijkstra���Ժ���:��ӡȫͼ���������߽ڵ�����֮������·��
�������:��ȫͼ��������ȡ���������߽ڵ㵽����ڵ�����·�����ٴ�ӡȫͼ����
�����߽ڵ�����֮������·��
��ν���:cluster_id(������)p2p_path_info(�㵽������·����Ϣ)consumer_link_info(���ѽڵ�������Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void dijkstra_debug_consumer_all(int cluster_id, STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info)
{
    int i = 0;
    int j = 0;
    int m = 0;
    int id_source = 0;
    int id_end = 0;
    STNode *p = NULL;
    
    //��ӡ�����������߽ڵ�����֮������·��:
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
��������:dijkstra���Ժ���:��ӡ��ָ������������ڵ�֮������·��
�������:��ӡ����������ڵ�֮���·����Ϣ
��ν���:cluster_id(������)net_node_info(��������ڵ���Ϣ)p2p_path_info(�㵽������·����Ϣ)
id_source(Դ�ڵ�)id_end(Ŀ�Ľڵ�)
����ֵ:��
��������:������
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
��������:dijkstra���Ժ���:��ӡ��ָ�������������߽ڵ�֮������·��
�������:��ӡ��ָ�������������߽ڵ�֮�����Զ����
��ν���:cluster_id(������)p2p_path_info(�㵽������·����Ϣ)consumer_link_info(���ѽڵ�������Ϣ)
id_consumer_source(��Զ����·����Դ���ѽڵ�)id_consumer_end(��Զ����·����Ŀ�����ѽڵ�)
����ֵ:��
��������:������
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


