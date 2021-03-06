#include "TaskPool.h"

#include <SDL/SDL_mutex.h>
#include <assert.h>

using namespace MPL;

TaskPool::TaskPool()
: m_openList(new std::map<TaskId, Task*>()), m_queue(new std::list<WorkItem*>()), m_workTaskConnection(new std::map<WorkItem*, Task*>())
{
	m_idMutex = SDL_CreateMutex();
	m_taskMutex = SDL_CreateMutex();

	/* Mutex couldn't be created */
	if (!m_idMutex || !m_taskMutex) { printf("MPL Error: %s\n", SDL_GetError()); abort(); }
	
	m_nextId = 0;
}

TaskPool::~TaskPool()
{
	delete m_openList;
	delete m_queue;
	delete m_workTaskConnection;
	
	SDL_DestroyMutex(m_idMutex);
	SDL_DestroyMutex(m_taskMutex);
}

TaskId TaskPool::CreateTask(TaskId _dependency, const std::vector<WorkItem*>& _workItems)
{
	Task* task = new Task();
	task->Id = GenerateTaskId();
	task->Dependency = _dependency;
	task->Work = 0;
	task->OpenWorkItemCount = 0;
	AddToOpenList(task);
	AddToQueue(_workItems, task->Id);

	return task->Id;
}

TaskId TaskPool::BeginCreateTask(TaskId _dependency, WorkItem* _workItem)
{
	Task* task = new Task();
	task->Id = GenerateTaskId();
	task->Dependency = _dependency;
	task->Work = _workItem;
	task->OpenWorkItemCount = 1;
	AddToOpenList(task);
	if (task->Work != 0)
		AddToQueue(_workItem, task->Id);

	return task->Id;
}

void TaskPool::FinishCreateTask(TaskId _id)
{
	SDL_LockMutex(m_taskMutex);
	DecreaseWorkCount((*m_openList)[_id]);
	SDL_UnlockMutex(m_taskMutex);
}

void TaskPool::CreateChild(TaskId _parentId, WorkItem* _workItem)
{
	AddToQueue(_workItem, _parentId);
}

void TaskPool::CreateChildren(TaskId _parentId, const std::vector<WorkItem*>& _workItems)
{
	for (auto workItem : _workItems)
		AddToQueue(workItem, _parentId);
}

TaskId TaskPool::GenerateTaskId()
{
	SDL_LockMutex(m_idMutex);
	TaskId id = ++m_nextId;
	SDL_UnlockMutex(m_idMutex);
	return id;
}

void TaskPool::AddToOpenList(Task* _task)
{
	SDL_LockMutex(m_taskMutex);
	(*m_openList)[_task->Id] = _task;
	SDL_UnlockMutex(m_taskMutex);
}

void TaskPool::AddToQueue(WorkItem* _workItem, TaskId _taskId)
{
	SDL_LockMutex(m_taskMutex);

	Task* task = (*m_openList)[_taskId];

	/* Add work item to queue */
	m_queue->push_back(_workItem);

	/* Create connection between work item and task id */
	(*m_workTaskConnection)[_workItem] = task;

	/* Increase open work item count */
	IncreaseWorkCount(task);

	SDL_UnlockMutex(m_taskMutex);
}

void TaskPool::AddToQueue(const std::vector<WorkItem*>& _workItems, TaskId _taskId)
{
	SDL_LockMutex(m_taskMutex);

	Task* task = (*m_openList)[_taskId];

	for (auto workItem : _workItems)
	{
		/* Add work item to queue */
		m_queue->push_back(workItem);

		/* Create connection between work item and task id */
		(*m_workTaskConnection)[workItem] = task;

		/* Increase open work item count */
		IncreaseWorkCount(task);
	}

	SDL_UnlockMutex(m_taskMutex);
}

void TaskPool::IncreaseWorkCount(Task* _task)
{
	++_task->OpenWorkItemCount;
}

void TaskPool::DecreaseWorkCount(Task* _task)
{
	--_task->OpenWorkItemCount;
	if (_task->OpenWorkItemCount == 0)
	{
		m_openList->erase(_task->Id);
		delete(_task);
	}
}

WorkItem* TaskPool::FetchWork(FetchWorkStatus& _out)
{
	_out = WAITING_FOR_TASK;
	WorkItem* workItem = 0;

	SDL_LockMutex(m_taskMutex);

	if (m_openList->size() == 0)
		_out = EMPTY_OPEN_LIST;

	/* Fetch work to do from the queue */
	if (m_queue->size() != 0)
	{
		WorkItem* front = m_queue->front();
		TaskId dependency = (*m_workTaskConnection)[front]->Dependency;
		if (dependency == -1 || m_openList->find(dependency) == m_openList->end())
		{
			workItem = front;
			m_queue->pop_front();
			_out = OK;
		}
	}
	else
		_out = EMPTY_WORK_LIST;

	SDL_UnlockMutex(m_taskMutex);

	return workItem;
}

WorkDoneStatus TaskPool::WorkDone(WorkItem* _workItem)
{
	WorkDoneStatus status = WorkDoneStatus();

	SDL_LockMutex(m_taskMutex);

	/* Get task through the work-task connection */
	Task* task = (*m_workTaskConnection)[_workItem];

	/* Check if entire task is completed */
	if (task->OpenWorkItemCount == 1)
		status.TaskCompleted = true;

	/* Decrease open work item count */
	DecreaseWorkCount(task);

	/* Check if m_openList is empty */
	if (m_openList->size() == 0)
		status.OpenListEmpty = true;
	else status.OpenListEmpty = false;

	SDL_UnlockMutex(m_taskMutex);

	return status;
}

bool TaskPool::IsTaskDone(TaskId _id)
{
	SDL_LockMutex(m_taskMutex);
	bool isDone = (m_openList->find(_id) == m_openList->end());
	SDL_UnlockMutex(m_taskMutex);
	return isDone;
}