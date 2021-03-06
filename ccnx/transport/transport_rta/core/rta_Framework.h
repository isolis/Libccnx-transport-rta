/*
 * Copyright (c) 2013-2014, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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
 * @file rta_Framework.h
 * @brief <#Brief Description#>
 *
 * rtaFramework executes inside the worker thread in callback from the event scheduler.
 *
 * It provides service functions to components and connectors so they do not need
 * to be event aware.
 *
 * It also manages the command channel to communicate with rtaTransport in the API's thread.
 *
 * _Create(), _Start(), and _Destroy() are called from the API's thread.  You should not
 * call _Destroy until rtaFramework_GetStatus() is FRAMEWORK_SHUTDOWN.
 *
 * The framework can run in threaded mode or non-threaded mode.  Including this one
 * header gives you both sets of operations, but they are not compatible.
 *
 * THREADED MODE:
 *      call _Create
 *      call _Start
 *      ... do work ...
 *      call _Shutdown
 *      call _Destroy
 *
 * NON-THREADED MODE
 *      call _Create
 *      ... do work ...
 *      call _Step or _StepCount or _StepTimed
 *      ... do work ...
 *      call _Step or _StepCount or _StepTimed
 *      ... do work ...
 *      call _Teardown
 *      call _Destroy
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2014, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
#ifndef Libccnx_rta_Framework_h
#define Libccnx_rta_Framework_h

#include <parc/concurrent/parc_RingBuffer_1x1.h>
#include <parc/concurrent/parc_Notifier.h>
#include <ccnx/transport/transport_rta/core/rta_Logger.h>

// ===================================
// External API, used by rtaTransport

struct rta_framework;
typedef struct rta_framework RtaFramework;

#define RTA_MAX_PRIORITY    0
#define RTA_NORMAL_PRIORITY 1
#define RTA_MIN_PRIORITY    2

/**
 * Transient states: STARTING, STOPPING.  You don't want to block waiting for those
 * as you could easily miss them
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
typedef enum {
    FRAMEWORK_INIT = 0,       /** Initial status after Create() *
                               * Example:
                               * @code
                               * <#example#>
                               * @endcode
                               */
    FRAMEWORK_SETUP = 1,      /** Configured in non-threaded mode *
                               * Example:
                               * @code
                               * <#example#>
                               * @endcode
                               */

    FRAMEWORK_STARTING = 2,   /** Between calling _Start() and the thread running *
                               * Example:
                               * @code
                               * <#example#>
                               * @endcode
                               */
    FRAMEWORK_RUNNING = 3,    /** After event scheduler thread starts *
                               * Example:
                               * @code
                               * <#example#>
                               * @endcode
                               */
    FRAMEWORK_STOPPING = 4,   /** When shutdown is finished, but before event scheduler exists *
                               * Example:
                               * @code
                               * <#example#>
                               * @endcode
                               */

    FRAMEWORK_TEARDOWN = 5,   /** After cleanup from SETUP *
                               * Example:
                               * @code
                               * <#example#>
                               * @endcode
                               */
    FRAMEWORK_SHUTDOWN = 6,   /** After event scheduler exits *
                               * Example:
                               * @code
                               * <#example#>
                               * @endcode
                               */
} RtaFrameworkStatus;

/**
 * Creates the framework context, but does not start the worker thread.
 * <code>command_fd</code> is the socketpair or pipe (one-way is ok) over which
 * RTATransport will send commands.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
RtaFramework *rtaFramework_Create(PARCRingBuffer1x1 *commandRingBuffer, PARCNotifier *commandNotifier);


void rtaFramework_Destroy(RtaFramework **frameworkPtr);

/**
 * Returns the Logging system used by the framework
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] framework An allocated RtaFramework
 *
 * @retval non-null The Logging system
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
RtaLogger *rtaFramework_GetLogger(RtaFramework *framework);

/**
 * May block briefly, returns the current status of the framework.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
RtaFrameworkStatus rtaFramework_GetStatus(RtaFramework *framework);

/**
 * Blocks until the framework status equals or exeeds the desired status
 * Transient states: STARTING, STOPPING.  You don't want to block waiting for those
 * as you could easily miss them
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
RtaFrameworkStatus rtaFramework_WaitForStatus(RtaFramework *framework,
                                              RtaFrameworkStatus status);


#include "rta_Framework_Threaded.h"
#include "rta_Framework_NonThreaded.h"
#endif
