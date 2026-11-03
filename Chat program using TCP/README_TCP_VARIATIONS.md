# TCP Exam Variations & Creative Questions Cheat Sheet

In Computer Networks lab exams, examiners often ask for "creative" TCP socket programs instead of a standard generic chat. 

The good news: **95% of the code (socket, bind, listen, accept, connect, recv, send) stays completely identical!** You only change **3 to 6 lines** inside the server's receive-and-reply block.

---

## 🎯 The Universal Recipe: Where to Make the Change

In your base server file (like [`loop_tcp_server.c`](loop_tcp_server.c) or [`tcp_chat_server.c`](tcp_chat_server.c)), locate this exact section inside `while(1)`:

```c
// 1. SERVER RECEIVES INPUT FROM CLIENT
int n = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
buffer[n] = '\0';
buffer[strcspn(buffer, "\r\n")] = '\0'; // strip trailing newline

// =========================================================================
// 2. 👉 REPLACE THIS PART WITH THE CUSTOM EXAM LOGIC (Choose from below) 👈
// =========================================================================

// 3. SERVER SENDS RESPONSE BACK TO CLIENT
send(client_socket, response, strlen(response), 0);
```

---

## 💡 Creative Exam Questions & Their Replacement Snippets

### 1. Date & Time Server
> **Exam Question:** *"Design a TCP client-server application where the client requests the current system date and time, and the server returns it."*

* **Header to add at top of server:** `#include <time.h>`
* **Server replacement code:**
  ```c
  time_t now = time(NULL);
  char response[BUFFER_SIZE];
  snprintf(response, sizeof(response), "Current Server Time: %s", ctime(&now));
  send(client_socket, response, strlen(response), 0);
  ```

---

### 2. Lowercase / Uppercase / Toggle Case Server
> **Exam Question:** *"Implement a TCP program where the client sends a string, and the server converts it to lowercase (or uppercase / toggle case) and returns it."*

* **Header to add at top of server:** `#include <ctype.h>`

#### A. Convert to Lowercase:
```c
for (int i = 0; buffer[i] != '\0'; i++) {
    buffer[i] = tolower((unsigned char)buffer[i]);
}
send(client_socket, buffer, strlen(buffer), 0);
```

#### B. Convert to Uppercase:
```c
for (int i = 0; buffer[i] != '\0'; i++) {
    buffer[i] = toupper((unsigned char)buffer[i]);
}
send(client_socket, buffer, strlen(buffer), 0);
```

#### C. Reverse Case (Toggle Case: 'A' -> 'a', 'a' -> 'A'):
```c
for (int i = 0; buffer[i] != '\0'; i++) {
    if (isupper((unsigned char)buffer[i]))
        buffer[i] = tolower((unsigned char)buffer[i]);
    else if (islower((unsigned char)buffer[i]))
        buffer[i] = toupper((unsigned char)buffer[i]);
}
send(client_socket, buffer, strlen(buffer), 0);
```

---

### 3. String Reverse & Palindrome Checker
> **Exam Question:** *"Client sends a string to the server. The server reverses the string and determines whether it is a palindrome."*

* **Server replacement code:**
  ```c
  int len = strlen(buffer);
  char reversed[BUFFER_SIZE];
  int is_palindrome = 1;

  for (int i = 0; i < len; i++) {
      reversed[i] = buffer[len - 1 - i];
      if (tolower((unsigned char)buffer[i]) != tolower((unsigned char)buffer[len - 1 - i])) {
          is_palindrome = 0;
      }
  }
  reversed[len] = '\0';

  char response[BUFFER_SIZE];
  snprintf(response, sizeof(response),
           "Reversed: \"%s\" | %s\n",
           reversed, is_palindrome ? "PALINDROME" : "NOT A PALINDROME");

  send(client_socket, response, strlen(response), 0);
  ```

---

### 4. Arithmetic Calculator Server
> **Exam Question:** *"Implement a Remote Calculator using TCP sockets where the client sends an expression (e.g. 10 + 20) and the server computes and returns the answer."*

* **Server replacement code:**
  ```c
  int num1, num2, result = 0;
  char op;
  char response[BUFFER_SIZE];

  // Parses: "10 + 20" or "50 * 4"
  if (sscanf(buffer, "%d %c %d", &num1, &op, &num2) == 3) {
      if (op == '+') result = num1 + num2;
      else if (op == '-') result = num1 - num2;
      else if (op == '*') result = num1 * num2;
      else if (op == '/') {
          if (num2 != 0) result = num1 / num2;
          else {
              strcpy(response, "Error: Division by zero!\n");
              send(client_socket, response, strlen(response), 0);
              continue;
          }
      } else {
          strcpy(response, "Error: Unknown operator! (+, -, *, / allowed)\n");
          send(client_socket, response, strlen(response), 0);
          continue;
      }
      snprintf(response, sizeof(response), "Calculation Result: %d %c %d = %d\n", num1, op, num2, result);
  } else {
      strcpy(response, "Error: Format must be 'num1 op num2' (e.g. 15 + 25)\n");
  }

  send(client_socket, response, strlen(response), 0);
  ```

---

### 5. Vowel, Consonant & Word Counter
> **Exam Question:** *"Develop a TCP server that analyzes a sentence sent by the client and returns the total count of vowels, consonants, and words."*

* **Header to add at top of server:** `#include <ctype.h>`
* **Server replacement code:**
  ```c
  int vowels = 0, consonants = 0, words = 0, in_word = 0;

  for (int i = 0; buffer[i] != '\0'; i++) {
      char ch = tolower((unsigned char)buffer[i]);
      if (isalpha((unsigned char)ch)) {
          if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u')
              vowels++;
          else
              consonants++;
      }
      if (!isspace((unsigned char)ch) && !in_word) {
          in_word = 1;
          words++;
      } else if (isspace((unsigned char)ch)) {
          in_word = 0;
      }
  }

  char response[BUFFER_SIZE];
  snprintf(response, sizeof(response),
           "Statistics -> Vowels: %d | Consonants: %d | Total Words: %d\n",
           vowels, consonants, words);

  send(client_socket, response, strlen(response), 0);
  ```

---

### 6. Factorial & Prime Number Checker
> **Exam Question:** *"Client sends a number to the server. The server computes its factorial and reports if it is a prime number."*

* **Server replacement code:**
  ```c
  int val = atoi(buffer);
  char response[BUFFER_SIZE];

  if (val < 0) {
      strcpy(response, "Error: Please enter a positive integer.\n");
  } else {
      // Factorial calculation
      long long fact = 1;
      for (int i = 1; i <= val && i <= 20; i++) {
          fact *= i;
      }

      // Prime number check
      int is_prime = (val > 1);
      for (int i = 2; i * i <= val; i++) {
          if (val % i == 0) {
              is_prime = 0;
              break;
          }
      }

      snprintf(response, sizeof(response),
               "Number: %d | Factorial: %lld | Status: %s\n",
               val, fact, is_prime ? "PRIME" : "NOT PRIME");
  }

  send(client_socket, response, strlen(response), 0);
  ```

---

### 7. File Content Request (Mini Remote File Server)
> **Exam Question:** *"Client sends a filename to the server. The server opens the file and transmits its text contents back to the client."*

* **Server replacement code:**
  ```c
  char response[BUFFER_SIZE];
  FILE *fp = fopen(buffer, "r");

  if (fp == NULL) {
      snprintf(response, sizeof(response), "Error: File '%s' not found on server!\n", buffer);
  } else {
      int bytes_read = (int)fread(response, 1, sizeof(response) - 1, fp);
      response[bytes_read] = '\0';
      fclose(fp);
  }

  send(client_socket, response, strlen(response), 0);
  ```

---

## 📋 Summary of Headers to Remember on Exam Day

| Function Used | Header File Needed |
|---|---|
| `time()`, `ctime()` | `#include <time.h>` |
| `tolower()`, `toupper()`, `isalpha()`, `isspace()` | `#include <ctype.h>` |
| `atoi()`, `abs()`, `exit()` | `#include <stdlib.h>` |
| `strlen()`, `strcpy()`, `strcmp()`, `snprintf()` | `#include <string.h>` |

