// CExceptionConfig.h

#include <stdio.h>
#include <stdint.h>
#include "CException.h"

#ifndef CEXCEPTION_CONFIG_H
#define CEXCEPTION_CONFIG_H


// Define the Exception ID enumeration
typedef enum {
    ERR_TEMP_LOW = 1,
    ERR_TEMP_HIGH,
    ERR_BIT_ERROR,
    // Add more error IDs as needed
} ExceptionID;

// CException configuration settings
#ifdef CEXCEPTION_t
#define CEXCEPTION_T ExceptionID        // Use the ExceptionID type for exception IDs
#endif


#ifndef CEXCEPTION_NUM_ID 
#define CEXCEPTION_NUM_ID 2             // Number of tasks for multitasking
#endif


#ifndef CEXCEPTION_GET_ID
#define CEXCEPTION_GET_ID GetTaskID     // Function for retrieving task IDs
#endif

// Fallback handler for unhandled exceptions
#ifndef CEXCEPTION_NO_CATCH_HANDLER
#define CEXCEPTION_NO_CATCH_HANDLER(id) NoCatchHandler(id)
#endif

// Optional hooks
#ifndef CEXCEPTION_HOOK_START_TRY
#define CEXCEPTION_HOOK_START_TRY StartTryHook()
#endif

#ifndef CEXCEPTION_HOOK_HAPPY_TRY
#define CEXCEPTION_HOOK_HAPPY_TRY HappyTryHook()
#endif

#ifndef CEXCEPTION_HOOK_AFTER_TRY 
#define CEXCEPTION_HOOK_AFTER_TRY AfterTryHook()
#endif

#ifndef CEXCEPTION_HOOK_START_CATCH
#define CEXCEPTION_HOOK_START_CATCH StartCatchHook()
#endif


// Function prototypes
int GetTaskID(void);
void NoCatchHandler(int id);
void StartTryHook(void);
void HappyTryHook(void);
void AfterTryHook(void);
void StartCatchHook(void);


#endif // CEXCEPTION_CONFIG_H

int GetTaskID(void) {
    // Logic to get the current task ID
    return 0;  // Simplified for a single-task environment
}


void NoCatchHandler(int id) {
    printf("Critical Error: Unhandled exception with ID %d\n", id);
    // Implement any emergency actions or system reset if necessary
}

void StartTryHook(void) {
    printf("Starting Try block...\n");
}

void HappyTryHook(void) {
    printf("Try block executed successfully with no errors.\n");
}

void AfterTryHook(void) {
    printf("After Try block, moving to Catch or ending if no exception.\n");
}

void StartCatchHook(void) {
    printf("Handling exception in Catch block...\n");
}

