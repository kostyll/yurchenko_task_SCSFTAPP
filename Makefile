linux: 
	g++ -o server/_server server/server_linux.cpp
	g++ -o client/_client_linux client/client_linux.cpp
win32:
	cl /c client/client_win32.cpp 
	link ws2_32.lib client_win32.obj /out:"client/client_win32.exe" 