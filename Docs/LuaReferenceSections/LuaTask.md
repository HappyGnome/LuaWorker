# LuaTask

## Methods

### Await
```
task:Await( millis )
```

Wait for a limited time for the task to reach a final status. Returns when this is the case, or after the timeout, whichever occurs first.

**Arguments** :
\#  |Type		| Description
----|-----------|-----------
1	| Integer	| Maximum milliseconds to wait for the task to reach a final status.

**Returns** :

If the task is complete:
\#  |Type		| Description
----|-----------|-----------
1	| String	| Return value of the task

otherwise nothing (e.g. on timeout, cancellation or error).

**Examples**
```
taskReturned = task:Await()
```

### Finalized
```
task:Finalized()
```

Check whether this task is in a final state

**Arguments** : None.

**Returns** :
\#  |Type		| Description
----|-----------|-----------
1	| Boolean	| `true` if and only if this task is in a final status

**Examples**
```
final = task:Finalized()
```

### Status
```
task:Status()
```

Get status of this task.

**Arguments** : None.

**Returns** :
\#  |Type												| Description
----|---------------------------------------------------|-----------
1	| [**TaskStatus**](LuaWorkerModule.md/#taskstatus)	| Current status of the task


**Examples**
```
status = task:Status()
```