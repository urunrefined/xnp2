/*
 * Copyright (c) 2003 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "compiler.h"

#include "np2.h"
#include "fdd/diskdrv.h"
#include "dosio.h"
#include "ini.h"
#include "parts.h"
#include "pccore.h"
#include "s98.h"
#include "scrndraw.h"
#include "serial.h"
#include "timing.h"
#include "milstr.h"

#include "viewer.h"
#include "debugwin.h"

#include "commng.h"
#include "fontmng.h"
#include "joymng.h"
#include "kbdmng.h"
#include "scrnmng.h"
#include "soundmng.h"
#include "taskmng.h"
#include "trace.h"
#include "signalFD.h"

#include "loop.h"
#include "exception.h"

#include <SDL.h>

#include <sys/stat.h>
#include <getopt.h>
#include <locale.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include <stdexcept>


static const char appname[] = "np2";

/*
 * option
 */
static struct option longopts[] = {
	{ "config", required_argument,  0,  'c' },
	{ "help",   no_argument,        0,  'h' },
	{ 0,        0,                  0,   0  },
};

static void
usage(const char *progname)
{
	printf("Usage: %s [options] [[FD1 image] [[FD2 image] [[FD3 image] [FD4 image]]]]\n\n", progname);
	printf("options:\n");
	printf("\t--help            [-h]        : print this message\n");
	printf("\t--config          [-c] <file> : specify config file\n");

	exit(1);
}

static int parseArguments(int argc, char *argv[]){
	int ch;

	while ((ch = getopt_long(argc, argv, "c:C:t:vh", longopts, NULL)) != -1) {
		switch (ch) {
		case 'c':
		{
			struct stat sb;

			if (stat(optarg, &sb) < 0 || !S_ISREG(sb.st_mode)) {
				printf("Can't access %s.\n", optarg);
				exit(1);
			}
			milstr_ncpy(modulefile, optarg, sizeof(modulefile));
			break;
		}
		case 'h':
		case '?':
		default:
			usage(argv[0]);
			break;
		}
	}

	//optind is a global in getopt_core
	return optind;
}

static void discoverConfigFile(){
	if (modulefile[0] == '\0') {
		char *env = getenv("HOME");
		if (env) {
			/* base dir */
			snprintf(modulefile, sizeof(modulefile),
				"%s/.np2/", env);

			struct stat sb;

			if (stat(modulefile, &sb) < 0) {
				if (mkdir(modulefile, 0700) < 0) {
					perror(modulefile);
					exit(1);
				}
			} else if (!S_ISDIR(sb.st_mode)) {
				printf("%s isn't directory.\n",
					modulefile);
				exit(1);
			}

			/* config file */
			milstr_ncat(modulefile, appname, sizeof(modulefile));
			milstr_ncat(modulefile, "rc", sizeof(modulefile));
			if ((stat(modulefile, &sb) >= 0)
			 && !S_ISREG(sb.st_mode)) {
				printf("%s isn't regular file.\n",
					modulefile);
			}
		}
	}
}

static void discoverFontFile(){
	if (modulefile[0] != '\0') {
		/* font file */
		file_cpyname(np2cfg.fontfile, modulefile,
			sizeof(np2cfg.fontfile));
		file_cutname(np2cfg.fontfile);
		file_setseparator(np2cfg.fontfile, sizeof(np2cfg.fontfile));
		file_catname(np2cfg.fontfile, "font.bmp",
			sizeof(np2cfg.fontfile));

		/* statsave dir */
		file_cpyname(statpath, modulefile, sizeof(statpath));
		file_cutname(statpath);
		file_catname(statpath, "/sav/", sizeof(statpath));

		struct stat sb;

		if (stat(statpath, &sb) < 0) {
			if (mkdir(statpath, 0700) < 0) {
				perror(statpath);
				exit(1);
			}
		} else if (!S_ISDIR(sb.st_mode)) {
			printf("%s isn't directory.\n",
				statpath);
			exit(1);
		}
		file_catname(statpath, appname, sizeof(statpath));
	}
}

static void go(int argc, char *argv[]){
	BR::SignalFD sfd;

	int parsedCount = parseArguments(argc, argv);

	argc -= parsedCount;
	argv += parsedCount;

	discoverConfigFile();
	discoverFontFile();

	dosio_init();
	file_setcd(modulefile);
	initload();

	rand_setseed((SINT32)time(NULL));

	TRACEINIT();

	viewer_init();
	keystat_initialize();
	soundmng_initialize();

	joymng_initialize();

	commng_initialize();
	taskmng_initialize();

	pccore_init();
	S98_init();
	pccore_reset();

	int drvmax = (argc < 4) ? argc : 4;
	for (int i = 0; i < drvmax; i++) {
		diskdrv_readyfdd(i, argv[i], 0);
	}

	BR::loop(sfd);

	printf("Normal exit\n");

	pccore_cfgupdate();
	joymng_deinitialize();
	S98_trash();

	pccore_term();
	debugwin_destroy();

	soundmng_deinitialize();

	initsave();

	TRACETERM();
	dosio_term();

	viewer_term();
}

/*
 * main
 */
int
main(int argc, char *argv[])
{
	try {
		go(argc, argv);
	}
	catch(BR::Exception& ex){
		printf("np2 exception thrown [%s]\n", ex.msg);
	}
	catch(BR::CException& ex){
		printf("np2 C lib exception thrown [%s] errno [%d, %s]\n",
			ex.msg, ex.osErrno, strerror(ex.osErrno));
	}
	catch(std::exception& ex){
		printf("stdlib exception thrown [%s]\n", ex.what());
	}
	catch(...){
		printf("Unknown exception thrown\n");
	}

	return 0;
}
