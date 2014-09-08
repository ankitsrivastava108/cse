#!/usr/bin/env python

# A short Python script to analyze kdump output for the `utrace'
# debugging output of FreeBSD malloc.

# Copyright (c) 2008 Giorgos Keramidas <keramida@FreeBSD.org>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

import errno
import getopt
import re
import sys
import os

# -------------------- script options ----------------------------------

verbose = None                  # Run in `quiet' mode by default.

# -------------------- useful functions --------------------------------

def ignoremsg(msg=None):
    """Print nothing.  Ignore the message string passed to us.
    This is useful as a `callback' that uses the same function stub
    as message() but prints nothing."""
    return None

def message(msg=None):
    """Print an optional `msg' message string to our standard error
    stream.  Note that a newline is appended automatically to the
    message string, as if it was displayed with print(), and no
    exceptions are captured by message()."""
    if msg:
        s = "%s\n" % (msg)
        sys.stderr.write(s)
        sys.stderr.flush()
    return None

def error(code, msg=None):
    """Print `msg' as an optional error message string, and die with an
    error of `code'."""
    if msg:
        s = '%s: error: %s' % (progname, msg)
        message(s)
    sys.exit(code)

def warning(msg=None):
    """Print `msg' as an optional warning string."""
    if msg:
        s = '%s: warning: %s' % (progname, msg)
        message(s)
    return None

def debug(level=1, msg=None):
    """Print an optional debugging message `msg', when the verbosity
    level `verbose' of the script exceeds or is equal to `level'
    (default=1)."""
    if verbose and verbose >= level and msg:
        message("# " + msg)

# -------------------- main script body --------------------------------

# Precompiled regular expressions, to match the `utrace' output format
# of the kdump utility, and extract bits of the allocation or memory
# freeing trace entries.  The output of `kdump' looks like this:
#
# 42246 a.out    1224038211.153343 USER  malloc_init()
# 42246 a.out    1224038211.153434 USER  0x8101080 = malloc(100)
# 42246 a.out    1224038211.153453 USER  0x8103300 = malloc(600)
# 42246 a.out    1224038211.153462 USER  free(0x8103300)
allocre = None
freere = None

def loadtrace(fp):
    """Read `kdump' output from the open file passed as `fp', and try to
    detect memory leaks by matching the allocation entries with the
    memory freeing trace entries."""

    global allocre, freere
    if not allocre:
        try:
            debug(2, "precompiling alloc-re")
            allocre = re.compile(r'^.*\s+USER\s+.*(0x[0-9a-fA-F]+)\s+=\s+malloc\((\d+)\)\s*$')
        except Exception, inst:
            error(1, "invalid regular expression: %s" % inst)
    if not freere:
        try:
            debug(2, "precompiling free-re")
            freere = re.compile(r'^.*\s+USER\s+free\((0x[0-9a-fA-F]+)\)\s*$')
        except Exception, inst:
            error(1, "invalid regular expression: %s" % inst)

    if verbose and verbose > 0:
        log = message
    else:
        log = ignoremsg

    debug(2, "initializing address allocation map")
    amap = {}                   # Empty allocation map
    debug(2, "initializing address cache")
    acache = {}
    for l in fp.readlines():
        l = l.rstrip('\r\n')
        debug(4, "line \"%s\"" % l)
        m = allocre.match(l)
        if m:
            debug(3, "alloc line: %s" % l)
            addr = int(m.group(1), 0)
            size = int(m.group(2), 0)
            if addr in amap:
                error(1, "memory at 0x%x allocated already" % addr)
            amap[addr] = (size, l)
            acache[addr] = True
            log("trace: allocated %d bytes at 0x%x" % (size, addr))
            debug(2, "  utrace record:")
            debug(2, "  %s" % l)
        m = freere.match(l)
        if m:
            debug(3, "free line: %s" % l)
            addr = int(m.group(1), 0)
            if addr in amap:
                (size, msg) = amap.pop(addr)
                log("trace: freed %d bytes at 0x%x" % (size, addr))
                debug(2, "  utrace record:")
                debug(2, "  %s" % l)
            else:
                if addr in acache:
                    log("warning: double-free at 0x%x" % addr)
                else:
                    log("warning: free of non-malloced region at 0x%x" % addr)
                debug(2, "  utrace record:")
                debug(2, "  %s" % l)

    leaks = amap.keys()
    if len(leaks) == 0:
        debug(2, "no leaks found; address map empty")
        return None
    message("--- memory leaks ---");
    leakedbytes = 0
    for addr in leaks:
        (size, msg) = amap[addr]
        message("leaked %d bytes at 0x%x" % (size, addr))
        message("  utrace record:")
        message("  %s" % msg)
        leakedbytes += size
    message("total leaked bytes = %d" % leakedbytes)
    return leakedbytes

if __name__ == '__main__':
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'v')
    except getopt.GetoptError, err:
        usage()

    # Go through the list of command-line options, and tune the globals
    # which affect the behavior of the script accordingly.
    for o, v in opts:
        if o == '-v':
            verbose = verbose and (verbose + 1) or 1
            debug(1, "Bumping verbosity level to %d" % verbose)
        else:
            assert False, "Unhandled option `%s'" % o

    try:
        if len(args) == 0:
            debug(1, "analyzing trace from <stdin>")
            loadtrace(sys.stdin)
        else:
            for fname in args:
                try:
                    debug(1, "analyzing trace `%s'")
                    loadtrace(file(fname))
                except IOError, inst:
                    if inst.errno == errno.ENOENT:
                        print "%s: %s" % (fname, str(inst))
		else:
                        raise
    except KeyboardInterrupt, inst:
        message("Interrupted.")
        sys.exit(0)
