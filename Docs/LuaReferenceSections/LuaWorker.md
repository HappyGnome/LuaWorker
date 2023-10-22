# LuaWorker

## Methods

### DoCoroutine
```
worker:DoCoroutine( function,args... )
```

Queue a task for this worker. The task starts a coroutine, which can yield to be resumed after a delay

**Arguments** :
\#  |Type		| Description
----|-----------|-----------
1	| String	| When executed in the worker thread, results in a lua function
2+  | String	| When executed in the worker thread, each results in an argument for the function

**Returns** :
\#  |Type					| Description
----|-----------------------|-----------
1	| [LuaTask](LuaTask.md)	| Task queued

**Examples**
```
task = worker:DoCoroutine("YieldingFunc","'a'","1000")
```

### DoFile
```
worker:DoFile( path )
```

Queue a task for this worker. The task executes the lua file at the specified path.

**Arguments** :
\#  |Type		| Description
----|-----------|-----------
1	| String	| Path of lua file to execute in the worker thread's lua environment

**Returns** :
\#  |Type					| Description
----|-----------------------|-----------
1	| [LuaTask](LuaTask.md)	| Task queued

**Examples**
```
task = worker:DoFile("myfile.lua")
```

### DoSleep
```
worker:DoSleep( millis )
```

Queue a task for this worker. The task causes the worker to sleep for the requested time.
Note the worker only sleeps when the task is executed, which may not be immediately e.g. if other tasks are queued.

**Arguments** :
\#  |Type		| Description
----|-----------|-----------
1	| String	| Milliseconds to sleep for

**Returns** :
\#  |Type					| Description
----|-----------------------|-----------
1	| [LuaTask](LuaTask.md)	| Task queued

**Examples**
```
task = worker:DoSleep(1000)
```

### DoString
```
worker:DoString( luaString )
```

Queue a task for this worker. The task executes lua code from a string.

**Arguments** :
\#  |Type		| Description
----|-----------|-----------
1	| String	| Lua to execute in the worker thread's lua environment

**Returns** :
\#  |Type					| Description
----|-----------------------|-----------
1	| [LuaTask](LuaTask.md)	| Task queued

**Examples**
```
task = worker:DoString("os.execute('timeout 5')")
```

### PopLogLine
```
worker:PopLogLine()
```
Get the next line logged by the worker (ordering is FIFO), removes that line from the log queue. Note that only a limited number of lines are stored, so it is advisable to pop all log lines regularly.

**Arguments** : None.

**Returns** :

If log not empty:
\#  |Type											| Description
----|-----------------------------------------------|-----------
1	| String										| The log line popped
2	| [**LogLevel**](LuaWorkerModule.md/#loglevel)	| The type of log event

otherwise nothing.

**Examples**
```
line, level = worker:PopLogLine()
```

### Start
```
worker:Start()
```

Start the worker thread

**Arguments** : None

**Returns** :
\#  |Type												| Description
----|---------------------------------------------------|-----------
1	| [WorkerStatus](LuaWorkerModule.md/#workerstatus)	| New status of the worker

**Examples**
```
status = worker:Start()
```

### Status
```
worker:Status()
```

Get current status of this worker

**Arguments** : None

**Returns** :
\#  |Type												| Description
----|---------------------------------------------------|-----------
1	| [WorkerStatus](LuaWorkerModule.md/#workerstatus)	| Current status of the worker

**Examples**
```
status = worker:Status()
```

### Stop
```
worker:Stop()
```

Permanently stop this worker thread, cancelling any queued tasks.

**Arguments** : None

**Returns** :
\#  |Type												| Description
----|---------------------------------------------------|-----------
1	| [WorkerStatus](LuaWorkerModule.md/#workerstatus)	| New status of the worker

**Examples**
```
status = worker:Stop()
```