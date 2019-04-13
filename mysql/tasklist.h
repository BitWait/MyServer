#pragma once

#include "mysqltask.h"

#define MAX_TASK_NUM 1000

class CTaskList
{
public:
	CTaskList();
	~CTaskList(void);

	bool Push(IMysqlTask* poTask);                  // �߼��߳��޸� 
	IMysqlTask* Pop();				                // ���ݿ��߳��޸� 

private:
	uint16_t            m_uReadIndex;               // ���ݿ��߳��޸� 
	uint16_t            m_uWriteIndex;              // �߼��߳��޸�   
	IMysqlTask*         m_pTaskNode[MAX_TASK_NUM];
};