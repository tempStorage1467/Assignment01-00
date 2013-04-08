/*
 * File: SendEmail.h
 * ----------------
 * Name: Eric Beach
 * Section: [TODO: enter section leader here]
 * Copyright [2013] <Eric Beach>
 * This file defines a class used to send an email.
 * [Originally written by Julie Zelenski]
 *
 * This file lightly linted using
 * http://google-styleguide.googlecode.com/svn/trunk/cpplint/cpplint.py
 */

/*
 * Credit to: http://stackoverflow.com/questions/58210/c-smtp-example
 * I modified this code, put it into a class, and got it to compile.
 */

#ifndef __Warmup__SendEmail__
#define __Warmup__SendEmail__

// Includes for Emailing
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string>
#include <math.h>

using namespace std;

class SendEmail {
private:
    int sock;
    struct sockaddr_in server;
    struct hostent *hp;
    char buf[BUFSIZ+1];
    int numBytesRead;
    static const int MAX_BODY_SIZE = 5000;
    string hashCode;
    string nameToHash;

    char *destination_ip = "173.194.70.26";
    char *from_id = "root@apps-email.info";
    char to_id[MAX_BODY_SIZE] = "eric.a.beach@gmail.com";
    char to_name[MAX_BODY_SIZE] = "Eric Beach";
    char *sub = "CS106b - Assignment 1 - Warmup - Submission\r\n";
    char raw_body[MAX_BODY_SIZE] = "";
    
    char *HELO = "HELO apps-apps.info\r\n";
    char *DATA = "DATA\r\n";
    char *QUIT = "QUIT\r\n";
    
    void send_socket(char *s);
    void read_socket();
    void constructEmailBody();
    void resolveMxResords();
    string digitToString(int n);
    string intToString(int n);
    
public:
    bool setDestinationEmail(string address);
    bool setDestinationName(string name);
    bool setHashCode(int hCode);
    bool setNameToHash(string name);
    int sendMail();
};

#endif /* defined(__Warmup__SendEmail__) */
