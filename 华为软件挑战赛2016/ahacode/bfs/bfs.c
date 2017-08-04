#include "stdio.h"
#include "stdlib.h"

#define Num 6//排序所用数字个数
#define VOS_TRUE 0//定义函数返回值 true
#define VOS_FALSE 1//定义函数返回值 false

/*
函数功能：实现宽度优先搜索算法
函数参数：step:当前所处的步骤；book:标记各个数字使用状态（指针）；pDataIn：输入待排序数字（指针）；outputNum：总的排列的个数；pDataOut：输出排序后列表（指针）
作者：追梦少年
修改时间：20170226
*/
void bfs_process(int step, int *book,int *pDataIn, int *pOutputNum, int *pDataOut)
{
	int i = 0;
	if(step == Num)
	{
		*pOutputNum = *pOutputNum + 1;
		printf("\n No%d ",*pOutputNum);
		for(i = 0; i < Num; i++)
		{
			printf("%d ",*(pDataOut + i));
		}
		return;
	}
	for(i = 0; i < Num; i++)
	{
		if(*(book + i) == 0)
		{
			*(pDataOut + step) = *(pDataIn + i);
			*(book + i) = 1;
			bfs_process(step+1, book, pDataIn, pOutputNum, pDataOut);//递归调用，实现深度优先搜索
			*(book + i) = 0;
		}
	}
	return;
}

/*
函数功能：宽度优先算法处理函数入口
函数参数：dataIn：输入待排序数字（指针）；
作者：追梦少年
修改时间：20170226
*/
int bfs_main(int *dataIn)
{
	int i = 0;
	int outputNum = 0;
	int book[Num] = {0};
	int step = 0;
	int *pDataIn = dataIn;
	int *pDataOut = malloc(16000);//申请1000个排列的内存空间
	int ret = 0;

	printf("Breadth priority\n");
	printf("Sort number:");
	for(i = 0; i < Num; i++)
	{
		printf("%d ",dataIn[i]);
	}
	printf("\n");

	bfs_process(step, book, pDataIn, &outputNum, pDataOut);//调用宽度优先算法处理算法
	printf("\n total = %d\n",outputNum);
	
	free(pDataOut);//释放内存空间
	return VOS_TRUE;
}

/*
函数功能：主函数
函数参数：NA
作者：追梦少年
修改时间：20170226
*/
int main()
{
	int dataIn[Num] = {1,2,3,4,5,6};//构造输入条件
	int ret = 0;
	
	ret = bfs_main(dataIn);//调用宽度优先算法处理函数入口
	
	if(ret == VOS_TRUE)
	{
		printf("[Info]bfs run success\n");
	}
	else
	{
		printf("[Error]bfs run fail!\n");
	}
	
	return VOS_TRUE;
}
