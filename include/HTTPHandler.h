#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H
#include <stdlib.h>
#include <string.h>


#define NOT_FOUND_404 "\
HTTP/1.1 404 Not Found\n\
Content-Type: text/html\n\
Content-Length: 139\n\
\n\
<html>\
<head>\
<title>Front Page</title>\
</head>\
<body bgcolor=\"gray\">\
<h1 center>HTTP 404 resource not found<h1>\
</body>\
</html>"


typedef enum httpMethod
{
    UNSUPPORTED_METHOD,
    GET,
    POST,
    PUT,
    HEAD,
    DELETE,
}httpMethod;

typedef enum httpVersion
{
    UNSUPPORTED_VER,
    HTTP10,
    HTTP11,
}httpVersion;

typedef struct Header
{
    char *name;
    char *value;
    struct Header *next;
} Header;


typedef struct URI
{
    char* url;
    int is_path;
    Header* headers;
}URI;

typedef struct Request
{
    int isValid;
    httpMethod method;
    URI *uri;
    httpVersion http_ver;
    Header *headers;
    char *body;
}Request;



typedef struct Response
{
    httpVersion http_ver;
    int code;
    char *reason;
    Header *headers;
    char *body;
    FILE *fp;
}Response;



Header* makeHeaderNode(char *name, char *value);
Header *copyHeaderList(Header *head);
Header *appendHeader(Header *head, Header *node);
Request *http_req_parser(const char *str);
Response *http_res_builder(int http_ver, int code, char *reason, Header *head, char*body, FILE *fp);
char *requestToString(Request *req);
char *responseToString(Response *res);
Response * generateGETResponse(Request *req);
Response * generatePOSTResponse(Request *req);

char *readResourceFile(char *filename);
URI* parseURI(char * uri);
char *readFile(char *filename);
char *res_404();
char* getHTTPResponse(char* requestStr);
// int getHTTPVersion(char *version,int clientSockDes);
char * generateHTTPResponse(Request *req);
char *httpHeaderToString(Response *res);
Response * getResponseObj(Request *req);
int chatLogWrite(char *text, int append);
char * chatLogRead(int n);
void free_req(Request *req);
void free_res(Response *req);
#endif