# LuaWorker

## Methods

### DoFile
```
worker:DoFile(path)
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
worker:DoString(luaString)
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

### Start
```
worker:Start()
```

Start the worker thread

**Arguments** : None

**Returns** :
\#  |Type												| Description
----|---------------------------------------------------|-----------
1	| [WorkerStatus](LuaWorkerModule.md/#wokerstatus)	| New status of the worker

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
1	| [WorkerStatus](LuaWorkerModule.md/#wokerstatus)	| Current status of the worker

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
1	| [WorkerStatus](LuaWorkerModule.md/#wokerstatus)	| New status of the worker

**Examples**
```
status = worker:Stop()
```