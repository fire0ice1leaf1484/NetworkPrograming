#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#define BUF_SIZE 256
#define MONEY_DIGIT_SIZE 10

void DieWithError(char *);
void my_scanf(char *, int);
void commun(int);
void read_until_delim(int, char *, char, int);

int prepare_client_socket(char *, int);

int main(int argc, char *argv[])
{
    if (argc != 3)
        DieWithError("usage: ./client ip_address port");

    int sock = prepare_client_socket(argv[1], atoi(argv[2]));

    commun(sock);

    close(sock);

    return 0;
}

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

int prepare_client_socket(char *ipaddr, int port)
{
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        DieWithError("socket() failed");

    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_addr.s_addr = inet_addr(ipaddr);
    target.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *)&target, sizeof(target)) < 0)
        DieWithError("connect() failed");

    return sock;
}

void my_scanf(char *buf, int num_letter)
{
    char format[20];
    sprintf(format, "%s%d%s", "%", num_letter, "s%*[^\n]");
    scanf(format, buf);
    getchar();
}

void read_until_delim(int sock, char *buf, char delimiter, int max_length)
{
    int len_r = 0;
    int index_letter = 0;

    while (index_letter < max_length - 1)
    {
        if ((len_r = recv(sock, buf + index_letter, 1, 0)) <= 0)
            DieWithError("recv() failed");
        if (buf[index_letter] == delimiter)
            break;
        else
            index_letter++;
    }
    buf[index_letter] = '\0';
}

void commun(int sock)
{
    char cmd[2] = "";
    char withdraw[MONEY_DIGIT_SIZE + 1];
    char deposit[MONEY_DIGIT_SIZE + 1];
    char msg[BUF_SIZE];

    printf("0:引き出し　1:預け入れ　2:残高照会　9:終了\n");
    printf("何をしますか？ > ");
    my_scanf(cmd, 1);

    switch (cmd[0])
    {
    case '0':
        printf("引き出す金額を入力してください > ");
        my_scanf(withdraw, MONEY_DIGIT_SIZE);

        sprintf(msg, "0_%s_", withdraw);
        break;
    case '1':
        printf("預け入れる金額を入力してください > ");
        my_scanf(deposit, MONEY_DIGIT_SIZE);

        sprintf(msg, "%s_0_", deposit);
        break;
    case '2':
        strcpy(msg, "0_0_");
        break;
    default:
        printf("番号が確認できませんでした。\n");
        return;
    }

    if (send(sock, msg, strlen(msg), 0) != strlen(msg))
        DieWithError("send() sent a message of unexpected bytes");

    read_until_delim(sock, msg, '_', BUF_SIZE);
    printf("残高は%d円になりました", atoi(msg));
}