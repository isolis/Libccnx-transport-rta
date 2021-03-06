/*
 * Copyright (c) 2014-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2014-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../rta_Logger.c"
#include <stdio.h>
#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>

LONGBOW_TEST_RUNNER(rta_Logger)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(rta_Logger)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(rta_Logger)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// ==========================================================

/*
 * _testWritter will vsprintf to this buffer
 */
#define _logLength 1024
static char _lastLogMessage[_logLength];

static int
_testWriter(const char *message)
{
    int written = 0;
    written = snprintf(_lastLogMessage, _logLength, "%s", message);
    return written;
}

static PARCLogReporter *
_testWriter_Acquire(const PARCLogReporter *reporter)
{
    return parcObject_Acquire(reporter);
}

static void
_testWriter_Release(PARCLogReporter **reporterPtr)
{
    parcObject_Release((void **) reporterPtr);
}

static void
_testWriter_Report(PARCLogReporter *reporter, const PARCLogEntry *entry)
{
    char *string = parcLogEntry_ToString(entry);
    _testWriter(string);
    parcMemory_Deallocate((void **) &string);
}

static PARCLogReporter *
_testWriter_Create(void)
{
    return parcLogReporter_Create(_testWriter_Acquire, _testWriter_Release, _testWriter_Report, NULL);
}

// ==========================================================

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, rtaLogger_FacilityString_Found);
    LONGBOW_RUN_TEST_CASE(Global, rtaLogger_FacilityString_NotFound);
    LONGBOW_RUN_TEST_CASE(Global, rtaLogger_Create);
    LONGBOW_RUN_TEST_CASE(Global, rtaLogger_Acquire);
    LONGBOW_RUN_TEST_CASE(Global, rtaLogger_SetLogLevel);
    LONGBOW_RUN_TEST_CASE(Global, rtaLogger_IsLoggable_True);
    LONGBOW_RUN_TEST_CASE(Global, rtaLogger_IsLoggable_False);
    LONGBOW_RUN_TEST_CASE(Global, rtaLogger_Log_IsLoggable);
    LONGBOW_RUN_TEST_CASE(Global, rtaLogger_Log_IsNotLoggable);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, rtaLogger_FacilityString_Found)
{
    for (RtaLoggerFacility i = 0; i < RtaLoggerFacility_END; i++) {
        const char *test = rtaLogger_FacilityString(i);
        assertNotNull(test, "Got null string for facility %d", i);
    }
}

LONGBOW_TEST_CASE(Global, rtaLogger_FacilityString_NotFound)
{
    const char *test = rtaLogger_FacilityString(1000);
    assertTrue(strcmp(test, "Unknown") == 0, "Got wrong string for unknown facility");
}

LONGBOW_TEST_CASE(Global, rtaLogger_Create)
{
    PARCLogReporter *reporter = _testWriter_Create();
    RtaLogger *logger = rtaLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);

    rtaLogger_Release(&logger);
}

LONGBOW_TEST_CASE(Global, rtaLogger_Acquire)
{
    PARCLogReporter *reporter = _testWriter_Create();
    RtaLogger *logger = rtaLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);

    RtaLogger *copy = rtaLogger_Acquire(logger);
    rtaLogger_Release(&logger);
    rtaLogger_Release(&copy);
}

LONGBOW_TEST_CASE(Global, rtaLogger_SetLogLevel)
{
    PARCLogReporter *reporter = _testWriter_Create();
    RtaLogger *logger = rtaLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);

    rtaLogger_SetLogLevel(logger, RtaLoggerFacility_Framework, PARCLogLevel_Off);

    PARCLogLevel test = parcLog_GetLevel(logger->loggerArray[RtaLoggerFacility_Framework]);
    assertTrue(test == PARCLogLevel_Off, "wrong log level, expected %d got %d", PARCLogLevel_Off, test);
    rtaLogger_Release(&logger);
}

LONGBOW_TEST_CASE(Global, rtaLogger_IsLoggable_True)
{
    PARCLogReporter *reporter = _testWriter_Create();
    RtaLogger *logger = rtaLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);

    rtaLogger_SetLogLevel(logger, RtaLoggerFacility_Framework, PARCLogLevel_Warning);
    bool isLoggable = rtaLogger_IsLoggable(logger, RtaLoggerFacility_Framework, PARCLogLevel_Warning);
    assertTrue(isLoggable, "Did not get true for isLoggable when expecting true");
    rtaLogger_Release(&logger);
}

LONGBOW_TEST_CASE(Global, rtaLogger_IsLoggable_False)
{
    PARCLogReporter *reporter = _testWriter_Create();
    RtaLogger *logger = rtaLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);

    rtaLogger_SetLogLevel(logger, RtaLoggerFacility_Framework, PARCLogLevel_Warning);
    bool isLoggable = rtaLogger_IsLoggable(logger, RtaLoggerFacility_Framework, PARCLogLevel_Debug);
    assertFalse(isLoggable, "Logging debug to warning facility should have been false");
    rtaLogger_Release(&logger);
}

LONGBOW_TEST_CASE(Global, rtaLogger_Log_IsLoggable)
{
    PARCLogReporter *reporter = _testWriter_Create();
    RtaLogger *logger = rtaLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);

    rtaLogger_SetLogLevel(logger, RtaLoggerFacility_Framework, PARCLogLevel_Warning);
    memset(_lastLogMessage, 0, _logLength);

    rtaLogger_Log(logger, RtaLoggerFacility_Framework, PARCLogLevel_Warning, __func__, "hello");
    assertTrue(strlen(_lastLogMessage) > 0, "Did not write to log message");
    rtaLogger_Release(&logger);
}

LONGBOW_TEST_CASE(Global, rtaLogger_Log_IsNotLoggable)
{
    PARCLogReporter *reporter = _testWriter_Create();
    RtaLogger *logger = rtaLogger_Create(reporter, parcClock_Wallclock());
    parcLogReporter_Release(&reporter);

    rtaLogger_SetLogLevel(logger, RtaLoggerFacility_Framework, PARCLogLevel_Warning);
    memset(_lastLogMessage, 0, _logLength);

    rtaLogger_Log(logger, RtaLoggerFacility_Framework, PARCLogLevel_Debug, __func__, "hello");
    assertTrue(strlen(_lastLogMessage) == 0, "Should not have written to log message");
    rtaLogger_Release(&logger);
}


// ==========================================================

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(rta_Logger);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}

