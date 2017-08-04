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
    char *str = 0;

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
    char *str = 0;

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
    for(i = 0; i < (link_sum - 1); i++)
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
    //���һ�����ݲ�����\n��
    change_int_to_char(result_info[i].id_server, str);
    strcat(topo_file,str);
    strcat(topo_file," ");
    change_int_to_char(result_info[i].id_consumer, str);
    strcat(topo_file,str);
    strcat(topo_file," ");
    change_int_to_char(result_info[i].cost_bandwidth, str);
    strcat(topo_file,str);
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
    STLinkInfo *link_info = 0;
    STConsumerLinkInfo *consumer_link_info = 0;
    STResultInfo *result_info = 0;
    // ��Ҫ���������
    char * topo_file = 0;

    //��������������Ϣ
    input_analyse_base(topo);
    
    //������·ֵ�������߽ڵ���������̬���伸���ڴ�ռ䣬���ڴ洢������Ϣ:
    link_info = (STLinkInfo *)malloc(num_link * sizeof(STLinkInfo));
    consumer_link_info = (STConsumerLinkInfo *)malloc(num_consumer * sizeof(STConsumerLinkInfo));
    result_info = (STResultInfo *)malloc(num_consumer * sizeof(STResultInfo));
    topo_file = (char *)malloc(2 * num_consumer * sizeof(STResultInfo));//����伸�ֽڣ�ȷ������

    //��һ������������Ϣ����ȡ����������Ϣ
    input_analyse_other(link_info, consumer_link_info, topo);

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
