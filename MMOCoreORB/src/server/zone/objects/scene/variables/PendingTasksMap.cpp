/*
 * PendingTasksMap.cpp
 *
 *  Created on: Apr 14, 2015
 *      Author: TheAnswer
 */

#include "server/zone/objects/scene/SceneObject.h"

#include "PendingTasksMap.h"
#include "OrderedTaskExecutioner.h"

using namespace server::zone::objects::scene::variables;
using namespace server::zone::objects::scene;

// The queue is not fixed-size: push() allocates nodes on demand and keeps
// popped ones for reuse, so preallocating only costs memory per object.
PendingTasksMap::PendingTasksMap() : taskMap(1, 1), pendingTasks(0) {
	taskMap.setNoDuplicateInsertPlan();
	taskMap.setNullValue(nullptr);
}

PendingTasksMap::~PendingTasksMap() {
	Task* task;

	while (pendingTasks.pop(task)) {
		if (task != nullptr) {
			task->release();
		}
	}
}

int PendingTasksMap::put(const String& name, Task* task) {
	Locker guard(&mutex);

	return taskMap.put(name, task);
}

void PendingTasksMap::drop(const String& name) {
	Locker guard(&mutex);

	taskMap.drop(name);
}

bool PendingTasksMap::contains(const String& name) {
	Locker guard(&mutex);

	return taskMap.contains(name);
}

Reference<Task*> PendingTasksMap::get(const String& name) const {
	Locker guard(&mutex);

	return taskMap.get(name);
}

Reference<Task*> PendingTasksMap::popNextOrderedTask() {
	Reference<Task*> strongTaskReference;
	Task* task;

	const auto result = pendingTasks.pop(task);

	if (result && task) {
		strongTaskReference.initializeWithoutAcquire(task);
	}

	return strongTaskReference;
}

