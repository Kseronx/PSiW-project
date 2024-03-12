#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define linijkiS 100
#define kolumnyS 2
char user[100];
char nazwafifo2[100];
char buf[1024];
char buf1[1024];
char buf2[1024];
char buf3[1024];
int fd1;
int fd2;
int fd3;
int fd4;
int n;

int load_data(char* filename, char data[linijkiS][kolumnyS][100]) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return -1;
    }
    char buffer[1];
    char line[100];
    int linijki = 0;
    int kolumny = 0;
    int len = 0;
    while (read(fd, buffer, 1) > 0) {
        if (buffer[0] == ':') {
            strcpy(data[linijki][kolumny], line);
            kolumny++;
            len = 0;
        } else if (buffer[0] == '\n') {
            strcpy(data[linijki][kolumny], line);
            linijki++;
            kolumny = 0;
            len = 0;
        } else {
            line[len] = buffer[0];
            len++;
        }
    }

    if (errno != 0) {
        perror("Error reading file");
        close(fd);
        return -1;
    }
    if(close(fd)==-1){
        perror("Blad zamkniecia kolejki drugiej");
		exit(1);
	}
    return linijki;
}

int main(int argc, char const *argv[]) {
    char data[linijkiS][kolumnyS][100];
    int linijkis = load_data("konfiguracja.txt", data);
    if (linijkis == -1) {
        return -1;
    }
    if (argc < 2)
    {
        perror("Nie wystarczająco argumentów\n");
        exit(1);
    }
    char* nazwafifo;
    for(int i=0;i<linijkis;i++){
    	if( strcmp( argv[1], data[i][0] ) == 0 ){	
    		nazwafifo = data[i][1];
    	}
    }
    if(mkfifo(nazwafifo, 0600)==-1){
    	perror("Tworzenie kolejki fifo globalnej");
    	exit(1);
    	}
while(1){
    pid_t child_pid = fork();
    if (child_pid == 0)
    {
    
    
    
    fd1 = open(nazwafifo, O_RDONLY);
    if (fd1 == -1){
		perror("Otwarcie potoku do zapisu 1");
		exit(1);
	}
    printf("odeslano wyniki");
    int n1;
    n1 = read(fd1, buf2, sizeof buf2);
    n1 = read(fd1, buf3, sizeof buf3);
    if(close(fd1)==-1){
        perror("Blad zamkniecia fd1");
		exit(1);
        }
    printf("do z kolejki globalnej odczytano nazwe: %s oraz komende: %s", buf2, buf3);
    close(1);
    fd4 = open(buf2, O_WRONLY);
    if (fd2 == -1){
		perror("Otwarcie potoku do zapisu 2");
		exit(1);
	}
   
    char *ex[4];
    ex[0] = "sh";
    ex[1] = "-c";
    ex[2] = buf3;
    ex[3] = NULL;
    execvp("/bin/sh", ex);
    if(close(fd4)==-1){
        perror("Blad zamkniecia fd4");
		exit(1);
    }
    dup(1);
    }
    else if(child_pid == -1){
    perror("Blad utworzenia procesu potomnego");
    exit(1);
    }
    else{ 
        // zakonczenie jak wpiszemy -1 "" ""
        printf("Podaj nazwe procesu, komende oraz nazwe kolejki fifo:\n");
        scanf("%s \"%[^\"]\" \"%[^\"]\"", user, buf, nazwafifo2);        
        char* fifo2 = nazwafifo2;
        
        if(strcmp(user,"-1")==0){
            unlink(nazwafifo);
            raise(SIGINT);
        }
        if(mkfifo(fifo2, 0600)==-1){
    		perror("Tworzenie kolejki fifo 2");
    		exit(1);
    	}
        printf("%s",fifo2);
        char* fifo3;
        for(int i=0;i<linijkis;i++){
    	if( strcmp( user, data[i][0] ) == 0 ){	
    		fifo3 = data[i][1];
    	    }
        }
        printf("%s",fifo3);
        
        
        fd2 = open(fifo3, O_WRONLY);
        if (fd2 == -1){
		perror("Otwarcie potoku do zapisu fifo3");
		exit(1);
	    }
        if(write(fd2, fifo2, strlen(fifo2))==-1){
        	perror("Blad zapisu do kolejki drugiego procesu nazwy kolejki lokalnej");
		    exit(1);
	    }
        sleep(1);
        if(write(fd2, buf, strlen(buf))==-1){
        	perror("Blad zapisu pliku docelowego 1");
		    exit(1);
        }
        if(close(fd2)==-1){
        	perror("Blad zamkniecia kolejki drugiej");
		    exit(1);
	    }
        
        
        
        fd3 = open(fifo2, O_RDONLY);
        if (fd3 == -1){
		    perror("Otwarcie kolejki lokalnej fifo2");
		    exit(1);
	    }
        n = read(fd3, buf1, sizeof buf1);
        printf("%s\n", buf1);
        if(close(fd3)==-1){
        	perror("Blad zamkniecia kolejki fifo lokalnej");
		    exit(1);
	    }
        unlink(fifo2);

	    }    
    }
    return 0;
}
    
    
    
    
