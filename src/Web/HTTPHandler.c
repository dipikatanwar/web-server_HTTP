#include<string.h>
#include<stdio.h>
#include "HTTPHandler.h"
#include "HttpStatusCodes.h"
#include "Config.h"
#include "Mime.h"
#include "myOwnthread.h"

extern myThread_mutex gameLock;
extern int gameVar;

char *duplicate(const char *str, int n)
{
    int s = (n == 0)? (strlen(str) + 1) : (n + 1);
    char *copy = (char*)malloc(s);
    memset(copy, 0, s);
    memcpy(copy,str,s-1);
    return copy;
}

Header *makeHeaderNode(char *name, char *value)
{
    Header *t =(Header*)malloc(sizeof(Header));
    t->name = duplicate(name,0);
    t->value = duplicate(value,0);
    t->next = NULL;
    return t;
}

void printH(Header *head)
{
    Header *itr = head;
    while(itr)
    {
        printf("%s:%s \n", itr->name, itr->value);
        itr = itr->next;
    }

}

void free_header(Header *head)
{
    if(head == NULL) return;
    while(head)
    {
        Header *t = head;
        head = head->next;
        if(t->name) free(t->name);
        if(t->value) free(t->value);
        free(t);
    }
}

URI* parseURI(char * uristr)
{
    URI* uri = (URI*)malloc(sizeof(URI));
    int t = strcspn(uristr,"?");
    uri->url = duplicate(uristr,t);
    uri->is_path = (strchr(uri->url,'.') == NULL) ? 1:0;
    uri->headers = NULL;
    return uri;
}

void free_uri(URI *uri)
{
    if(uri == NULL) return;
    free(uri->url);
    free_header(uri->headers);
    free(uri);
}

Header *copyHeaderList(Header *head)
{
    if(head == NULL) return NULL;
    Header *t = makeHeaderNode(head->name, head->value);
    t->next = copyHeaderList(head->next);
    return t;
}

Header *appendHeader(Header *head, Header *node)
{
    if(head == NULL) return node;
    head->next = appendHeader(head->next, node);
    return head;
}

int getMethodType(char *method)
{
    if(strncmp(method,"GET",3)==0) return GET;
    if(strncmp(method,"POST",4)==0) return POST;
    if(strncmp(method,"HEAD",4)==0) return HEAD;
    printf("[SERVER] Bad request -> Unsupported/Wrong HTTP version %s from client\n", method);
    return UNSUPPORTED_METHOD;
}

int getVersionType(char *ver)
{
    if(strncmp(ver,"HTTP/1.0",8)==0) return HTTP10;
    if(strncmp(ver,"HTTP/1.1",8)==0) return HTTP11;
    printf("[SERVER] Bad request -> Unsupported/Wrong HTTP version %s from client\n", ver);
    return UNSUPPORTED_VER;    
}

char *methodToString(int method)
{
    if(method == GET) return "GET";
    if(method == POST) return "POST";
    if(method == HEAD) return "HEAD";

    return "UNSUPPORTED_METHOD";
}

char *versionToString(int version)
{
    if(version == HTTP10) return "HTTP/1.0";
    if(version == HTTP11) return "HTTP/1.1";
    return "UNSUPPORTED_VER";
}

Request *http_req_parser(const char *str)
{
    Request *req = (Request*)malloc(sizeof(Request));
    if(req==NULL) return NULL;req->headers = NULL;req->uri = NULL;req->body = NULL;
    int firstLine = 0, l,t;
    int error = 0;
    while(*str)
    {

        l = strcspn(str,"\r\n");
        if(l==0)
        {
            if(firstLine == 0){error=1; break;} 
            req->body = duplicate(str,0);
            break;
        }
        char *line = duplicate(str,l);
        char *ttt = line;
        if(firstLine==0)
        {
            t = strcspn(line," ");req->method = getMethodType(line); line += t+1; if(req->method == UNSUPPORTED_METHOD){error=1; break;}
            t = strcspn(line," ");line[t]=0; req->uri = parseURI(line);line += t+1;
            t = strcspn(line," ");req->http_ver = getVersionType(line);line += t+1; if(req->method == UNSUPPORTED_VER){error=1; break;}
            firstLine = 1;
        }
        else
        {
            t = strcspn(line,":"); line[t] = 0;
            Header *temp = makeHeaderNode(line, line+t+1);
            req->headers = appendHeader(req->headers, temp);
        }
        free(ttt);
        str += l+1;
        if(*str == '\n') str++;
    }
    
    if(error != 0) {free_req(req);return NULL;} 
    return req;
}

Response *http_res_builder(int http_ver, int code, char *reason, Header *head, char*body, FILE*fp)
{
    Response *res = (Response*)malloc(sizeof(Response));
    if(res==NULL) return NULL; res->reason = NULL; res->headers =NULL; res->body = NULL;
    res->http_ver = http_ver;
    res->code = code;
    res->reason = reason;
    res->headers = head;
    res->body = body;
    res->fp = fp;
    return res;
}

char *requestToString(Request *req)
{
    char * requestString = (char*)malloc(BUFFER_SIZE);
    memset(requestString,0,BUFFER_SIZE);

    sprintf(requestString,"%s %s %s\n", methodToString(req->method), req->uri->url, versionToString(req->http_ver));
    Header *itr = req->headers;
    while(itr!=NULL)
    {
        sprintf(requestString + strlen(requestString),"%s: %s\n", itr->name, itr->value);
        itr = itr->next;
    }
    sprintf(requestString + strlen(requestString),"%s", "\n");
    if(req->body)
    sprintf(requestString + strlen(requestString),"%s",req->body);
    return requestString;
}

char *responseToString(Response *res)
{
    char * responseString = (char*)malloc(BUFFER_SIZE);   
    memset(responseString,0,BUFFER_SIZE);
    sprintf(responseString,"%s %d %s\n", versionToString(res->http_ver), res->code, res->reason);
    Header *itr = res->headers;
    while(itr!=NULL)
    {
        sprintf(responseString + strlen(responseString),"%s: %s\n", itr->name, itr->value);
        itr = itr->next;
    }
    sprintf(responseString + strlen(responseString),"\n%s", res->body);
    return responseString;
}

char *httpHeaderToString(Response *res)
{
    char * responseString = (char*)malloc(BUFFER_SIZE);   
    memset(responseString,0,BUFFER_SIZE);
    sprintf(responseString,"%s %d %s\n", versionToString(res->http_ver), res->code, res->reason);
    Header *itr = res->headers;
    while(itr!=NULL)
    {
        sprintf(responseString + strlen(responseString),"%s: %s\n", itr->name, itr->value);
        itr = itr->next;
    }
    sprintf(responseString + strlen(responseString),"%s", "\n");
    return responseString;
}

FILE * isResourseExits(char *filename)
{
    char file_path[100];
    sprintf(file_path,"%s%s",SITE_ROOT,filename);
    FILE *f = fopen(file_path, "r+");if(f==NULL) return NULL;
    return f;
}

char *readFileFromFilePointer(FILE *f)
{
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *string = (char*)malloc(fsize + 1);
  fread(string, 1, fsize, f);
  fclose(f);
  string[fsize] = 0;
  return string;
}

char *readFile(char *filename)
{
   FILE *fp = isResourseExits(filename);
   if(fp == NULL) return NULL;
   return readFileFromFilePointer(fp);
}

char *res_404()
{
    return readFile("/404Error.html");
}

char *readResourceFile(char *filename)
{
    char file_path[100];
    sprintf(file_path,"%s%s",SITE_ROOT,filename);
    char *str = readFile(file_path);
    return str==NULL?res_404():str;
}

int chatLogWrite(char *text, int append)
{
    if(strlen(text) == 0) return -1;
    FILE *f = fopen(CHAT_LOGS, (append==1)?"a":"w");if(f==NULL) return -1;
    fprintf(f, "%s\n",text);
    fclose(f);
    return 1;
}

int writeInFile(char *filename, char *text, int append)
{
    char file_path[100];
    sprintf(file_path,"%s%s",SITE_ROOT,filename);
    FILE *f = fopen(file_path, (append==1)?"a":"w");if(f==NULL) return -1;
    fprintf(f, "%s",text);
    fclose(f);
    return 1;
}

char * chatLogRead(int n) 
{ 
    FILE *in = fopen(CHAT_LOGS, "r");if(in==NULL) return NULL;
    int count = 0; unsigned long long pos; 
    char *data = (char*)malloc(50*(n+1));memset(data, 0, (50*(n+1)));char str[50];

    if (fseek(in, 0, SEEK_END))
        {
        perror("fseek() failed");
        return NULL;
        }
    else
    {
        pos = ftell(in);
        while (pos)
        {
            if (!fseek(in, --pos, SEEK_SET))
            {
                if (fgetc(in) == '\n')
                    if (count++ == n)
                        break;
            }
            else
                perror("fseek() failed");
        }
    }
        while (fgets(str, sizeof(str), in))
        sprintf(data+strlen(data), "%s\n", str);
        fclose(in);
        return data;
} 

void free_req(Request *req)
{
    if(req == NULL) return;
    free_uri(req->uri);
    free_header(req->headers);
    free(req->body);
    free(req);
}

void free_res(Response *res)
{
    // printf("in res \n");
    if(res == NULL) return;
    free(res->reason);
    free_header(res->headers);
    // printf("in body \n");

    if(res->body) free(res->body);
    free(res);
}

// Response *generateResponseObjFromCode(int code, )
// {
//     char *reason, *content;char filename[20];
//     sprintf(filename,"/%dError.html",code);
//     content=readFile(filename);

//     Header *head = NULL;
//     head = appendHeader(head,makeHeaderNode("Content-Type",mime_type_get(req->uri->url)));
//     char len[5] = {0};
//     sprintf(len,"%ld",(content!=NULL)?strlen(content):0);
//     head = appendHeader(head,makeHeaderNode("Content-Length",len));
//     Response *res =http_res_builder(req->http_ver, code, reason, head, content,fp);

// }

char * readResourcePOST(URI *uri, char * body)
{
    if (strstr(uri->url,"chat/logs")!= NULL) 
    {
        chatLogWrite(body, 1);
        char *buff = chatLogRead(10);
        return buff;
    }
    if (strstr(uri->url,"game/logs")!= NULL) 
    {
        myThread_mutex_lock(&gameLock);
        writeInFile(uri->url, body, 0);
        char *num = readFile(uri->url);
        myThread_mutex_unlock(&gameLock);
        return num;
    }
    char *content = readFile(uri->url);
    return content;
}

char * readResourceGET(URI *uri)
{
    if (strstr(uri->url,"game/logs")!= NULL)
    {
        myThread_mutex_lock(&gameLock);
        char *num = readFile(uri->url);
        myThread_mutex_unlock(&gameLock);
        return num;
    }
    if (strstr(uri->url,"/hello")!= NULL)
    {
        char *str =(char *)malloc(50);
        sprintf(str,"%s", "Hello From Server");
        return str; 
    }

    return readFile(uri->url);
}

char * readResourceHEAD(URI *uri)
{
    if (strstr(uri->url,"chat/logs")!= NULL)
    {
        chatLogWrite("New Chat Session \n", 0);
        return "";
    }
    if (strstr(uri->url,"game/logs")!= NULL)
    {
        myThread_mutex_lock(&gameLock);
        writeInFile(uri->url, "Initialize With 1", 0);
        myThread_mutex_unlock(&gameLock);
        return "";
    }

    return isResourseExits(uri->url)==NULL?NULL:"";
}

Response *getResponseObj(Request *req)
{
    char *reason, *content;int code;
    // printf("%s \n", methodToString(req->method));
    if(req->method == GET)
    {
        content = readResourceGET(req->uri);
    }
    else if(req->method == POST)
    {
        content = readResourcePOST(req->uri, req->body);
    }
    else if(req->method == HEAD)
    {
        content = readResourceHEAD(req->uri);
    }
    
    FILE *fp = isResourseExits(req->uri->url);
    if(content != NULL)
    {
         code = 200;
         reason = duplicate("OK",0);
         if(req->method == HEAD) content = NULL;
    }
    else
    {
        code = 404;
        reason = duplicate("Not Found",0);
        content = res_404();
        free(req->uri->url); 
        req->uri->url = duplicate(".html",0);
        fp = isResourseExits("/404Error.html");
    }
    Header *head = NULL;
    head = appendHeader(head,makeHeaderNode("Content-Type",mime_type_get(req->uri->url)));
    char len[5] = {0};
    sprintf(len,"%ld",(content!=NULL)?strlen(content):0);
    head = appendHeader(head,makeHeaderNode("Content-Length",len));
    Response *res =http_res_builder(req->http_ver, code, reason, head, content,fp);
    return res;
}
