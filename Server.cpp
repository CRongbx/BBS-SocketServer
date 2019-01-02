#include <stdio.h>
#include <iostream>
#include <winsock2.h>	// windows socket的头文件
#include <vector>
#include <fstream>
#include <string>
#pragma comment (lib, "ws2_32.lib")		//链接Winsock2.h的静态库文件
using namespace std;

int main(void) {
	//WSA初始化
	WORD sockVersion = MAKEWORD(2, 2);	//调用的WinSocket版本：2.2版 （支持多协议，且有良好的向后兼容性）
	WSADATA wasData;	//存储系统返回的关于WINSOCKET的资料
	if (WSAStartup(sockVersion, &wasData) != 0) {
		//返回值不为0，说明初始化失败
		cout << "WSAStartup failed:" << GetLastError() << endl;
		return 0;
	}

	//创建套接字
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, 0);	//IPV4,TCP,协议编号0
	vector<SOCKET> slistens;
	if (slisten == INVALID_SOCKET) {
		//监听到无效SOCKET
		cout << "Socket Error!" << endl;
		return 0;
	}

	//bind IP and Port
	sockaddr_in sin;	//Internet的统一网络地址结构
	sin.sin_family = AF_INET;	//地址族-IPV4
	sin.sin_port = htons(8888);	//TCP端口号，htons将主机字节序转换为网络字节序
	sin.sin_addr.S_un.S_addr = htonl(INADDR_ANY);	//指定32位IP地址为不确定地址0.0.0.0
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR) {
		//将本地地址绑定到所创建的socket上，以在网络上标识该socket
		//绑定出错
		cout << "Bind Error!" << endl;
	}

	//开始监听，准备接受连接请求
	if (listen(slisten, 5) == SOCKET_ERROR) {
		//5 表示当有多个客户端程序和服务端相连时，最多可以接受的排队长度
		cout << "Listen Error!" << endl;
		return 0;
	}

	//循环接受数据
	SOCKET sClient;
	sockaddr_in remoteAddr;	//远端服务地址
	int nAddrlen = sizeof(remoteAddr);	//地址长度
	char revData[2048];		//receive buffer
	

	while (true) {
		cout << "等待连接..." << endl;
		//为一个连接请求提供服务，返回服务器与客户端的连接后的socket
		sClient = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET) {
			cout << "Accept Error!" << endl;
			continue;
		}
		//inet_ntoa()将网络地址转换为点分十进制的字符串指针
		cout << "接受到一个连接：" << inet_ntoa(remoteAddr.sin_addr) << endl;

		//接收数据
		int ret = recv(sClient, revData, 2048, 0);
		if (ret > 0) {
			revData[ret] = '\0';	//buffer 的结束符
			cout << revData << endl;
		}
		if (ret > 4) {
			//是文件bbs.txt
			ofstream fout;
			fout.open("bbs.txt");
			string writetofile(revData);
			fout << writetofile;
			fout.close();
		}

		//发送数据
		ifstream fin;
		fin.open("bbs.txt");
		string str;
		if (!fin.is_open()) {
			cerr << "file not open!" << endl;
		}
		int j = 0;
		char sendData[2048];		//send buffer
		while (getline(fin,str)) {
			for (int i = 0; i < str.length(); i++, j++)
				sendData[j] = str[i];
			sendData[j++] = '\n';
		}
		sendData[j] = '\0';
		send(sClient, sendData, strlen(sendData), 0);
		fin.close();
		//关闭通信
		closesocket(sClient);
	}
	closesocket(slisten);
	WSACleanup();
	return 0;
}