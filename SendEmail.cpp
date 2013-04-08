/*
 * File: SendEmail.cpp
 * ----------------
 * Name: Eric Beach
 * Section: [TODO: enter section leader here]
 * Copyright [2013] <Eric Beach>
 * This file implements a class used to send an email.
 * [Originally written by Julie Zelenski]
 *
 * This file lightly linted using
 * http://google-styleguide.googlecode.com/svn/trunk/cpplint/cpplint.py
 */

/*
 * Credit to: http://stackoverflow.com/questions/58210/c-smtp-example
 * I modified this code, put it into a class, and got it to compile.
 *
 * Additional insiration/credit for the DNS resolution:
 * http://stackoverflow.com/questions/1093410/pulling-mx-record-from-dns-server
 * http://stackoverflow.com/questions/258284/srv-record-lookup-with-iphone-sdk
 * http://www.opensource.apple.com/source/mDNSResponder/mDNSResponder-58/mDNSMacOSX/SampleUDSClient.c
 * http://linux.die.net/man/3/inet_ntoa
 */

#include "SendEmail.h"
#include <dns_sd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

// constants
#define MAX_DOMAIN_LABEL 63
#define MAX_DOMAIN_NAME 255
#define MAX_CSTRING 2044
const int T_TXT = 16;
const int T_A = 1;
const int T_MX = 15;

// data structure defs
typedef union { unsigned char b[2]; unsigned short NotAnInteger; } Opaque16;

typedef struct { u_char c[ 64]; } domainlabel;
typedef struct { u_char c[256]; } domainname;


typedef struct
{
    uint16_t priority;
    uint16_t weight;
    uint16_t port;
    domainname target;
} srv_rdata;

//static void print_rdata(int type, int len, const u_char *rdata)
static void print_rdata(int type, int len, const void *rdata)
{
    struct in_addr in;
    
    switch (type)
    {
        case T_MX:
        case T_A:
            memcpy(&in, rdata, sizeof(in));
            printf("%s\n", inet_ntoa(in));
            return;
        default:
            printf("ERROR: I dont know how to print RData of type %d\n", type);
            return;
    }
}


static void query_cb(const DNSServiceRef DNSServiceRef, const DNSServiceFlags flags, const u_int32_t interfaceIndex, const DNSServiceErrorType errorCode, const char *name, const u_int16_t rrtype, const u_int16_t rrclass, const u_int16_t rdlen, const void *rdata, const u_int32_t ttl, void *context)
{
    (void)DNSServiceRef;
    (void)flags;
    (void)interfaceIndex;
    (void)rrclass;
    (void)ttl;
    (void)context;
    
    if (errorCode)
    {
        printf("query callback: error==%d\n", errorCode);
        return;
    }
    printf("query callback - name = %s, rdata=\n", name);
    print_rdata(rrtype, rdlen, rdata);
}

void SendEmail::resolveMxResords() {
    DNSServiceRef sdRef;
    DNSServiceErrorType res;
    
    DNSServiceQueryRecord(
                          &sdRef, 0, 0,
                          "stanford.edu",
                          kDNSServiceType_MX,
                          kDNSServiceClass_IN,
                          query_cb,
                          NULL
                          );
    
    DNSServiceProcessResult(sdRef);
    DNSServiceRefDeallocate(sdRef);
}

/*
 * Send a C-style string to the open socket.
 */
void SendEmail::send_socket(char *s)
{
    write(sock,s,strlen(s));
    write(1,s,strlen(s));
    // printf("Client:%s\n",s);
}

bool SendEmail::setDestinationEmail(string address)
{
    strncpy(to_id, address.c_str(), MAX_BODY_SIZE);
    return true;
}

bool SendEmail::setDestinationName(string name) {
    strncpy(to_name, name.c_str(), MAX_BODY_SIZE);
    return true;
}

string SendEmail::digitToString(int n) {
    return string() + char(n % 10 + '0');
}

string SendEmail::intToString(int n) {
    if (n == 0) {
        return string("0");
    } else if (n < 0) {
        return "-" + intToString(n*-1);
    } else if (n < 10) {
        return digitToString(n % 10);
    } else {
        return intToString(n / 10) + digitToString(n % 10);
    }
}

bool SendEmail::setHashCode(int hCode) {
    hashCode = intToString(hCode);
    return true;
}

bool SendEmail::setNameToHash(string name) {
    nameToHash = name;
    return true;
}

void SendEmail::constructEmailBody() {
    string body = "From: CS106B #1 (Eric Beach) <root@apps-email.info>\r\n"
    "To: " + string(to_name) + " <"
      + string(to_id) + ">\r\n"
    "Content-Type: multipart/alternative; "
    "boundary=047d7b5d59b0ef490904d9825525\r\n"
    "\r\n"
    "--047d7b5d59b0ef490904d9825525\r\n"
    "Content-Type: text/plain; charset=ISO-8859-1\r\n"
    "\r\n"
    "Name: " + nameToHash + "\r\n"
    "Hash Code: " + hashCode + "\r\n"
    "\r\n"
    "----\r\n"
    "This email is auto-generated as part of Eric Beach's first assignment"
      " submission for CS106b.\r\n"
    "\r\n"
    "--047d7b5d59b0ef490904d9825525\r\n"
    "Content-Type: text/html; charset=ISO-8859-1\r\n"
    "\r\n"
    "<div dir='ltr'>Name: " + nameToHash + "<div>"
      "Hash Code: " + hashCode + "</div>"
      "<div><br></div><div>----</div><div><br></div>"
      "<div style>This email is auto-generated as part of Eric Beach&#39;s "
      "first assignment submission for CS106b.</div></div>\r\n"
    "\r\n"
    "--047d7b5d59b0ef490904d9825525--\r\n";
    strncpy(raw_body, body.c_str(), MAX_BODY_SIZE);
}

/*
 * Read a string from the socket and write it to the terminal output.
 */
void SendEmail::read_socket()
{
    numBytesRead = read(sock, buf, BUFSIZ);
    write(1, buf, numBytesRead);
    // printf("Server:%s\n",buf);
}

/*
 * Send an email.
 */
int SendEmail::sendMail()
{
    resolveMxResords();
    constructEmailBody();
    cout << "Attempting to send email" << endl;
    /*=====Create Socket=====*/
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("opening stream socket");
        exit(1);
    } else {
        cout << "socket created\n";
    }
 
    cout << "Email Socket Created" << endl;

    
    /*=====Verify host=====*/
    server.sin_family = AF_INET;
    hp = gethostbyname(destination_ip);
    if (hp == (struct hostent *) 0)
    {
        fprintf(stderr, "%s: unknown host\n", destination_ip);
        exit(2);
    }
    
    cout << "Host Verified" << endl;
    
    /*=====Connect to port 25 on remote host=====*/
    memcpy((char *) &server.sin_addr, (char *) hp->h_addr, hp->h_length);
    server.sin_port=htons(25); /* SMTP PORT */
    if (connect(sock, (struct sockaddr *) &server, sizeof server)==-1)
    {
        perror("connecting stream socket");
        exit(1);
    } else {
        cout << "Connected\n";
    }
    
    cout << "Connected on Port 25" << endl;
    /*=====Write some data then read some =====*/
    read_socket(); /* SMTP Server logon string */
    send_socket(HELO); /* introduce ourselves */
    read_socket(); /*Read reply */
    send_socket("MAIL FROM: <");
    send_socket(from_id);
    send_socket(">\r\n");
    read_socket(); /* Sender OK */
    send_socket("RCPT TO: <"); /*Mail to*/
    send_socket(to_id);
    send_socket(">\r\n");
    read_socket(); // Recipient OK*/
    send_socket(DATA);// body to follow*/
    read_socket(); // Recipient OK*/
    send_socket("Subject: ");
    send_socket(sub);
    send_socket(raw_body);
    send_socket(".\r\n");
    read_socket();
    send_socket(QUIT); /* quit */
    read_socket(); // log off */
    
    cout << "Data Transmitted" << endl;
    
    //=====Close socket and finish=====*/
    close(sock);
    cout << "Sent Email to " << to_id;
    return 0;
}