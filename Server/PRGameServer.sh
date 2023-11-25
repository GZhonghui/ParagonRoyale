g++ PRGameServer.cpp  GameMessage.pb.cc -o PRGameServer -std=c++11 -lprotobuf -I/usr/include/mysql/ -L/usr/lib64/mysql -L/usr/local/lib/ -lmysqlclient_r
g++ PRGateServer.cpp  GameMessage.pb.cc -o PRGateServer -std=c++11 -lprotobuf -I/usr/include/mysql/ -L/usr/lib64/mysql -L/usr/local/lib/ -lmysqlclient_r
g++ PRLoginServer.cpp  GameMessage.pb.cc -o PRLoginServer -std=c++11 -lprotobuf -I/usr/include/mysql/ -L/usr/lib64/mysql -L/usr/local/lib/ -lmysqlclient_r
 
