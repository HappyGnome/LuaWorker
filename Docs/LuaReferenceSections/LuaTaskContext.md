# Task lua context

A global table `InLuaWorker` is available within the lua state in which tasks execute, providing functionality such as logging back to the calling worker.

## Methods

### LogError
```
InLuaWorker.LogError( msg )
```
Add error message to the log of the calling [worker](LuaWorker.md).

**Arguments** : 
\#  |Type		| Description				
----|-----------|------------------------------
1	| String	| Message to log

**Returns** : Nothing

**Examples**
```
InLuaWorker.LogError( "Error in task lua!" )
```

### LogInfo
```
InLuaWorker.LogInfo( msg )
```
Add info message to the log of the calling [worker](LuaWorker.md).

**Arguments** : 
\#  |Type		| Description				
----|-----------|------------------------------
1	| String	| Message to log

**Returns** : Nothing

**Examples**
```
InLuaWorker.LogInfo( "Hello logger, from task context!" )
```

### Sleep
```
InLuaWorker.Sleep( millis )
```
Sleep worker thread for at least the specified time.

**Arguments** : 
\#  |Type		| Description				
----|-----------|------------------------------
1	| Integer	| Milliseconds to sleep in worker thread

**Returns** : Nothing

**Examples**
```
InLuaWorker.Sleep(1000)
```

### YieldFor
```
InLuaWorker.YieldFor( millis , result )
```
Yield this coroutine and resume after at least the specified delay. Set a result in the task that launched the coroutine.
Calling this outside of a task created with DoCoroutine is an error.

**Arguments** : 
\#  |Type		| Description				
----|-----------|------------------------------
1	| Integer	| Milliseconds to wait after yielding to resume worker thread
2	| String	| Result to return to awaited task

**Returns** : Nothing

**Examples**
```
InLuaWorker.YieldFor(1000, "IntermediateResult")
```