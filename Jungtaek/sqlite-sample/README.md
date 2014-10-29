#SQLite Sample Code
Jungtaek Kim / jungtaek.kim@jt-inc.net

* Install SQLite3<br/>
	On your own!

* Create a database<br/>
```
sqlite3 test.db
```

* Complile C source<br/>
```
gcc sample.c -o sample.o -l sqlite3
```
* Database Schema<br/>
SENTENCE<br/>

|  KEY | FIELD   | TYPE      |
| ---- | ------- | --------- |
|  PK  | ID      | INT       |
|      | TIME    | TIMESTAMP |
|      | IDARRAY | CHAR(100) |
|      | ETC     | TEXT      |

