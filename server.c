#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdbool.h>

/* portul folosit */
#define PORT 2908

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData
{
    int idThread; // id-ul thread-ului tinut in evidenta de acest program
    int cl;		  // descriptorul intors de accept
} thData;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);
bool functii(char *, void *);

int i, j;
char tabla[17][17][5], text[1000]; // text - matricea trimisa de catre server catre client

int main()
{
    start();
    print();

    struct sockaddr_in server; // structura folosita de server
    struct sockaddr_in from;

    char cmd[100]; // comanda primita de la client

    int sd; // descriptorul de socket
    int pid;
    pthread_t th[100]; // Identificatorii thread-urilor care se vor crea
    int i = 0;

    /* crearea unui socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[server]Eroare la socket().\n");
        return errno;
    }
    /* utilizarea optiunii SO_REUSEADDR */
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    /* pregatirea structurilor de date */
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons(PORT);

    /* atasam socketul */
    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server]Eroare la bind().\n");
        return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen(sd, 4) == -1)
    {
        perror("[server]Eroare la listen().\n");
        return errno;
    }
    /* servim in mod concurent clientii...folosind thread-uri */
    while (1)
    {
        int client;
        thData *td; // parametru functia executata de thread
        socklen_t length = sizeof(from);

        printf("[server]Asteptam la portul %d...\n", PORT);
        fflush(stdout);

        // client= malloc(sizeof(int));
        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
        {
            perror("[server]Eroare la accept().\n");
            continue;
        }

        /* s-a realizat conexiunea, se astepta mesajul */

        // int idThread; //id-ul threadului
        // int cl; //descriptorul intors de accept

        td = (struct thData *)malloc(sizeof(struct thData));
        td->idThread = i++;
        td->cl = client;

        pthread_create(&th[i], NULL, &treat, td);
    }
}

void static *treat(void *arg)
{
    pthread_detach(pthread_self());
    while (1)
    {
        struct thData tdL;
        tdL = *((struct thData *)arg);
        printf("[thread]- %d - Introducere mesaj\n", tdL.idThread); // Login or register a new account
        fflush(stdout);
        raspunde((struct thData *)arg);
    }
    /* am terminat cu acest client, inchidem conexiunea */
    // close((intptr_t)arg);
    fflush(stdout);
    return (NULL);
}

void raspunde(void *arg)
{
    int i = 0;
    char cmd[100];
    struct thData tdL;
    tdL = *((struct thData *)arg);
    bzero(cmd, 100);
    if (read(tdL.cl, cmd, 100) <= 0)
    {
        printf("[Thread %d]\n", tdL.idThread);
        perror("Eroare la read() de la client.\n");
    }
    printf("[Thread %d]Comanda a fost receptionata...%s\n", tdL.idThread, cmd);

    /*pregatim mesajul de raspuns */
    printf("[server]Trimitem mesajul inapoi...%s\n", cmd);

    /* returnam mesajul clientului */
    if (strcmp(cmd, "disconnect\n") == 0) // trb parse cmd
        disconnect(arg);
    else if (strcmp(cmd, "board\n") == 0)
        board(arg);
	else if (strcmp(cmd, "rules\n") == 0)
        rules(arg);
	else if (strcmp(cmd, "rules\n") == 0)
        dice(arg);
    else if (write(tdL.cl, cmd, 100) <= 0)
    {
        printf("[Thread %d] ", tdL.idThread);
        perror("[Thread]Eroare la write() catre client.\n");
    }
    else
        printf("[Thread %d]Mesajul a fost transmis cu succes.\n", tdL.idThread);
}

// bool functii(char * comanda,void *arg)
// {
//     struct thData tdL;
// 	tdL = *((struct thData *)arg);
//     if(strcmp(comanda,"disconnect\n")==0)//disconnect
// 		disconnect(arg);
//     else
//     if(strcmp(comanda,"move\n")==0)
//         move();return 1;
//     else if(strcmp(comanda,"leave\n")==0)
//         leave();return 1;
//     else if(strcmp(comanda,"dice\n")==0)
//         dice();return 1;
//     else if(strcmp(comanda,"board\n")==0)
//         board();return 1;
// 	else return 0;
// }
void disconnect(void *arg)
{
    struct thData tdL;
    tdL = *((struct thData *)arg);
    write(tdL.cl, "disconnect\n", 12);
    if (pthread_cancel(pthread_self()))
        printf("Failed to cancel the thread\n");
}
void move(void *arg)
{
	struct thData tdL;
    tdL = *((struct thData *)arg);
}
void leave(void *arg)
{
	struct thData tdL;
    tdL = *((struct thData *)arg);

}
void rules(void *arg)
{
	struct thData tdL;
    tdL = *((struct thData *)arg);
	char reguli[1000];
    strcpy(reguli,"REGULI\nFiecare jucator are 4 pioni de culoarea stabilita la inceputul jocului. Fiecare jucator da cu zarul (comanda dice).\nNumarul maxim de aruncari consecutive cu zarul este de 3. Exista 3 situatii in care jucatorul primeste o aruncare bonus: cand arunca 6, cand 'mananca' pionul altui jucator si \ncand ajunge cu un pion in baza (notata cu f pe tabla de joc). Daca jucatorul da 6, poate scoate un pion din baza de start. Acesta poate selecta prin \ncomanda leave daca vrea sa scoata un pion din baza si prin comanda move 'nr_pion' daca vrea sa mute un pion aflat pe board. Daca un pion este 'mancat' \nse intoarce inapoi in baza initiala. Pentru a ajunge la final (f), pionul trebuie sa se mute exact numarul de pozitii pana in f. Daca un pion se afla in\nuna din pozitiile notate cu (v,g,r,a) este in siguranta. Acum mai trebuie doar sa dea zarul exact pentru a ajunge in f.\n");
    write(tdL.cl, reguli, 1000);
}
int dice(void *arg)
{
    srand(time(NULL));
    return rand() % 6 + 1;
}
void board(void *arg)
{
    struct thData tdL;
    tdL = *((struct thData *)arg);
    write(tdL.cl, text, 1000);
}
// void send_for_all(void* arg)
// {
// 	struct thData tdL;
//     tdL = *((struct thData *)arg);
// 	for(int i=0;i<=)
// }

// functii pentru generare matrice
void start()
{
    int i, j;
    for (i = 0; i <= 16; i++)
        for (j = 0; j <= 16; j++)
            if (i == 0 || j == 0 || i == 16 || j == 16)
                strcpy(tabla[i][j], "= ");														   // border
            else if (i == 3 && j == 3 || i == 3 && j == 4 || i == 4 && j == 3 || i == 4 && j == 4) // baza albastru
                strcpy(tabla[i][j], "AA");
            else if (i == 3 && j == 12 || i == 3 && j == 13 || i == 4 && j == 12 || i == 4 && j == 13) // baza rosu
                strcpy(tabla[i][j], "R ");
            else if (i == 12 && j == 3 || i == 12 && j == 4 || i == 13 && j == 3 || i == 13 && j == 4) // baza galben
                strcpy(tabla[i][j], "G ");
            else if (i == 12 && j == 12 || i == 12 && j == 13 || i == 13 && j == 12 || i == 13 && j == 13) // baza verde
                strcpy(tabla[i][j], "V ");
            else if (i == 2 && j == 9)
                strcpy(tabla[i][j], "v "); // safespot
            else if (i == 7 && j == 2)
                strcpy(tabla[i][j], "> "); // safespot
            else if (i == 9 && j == 14)
                strcpy(tabla[i][j], "< "); // safespot
            else if (i == 14 && j == 7)
                strcpy(tabla[i][j], "^ "); // safespot
            else if (i == 7 && j >= 1 && j <= 15 && j != 7 && j != 8 && j != 9)
                strcpy(tabla[i][j], "* ");
            else if (i == 9 && j >= 1 && j <= 15 && j != 7 && j != 8 && j != 9)
                strcpy(tabla[i][j], "* ");
            else if (j == 7 && i >= 1 && i <= 15 && i != 7 && i != 8 && i != 9)
                strcpy(tabla[i][j], "* ");
            else if (j == 9 && i >= 1 && i <= 15 && i != 7 && i != 8 && i != 9)
                strcpy(tabla[i][j], "* ");
            else if (i == 8 && j >= 2 && j <= 6)
                strcpy(tabla[i][j], "a ");
            else if (i == 8 && j >= 10 && j <= 14)
                strcpy(tabla[i][j], "v ");
            else if (j == 8 && i >= 2 && i <= 6)
                strcpy(tabla[i][j], "r ");
            else if (j == 8 && i >= 10 && i <= 14)
                strcpy(tabla[i][j], "g ");
            else
                strcpy(tabla[i][j], "  ");

    strcpy(tabla[8][1], "* ");
    strcpy(tabla[1][8], "* ");
    strcpy(tabla[8][15], "* ");
    strcpy(tabla[15][8], "* ");
    strcpy(tabla[8][7], "f ");
    strcpy(tabla[7][8], "f ");
    strcpy(tabla[8][9], "f ");
    strcpy(tabla[9][8], "f ");
    strcpy(tabla[8][7], "f ");
}
void print()
{
    for (i = 0; i <= 16; i++)
    {
        for (j = 0; j <= 16; j++)
        {
            strcat(text, tabla[i][j]);
            strcat(text, " ");
            printf("%s ", tabla[i][j]);
        }
        strcat(text, "\n");
        printf("\n");
    }
    printf("%s", text);
    return 0;
}
