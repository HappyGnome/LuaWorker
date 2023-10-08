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