// arubasyslog v0.16 by GM
// changelog

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

#define BUFMSG 1000
#define SSIDLEN 64
#define NTHREAD 256
#define FILECONFIG "/arubasyslog/arubasyslog.conf"
#define MAXCLIENT 10
#define IPTOT 1048576
#define IPCLASS 0b00001010001000000000000000000000 // 10.32.0.0
#define IPTEST  0b00001010001011111111111111111111 // 10.47.255.255
#define IPMASK  0b11111111111100000000000000000000 // 255.240.0.0
 
struct arg_pass {
	char *mesg;
	int lenmesg;
	struct sockaddr_in cliaddr;
};
pthread_t *tid;
int sockfd,myprio,totclient;
unsigned long ipclient[MAXCLIENT];
char ssidtolog[SSIDLEN];
pthread_mutex_t lock;
time_t *ip_last;
FILE *fp;

// search function
char *mysearch(char *ptr,char *end,char delimiter){
	char *aux;
	aux=ptr;
	while(aux<end){
		if(*aux==delimiter)break;
		aux++;
	}
	if(aux>=end)return NULL;
	if(*aux!=delimiter)return NULL;
	*aux='\0';
	return aux+1;
}

// main process
void *manage(void *arg_void){
	struct arg_pass *myarg=(struct arg_pass *)arg_void;
	int i,priority;
	char *aux,*auxmax,buf[128],buf2[128];
	char *cpriority;
	char *aux1,*essid,*mac,*recv_sta;
	uint32_t ip_tocheck,ipsrcaddr,ipdstaddr;
	unsigned long ipidx;
	time_t now;
	struct sockaddr_in netip;
	
	// check address
	ip_tocheck=ntohl(myarg->cliaddr.sin_addr.s_addr);
	for(i=0;i<totclient;i++)if(ipclient[i]==ip_tocheck)break;
	if(i==totclient)return NULL;
		
	// parsing <PRI>
	aux=myarg->mesg;
	auxmax=aux+myarg->lenmesg;
	aux=mysearch(aux,auxmax,'<');
	if(aux==NULL)return NULL;
	cpriority=aux;
	aux=mysearch(aux,auxmax,'>');
	if(aux==NULL)return NULL;
	priority=atoi(cpriority);
	if(priority!=myprio)return NULL;
	
	// looking for essid presence
	aux1=strstr(aux,"essid");
	if(aux1==NULL)return NULL;
	essid=mysearch(aux1,auxmax,'-');
	if(essid==NULL)return NULL;
	aux1=mysearch(aux1,auxmax,'.');
	if(aux1==NULL)return NULL;
	if(!strcasecmp(essid,ssidtolog))return NULL;
	
	// looking for mac
	aux1=strstr(aux,"mac");
	if(aux1==NULL)return NULL;
	mac=mysearch(aux1,auxmax,'-');
	if(essid==NULL)return NULL;
	aux1=mysearch(aux1,auxmax,' ');
	if(aux1==NULL)return NULL;
	
	// looking for message
	recv_sta=strstr(aux,"recv_sta_");
	if(recv_sta==NULL)return NULL;
	aux1=mysearch(recv_sta,auxmax,':');
	if(aux1==NULL)return NULL;

printf("%s %lu %s %s\n",essid,ip_tocheck,mac,recv_sta);
	
return NULL;
	
	
	
}

int main(int argc, char**argv){
	struct arg_pass *myargs;
	unsigned long i,j;
	int listenport;
	struct sockaddr_in servaddr,auxaddr;
	socklen_t len;
	char buf[128];
	time_t now;
	
	// initialization
	tid=(pthread_t *)malloc(NTHREAD*sizeof(pthread_t));
	myargs=(struct arg_pass *)malloc(NTHREAD*sizeof(struct arg_pass));
	for(i=0;i<NTHREAD;i++)myargs[i].mesg=(char *)malloc(BUFMSG*sizeof(char));
	pthread_mutex_init(&lock,NULL);
	ip_last=(time_t *)malloc(sizeof(time_t)*IPTOT);
	for(i=0;i<IPTOT;i++)ip_last[i]=0;
	
	// parse configuration
	fp=fopen(FILECONFIG,"rt");
	fscanf(fp,"%d %d %s %d",&listenport,&myprio,ssidtolog,&totclient);
	for(i=0;i<totclient;i++){
		fscanf(fp,"%s",buf);
		inet_pton(AF_INET,buf,&(auxaddr.sin_addr));
		ipclient[i]=ntohl(auxaddr.sin_addr.s_addr);
	}
	fclose(fp);
	
	// log file open
	now=time(NULL);
	strftime(buf,128,"/arubasyslog/log/%Y%m%d.log",localtime(&now));
	fp=fopen(buf,"a+");
	
	// bindind
	sockfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	memset((char *)&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(listenport);
	bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	len=sizeof(struct sockaddr_in);
	
	for(j=0;;){
		// receive request and launch a processing thread
		myargs[j].lenmesg=recvfrom(sockfd,myargs[j].mesg,BUFMSG,0,(struct sockaddr *)&myargs[j].cliaddr,&len);
		myargs[j].mesg[myargs[j].lenmesg]='\0';
		pthread_create(&(tid[j]),NULL,&manage,&myargs[j]);
		pthread_detach(tid[j]);
		if(++j==NTHREAD)j=0;
	}
}
