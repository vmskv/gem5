/*
 * Copyright (c) 2001-2006 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Nathan Binkert
 *          Steve Reinhardt
 */

#include <ctype.h>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "base/misc.hh"
#include "base/trace.hh"
#include "base/str.hh"
#include "base/varargs.hh"

using namespace std;

namespace Trace {
const string DefaultName("global");
FlagVec flags(NumFlags, false);
bool enabled = true;

//
// This variable holds the output stream for debug information.  Other
// than setting up/redirecting this stream, do *NOT* reference this
// directly; use DebugOut() (see below) to access this stream for
// output.
//
ostream *dprintf_stream = &cerr;

ObjectMatch ignore;

Log theLog;

Log::Log()
{
    size = 0;
    buffer = NULL;
}


void
Log::init(int _size)
{
    if (buffer != NULL) {
        fatal("Trace::Log::init called twice!");
    }

    size = _size;

    buffer = new Record *[size];

    for (int i = 0; i < size; ++i) {
        buffer[i] = NULL;
    }

    nextRecPtr = &buffer[0];
    wrapRecPtr = &buffer[size];
}


Log::~Log()
{
    for (int i = 0; i < size; ++i) {
        delete buffer[i];
    }

    delete [] buffer;
}


void
Log::append(Record *rec)
{
    // dump record to output stream if there's one open
    if (dprintf_stream != NULL) {
        rec->dump(*dprintf_stream);
    } else {
        rec->dump(cout);
    }

    // no buffering: justget rid of it now
    if (buffer == NULL) {
        delete rec;
        return;
    }

    Record *oldRec = *nextRecPtr;

    if (oldRec != NULL) {
        // log has wrapped: overwrite
        delete oldRec;
    }

    *nextRecPtr = rec;

    if (++nextRecPtr == wrapRecPtr) {
        nextRecPtr = &buffer[0];
    }
}


void
Log::dump(ostream &os)
{
    if (buffer == NULL) {
        return;
    }

    Record **bufPtr = nextRecPtr;

    if (*bufPtr == NULL) {
        // next record slot is empty: log must not be full yet.
        // start dumping from beginning of buffer
        bufPtr = buffer;
    }

    do {
        Record *rec = *bufPtr;

        rec->dump(os);

        if (++bufPtr == wrapRecPtr) {
            bufPtr = &buffer[0];
        }
    } while (bufPtr != nextRecPtr);
}

PrintfRecord::~PrintfRecord()
{}

void
PrintfRecord::dump(ostream &os)
{
    string fmt = "";

    if (!name.empty()) {
        fmt = "%s: " + fmt;
        args.push_front(name);
    }

    if (cycle != (Tick)-1) {
        fmt = "%7d: " + fmt;
        args.push_front(cycle);
    }

    fmt += format;

    ccprintf(os, fmt.c_str(), args);
    os.flush();
}

DataRecord::DataRecord(Tick _cycle, const string &_name,
                       const void *_data, int _len)
    : Record(_cycle), name(_name), len(_len)
{
    data = new uint8_t[len];
    memcpy(data, _data, len);
}

DataRecord::~DataRecord()
{
    delete [] data;
}

void
DataRecord::dump(ostream &os)
{
    int c, i, j;

    for (i = 0; i < len; i += 16) {
        ccprintf(os, "%d: %s: %08x  ", cycle, name, i);
        c = len - i;
        if (c > 16) c = 16;

        for (j = 0; j < c; j++) {
            ccprintf(os, "%02x ", data[i + j] & 0xff);
            if ((j & 0xf) == 7 && j > 0)
                ccprintf(os, " ");
        }

        for (; j < 16; j++)
            ccprintf(os, "   ");
        ccprintf(os, "  ");

        for (j = 0; j < c; j++) {
            int ch = data[i + j] & 0x7f;
            ccprintf(os,
                     "%c", (char)(isprint(ch) ? ch : ' '));
        }

        ccprintf(os, "\n");

        if (c < 16)
            break;
    }
}
} // namespace Trace
