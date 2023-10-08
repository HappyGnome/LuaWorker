# LuaWorker scope

## Enums

###	LuaWorker.LogLevel

* **Info** - Information only
* **Warn** - Warning
* **Error** - Error

###	LuaWorker.TaskStatus

* **Cancelled** - Task cancelled before completion (final) 
* **Complete** - Task completed without error (final) 
* **Error** - Task failed due to an error (final) 
* **NotStarted** - Execution not started
* **Running** - Task currently executing

###	LuaWorker.WorkerStatus

* **NotStarted** - Worker thread not started
* **Starting** - [Start](LuaWorker.md/#start) called but worker thread not ready
* **Processing** - Worker thread executing
* **Cancelled** - Worker thread cancelled e.g. after calling [Stop](LuaWorker.md/#stop) (final)  
* **Error** - Worker thread ended with an error (final) 

## Methods

### LuaWorker.Create

Create a new [worker](LuaWorker.md) instance.

**Arguments**

* None.

**Return**

* The [worker](LuaWorker.md) object created.

**Examples**
```
worker = LuaWorker.Create()
```

### LuaWorker.PopLogLine

Get the next line logged by the worker (ordering is FIFO), removes that line from the log queue. Note that only a limited number of lines are stored, so it is advisable to pop all log lines regularly.

**Arguments**

* None.

**Return**
* None if the log queue is empty.
* If there are log lines queued:
	1. **string** The log line popped
	2. [**LogLevel**](#luaworkerloglevel) The type of log event

**Examples**
```
local line, level = LuaWorker.PopLogLine()
```
