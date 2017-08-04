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
    char str[5] = {0};//��Ŀ�涨������ֵ���Ϊ5000�����Ͽ��ַ����5���ַ�
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
��ν���:p2p_path_info(�㵽������·����Ϣ)consumer_link_info(���ѽڵ�������Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void dijkstra_debug_consumer_all(STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info)
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
��������:dijkstra���Ժ���:��ӡ��ָ�������������߽ڵ�֮������·��
�������:��ӡ��ָ�������������߽ڵ�֮�����Զ����
��ν���:p2p_path_info(�㵽������·����Ϣ)consumer_link_info(���ѽڵ�������Ϣ)
id_consumer_source(��Զ����·����Դ���ѽڵ�)id_consumer_end(��Զ����·����Ŀ�����ѽڵ�)
����ֵ:��
��������:������
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
��������:�����ú���:��ӡ��������������ڵ�����������
�������:��ӡ��������������ڵ�����������
��ν���:net_node_info(��������ڵ���Ϣ)num_node_area(��������ڵ����)
����ֵ:��
��������:������
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
            link_array[m][n].link_bandwidth = INF;//��ʼ��������·����
            link_array[m][n].cost_per_bandwidth = INF;
            p2p_path_info[m][n].distance = INF;//��ʼ���㵽������·����Ϣ
            p2p_path_info[m][n].link_num = 0;
            p2p_path_info[m][n].bandwidth = INF;
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
��������:dijkstra������:ָ��Դ�㵽�������е����̾���
�������:�ҳ�ָ���㵽�������е����̾��룬����̾���͸�·���������ĵ����ָ�������������
��ν���:id_source(Դ�ڵ�id)p2p_path_info(�㵽������·����Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void dijkstra_process_p2mp(int id_source, STP2PPathInfo **p2p_path_info)
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
        dis[i] = p2p_path_info[id_source][i].distance;
    }
    
    for (i = 0; i < num_net_dot; i++)//��ʼ��book����
    {
        book[i] = 0;
    }
    book[id_source] = 1;//��Դ�ڵ������ȷ��뼯��P������ɻ�

    for(i = 0; i < (num_net_dot - 1); i++)//һ��ѭ��(num_net_dot - 1)�ο����ҳ����
    {
        //�ҵ���Դ�����������ڵ�
        min = INF;
        for(j = 0; j < num_net_dot; j++)
        {
            if((book[j] == 0) && (dis[j] < min))
            {
                min = dis[j];
                u = j;
            }
        }
        book[u] = 1;//���ýڵ���뼯��P
        
        for(v = 0; v < num_net_dot; v++)
        {
            if(p2p_path_info[u][v].distance < INF)
            {
                if(dis[v] > dis[u] + p2p_path_info[u][v].distance)
                {
                    dis[v] = dis[u] + p2p_path_info[u][v].distance;//��ȡ����v���µ���С·��Ȩ��
                    
                    //ƴ��·������¼��·����·����Ϣ
                    //��һ��ƴ��
                    p = p2p_path_info[id_source][v].p_path;//ԭ�ȵ�·��
                    q = p2p_path_info[id_source][u].p_path;//��ƴ�ӵ���·��
                    list_release(p->pnext);//�ͷ�ԭ·��������ͷ���
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

    free(dis);
    free(book);
    
}


/************************************************************************
��������:dijkstra������:ȫͼ����
�������:ȫͼ�����������ȫͼ��������ڵ�֮������·��
��ν���:p2p_path_info(�㵽������·����Ϣ)
����ֵ:��
��������:������
*************************************************************************/
void dijkstra_process_all_distance(STP2PPathInfo **p2p_path_info)
{
    int i = 0;
    //�������������ڵ�����֮������·��
    for(i = 0; i < num_net_dot; i++)
    {
        dijkstra_process_p2mp(i, p2p_path_info);
    }
}


/************************************************************************
��������:dijkstra������:Ѱ�Ҿ�����Զ�������߽��
�������:�ҳ�ȫͼ�����߽ڵ�֮�����Զ·��������Ϊ��һ���ȼ�������һ������������ԽС��Ϊ���ԽԶ
��ν���:p2p_path_info(�㵽������·����Ϣ)consumer_link_info(���ѽڵ�������Ϣ)
id_consumer_source(��Զ����·����Դ���ѽڵ�)id_consumer_end(��Զ����·����Ŀ�����ѽڵ�)
����ֵ:��
��������:������
*************************************************************************/
void dijkstra_process_consumer_max(STP2PPathInfo **p2p_path_info, STConsumerLinkInfo *consumer_link_info, int *id_consumer_source, int *id_consumer_end)
{
    int i = 0;
    int j = 0;
    int distance_max = 0;//��¼·��distance���ֵ
    int bandwidth_min = 0;//��¼ͬ��·����bandwidth��Сֵ
    int id_source = consumer_link_info[0].id_net_dot;
    int id_end = consumer_link_info[1].id_net_dot;
    
    //�ҳ�ȫͼ������Զ�����������߽�㣬������һ��Զʱ��Ϊ��·����ԽС�������ԽԶ
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
                if(p2p_path_info[id_source][id_end].distance > distance_max)//ȡ������Զ��·��
                {
                    bandwidth_min = p2p_path_info[id_source][id_end].bandwidth;
                    distance_max = p2p_path_info[id_source][id_end].distance;
                    *id_consumer_source = i;
                    *id_consumer_end = j;
                }
                if(p2p_path_info[id_source][id_end].distance == distance_max)//�������ʱ��ȡ�����С��·��
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
    //��ӡ�������߽ڵ�����Զ·��
    dijkstra_debug_consumer_p2p(p2p_path_info, consumer_link_info, id_consumer_source, id_consumer_end);
}

/************************************************************************
��������:dijkstra�����������
�������:Ԥ����dijkstra�㷨����������dijkstra������
��ν���:p2p_path_info(�㵽������·����Ϣ)link_array(������·����)consumer_link_info(���ѽڵ�������Ϣ)
����ֵ:��
��������:������
*************************************************************************/

void dijkstra_process_main(STP2PPathInfo **p2p_path_info, STLink **link_array, STConsumerLinkInfo *consumer_link_info)
{ 
    int id_consumer_source = 0;
    int id_consumer_end = 0;
    //ȫͼ����
    dijkstra_process_all_distance(p2p_path_info);
    //��ӡ�����������߽ڵ�������ľ���
    dijkstra_debug_consumer_all(p2p_path_info, consumer_link_info);
    //�ҳ������߽��֮�����Զ����
    dijkstra_process_consumer_max(p2p_path_info, consumer_link_info, &id_consumer_source, &id_consumer_end);
    
}

/************************************************************************
��������:���㺯��:����������������߽ڵ�������
�������:����������������߽ڵ�������
��ν���:consumer_link_info(���ѽڵ�������Ϣ)num_consumer_area(���������߽ڵ����)
����ֵ:�������������߽��������
��������:������
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
��������:���㺯��:������ڸ�����ڵ����������
�������:������ڸ�����ڵ����������,
��ν���:link_array(������·����)consumer_link_info(�������ѽڵ�������Ϣ)num_consumer_area(���������߽ڵ����)
net_node_info(��������ڵ���Ϣ)num_node_area(��������ڵ����)
����ֵ:��
��������:������
*************************************************************************/
void calculate_node_outflow(STLink **link_array, STConsumerLinkInfo *consumer_link_info, int num_consumer_area, STNetNodeInfo *net_node_info, int num_node_area)
{
    int i = 0;
    int j = 0;
    int m = 0;
    int n = 0;
    int id = 0;
    //�����ҳ����ѽڵ����ڵ�����ڵ㣬�������ѽڵ�����Ĵ���
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
��������:���㺯��:��������貿����Ƶ����������Сֵ
�������:��������貿����Ƶ����������Сֵ
��ν���:link_array(������·����)consumer_link_info(�������ѽڵ�������Ϣ)num_consumer_area(���������߽ڵ����)
net_node_info(��������ڵ���Ϣ)num_node_area(��������ڵ����)
����ֵ:���������������ٷ�����̨��
��������:������
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
    //�����ڷ���������Ӧ����������С���������ɴ�С�ۼӣ��ҵ����������������߽�����С��������
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
    //�ָ�����ڵ���ñ�־λ
    for(i = 0; i < num_node_area; i++)
    {
        net_node_info[i].valid = 1;
    }
    //��ӡ��������Ҫ��������С��������Ҫ�ķ���������
    printf("In area,sum_consumer_need = %d num_server_min = %d\n",sum_consumer_need, num_server);
    return num_server;
}

/************************************************************************
��������:���ദ����:��ʼ������
�������:ȷ��ȫͼ��Ҫ��������ٷ�����̨��
��ν���:link_array(������·����)consumer_link_info(�������ѽڵ�������Ϣ)num_consumer_area(���������߽ڵ����)
net_node_info(��������ڵ���Ϣ)num_node_area(��������ڵ����)
����ֵ:��
��������:������
*************************************************************************/
void cluster_pre(STLink **link_array, STConsumerLinkInfo *consumer_link_info, int num_consumer_area, STNetNodeInfo *net_node_info, int num_node_area)
{
    int need_server_min = 0;
    need_server_min = calculate_server_minimum(link_array, consumer_link_info, num_consumer_area, net_node_info, num_node_area);
    //��ӡ��������������ڵ�Ķ�������
    calculate_debug_node_outflow(net_node_info, num_node_area);
}

/************************************************************************
��������:�����㷨�����
�������:���ø������㷨�ӿ�
��ν���:link_array(������·����)consumer_link_info(�������ѽڵ�������Ϣ)num_consumer_area(���������߽ڵ����)
net_node_info(��������ڵ���Ϣ)num_node_area(��������ڵ����)
����ֵ:��
��������:������
*************************************************************************/
void cluster_main(STLink **link_array, STConsumerLinkInfo *consumer_link_info, STNetNodeInfo *net_node_info)
{
    cluster_pre(link_array, consumer_link_info, num_consumer, net_node_info, num_net_dot);
    
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
    
    //����dijkstra��������ȫͼ�������·��
    dijkstra_process_main(p2p_path_info, link_array, consumer_link_info);

    //���þ��ദ����
    cluster_main(link_array, consumer_link_info, net_node_info);

    //�ͷ��ڴ�ռ�
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
    free(consumer_link_info);
    free(result_info);
    free(topo_file);

}
