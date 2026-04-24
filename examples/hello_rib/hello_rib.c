/*
 * hello_rib.c — minimal example: open a RIB file and count each RI call.
 *
 * Usage:  hello_rib [file.rib]
 *         (reads from stdin if no filename is given)
 *
 * Demonstrates:
 *   - Customising gRibRITable to intercept specific RI calls
 *   - Opening a RIB file with RibOpen (const char * filename, Phase 3 API)
 *   - Draining the parser with RibRead until EOF
 *   - Closing the handle with RibClose
 */
#include <stdio.h>
#include <stdlib.h>
#include <ribrdr.h>

/* Global tally */
static int g_call_count = 0;

/* ── Interceptor stubs ──────────────────────────────────────────────────── */

static RtVoid OnBegin(RtToken name)
{
    printf("  Begin(\"%s\")\n", name ? name : "");
    g_call_count++;
}

static RtVoid OnEnd(void)
{
    printf("  End\n");
    g_call_count++;
}

static RtVoid OnWorldBegin(void)
{
    printf("  WorldBegin\n");
    g_call_count++;
}

static RtVoid OnWorldEnd(void)
{
    printf("  WorldEnd\n");
    g_call_count++;
}

static RtVoid OnSphereV(RtFloat radius, RtFloat zmin, RtFloat zmax,
                        RtFloat thetamax,
                        RtInt n, RtToken tokens[], RtPointer parms[])
{
    printf("  Sphere radius=%.4g\n", radius);
    g_call_count++;
    (void)zmin; (void)zmax; (void)thetamax;
    (void)n; (void)tokens; (void)parms;
}

/* Catch-all: increment counter for every other call */
static RtVoid OnOther(void)
{
    g_call_count++;
}

int main(int argc, char *argv[])
{
    const char *filename = (argc > 1) ? argv[1] : NULL;

    /* Install intercept stubs into the global RI dispatch table.
     * gRibRITable is defined in libribrdr and indexed by kRIB_* enum values. */
    gRibRITable[kRIB_BEGIN]      = (PRIB_RIPROC)OnBegin;
    gRibRITable[kRIB_END]        = (PRIB_RIPROC)OnEnd;
    gRibRITable[kRIB_WORLDBEGIN] = (PRIB_RIPROC)OnWorldBegin;
    gRibRITable[kRIB_WORLDEND]   = (PRIB_RIPROC)OnWorldEnd;
    gRibRITable[kRIB_SPHERE]     = (PRIB_RIPROC)OnSphereV;

    /* For every other RI call, use the catch-all counter */
    for (int i = 0; i < kRIB_LAST_RI; i++) {
        if (!gRibRITable[i])
            gRibRITable[i] = (PRIB_RIPROC)OnOther;
    }

    /* NULL filename → read from stdin */
    RIB_HANDLE rib = RibOpen(filename, kRIB_LAST_RI, gRibRITable);
    if (rib == kRIB_ERRRC_PTR) {
        fprintf(stderr, "Error: could not open '%s'\n",
                filename ? filename : "<stdin>");
        return 1;
    }

    printf("Reading: %s\n", filename ? filename : "<stdin>");

    int rc;
    while ((rc = RibRead(rib)) != EOF) {
        if (rc != kRIB_OK)
            fprintf(stderr, "  Warning: RibRead returned %d\n", rc);
    }

    printf("Done — %d RI call(s) dispatched.\n", g_call_count);
    RibClose(rib);
    return 0;
}
