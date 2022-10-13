#pragma once

#include <signal.h>
#include <cstdio>

void crash() {
	fflush(stdout);
	raise(SIGSEGV);
}