/* An imap proxy.
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>

/* #include "ssl.h" */
/* #include "ykbuf.h" */

/* extern "C" int errno; */

int use_ssl = 1;

int cli_fd, serv_fd;


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

static FILE* server_log_file = NULL;
static FILE* server_file = NULL;
static int server_done = 0;

static void child_handler(int signum)
{
    switch(signum) {
    case SIGALRM: fprintf(server_log_file,"SIGALRM\n"); break;
    case SIGUSR1: fprintf(server_log_file,"SIGUSR1\n"); break;
    case SIGUSR2: fprintf(server_log_file,"SIGUSR2\n"); break;
    case SIGCHLD: fprintf(server_log_file,"SIGCHLD\n"); break;
    case SIGINT: fprintf(server_log_file,"SIGINT\n"); break;
    case SIGHUP: fprintf(server_log_file,"SIGHUP\n"); break;
    case SIGTERM: fprintf(server_log_file,"SIGTERM\n"); break;
    case SIGIO: fprintf(server_log_file,"SIGIO\n"); break;
    case SIGPIPE: fprintf(server_log_file,"SIGPIPE\n"); break;
    default: fprintf(server_log_file,"unknown signal\n");
    }
    fflush(server_log_file);

    server_done = 1;

/*     close(cli_fd); */
    
/*     // drain remaining data in serv_fd */
/*     fprintf(server_log_file,"Draining remaining data from socket.\n"); */
/*     fflush(server_log_file); */
/*     fclose(server_log_file); */
    
/*     fcntl(serv_fd,F_SETFL,O_NONBLOCK); */
/*     char buf[1001]; */
/*     int bytes_read = read(serv_fd, buf, 1000);  */
/*     while (bytes_read > 0) { */
/*       buf[bytes_read] = 0; */
/*       fprintf(server_file,"%s",buf); */
/*       bytes_read = read(serv_fd, buf, 1000);  */
/*     } */
/*     fflush(server_file); */
/*     fclose(server_file); */
/*     close(serv_fd); */

/*     exit(EXIT_FAILURE); */
}

typedef const char *string_t;

static void
insecure_proxy(int connfd, string_t host, int port) {

  char name[1000];

  // open local dump files
  int t = time(NULL);
  int sfile = sprintf(name,"dumps/server.%d.dump",t);
  server_file = fopen(name, "w");

  // set global client fd.
  cli_fd = connfd;

  /* Open connection to server */
  struct hostent *he;
  struct servent *se;
  struct protoent *pe;
  struct sockaddr_in sin;

  char buf[10000];
  int bytes, b;

  he = gethostbyname(host);
  if (!he) {
    fprintf(stderr, "unknown host - %s\n", host);
    exit(1);
  }
  se = getservbyname("telnet","tcp");
  if (!se) {
    fprintf(stderr, "unknown service\n");
    exit(1);
  }
  pe = getprotobyname(se->s_proto);
  if (!pe) {
    fprintf(stderr, "unknown protocol\n");
    exit(1);
  }
  bzero(&sin,sizeof(sin));
  sin.sin_family = (sa_family_t)he->h_addrtype;

  int fd = socket(he->h_addrtype, SOCK_STREAM, pe->p_proto);
  if (fd < 0) perror("socket");

  if (bind(fd, &sin, sizeof(sin)) < 0) perror("bind");
  bcopy(he->h_addr, &sin.sin_addr, he->h_length);

  sin.sin_port = htons((unsigned short)port);

  fprintf(stderr,"Connecting to server %s:%d... ",host,port);
  if (connect(fd, &sin, sizeof(sin)) < 0) {
    perror("connect");
    exit(1);
  }
  fprintf(stderr,"done\n");

  // set global server fd
  serv_fd = fd;

  ssize_t bytes_read;
  ssize_t bytes_written;
//   ykbuf_t @servykb = fd2ykbuf(fd);
  /* Read greeting from server and echo to client */
  bytes_read = read(serv_fd, buf, 1); 
  if (bytes_read == -1){
    perror("error reading greeting from server");
    close(serv_fd);
    close(cli_fd);
    fflush(server_file);
    exit(1);
  }
  buf[bytes_read] = 0;
  fprintf(server_file,"%s",buf);
  bytes_written = write(cli_fd,buf,bytes_read);    
  if (bytes_written == -1){
    perror("error echoing greeting to client");
    exit(1);
  }

  // flush the server file before forking so we can immediately close it in the 
  // client-reading process.
  fflush(server_file);

  pid_t parent = getpid();
  pid_t childpid = fork();
  if (childpid != 0) {
//     ykbuf_t @cliykb = fd2ykbuf(connfd);
    fclose(server_file);

    int r = sprintf(name,"dumps/client.%d.dump",t);
    FILE* client_file = fopen(name, "w");
    while (1) {
      /* Read command from client and echo to server */   
      bytes_read = read(cli_fd, buf, 1); 
      if (bytes_read == -1){
	perror("error reading from client");
	close(serv_fd);
	close(cli_fd);
	fflush(client_file);
	fclose(client_file);

	// let child know we're done.
// 	kill( childpid, SIGTERM );
/* 	fprintf(stderr,"Waiting (%d) for child (%d) to die ...\n",parent,childpid);fflush(stderr); */
/* 	int status = 0; */
/* 	pid_t wpid = waitpid(childpid,&status,WNOHANG); */
/* 	if ( wpid != childpid ){ */
/* 	  fprintf(stderr,"No such child: %d.\n",wpid);fflush(stderr); */
/* 	} */

	exit(0);
      }
      buf[bytes_read] = 0;
      fprintf(client_file,"%s",buf);

      bytes_written = write(serv_fd,buf,bytes_read);    
      if (bytes_written == -1){
	perror("error echoing data to server");
	close(serv_fd);

	// drain remaining data in cli_fd
	fprintf(stderr,"Draining remaining data from socket.\n");fflush(stderr);
	fcntl(cli_fd,F_SETFL,O_NONBLOCK);
	bytes_read = read(cli_fd, buf, 1000); 
	while (bytes_read > 0) {
	  buf[bytes_read] = 0;
	  fprintf(client_file,"%s",buf);
	  bytes_read = read(cli_fd, buf, 1000); 
	}
	fflush(client_file);
	fclose(client_file);
	close(cli_fd);

	exit(0);
      }
    }
  }
  else {
    int r = sprintf(name,"dumps/server.%d.log",t);
    server_log_file = fopen(name, "w");

    fprintf(server_log_file,"starting echo ...\n");
    fflush(server_log_file);
    /* Trap signals that we expect to recieve */
    signal(SIGCHLD,child_handler);
    signal(SIGUSR1,child_handler);
    signal(SIGUSR2,child_handler);
    signal(SIGALRM,child_handler);
    signal(SIGTERM,child_handler);
    signal(SIGHUP,child_handler);
    signal(SIGINT,child_handler);
    signal(SIGIO,child_handler);
    signal(SIGPIPE,child_handler);

    while (!server_done) {
      /* Read response from server and echo to client */
      bytes_read = read(serv_fd, buf, 1); 
      if (bytes_read == -1){
	const char * emsg = strerror(errno);
	fprintf(server_log_file,"error reading from server: %s.\n",emsg);
	close(serv_fd);
	close(cli_fd);
	fflush(server_log_file);
	fclose(server_log_file);
	fflush(server_file);
	fclose(server_file);
	exit(1);
      } 
      buf[bytes_read] = 0;
      fprintf(server_file,"%s",buf);
      bytes_written = write(cli_fd,buf,bytes_read);    

      if (bytes_written == -1){
	const char* emsg = strerror(errno);
	fprintf(server_log_file,"error echoing data to client: %s.\n",emsg);
	server_done = 1;
      }
    }

    //
    // Server echo is done.
    //

    close(cli_fd);

    // drain remaining data in cli_fd
    fprintf(server_log_file,"Draining remaining data from socket.\n");
    fflush(server_log_file);
    fclose(server_log_file);

    fcntl(serv_fd,F_SETFL,O_NONBLOCK);
    bytes_read = read(serv_fd, buf, 1000); 
    while (bytes_read > 0) {
      buf[bytes_read] = 0;
      fprintf(server_file,"%s",buf);
      bytes_read = read(serv_fd, buf, 1000); 
    }
    fflush(server_file);
    fclose(server_file);
    close(serv_fd);

    exit(0);
  }
}



/* Wait for connection, when it comes, fork a proxy */
static void
server(string_t host, int lport, int port, int use_ssl) {
  int listenfd, connfd;
  pid_t childpid;
  socklen_t clilen;
  struct sockaddr_in cliaddr, servaddr;

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    fprintf(stderr, "Error opening socket\n");
    exit(1);
  }
  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(lport);
  if (bind(listenfd, &servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "Error binding socket\n");
    exit(1);
  }
  if (listen(listenfd, 64) < 0) {
    fprintf(stderr, "Error listening on socket\n");
    exit(1);
  }
  fprintf(stderr, "Proxy started on port %d\n",lport);
  while (1) {
    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, &cliaddr, &clilen);
    if (connfd < 0) {
      fprintf(stderr,"Error on accept\n");
      break;
    }
    if ( (childpid = fork()) == 0) {
      /* client */
      close(listenfd);
      if (use_ssl) 
	printf("SSL not currently supported.\n");
      else
	insecure_proxy(connfd,host,port);
      
      printf("Client is done.\n");
      fflush(stdout);
      exit(0);
    }
    /* server */
    close(connfd);
  }
}

int main(int argc, char** argv) {
  if (argc < 2 || argc > 4) {
    fprintf(stderr, "Usage: %s <ssl imap server> [local port]\n", argv[0]);
    exit(1);
  }

  int lport = 10000;
  int sport = 993;

  if (argc >= 3) {
    if (!strcmp(argv[2],"--no-ssl")){
      fprintf(stderr, "SSL disabled.\n");
      use_ssl = 0;
      sport = 143;
      if (argc == 4) lport = atoi(argv[3]);
    }
    else
      lport = atoi(argv[2]);
  }

  server(argv[1],lport,sport,use_ssl);
  return 0;
}

