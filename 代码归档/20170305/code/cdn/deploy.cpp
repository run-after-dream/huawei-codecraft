#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int num_net_dot;//网络节点数量
int num_link;//网络链路数量
int num_consumer;//消费节点数量
int cost_server;//视频内容服务器部署成本

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
    char *str = 0;

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
    char *str = 0;

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
函数功能:基本功能调试函数(调试用)
功能详解:调试用函数，将所有服务器部署在消费节点上端
入参解释:consumer_link_info(消费节点链接信息),result_info(输出网络路径信息)
返回值:网络路径数
函数作者:陈自立
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
函数功能:输出信息标准化函数(调试用)
功能详解:将int类型的输出信息转化为官方要求的字符串类型
入参解释:link_sum(总的网络路径数量),result_info(输出网络路径信息),topo_file(官方需要的输出字符串)
返回值:无
函数作者:陈自立
*************************************************************************/
void result_standard_debug(int link_sum, STResultInfo *result_info, char *topo_file)
{
    int i = 0;
    char str[5] = {0};//题目规定单个数值最大为5000，加上空字符最多5个字符
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
    //最后一行数据不带‘\n’
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
函数功能:待完成功能总入口
功能详解:华为官方提供的功能入口，在main函数中直接调用
入参解释:topo(官方接口读取输入文件，以字符串数组形式体现),line_num(输入文件总函数，不包含最后一空行)
filename(输出文件路径名称)
返回值:无
函数作者:官方提供的接口
修改点:增加调用解析入参，处理算法的接口
修改人员:陈自立
*************************************************************************/
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    int link_sum = 0;//返回的网络路径总数
    STLinkInfo *link_info = 0;
    STConsumerLinkInfo *consumer_link_info = 0;
    STResultInfo *result_info = 0;
    // 需要输出的内容
    char * topo_file = 0;

    //解析基础输入信息
    input_analyse_base(topo);
    
    //依据链路值和消费者节点数量，动态分配几段内存空间，用于存储输入信息:
    link_info = (STLinkInfo *)malloc(num_link * sizeof(STLinkInfo));
    consumer_link_info = (STConsumerLinkInfo *)malloc(num_consumer * sizeof(STConsumerLinkInfo));
    result_info = (STResultInfo *)malloc(num_consumer * sizeof(STResultInfo));
    topo_file = (char *)malloc(2 * num_consumer * sizeof(STResultInfo));//多分配几字节，确保够用

    //进一步解析输入信息，获取其他输入信息
    input_analyse_other(link_info, consumer_link_info, topo);

    //调试处理函数
    link_sum = process_debug(consumer_link_info, result_info);
    //调试用输出标准化函数
    result_standard_debug(link_sum, result_info, topo_file);
    
    // 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
    write_result(topo_file, filename);

    //释放内存空间
    free(link_info);
    free(consumer_link_info);
    free(result_info);
    free(topo_file);

}
