/*
 * File: SendEmail.h
 * ----------------
 * Name: Eric Beach
 * Section: SCPD, Aaron Broder <abroder@stanford.edu>
 * Copyright 2013 Eric Beach <ebeach@google.com>
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

#ifndef SENDEMAIL_H_
#define SENDEMAIL_H_

// Includes for Emailing
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string>

using namespace std;

/*
 * Define a class to send an email to a specified recipient.
 */
class SendEmail {
 private:
    // socket number
    int sock;

    // server socket
    struct sockaddr_in server;

    // host object
    struct hostent *hp;

    // buffer
    char buf[BUFSIZ+1];

    // number of bytes read from server
    int numBytesRead;

    // maximum size of email body
    static const int MAX_BODY_SIZE = 5000;

    // hash of user's name
    string hashCode;

    // input name that was hashed
    string nameToHash;

    // hostname to send email to (e.g., gmail.com)
    string destinationHostname;

    // destination IP
    char destination_ip[MAX_BODY_SIZE] = "173.194.70.26";

    // envelope and header from
    char from_id[MAX_BODY_SIZE] = "root@apps-apps.info";

    // default destination email address
    char to_id[MAX_BODY_SIZE] = "ebeachh@gmail.com";

    // recipient envelope and header name
    char to_name[MAX_BODY_SIZE] = "Eric Beach";

    // email subject
    char *sub = "CS106b - Assignment 1 - Warmup - Submission\r\n";

    // raw email body
    char raw_body[MAX_BODY_SIZE] = "";

    // data to transmit in SMTP session (HELO command)
    char *HELO = "HELO apps-apps.info\r\n";

    // data to transmit in SMTP session (DATA command)
    char *DATA = "DATA\r\n";

    // data to transmit in SMTP session (QUIT command)
    char *QUIT = "QUIT\r\n";

    // method to send data through the socket to the server
    void send_socket(char *s);

    // method to read data from the server
    void read_socket();

    // construct the body of the email
    void constructEmailBody();

    // resolve MX records for destination host
    void resolveMxResords();

    // convert a digit to a string
    string digitToString(int n);

    // convert an integer to a string
    string intToString(int n);

 public:
    // set the destination email address
    bool setDestinationEmail(string address);

    // set the recipient's name
    bool setDestinationName(string name);

    // set the hash value for the name
    bool setHashCode(int hCode);

    // set the name that was hashed
    bool setNameToHash(string name);

    // send the email
    int sendMail();
};

#endif  // SENDEMAIL_H_
