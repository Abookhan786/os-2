/**
 * ============================================================================
 * Problem: Client-Server Arithmetic Engine via System V Message Queues
 * ============================================================================
 * Description:
 *   Parent process (Client) and Child process (Server) communicate using a
 *   single System V Message Queue (msgget / msgsnd / msgrcv).
 *   Message types (mtype) are used for message multiplexing:
 *     - TYPE 1 (Request)  : Parent -> Child containing two numbers and operator.
 *     - TYPE 2 (Response) : Child -> Parent containing computed floating-point result.
 *   Parent sends multiple operations, Child processes each, sends back result,
 *   and terminates when receiving sentinel operator 'Q'.
 *
 * Compilation:
 *   gcc -Wall -o ipc_msg_queue_calc ipc_msg_queue_calc.c
 *
 * Execution:
 *   ./ipc_msg_queue_calc
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define TYPE_REQUEST  1
#define TYPE_RESPONSE 2

// Request message structure
typedef struct {
    long mtype;       // must be > 0
    double op1;
    double op2;
    char operation;   // '+', '-', '*', '/', or 'Q' (quit)
} request_msg_t;

// Response message structure
typedef struct {
    long mtype;
    double result;
    int error_code;   // 0 = Success, 1 = Division by zero, 2 = Unknown operator
} response_msg_t;

int main() {
    // 1. Create a private System V message queue
    int msgid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if (msgid < 0) {
        perror("msgget failed");
        exit(EXIT_FAILURE);
    }

    printf("===============================================================\n");
    printf("   IPC: Client-Server Math Engine via System V Message Queue   \n");
    printf("===============================================================\n");

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // ==========================================
        // CHILD PROCESS (Math Server)
        // ==========================================
        request_msg_t req;
        response_msg_t resp;

        while (1) {
            // Receive only messages with mtype == TYPE_REQUEST (1)
            if (msgrcv(msgid, &req, sizeof(request_msg_t) - sizeof(long), TYPE_REQUEST, 0) == -1) {
                perror("[Child Server] msgrcv error");
                break;
            }

            if (req.operation == 'Q') {
                printf("[Child Server] Received shutdown request. Exiting.\n");
                break;
            }

            resp.mtype = TYPE_RESPONSE;
            resp.error_code = 0;
            resp.result = 0.0;

            switch (req.operation) {
                case '+': resp.result = req.op1 + req.op2; break;
                case '-': resp.result = req.op1 - req.op2; break;
                case '*': resp.result = req.op1 * req.op2; break;
                case '/':
                    if (req.op2 == 0.0) {
                        resp.error_code = 1; // Division by zero
                    } else {
                        resp.result = req.op1 / req.op2;
                    }
                    break;
                default:
                    resp.error_code = 2; // Unknown operation
                    break;
            }

            // Send response back with mtype = TYPE_RESPONSE (2)
            msgsnd(msgid, &resp, sizeof(response_msg_t) - sizeof(long), 0);
        }

        exit(EXIT_SUCCESS);
    } else {
        // ==========================================
        // PARENT PROCESS (Client)
        // ==========================================
        // Test suite of queries
        struct {
            double a;
            double b;
            char op;
        } tasks[] = {
            {25.0, 5.0, '+'},
            {100.0, 45.5, '-'},
            {12.5, 8.0, '*'},
            {77.0, 7.0, '/'},
            {10.0, 0.0, '/'},  // Test division by zero
        };

        int num_tasks = sizeof(tasks) / sizeof(tasks[0]);
        request_msg_t req;
        response_msg_t resp;

        for (int i = 0; i < num_tasks; i++) {
            req.mtype = TYPE_REQUEST;
            req.op1 = tasks[i].a;
            req.op2 = tasks[i].b;
            req.operation = tasks[i].op;

            printf("[Parent Client] Request : %.2f %c %.2f\n", req.op1, req.operation, req.op2);
            msgsnd(msgid, &req, sizeof(request_msg_t) - sizeof(long), 0);

            // Wait for response with mtype == TYPE_RESPONSE (2)
            msgrcv(msgid, &resp, sizeof(response_msg_t) - sizeof(long), TYPE_RESPONSE, 0);

            if (resp.error_code == 1) {
                printf("  -> [Response] Error: Division by zero!\n");
            } else if (resp.error_code == 2) {
                printf("  -> [Response] Error: Unsupported operator!\n");
            } else {
                printf("  -> [Response] Result = %.2f\n", resp.result);
            }
        }

        // Send quit signal to server
        req.mtype = TYPE_REQUEST;
        req.operation = 'Q';
        msgsnd(msgid, &req, sizeof(request_msg_t) - sizeof(long), 0);

        // Wait for child to exit
        wait(NULL);

        // Remove message queue from system
        msgctl(msgid, IPC_RMID, NULL);
        printf("[Parent Client] Message queue successfully deallocated.\n");
    }

    return 0;
}
