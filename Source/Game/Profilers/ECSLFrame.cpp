#include "ECSLFrame.h"

using namespace Profilers;

ECSLFrame::ECSLFrame(unsigned int _threadCount, float _frameTime)
{
	m_threadEfficiency = FloatArray(_threadCount);
	m_threadOverheadTime = FloatArray(_threadCount);
	m_threadWorkTime = FloatArray(_threadCount);
	m_frameTime = _frameTime;
	m_totalEfficiency = 0.0f;
	m_totalOverheadTime = 0.0f;
	m_totalWorkTime = 0.0f;
	m_workItemStats = new std::unordered_map<unsigned int, std::vector<WorkItemStatistic*>*>();
}

ECSLFrame::~ECSLFrame()
{
	delete(m_threadEfficiency);
	delete(m_threadWorkTime);
	delete(m_threadOverheadTime);
	for (auto group : *m_workItemStats)
	{
		for (auto statistic : *group.second)
			delete(statistic);
		delete(group.second);
	}
	delete(m_workItemStats);
}

void ECSLFrame::AddOverheadData(MPL::LoggedAction* _action, unsigned int _threadId)
{
	m_totalOverheadTime += _action->duration;
	m_threadEfficiency[_threadId] = _action->duration;
}

void ECSLFrame::AddWorkData(MPL::LoggedAction* _action, unsigned int _threadId)
{
	m_totalWorkTime += _action->duration;
	m_threadWorkTime[_threadId] = _action->duration;
}

void ECSLFrame::AddWorkItemStatistic(MPL::LoggedAction* _action, unsigned int _threadId)
{
	WorkItemStatistic* workItemStat = new WorkItemStatistic();
	workItemStat->name = new std::string(*_action->workItem->ProfilerName);
	workItemStat->groupId = _action->workItem->ProfilerGroupId;
	workItemStat->duration = _action->duration;

	auto it = m_workItemStats->find(workItemStat->groupId);
	if (it == m_workItemStats->end())
		(*m_workItemStats)[workItemStat->groupId] = new std::vector<WorkItemStatistic*>();
	(*m_workItemStats)[workItemStat->groupId]->push_back(workItemStat);
}

void ECSLFrame::CalculateEfficiencyData(unsigned int _threadCount)
{
	m_totalEfficiency = m_totalWorkTime / (m_totalWorkTime + m_totalOverheadTime);
	for (unsigned int threadId = 0; threadId < _threadCount; ++threadId)
		m_threadEfficiency[threadId] = m_threadWorkTime[threadId] / (m_threadWorkTime[threadId] + m_threadOverheadTime[threadId]);
}

float* Profilers::FloatArray(unsigned int _size)
{
	float* out = new float[_size];
	for (unsigned int i = 0; i < _size; ++i)
		out[i] = 0.0f;
	return out;
}