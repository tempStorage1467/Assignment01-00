/*
 * File: Warmup.cpp
 * ----------------
 * Name: Eric Beach
 * Section: SCPD, Aaron Broder <abroder@stanford.edu>
 * Copyright 2013 Eric Beach <ebeach@google.com>
 * Assignment 1 - Pt. 0 - Wramup
 * This program generates a hash code based on user's name.
 * As given, this code has two compiler errors you need to track down 
 * and fix in order to get the program up and running.
 * [Originally written by Julie Zelenski]
 *
 * This file lightly linted using
 * http://google-styleguide.googlecode.com/svn/trunk/cpplint/cpplint.py
 */

#include <iostream>
#include <string>
#include "console.h"
#include "simpio.h"
#include "SendEmail.h"

using namespace std;

/* Constants */

const int HASH_SEED = 5381;               /* Starting point for first cycle */
const int HASH_MULTIPLIER = 33;           /* Multiplier for each cycle      */
const int HASH_MASK = unsigned(-1) >> 1;  /* All 1 bits except the sign     */

/* Function prototypes */

int hashCode(string key);

/* Main program to test the hash function */

int main() {
    string name = getLine("Please enter your name: ");
    int code = hashCode(name);
    cout << "The hash code for your name is " << code << "." << endl;

    string prompt = "Email this to another person (Y or N): ";
    string emailSubmission = getLine(prompt);
    if (emailSubmission != "y" && emailSubmission != "Y") {
        return 0;
    }

    prompt = getLine("Enter recipient's name: ");
    string destinationName = prompt;

    cout << "Note (1): Since this is unauthenticated email, the message will "
       "likely end up in the spam folder, so check there."  << endl;
    cout << "Note (2): You can only send to @stanford.edu or @gmail.com "
       "email addresses." << endl;
    cout << "Note (3): Many ISPs block port 25 outbound to prevent spam. "
       "Ensure you are using a network on which port 25 is open outbound. "
       "To determine this, run 'telnet aspmx.l.google.com 25'" << endl;
    prompt = getLine("Enter recipient's email address: ");
    string destinationEmail = prompt;

    SendEmail mailer = SendEmail();
    mailer.setDestinationName(destinationName);
    mailer.setDestinationEmail(destinationEmail);
    mailer.setNameToHash(name);
    mailer.setHashCode(code);
    mailer.sendMail();
    return 0;
}

/*
 * Function: hashCode
 * Usage: int code = hashCode(key);
 * --------------------------------
 * This function takes the key and uses it to derive a hash code,
 * which is nonnegative integer related to the key by a deterministic
 * function that distributes keys well across the space of integers.
 * The general method is called linear congruence, which is also used
 * in random-number generators.  The specific algorithm used here is
 * called djb2 after the initials of its inventor, Daniel J. Bernstein,
 * Professor of Mathematics at the University of Illinois at Chicago.
 */
int hashCode(string str) {
    unsigned hash = HASH_SEED;
    int nchars = str.length();
    for (int i = 0; i < nchars; i++) {
        hash = HASH_MULTIPLIER * hash + str[i];
    }
    return (hash & HASH_MASK);
}
