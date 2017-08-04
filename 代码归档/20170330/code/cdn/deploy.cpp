#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int num_net_dot;//����ڵ�����
int num_link;//������·����
int num_consumer;//���ѽڵ�����
int cost_server;//��Ƶ���ݷ���������ɱ�

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
��������:�������������ļ�(��������)
�������:��ȡ�����ļ���һ��������е���Ϣ��Ϊ������ȡ��·��Ϣ�����ѽڵ���Ϣ�ṩ����
��ν���:topo(�ٷ��ӿڶ�ȡ�����ļ������ַ���������ʽ����)
����ֵ:��
��������:������
*************************************************************************/
void input_analyse_base(char * topo[MAX_EDGE_NUM])
{
    int i = 0;
    int j = 0;
    int begin = 0;
    int end = 0;
    char *str = NULL;

    //���������ļ���һ����Ϣ
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

    //���������ļ���������Ϣ
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
��������:��������������Ϣ(��·��Ϣ�����ѽڵ�������Ϣ)
�������:��ȡ�����������ļ��е����п�ʼ����·��Ϣ�����ѽڵ�������Ϣ�����뵽��Ӧ���ڴ�ռ���
��ν���:link_info(��·��Ϣ),consumer_link_info(���ѽڵ�������Ϣ),topo(�ٷ��ӿڶ�ȡ�����ļ������ַ���������ʽ����)
����ֵ:��
��������:������
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

    //������·��·��Ϣ
    for(i = 0; i < num_link; i++)
    {
        k = i + 4;//�ӵ�5�п�ʼ
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

    //���������߽ڵ���Ϣ
    for(i = 0; i < num_consumer; i++)
    {
        consumer_link_info[i].cluster_id = 1;
        k = i + 5 + num_link;//�ӵ�6 + num_link�п�ʼ
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
��������:�������ܵ��Ժ���(������)
�������:�����ú����������з��������������ѽڵ��϶�
��ν���:consumer_link_info(���ѽڵ�������Ϣ),result_info(�������·����Ϣ)
����ֵ:����·����
��������:������
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
��������:�����Ϣ��׼������(������)
�������:��int���͵������Ϣת��Ϊ�ٷ�Ҫ����ַ�������
��ν���:link_sum(�ܵ�����·������),result_info(�������·����Ϣ),topo_file(�ٷ���Ҫ������ַ���)
����ֵ:��
��������:������
*************************************************************************/
void result_standard_debug(int link_sum, STResultInfo *result_info, char *topo_file)
{
    int i = 0;
    char str[7] = {0};//��Ŀ�涨������ֵ���Ϊ100000�����Ͽ��ַ����7���ַ�
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

/************************************************************************
��������:������ϢԤ����ģ��
�������:Ԥ����������Ϣ,��link_info�е����ݴ���link_array��,��ʼ���㵽������·����Ϣ��
������ڵ���Ϣ��ʼ��
��ν���:link_info(��·��Ϣ)link_array(������·����)p2p_path_info(��ŵ㵽������·����Ϣ)
net_node_info(����ڵ���Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void process_pre(STLinkInfo *link_info, STLink **link_array, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int m = 0;
    int n = 0;

    //��ʼ��
    for(m = 0; m < num_net_dot; m++)
    {
        for(n = 0; n < num_net_dot; n++)
        {
            if(m == n)
            {
                link_array[m][n].link_bandwidth = 0;//�Լ����Լ��ľ���ʼ��Ϊ0
                link_array[m][n].cost_per_bandwidth = 0;
                p2p_path_info[m][n].distance = 0;//��ʼ���㵽������·����Ϣ
                p2p_path_info[m][n].link_num = 0;
                p2p_path_info[m][n].bandwidth = 0;
            }
            else
            {
                link_array[m][n].link_bandwidth = INF;//��ʼ��������·����
                link_array[m][n].cost_per_bandwidth = INF;
                p2p_path_info[m][n].distance = INF;//��ʼ���㵽������·����Ϣ
                p2p_path_info[m][n].link_num = 0;
                p2p_path_info[m][n].bandwidth = INF;
            }
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
        link_array[m][n].cost_per_bandwidth = link_info[i].cost_per_bandwidth;
        link_array[n][m].link_bandwidth = link_info[i].link_bandwidth;
        link_array[n][m].cost_per_bandwidth = link_info[i].cost_per_bandwidth;
    }

    //�����ڽڵ�ĵ㵽��·����Ϣ����p2p_path_info
    for(m = 0; m < num_net_dot; m++)
    {
        net_node_info[m].out_flow = 0;
        net_node_info[m].valid= 1;
        net_node_info[m].cluster_id = 1;
        for(n = 0; n < num_net_dot; n++)
        {
            if((link_array[m][n].link_bandwidth < INF) && (m != n))
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
��������:dijkstra������:ָ��Դ�㵽�������е����̾���
�������:�ҳ�ָ���㵽�������е����̾��룬����̾���͸�·���������ĵ����ָ�������������
��ν���:cluster_id(������)id_source(Դ�ڵ�id)p2p_path_info(�㵽������·����Ϣ)net_node_info(��������ڵ���Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void dijkstra_process_p2mp(int cluster_id, int id_source, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int j = 0;
    int u = 0;
    int v = 0;
    int min = INF;//�����̾���
    char *book = NULL;//��ǵ�ǰ�㵽�ƶ�Դ��ľ����Ƿ���С
    int *dis = NULL;//���Դ�㵽���������̾���
    STNode *p = NULL;
    STNode *q = NULL;
    dis = (int *)malloc(num_net_dot * sizeof(int));
    book = (char *)malloc(num_net_dot);

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

    for(i = 0; i < (num_net_dot - 1); i++)//һ��ѭ��(num_net_dot - 1)�ο����ҳ����
    {
        //�ҵ���Դ�����������ڵ�
        min = INF;
        for(j = 0; j < num_net_dot; j++)
        {
            if((book[j] == 0) && (dis[j] < min) && (dis[j] > 0))
            {
                min = dis[j];
                u = j;
            }
        }
        book[u] = 1;//���ýڵ���뼯��P
        
        for(v = 0; v < num_net_dot; v++)
        {
            if(net_node_info[v].cluster_id == cluster_id)//�������ڵĽڵ���в���
            {
                if((p2p_path_info[u][v].distance < INF) && (p2p_path_info[u][v].distance > 0))
                {
                    if(dis[v] > dis[u] + p2p_path_info[u][v].distance)
                    {
                        dis[v] = dis[u] + p2p_path_info[u][v].distance;//��ȡ����v���µ���С·��Ȩ��
                        
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
            }
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
��ν���:cluster_id(������)p2p_path_info(�㵽������·����Ϣ)net_node_info(��������ڵ���Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void dijkstra_process_all_distance(int cluster_id, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    //�������������ڵ�����֮������·��
    for(i = 0; i < num_net_dot; i++)
    {
        if(net_node_info[i].cluster_id == cluster_id)
        {
            dijkstra_process_p2mp(cluster_id, i, p2p_path_info, net_node_info);
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
    int id_consumer_source = 0;
    int id_consumer_end = 0;
    int id_source = consumer_link_info[0].id_net_dot;
    int id_end = consumer_link_info[1].id_net_dot;
    
    //�ҳ�ȫͼ������Զ�����������߽�㣬������һ��Զʱ��Ϊ��·����ԽС�������ԽԶ
    distance_max = p2p_path_info[id_source][id_end].distance;
    bandwidth_min = p2p_path_info[id_source][id_end].bandwidth;
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
                    id_consumer_source = i;
                    id_consumer_end = j;
                }
                if(p2p_path_info[id_source][id_end].distance == distance_max)//�������ʱ��ȡ�����С��·��
                {
                    if(p2p_path_info[id_source][id_end].bandwidth < bandwidth_min)
                    {
                        distance_max = p2p_path_info[id_source][id_end].distance;
                        *id_cluster_A = id_source;
                        *id_cluster_B = id_end;
                        id_consumer_source = i;
                        id_consumer_end = j;
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
��ν���:cluster_id(������)p2p_path_info(�㵽������·����Ϣ)net_node_info(��������ڵ���Ϣ)
consumer_link_info(���ѽڵ�������Ϣ)
����ֵ:��
��������:������
*************************************************************************/

void dijkstra_process_main(int cluster_id, STP2PPathInfo **p2p_path_info, STNetNodeInfo *net_node_info, STConsumerLinkInfo *consumer_link_info)
{ 

    //����ȫͼ����
    dijkstra_process_all_distance(cluster_id, p2p_path_info, net_node_info);
    //��ӡ�����������߽ڵ�������ľ���
    //dijkstra_debug_consumer_all(cluster_id, p2p_path_info, consumer_link_info);

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
    if((p2p_path_info[id_net_source][id_net_end].distance < INF) && (p2p_path_info[id_net_source][id_net_end].distance > 0))
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
        cluster_info[i].id_cluster = 0;
        cluster_info[i].need_service_num = 0;
        cluster_info[i].valid = 0;
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
    int need_server_min = 0;
    int is_need_rebuild = 0;

    need_server_min = calculate_server_minimum(cluster_id, link_array, consumer_link_info, net_node_info);
    cluster_info[cluster_id].id_cluster = cluster_id;
    cluster_info[cluster_id].need_service_num = need_server_min;
    cluster_info[cluster_id].valid = 1;
    for(i = 0; i < num_net_dot; i++)
    {
        for(j = 0; j < num_net_dot; j++)
        {
            if((p2p_path_info[i][j].distance < INF) && (p2p_path_info[i][j].distance > 0))
            {
                if((net_node_info[i].cluster_id == cluster_id) && (net_node_info[j].cluster_id == cluster_id))
                {
                    is_need_rebuild = cluster_p2ppath_is_need_rebuild(cluster_id, p2p_path_info, net_node_info, link_array, i, j);
                    if(is_need_rebuild == 1)
                    {
                        if((link_array[i][j].link_bandwidth < INF) && (link_array[i][j].link_bandwidth > 0))
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
                            p2p_path_info[i][j].distance = INF;
                            p2p_path_info[i][j].link_num = 0;
                            p2p_path_info[i][j].bandwidth = INF;
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
    dijkstra_process_main(cluster_id, p2p_path_info, net_node_info, consumer_link_info);
    //�ҳ������߽��֮�����Զ����
    dijkstra_process_consumer_max(cluster_id, p2p_path_info, consumer_link_info, &id_cluster_A, &id_cluster_B);
    //���÷��Ѻ��������ݾ��뽫��ǰ���Ϊ����
    cluster_split(cluster_id, id_cluster_A, id_cluster_B, p2p_path_info, net_node_info, consumer_link_info);
    cluster_info[cluster_id].valid = 0;//���������ĸ����ʱ��Ϊ��Ч
    //���þ����ʼ����������ʼ�����Ѻ��µľ����е�p2p_path_info��Ϣ
    cluster_pre((2*cluster_id),cluster_info,p2p_path_info,link_array,net_node_info,consumer_link_info);
    cluster_pre((2*cluster_id + 1),cluster_info,p2p_path_info,link_array,net_node_info,consumer_link_info);

    cluster_id_ret = 2*cluster_id + 1;
    return cluster_id_ret;
}

/************************************************************************
��������:���ദ����:�����������
�������:����ָ������ľ�������
��ν���:cluster_id(������)cluster_info(������Ϣ)p2p_path_info(�㵽������·����Ϣ)
consumer_link_info(�������ѽڵ�������Ϣ)net_node_info(��������ڵ���Ϣ)
����ֵ:id_center(�����������Ľڵ�ID)
��������:������
*************************************************************************/
int cluster_center(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info)
{
    int i = 0;
    int j = 0;
    int id_center = 0;
    int value = 0;//��¼�ڵ�����Ȩֵ(�ڵ����-�ڵ㵽���������߽ڵ�ľ���֮��)
    int max_value = -99999999;
    int id_net = 0;
    int dis_sum = 0;
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
            value = net_node_info[i].out_flow - dis_sum;
            if(value > max_value)
            {
                max_value = value;
                id_center = i;
            }
            dis_sum = 0;
        }
    }
    return id_center;
}

/************************************************************************
��������:���ദ����:���������ַ����
�������:���������ַ����
��ν���:cluster_id(������)cluster_info(������Ϣ)p2p_path_info(�㵽������·����Ϣ)
link_array(������·����)consumer_link_info(�������ѽڵ�������Ϣ)net_node_info(��������ڵ���Ϣ)
����ֵ:id_center(�����������Ľڵ�ID)
��������:������
*************************************************************************/
void cluster_flow_out_main(int cluster_id, STClusterInfo *cluster_info, STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info)
{
    int id_service_net = 0;
    //����dijkstra�����������ڱ������·��
    dijkstra_process_main(cluster_id, p2p_path_info, net_node_info, consumer_link_info);
    id_service_net = cluster_center(cluster_id, cluster_info, p2p_path_info, consumer_link_info, net_node_info);
    cluster_debug_net_consumer(cluster_id, net_node_info, consumer_link_info);
    printf("id_service_net = %d\n",id_service_net);
}

/************************************************************************
��������:�����㷨�����
�������:���ø������㷨�ӿ�
��ν���:p2p_path_info(�㵽������·����Ϣ)link_array(������·����)consumer_link_info(�������ѽڵ�������Ϣ)
net_node_info(��������ڵ���Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void cluster_main(STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info)
{
    int i = 1;
    int cluster_id = 1;//��ʼ����IDΪ1
    int cluster_id_max = 1;
    int cluster_id_ret = 0;
    STClusterInfo *cluster_info = NULL;
    cluster_info = (STClusterInfo *)malloc((num_consumer + 1) * sizeof(STClusterInfo));//���������಻�ᳬ�������߽ڵ������������Ϣ�����1��ʼ
    //Ԥ��ʼ��������Ϣ����,ͬʱ��ʼ����ʼ����
    cluster_info_pre(cluster_info);
    cluster_pre(cluster_id, cluster_info, p2p_path_info, link_array, net_node_info, consumer_link_info);

    for(i = 1; i <= cluster_id_max; i++)
    {
        if(cluster_info[i].valid == 1)
        {
            if(cluster_info[i].need_service_num > 2)
            {
                cluster_id_ret = cluster_process(i, cluster_info, p2p_path_info, link_array, consumer_link_info, net_node_info);
                if(cluster_id_ret > cluster_id_max)
                {
                    cluster_id_max = cluster_id_ret;
                }
            }
            else
            {
                cluster_flow_out_main(i, cluster_info, p2p_path_info, link_array, consumer_link_info, net_node_info);
            }
        }
    }
    
    free(cluster_info);
    cluster_info = NULL;
}

/************************************************************************
��������:�㷨���������
�������:���ô������ӿڣ��õ�������
��ν���:link_info(��·��Ϣ),consumer_link_info(���ѽڵ�������Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void process_main(STLinkInfo *link_info, STConsumerLinkInfo *consumer_link_info)
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
    process_pre(link_info, link_array, p2p_path_info, net_node_info);

    //���þ��ദ����
    cluster_main(p2p_path_info, link_array, consumer_link_info, net_node_info);

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
    int link_sum = 0;//���ص�����·������
    STLinkInfo *link_info = NULL;
    STConsumerLinkInfo *consumer_link_info = NULL;
    STResultInfo *result_info = NULL;
    char * topo_file = NULL;// ��Ҫ���������
   
    //��������������Ϣ
    input_analyse_base(topo);

    //������·ֵ�������߽ڵ���������̬���伸���ڴ�ռ䣬���ڴ洢������Ϣ:
    link_info = (STLinkInfo *)malloc(num_link * sizeof(STLinkInfo));
    consumer_link_info = (STConsumerLinkInfo *)malloc(num_consumer * sizeof(STConsumerLinkInfo));
    result_info = (STResultInfo *)malloc(num_consumer * sizeof(STResultInfo));
    topo_file = (char *)malloc(2 * num_consumer * sizeof(STResultInfo));//����伸�ֽڣ�ȷ������

    //��һ������������Ϣ����ȡ����������Ϣ
    input_analyse_other(link_info, consumer_link_info, topo);

    //�㷨���������
    process_main(link_info, consumer_link_info);

    //���Դ�����
    link_sum = process_debug(consumer_link_info, result_info);
    //�����������׼������
    result_standard_debug(link_sum, result_info, topo_file);
    
    // ֱ�ӵ�������ļ��ķ��������ָ���ļ���(ps��ע���ʽ����ȷ�ԣ�����н⣬��һ��ֻ��һ�����ݣ��ڶ���Ϊ�գ������п�ʼ���Ǿ�������ݣ�����֮����һ���ո�ָ���)
    write_result(topo_file, filename);

    //�ͷ��ڴ�ռ�
    free(link_info);
    link_info = NULL;
    free(consumer_link_info);
    consumer_link_info = NULL;
    free(result_info);
    result_info = NULL;
    free(topo_file);
    topo_file = NULL;

}
