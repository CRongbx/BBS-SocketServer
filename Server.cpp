#include <stdio.h>
#include <iostream>
#include <winsock2.h>	// windows socket��ͷ�ļ�
#include <vector>
#include <fstream>
#include <string>
#pragma comment (lib, "ws2_32.lib")		//����Winsock2.h�ľ�̬���ļ�
using namespace std;

int main(void) {
	//WSA��ʼ��
	WORD sockVersion = MAKEWORD(2, 2);	//���õ�WinSocket�汾��2.2�� ��֧�ֶ�Э�飬�������õ��������ԣ�
	WSADATA wasData;	//�洢ϵͳ���صĹ���WINSOCKET������
	if (WSAStartup(sockVersion, &wasData) != 0) {
		//����ֵ��Ϊ0��˵����ʼ��ʧ��
		cout << "WSAStartup failed:" << GetLastError() << endl;
		return 0;
	}

	//�����׽���
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, 0);	//IPV4,TCP,Э����0
	vector<SOCKET> slistens;
	if (slisten == INVALID_SOCKET) {
		//��������ЧSOCKET
		cout << "Socket Error!" << endl;
		return 0;
	}

	//bind IP and Port
	sockaddr_in sin;	//Internet��ͳһ�����ַ�ṹ
	sin.sin_family = AF_INET;	//��ַ��-IPV4
	sin.sin_port = htons(8888);	//TCP�˿ںţ�htons�������ֽ���ת��Ϊ�����ֽ���
	sin.sin_addr.S_un.S_addr = htonl(INADDR_ANY);	//ָ��32λIP��ַΪ��ȷ����ַ0.0.0.0
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR) {
		//�����ص�ַ�󶨵���������socket�ϣ����������ϱ�ʶ��socket
		//�󶨳���
		cout << "Bind Error!" << endl;
	}

	//��ʼ������׼��������������
	if (listen(slisten, 5) == SOCKET_ERROR) {
		//5 ��ʾ���ж���ͻ��˳���ͷ��������ʱ�������Խ��ܵ��Ŷӳ���
		cout << "Listen Error!" << endl;
		return 0;
	}

	//ѭ����������
	SOCKET sClient;
	sockaddr_in remoteAddr;	//Զ�˷����ַ
	int nAddrlen = sizeof(remoteAddr);	//��ַ����
	char revData[2048];		//receive buffer
	

	while (true) {
		cout << "�ȴ�����..." << endl;
		//Ϊһ�����������ṩ���񣬷��ط�������ͻ��˵����Ӻ��socket
		sClient = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET) {
			cout << "Accept Error!" << endl;
			continue;
		}
		//inet_ntoa()�������ַת��Ϊ���ʮ���Ƶ��ַ���ָ��
		cout << "���ܵ�һ�����ӣ�" << inet_ntoa(remoteAddr.sin_addr) << endl;

		//��������
		int ret = recv(sClient, revData, 2048, 0);
		if (ret > 0) {
			revData[ret] = '\0';	//buffer �Ľ�����
			cout << revData << endl;
		}
		if (ret > 4) {
			//���ļ�bbs.txt
			ofstream fout;
			fout.open("bbs.txt");
			string writetofile(revData);
			fout << writetofile;
			fout.close();
		}

		//��������
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
		//�ر�ͨ��
		closesocket(sClient);
	}
	closesocket(slisten);
	WSACleanup();
	return 0;
}