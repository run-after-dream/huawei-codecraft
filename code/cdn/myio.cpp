/***********************************************************************
�ļ�����:myio.cpp
�ļ�����:���������������ģ�飬������ĿҪ�󣬱�д�Լ������������ݴ���������������
�ļ�����ʱ��:20170409
�ļ�����:������
***********************************************************************/

#include "deploy.h"
#include "mybase.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
    num_link_actual = 2*num_link;
    num_result_link = 0;
    num_consumer_remain = num_consumer;
    num_server = 0;
    
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
��������:�����Ϣ��׼������(������)
�������:��int���͵������Ϣת��Ϊ�ٷ�Ҫ����ַ�������
��ν���:link_sum(�ܵ�����·������),result_debug_info(�������·����Ϣ),topo_file(�ٷ���Ҫ������ַ���)
����ֵ:��
��������:������
*************************************************************************/
void result_standard_debug(int link_sum, STResultDebugInfo *result_debug_info, char *topo_file)
{
    int i = 0;
    char str[7] = {0};//��Ŀ�涨������ֵ���Ϊ100000�����Ͽ��ַ����7���ַ�
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
��������:�����Ϣ��׼������
�������:��int���͵������Ϣת��Ϊ�ٷ�Ҫ����ַ�������
��ν���:result_info(��������·��Ϣ),topo_file(�ٷ���Ҫ������ַ���)
����ֵ:��
��������:������
*************************************************************************/
void result_standard(STResultInfo *result_info, char *topo_file)
{
    int i = 0;
    int j = 0;
    char str[7] = {0};//��Ŀ�涨������ֵ���Ϊ100000�����Ͽ��ַ����7���ַ�
    STNode *p = NULL;
    change_int_to_char(num_result_link, str);//����·����
    strcpy(topo_file,str);
    strcat(topo_file,"\n\n");
    for(i = 0; i < num_result_link; i++)//���������·��Ϣ
    {
        change_int_to_char(result_info[i].p_path->id_node, str);//���������·ͷ���
        strcat(topo_file,str);
        strcat(topo_file," ");
        p = result_info[i].p_path;
        for(j = 0; j < result_info[i].link_num; j++)//���������·�м�ڵ�
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
        change_int_to_char(result_info[i].id_consumer, str);//���������·�����߽ڵ�
        strcat(topo_file,str);
        strcat(topo_file," ");
        change_int_to_char(result_info[i].cost_bandwidth, str);//���������·����
        strcat(topo_file,str);
        strcat(topo_file,"\n");
    }
}



