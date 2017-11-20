/*!
   \file sbfPublisher.c
   \brief This file defines a publisher sending messages for a given rate.
   \copyright © Copyright 2016 Neueda all rights reserved.
*/

#include "sbfCommon.h"
#include "sbfMw.h"
#include "sbfPerfCounter.h"
#include "sbfTport.h"

uint32_t gPublished;

/*!
   \brief This is a callback raised by the thread consuming messages from the queue.
   \param closure the data linked to the timer callback.
*/
static void*
dispatchCb (void* closure)
{
    sbfQueue_dispatch (closure);
    return NULL;
}

/*!
   \brief This is a callback raised by the timer.
   \param sbfTimer the timer invokign this callback.
   \param closure the data linked to the timer callback.
*/
static void
timerCb (sbfTimer timer, void* closure)
{
    printf ("%u" SBF_EOL, gPublished);
    fflush (stdout);

    gPublished = 0;
}

/*!
   \brief This function prints out the usage of the publisher command.
   \param argv0 the name of the command.
*/
static void
usage (const char* argv0)
{
    fprintf (stderr,
             "usage: %s "
             "[-h handler] "
             "[-i interface] "
             "[-r rate] "
             "[-s size] "
             "[-t topic] "
             "[-v level]\n",
             argv0);

    exit (1);
}

/*!
   \brief This function is the main entry point for the subscriber command.
   You can specify the following parameters to run the subscriber:
   "[-h handler] " the type of connection handler: tcp or udp.
   "[-i interface] " the connection interface e.g. eth0.
   "[-r rate] " the rate to send a message into the topic in milliseconds.
   "[-s size] " the size of the message in bytes.
   "[-t topic] " the name of the topic where messages are published.
   "[-v level]\n" log severity level.
   \param argc The number of arguments passed to the command.
   \param argv array of null terminated strings.
   \return Error status: 0 for Successfull another indicates an error code.
*/
int
main (int argc, char** argv)
{
    const char*        argv0 = argv[0];
    sbfMw              mw;
    sbfQueue           queue;
    sbfThread          t;
    sbfTport           tport;
    sbfKeyValue        properties;
    sbfLog             log;
    sbfPub             pub;
    uint64_t           interval;
    uint64_t           until;
    int                opt;
    const char*        topic = "OUT";
    uint64_t           rate = 1000;
    const char*        handler = "udp";
    const char*        interf = "eth0";
    unsigned long long ull;
    size_t             size = 200;
    uint64_t*          payload;

    // Initialise the logging system
    log = sbfLog_create (NULL, "%s", "");
    sbfLog_setLevel (log, SBF_LOG_OFF);

    // Apply command options and print message if wrong option is found
    while ((opt = getopt (argc, argv, "h:i:r:s:t:v:")) != -1) {
        switch (opt) {
        case 'h':
            handler = optarg;
            break;
        case 'i':
            interf = optarg;
            break;
        case 'r':
            rate = strtoull (optarg, NULL, 10);
            break;
        case 's':
            ull = strtoull (optarg, NULL, 10);
            if (ull < sizeof t)
                ull = sizeof t;
            if (ull < SIZE_MAX)
                size = (size_t)ull;
        case 't':
            topic = optarg;
            break;
        case 'v':
            sbfLog_setLevel (log, sbfLog_levelFromString (optarg, NULL));
            break;
        default:
            usage (argv0);
        }
    }
    argc -= optind;
    argv += optind;
    if (argc != 0)
        usage (argv0);

    // Initialise the middleware by defining some properties like the handler
    // (e.g. tcp, udp) and the  connection interface (e.g. eth0).
    properties = sbfKeyValue_create ();
    sbfKeyValue_put (properties, "transport.default.type", handler);
    sbfKeyValue_put (properties, "transport.default.udp.interface", interf);

    mw = sbfMw_create (log, properties);

    // Create a queue, the connection port and a thread to dispatch events
    queue = sbfQueue_create (mw, "default");
    sbfThread_create (&t, dispatchCb, queue);

    tport = sbfTport_create (mw, "default", SBF_MW_ALL_THREADS);

    // Create the timer consuming messages from the queue and
    sbfTimer_create (sbfMw_getDefaultThread (mw),
                     queue,
                     timerCb,
                     NULL,
                     1);

    pub = sbfPub_create (tport, queue, topic, NULL, NULL);

    // Translate timing into ticks (high performance counter)
    if (rate > 0)
        interval = (uint64_t)(sbfPerfCounter_ticks (1000000) / rate);
    else
        interval = 0;

    // Alloc size for the payload and send the high performance counter values
    // during the defined rate.
    payload = xmalloc (size);
    for (;;)
    {
        *payload = sbfPerfCounter_get ();
        until = (*payload) + interval;

        sbfPub_send (pub, payload, size);
        gPublished++;

        while (sbfPerfCounter_get () < until)
            /* nothing */;
    }

    exit (0);
}
