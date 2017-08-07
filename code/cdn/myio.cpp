/***********************************************************************
文件名称:myio.cpp
文件功能:基本输入输出处理模块，根据题目要求，编写自己的输出输出数据处理函数，函数定义
文件创建时间:20170409
文件作者:陈自立
***********************************************************************/

#include "deploy.h"
#include "mybase.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/************************************************************************
函数功能:解析基本输入文件(基础数据)
功能详解:获取输入文件第一行与第三行的信息，为后面提取链路信息和消费节点信息提供参数
入参解释:topo(官方接口读取输入文件，以字符串数组形式体现)
返回值:无
函数作者:陈自立
*************************************************************************/
void input_analyse_base(char * topo[MAX_EDGE_NUM])
{
    int i = 0;
    int j = 0;
    int begin = 0;
    int end = 0;
    char *str = NULL;

    //解析输入文件第一行信息
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

    //解析输入文件第三行信息
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
函数功能:解析其他输入信息(链路信息与消费节点链接信息)
功能详解:读取并解析输入文件中第三行开始的链路信息与消费节点链接信息，存入到对应的内存空间中
入参解释:link_info(链路信息),consumer_link_info(消费节点链接信息),topo(官方接口读取输入文件，以字符串数组形式体现)
返回值:无
函数作者:陈自立
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

    //解析网路链路信息
    for(i = 0; i < num_link; i++)
    {
        k = i + 4;//从第5行开始
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

    //解析消费者节点信息
    for(i = 0; i < num_consumer; i++)
    {
        consumer_link_info[i].cluster_id = 1;
        k = i + 5 + num_link;//从第6 + num_link行开始
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
函数功能:输出信息标准化函数(调试用)
功能详解:将int类型的输出信息转化为官方要求的字符串类型
入参解释:link_sum(总的网络路径数量),result_debug_info(输出网络路径信息),topo_file(官方需要的输出字符串)
返回值:无
函数作者:陈自立
*************************************************************************/
void result_standard_debug(int link_sum, STResultDebugInfo *result_debug_info, char *topo_file)
{
    int i = 0;
    char str[7] = {0};//题目规定单个数值最大为100000，加上空字符最多7个字符
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
函数功能:输出信息标准化函数
功能详解:将int类型的输出信息转化为官方要求的字符串类型
入参解释:result_info(输出结果链路信息),topo_file(官方需要的输出字符串)
返回值:无
函数作者:陈自立
*************************************************************************/
void result_standard(STResultInfo *result_info, char *topo_file)
{
    int i = 0;
    int j = 0;
    char str[7] = {0};//题目规定单个数值最大为100000，加上空字符最多7个字符
    STNode *p = NULL;
    change_int_to_char(num_result_link, str);//网络路径数
    strcpy(topo_file,str);
    strcat(topo_file,"\n\n");
    for(i = 0; i < num_result_link; i++)//输出各条链路信息
    {
        change_int_to_char(result_info[i].p_path->id_node, str);//输出单条链路头结点
        strcat(topo_file,str);
        strcat(topo_file," ");
        p = result_info[i].p_path;
        for(j = 0; j < result_info[i].link_num; j++)//输出单条链路中间节点
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
        change_int_to_char(result_info[i].id_consumer, str);//输出单条链路消费者节点
        strcat(topo_file,str);
        strcat(topo_file," ");
        change_int_to_char(result_info[i].cost_bandwidth, str);//输出单条链路带宽
        strcat(topo_file,str);
        strcat(topo_file,"\n");
    }
}



