/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  Author: Zhou YuChen																										 *
*  Date: 2023-05-09																												 *
*  Version: v 1.0.0																													 *
*  Describtion: This is a mini HTTP server running on the Windows platform					 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<WinSock2.h>
#pragma comment(lib, "WS2_32.lib")

#define PRINTF(str) printf("\n[ %s  ->  %d ] -------- "#str" = %s", __func__, __LINE__, str);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Obtain the corresponding file type based on the file name							  				 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
const char* getHeadType(char* fileName) {
	const char* res;
	res = "text/html\r\n";
	const char* suffix = strrchr(fileName, '.');
	if (!suffix)
		return res;
	suffix++;
	if (!strcmp(suffix, "css")) res = "text/css\r\n";
	else if (!strcmp(suffix, "jpg")) res = "image/jpeg\r\n";
	else if (!strcmp(suffix, "png")) res = "image/png\r\n";
	else if (!strcmp(suffix, "js")) res = "application/x-javascript\r\n";
	return res;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*   When a request resource exists, send response packet header information				 *
*	 200 OK																															 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void headers(int client, const char* fileType) {
	char buff[1024];
	strcpy_s(buff, "HTTP/1.0 200 OK\r\n");
	send(client, buff, strlen(buff), 0);
	strcpy_s(buff, "Server: MiniHttpd/1.0 \r\n");
	send(client, buff, strlen(buff), 0);
	strcpy_s(buff, "Content-type:");
	send(client, buff, strlen(buff), 0);
	strcpy_s(buff, fileType);
	send(client, buff, strlen(buff), 0);
	strcpy_s(buff, "\r\n");
	send(client, buff, strlen(buff), 0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Send response packet header information when the requested resource does not exist *
* 404 NOT FOUND																												   *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void not_found(int client)
{
	char buff[1024];
	sprintf_s(buff, "HTTP/1.0 404 NOT FOUND\r\n");
	send(client, buff, strlen(buff), 0);
	sprintf_s(buff, "Content-Type: text/html\r\n");
	send(client, buff, strlen(buff), 0);
	sprintf_s(buff, "Server: MiniHttpd/1.0 \r\n");
	send(client, buff, strlen(buff), 0);
	sprintf_s(buff, "\r\n");
	send(client, buff, strlen(buff), 0);
	sprintf_s(buff, "<HTML><TITLE>Not Found</TITLE>\r\n");
	send(client, buff, strlen(buff), 0);
	sprintf_s(buff, "<BODY><P>The server could not fulfill\r\n");
	send(client, buff, strlen(buff), 0);
	sprintf_s(buff, "your request because the resource specified\r\n");
	send(client, buff, strlen(buff), 0);
	sprintf_s(buff, "is unavailable or nonexistent.\r\n");
	send(client, buff, strlen(buff), 0);
	sprintf_s(buff, "</BODY></HTML>\r\n");
	send(client, buff, strlen(buff), 0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* When a user sends a bad request																					 *
* 400 BAD REQUEST																											 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void bad_request(int client)
{
	char buff[1024];

	sprintf_s(buff, "HTTP/1.0 400 BAD REQUEST\r\n");
	send(client, buff, sizeof(buff), 0);
	sprintf_s(buff, "Content-type: text/html\r\n");
	send(client, buff, sizeof(buff), 0);
	sprintf_s(buff, "Server: MiniHttpd/1.0 \r\n");
	send(client, buff, sizeof(buff), 0);
	sprintf_s(buff, "\r\n");
	send(client, buff, sizeof(buff), 0);
	sprintf_s(buff, "<HTML><TITLE>Bad Request</TITLE>\r\n");
	send(client, buff, strlen(buff), 0);
	sprintf_s(buff, "<BODY><P>Your browser sent a bad request</P>\r\n");
	send(client, buff, sizeof(buff), 0);
	sprintf_s(buff, "</BODY></HTML>\r\n");
	send(client, buff, strlen(buff), 0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* When the server is unavailable																							 *
* 500 Internal Server Error																									 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void unimplement(int client) {
	char buff[1024];
	sprintf_s(buff, "HTTP/1.0 500 Internal Server Error\r\n");
	send(client, buff, strlen(buff), 0);
	sprintf_s(buff, "Content-type: text/html\r\n");
	send(client, buff, strlen(buff), 0);
	sprintf_s(buff, "Server: MiniHttpd/1.0 \r\n");
	send(client, buff, sizeof(buff), 0);
	sprintf_s(buff, "\r\n");
	send(client, buff, strlen(buff), 0);
	sprintf_s(buff, "<HTML><TITLE>Not Available</TITLE>\r\n");
	send(client, buff, strlen(buff), 0);
	sprintf_s(buff, "<BODY><P>Service resources are temporarily unavailable</P>\r\n");
	send(client, buff, sizeof(buff), 0);
	sprintf_s(buff, "</BODY></HTML>\r\n");
	send(client, buff, strlen(buff), 0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Send requested resource data to the browser																	 * 
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void cat(int client, FILE* resource) {
	char buff[4096];
	int count = 0;
	while (1) {
		int res = fread(buff, sizeof(char), sizeof(buff), resource);
		if (res <= 0) {
			break;
		}
		count += res;
		send(client, buff, res, 0);
	}
	printf("send resourse [ %d ] bytes \n", count);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Read one line from socket                                                                                                *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int get_line(int socket, char* buff, int size) {
	char c = 0;
	int i = 0;

	while (i < size - 1 && c != '\n') {
		int n = recv(socket, &c, 1, 0);
		if (n > 0) {
			if (c == '\r') {
				n = recv(socket, &c, 1, MSG_PEEK);
				if (n > 0 && c == '\n') {
					n = recv(socket, &c, 1, 0);
				}
				else {
					c = '\n';
				}
			}
			buff[i++] = c;
		}
		else {
			c = '\n';
		}
	}

	buff[i] = '\0';
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Used to return resources requested by users                                                                   *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void server_file(int client, char* path) {
	int chars_number = 1;
	char buff[1024];

	// Read remaining packets
	while (chars_number > 0 && strcmp(buff, "\n") != 0) {
		chars_number = get_line(client, buff, sizeof(buff));
	}

	FILE* resource = NULL;
	if (strcmp(getHeadType(path), "text/html") == 0) {
		fopen_s(&resource, path, "r");
	}
	else {
		fopen_s(&resource, path, "rb");
	}
	if (resource == NULL) {
		not_found(client);
	}
	else {
		// send response header
		headers(client, getHeadType(path));
		// send file 
		cat(client, resource);
		printf("successful send !!! \n");
	}
	fclose(resource);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Thread functions for processing user requests                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
DWORD WINAPI	 accept_request(LPVOID arg) {
	char buff[1024];	// 1K
	int client = (SOCKET)arg;
	int chars_number =  get_line(client, buff, sizeof(buff));
	PRINTF(buff);
	// method
	char method[255];
	int buff_index = 0, method_index = 0;
	while (!isspace(buff[buff_index]) && method_index < sizeof(method) - 1)  {
		method[method_index++] = buff [buff_index++];
	}
	method[method_index] = '\0';
	PRINTF(method);
	// check
	if (_stricmp(method, "GET") && _stricmp(method, "POST")) {
		unimplement(client);
		return 0;
	}
	// resource path
	char url[255];
	int url_index = 0;
	while (isspace(buff[buff_index]) && buff_index < sizeof(buff)) {
		buff_index++;
	}
	while (!isspace(buff[buff_index]) && url_index < sizeof(url) - 1) {
		url[url_index++] = buff[buff_index++];
	}
	url[url_index] = '\0';
	PRINTF(url);
	// path
	char path[512] = "";
	sprintf_s(path, "htdocs%s", url);
	if (path[strlen(path) - 1] == '/') {
		strcat_s(path, "index.html");
	}
	PRINTF(path);
	
	struct stat status;
	if (stat(path, &status) == -1) {
		// Read remaining packets
		while (chars_number > 0 && strcmp(buff, "\n") ) {
			chars_number = get_line(client, buff, sizeof(buff));
		}
		not_found(client);
	}
	else {
		if ((status.st_mode & S_IFMT) == S_IFDIR) {
			strcat_s(path, "index.html");
		}
		// Send resources back
		server_file(client, path);
	}
	// Sleep(500);
	shutdown(client, SD_SEND);
	// closesocket(client);
	return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  Prompt an error message and terminate httpd directly													 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void error_die(const char* str) {
	perror(str);
	exit(1);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Realize the initialization of the network																			 *
* return value is a socketThe parameter indicates the port usedIf you use port 0,			 *
* Httpd will dynamically assign a port																				 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int startup(unsigned short *port) {
	// 1. Initialization of network communication,  version 1.1
	WSADATA data;
	int res = WSAStartup(MAKEWORD(1, 1), &data);
	if (res) {
		error_die("Failed to initialize network");
	}
	// 2. Create socket
	int server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == -1) {
		error_die("Failed to create socket");
	}
	// 3. Set socket properties, set the port to be reusable
	int opt = 1;
	res = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
	if (res == -1) {
		error_die("Failed to set socket properties");
	}
	// 4. Bind socket
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons( * port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		error_die("Failed to bind port");
	}
	// 5. Dynamically assign ports
	int nameLen = sizeof(server_addr);
	if (*port == 0) {
		if (getsockname(server_socket, (struct sockaddr*)&server_addr, &nameLen) < 0) {
			error_die("Dynamic allocation of port number failed");
		}
	}
	*port = server_addr.sin_port;
	// 6. Create a listening queue
	if (listen(server_socket, 10) < 0) {
		error_die("Failed to create listening queue");
	}

	return server_socket;
}


int main(void) {
	unsigned short server_port = 80;
	int server_socket = startup(&server_port);
	printf(" The http server has been started and is listening on %d port... ",  htons(server_port));

	struct  sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);

	while (1) {
		// Blocking waiting for user initiated access
		int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
		if (client_socket == -1) {
			error_die("Failed to accept user access");
		}

		// Create a new thread to serve users
		DWORD thread_id = 0;
		CreateThread(0, 0, accept_request, (void *)client_socket, 0, &thread_id);
	}
	closesocket(server_socket);
	return 0;
}