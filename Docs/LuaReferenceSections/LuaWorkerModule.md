# Module scope

## Enums

###	LuaWorker.LogLevel

Name		| Description
------------|---------------------------
**Info**	| Information only
**Warn**	| Warning
**Error**	| Error

###	LuaWorker.TaskStatus
Name			| Description						| Final
----------------|-----------------------------------|--------
**Cancelled**	| Task cancelled before completion	| :heavy_check_mark: 
**Complete**	| Task completed without error		| :heavy_check_mark:
**Error**		| Task failed due to an error		| :heavy_check_mark:
**NotStarted**	| Execution not started				| 
**Running**		| Task currently executing			| 

###	LuaWorker.WorkerStatus
Name			| Description															| Final
----------------|-----------------------------------------------------------------------|--------
**NotStarted**	| Worker thread not started												| 
**Starting**	| [Start](LuaWorker.md/#start) called but worker thread not ready		| 
**Processing**	| Worker thread executing												| 
**Cancelled**	| Worker thread cancelled e.g. after calling [Stop](LuaWorker.md/#stop) | :heavy_check_mark:  
**Error**		| Worker thread ended with an error										| :heavy_check_mark:

## Methods

### LuaWorker.Create

Create a new [worker](LuaWorker.md) instance.

**Arguments** : None.

**Returns** :

\#  |Type                       | Description
----|---------------------------|-----------
1	|[LuaWorker](LuaWorker.md)	| The worker object created

**Examples**
```
worker = LuaWorker.Create()
```

### LuaWorker.PopLogLine

Get the next line logged by the worker (ordering is FIFO), removes that line from the log queue. Note that only a limited number of lines are stored, so it is advisable to pop all log lines regularly.

**Arguments** : None.

**Returns** :

If log not empty:
\#  |Type									| Description
----|---------------------------------------|-----------
1	| String								| The log line popped
2	| [**LogLevel**](#luaworkerloglevel)	| The type of log event

otherwise nothing.

**Examples**
```
local line, level = LuaWorker.PopLogLine()
```
