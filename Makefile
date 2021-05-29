IDIR	:= include
CXX		:= cc
GNU		:= gnuplot
CFLAGS	= -I$(IDIR)
LDFLAGS := -ll
BUILD	:= ./build
OBJ_DIR	:= $(BUILD)/obj
BIN_DIR	:= $(BUILD)/bin
TARGET	:= result
SRC		:= src
LIB		:= lib
DATA	:= data
SCRIPTS :=scripts
SITE_ROOT	:= ./site_root

export LD_LIBRARY_PATH=./lib

$(OBJ_DIR)/myThread.o :$(LIB)/myThread.c $(IDIR)/myOwnthread.h
	$(CXX) -Wall -fpic -c $(LIB)/myThread.c -o $(OBJ_DIR)/myThread.o $(CFLAGS)
$(OBJ_DIR)/Queue.o :$(LIB)/Queue.c $(IDIR)/Queue.h
	$(CXX) -Wall -fpic -c $(LIB)/Queue.c -o $(OBJ_DIR)/Queue.o $(CFLAGS)	
$(OBJ_DIR)/alarm.o :$(LIB)/alarm.c $(IDIR)/alarm.h
	$(CXX) -Wall -fpic -c $(LIB)/alarm.c -o $(OBJ_DIR)/alarm.o $(CFLAGS)

lib:$(OBJ_DIR)/myThread.o \
	$(OBJ_DIR)/Queue.o \
	$(OBJ_DIR)/alarm.o

	$(CXX) -o $(LIB)/libmyOwnthread.so \
	$(OBJ_DIR)/alarm.o \
	$(OBJ_DIR)/Queue.o \
	$(OBJ_DIR)/myThread.o -shared


$(OBJ_DIR)/ThreadQueue.o: $(SRC)/Web/ThreadQueue.c $(IDIR)/ThreadQueue.h
		$(CXX) -c $(SRC)/Web/ThreadQueue.c -o $(OBJ_DIR)/ThreadQueue.o $(CFLAGS)

$(OBJ_DIR)/Mime.o: $(SRC)/Web/Mime.c $(IDIR)/Mime.h
		$(CXX) -c $(SRC)/Web/Mime.c -o $(OBJ_DIR)/Mime.o $(CFLAGS)

$(OBJ_DIR)/HTTPHandler.o: $(SRC)/Web/HTTPHandler.c $(IDIR)/HTTPHandler.h
		$(CXX) -c $(SRC)/Web/HTTPHandler.c -o $(OBJ_DIR)/HTTPHandler.o $(CFLAGS)

$(OBJ_DIR)/SocketHandler.o: $(SRC)/Web/SocketHandler.c $(IDIR)/SocketHandler.h
	$(CXX) -c $(SRC)/Web/SocketHandler.c -o $(OBJ_DIR)/SocketHandler.o $(CFLAGS)

$(OBJ_DIR)/ServerSignals.o: $(SRC)/Web/ServerSignals.c $(IDIR)/ServerSignals.h
	$(CXX) -c $(SRC)/Web/ServerSignals.c -o $(OBJ_DIR)/ServerSignals.o $(CFLAGS)

$(OBJ_DIR)/webserver.o: $(SRC)/Web/webserver.c 
	$(CXX) -c $(SRC)/Web/webserver.c -o $(OBJ_DIR)/webserver.o $(CFLAGS) -L$(LIB) -lmyOwnthread

$(OBJ_DIR)/httpClient.o: $(SRC)/Web/httpClient.c 
	$(CXX) -c $(SRC)/Web/httpClient.c -o $(OBJ_DIR)/httpClient.o $(CFLAGS) -L$(LIB) -lmyOwnthread

$(OBJ_DIR)/helloFromClient.o: $(SRC)/Web/helloFromClient.c 
	$(CXX) -c $(SRC)/Web/helloFromClient.c -o $(OBJ_DIR)/helloFromClient.o $(CFLAGS) -L$(LIB) -lmyOwnthread


webserver: 	$(OBJ_DIR)/HTTPHandler.o \
			$(OBJ_DIR)/webserver.o \
			$(OBJ_DIR)/SocketHandler.o \
			$(OBJ_DIR)/Mime.o \
			$(OBJ_DIR)/ThreadQueue.o \
			$(OBJ_DIR)/ServerSignals.o \
			lib

	$(CXX) -o $(BIN_DIR)/webserver \
	$(OBJ_DIR)/webserver.o \
	$(OBJ_DIR)/HTTPHandler.o \
	$(OBJ_DIR)/SocketHandler.o \
	$(OBJ_DIR)/Mime.o \
	$(OBJ_DIR)/ThreadQueue.o \
	$(OBJ_DIR)/ServerSignals.o \
	$(CFLAGS) -L$(LIB) -lmyOwnthread
	./$(BIN_DIR)/webserver

client:	$(OBJ_DIR)/httpClient.o \
		$(OBJ_DIR)/SocketHandler.o

	$(CXX) -o $(BIN_DIR)/httpClient \
	$(OBJ_DIR)/httpClient.o \
	$(OBJ_DIR)/SocketHandler.o $(CFLAGS)
	./$(BIN_DIR)/httpClient


helloClient:	$(OBJ_DIR)/helloFromClient.o \
				$(OBJ_DIR)/SocketHandler.o

	$(CXX) -o $(BIN_DIR)/helloFromClient \
	$(OBJ_DIR)/helloFromClient.o \
	$(OBJ_DIR)/SocketHandler.o $(CFLAGS)
	./$(BIN_DIR)/helloFromClient


runmultipleclient:$(SCRIPTS)/call_client.py
	python3 $(SCRIPTS)/call_client.py

testData:
	rm -rf $(SITE_ROOT)/data/*.txt
	python3 $(SCRIPTS)/generate_data.py

plot:
	make testData
	make client
	$(GNU) $(SCRIPTS)/Plot.p

clean:
	rm -rf *.o $(OBJ_DIR)/*	\
	$(BIN_DIR)/* \
	$(LIB)/*.so	\
	rm -rf $(SITE_ROOT)/data/*.txt