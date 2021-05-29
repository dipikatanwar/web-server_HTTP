configure web server setting in include/config.h
Available parameters : PORT, BUFFER_SIZE , THREAD_POOL_SIZE and CHUNK_SIZE.

How to run in Project root directory?
Run Web Server :
make webserver.


Test1 :
	
	a)	make helloClient 
	b)  http://localhost:8888/index.html

Test2 :
Run multiple clients : make runmultipleclient

Test3:
	HTTP/1.0 vs HTTP/1.1 Plot

Test4:
Visit to http://localhost:8888/chat/chat.html from multiple client windows.

Test5:
Visit to http://localhost:8888/game/game.html from multiple client and play.

Make sure web server  running all time when these operations is performed.


