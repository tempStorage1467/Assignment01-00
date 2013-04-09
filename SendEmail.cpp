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
 * I almost got DNS resolution working. If you look at the git commit
 *   2da8c7a4538b19ad8a5a13ecdc1ec5edb18f2c80, you will see that I was able
 *   to get a query to execute (confirmed with Wireshark). Further, I somewhat
 *   got the results to parse. The problem was the code started getting into
 *   weird C stuff I didn't understand and didn't have time to learn (sadly)
 *   with a fulltime job. In addition, I would need to re-assemble DNS packets
 *   as per "4.1.4. Message compression" of RFC 1035. Consequently, I hardcoded
 *   in @gmail.com and @stanford.edu
 *
 * Additional insiration/credit for the DNS resolution:
 * http://stackoverflow.com/questions/1093410/pulling-mx-record-from-dns-server
 * http://stackoverflow.com/questions/258284/srv-record-lookup-with-iphone-sdk
 * http://www.opensource.apple.com/source/mDNSResponder/mDNSResponder-58/mDNSMacOSX/SampleUDSClient.c
 * http://linux.die.net/man/3/inet_ntoa
 */

#include "SendEmail.h"
#include <string>

using namespace std;

/*
 * Simulate a resolution of DNS (MX -> A) to get an IP.
 * To simplify, only two hosts are permitted (gmail.com or stanford.edu)
 */
void SendEmail::resolveMxResords() {
    if (destinationHostname == "gmail.com") {
        strncpy(destination_ip, "173.194.70.26", MAX_BODY_SIZE);
    } else {
        // use Stanford's MailEXchanger
        strncpy(destination_ip, "171.67.219.71", MAX_BODY_SIZE);
    }
}

/*
 * Send a C-style string to the open socket.
 */
void SendEmail::send_socket(char *s) {
    write(sock, s, strlen(s));
    write(1, s, strlen(s));
    // printf("Client:%s\n",s);
}

/*
 * Set the destination name (i.e., the address of the individual receiving
 *   the email).
 */
bool SendEmail::setDestinationEmail(string address) {
    destinationHostname = address.substr(address.find("@") + 1);
    strncpy(to_id, address.c_str(), MAX_BODY_SIZE);
    return true;
}

/*
 * Set the destination name (i.e., the name of the individual receiving 
 *   the email).
 */
bool SendEmail::setDestinationName(string name) {
    strncpy(to_name, name.c_str(), MAX_BODY_SIZE);
    return true;
}

/*
 * Convert a digit to a string.
 */
string SendEmail::digitToString(int n) {
    return string() + char(n % 10 + '0');
}

/*
 * Convert an integer to a string.
 */
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

/*
 * Set the hash code to be transmitted in the email.
 */
bool SendEmail::setHashCode(int hCode) {
    hashCode = intToString(hCode);
    return true;
}

/*
 * Constrict an RFC822 compliant message body for transmit via SMTP.
 */
bool SendEmail::setNameToHash(string name) {
    nameToHash = name;
    return true;
}

/*
 * Constrict an RFC822 compliant message body for transmit via SMTP.
 */
void SendEmail::constructEmailBody() {
    string body = "From: CS106B #1 (Eric Beach) <root@apps-apps.info>\r\n"
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
void SendEmail::read_socket() {
    numBytesRead = read(sock, buf, BUFSIZ);
    write(1, buf, numBytesRead);
    // printf("Server:%s\n",buf);
}

/*
 * Send an email.
 */
int SendEmail::sendMail() {
    resolveMxResords();

    constructEmailBody();
    cout << "Attempting to send email" << endl;
    // Create Socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("opening stream socket");
        exit(1);
    } else {
        cout << "socket created\n";
    }

    cout << "Email Socket Created" << endl;


    /*=====Verify host=====*/
    server.sin_family = AF_INET;
    hp = gethostbyname(destination_ip);
    if (hp == (struct hostent *) 0) {
        fprintf(stderr, "%s: unknown host\n", destination_ip);
        exit(2);
    }

    cout << "Host Verified" << endl;

    // Connect to port 25 on remote host
    memcpy((char *) &server.sin_addr, (char *) hp->h_addr, hp->h_length);
    server.sin_port = htons(25);  // SMTP PORT
    if (connect(sock, (struct sockaddr *) &server, sizeof server) == -1) {
        perror("connecting stream socket");
        exit(1);
    } else {
        cout << "Connected\n";
    }

    cout << "Connected on Port 25" << endl;
    //  Write some data then read some
    read_socket();  // SMTP Server logon string
    send_socket(HELO);
    read_socket();  // Read reply
    send_socket("MAIL FROM: <");
    send_socket(from_id);
    send_socket(">\r\n");
    read_socket();  // Sender OK
    send_socket("RCPT TO: <");  // Mail to
    send_socket(to_id);
    send_socket(">\r\n");
    read_socket();  // Recipient OK
    send_socket(DATA);  // body to follow
    read_socket();  // Recipient OK
    send_socket("Subject: ");
    send_socket(sub);
    send_socket(raw_body);
    send_socket(".\r\n");
    read_socket();
    send_socket(QUIT);
    read_socket();  // log off

    cout << "Data Transmitted" << endl;

    // Close socket and finish
    close(sock);
    cout << "Sent Email to " << to_id;
    return 0;
}
