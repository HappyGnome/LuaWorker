# Module scope

## Enums

###	LogLevel
```
LuaWorker.LogLevel
```

Name		| Description
------------|---------------------------
**Info**	| Information only
**Warn**	| Warning
**Error**	| Error

###	TaskStatus
```
LuaWorker.TaskStatus
```
Name			| Description						| Final
----------------|-----------------------------------|--------
**Cancelled**	| Task cancelled before completion	| :heavy_check_mark: 
**Complete**	| Task completed without error		| :heavy_check_mark:
**Error**		| Task failed due to an error		| :heavy_check_mark:
**NotStarted**	| Execution not started				| 
**Running**		| Task currently executing			| 

###	WorkerStatus
```
LuaWorker.WorkerStatus
```
Name			| Description															| Final
----------------|-----------------------------------------------------------------------|--------
**NotStarted**	| Worker thread not started												| 
**Starting**	| [Start](LuaWorker.md/#start) called but worker thread not ready		| 
**Processing**	| Worker thread executing												| 
**Cancelled**	| Worker thread cancelled e.g. after calling [Stop](LuaWorker.md/#stop) | :heavy_check_mark:  
**Error**		| Worker thread ended with an error										| :heavy_check_mark:

## Methods

### Create
```
LuaWorker.Create( logSize )
```
Create a new [worker](LuaWorker.md) instance.

**Arguments** : 
\#  |Type		| Description					| Optional
----|-----------|-------------------------------|-------------
1	| Integer	| Maximum length of log queue	| :heavy_check_mark:

**Returns** :

\#  |Type                       | Description
----|---------------------------|-----------
1	|[LuaWorker](LuaWorker.md)	| The worker object created

**Examples**
```
worker = LuaWorker.Create()
```
