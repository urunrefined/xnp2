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
#include "commng.h"
#include "soundmng.h"
#include "signalFD.h"
#include "loop.h"
#include "exception.h"
#include "keystat.h"
#include "inicfg.h"
#include "global.h"

#include <sys/stat.h>
#include <getopt.h>
#include <locale.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include <stdexcept>

static const char appname[] = "np2";
static const char ini_title[] = "NekoProjectII_Katze";

/*
 * option
 */
static struct option longopts[] = {
	{ "config",     required_argument, 0,  'c' },
	{ "newconfig",  required_argument, 0,  'n' },
	{ "listconfig", no_argument,       0,  'l' },
	{ "help",       no_argument,       0,  'h' },
	{ 0,            0,                 0,   0  },
};

static void
usage(const char *progname)
{
	printf("Usage: %s [options]\n\n", progname);
	printf("options:\n");
	printf("  --help            [-h]              : print this message\n");
	printf("  --config          [-c] <configname> : specify config file\n");
	printf("  --newconfig       [-n] <configname> : specify config name\n");
	printf("  --listconfig      [-l]              : list available configs\n");

	exit(1);
}

static void parseArguments(int argc, char *argv[],
		std::string& progname, std::string& configName, bool& help, bool& newConfig, bool& listConfig, bool& useConfig){

	int ch;

	if(argc < 1){
		printf("Shell error -- No program name");
		exit(56);
	}

	progname = argv[0];

	while ((ch = getopt_long(argc, argv, "hlc:n:", longopts, NULL)) != -1) {
		switch (ch) {
		case 'c':
		{
			useConfig = true;
			configName = optarg;
			break;
		}
		case 'n':
		{
			newConfig = true;
			configName = optarg;
			break;
		}
		case 'l':
		{
			listConfig = true;
			break;
		}

		case 'h':
		default:
			help = true;
			break;
		}
	}
}

class Directory{
public:
	DIR *dir;

	Directory(const char *name){
		dir = opendir(name);
		if(!dir){
			throw std::string ("Cannot open directory ") + name;
		}
	}

	~Directory(){
		closedir(dir);
	}
};

std::vector<std::string> getDirFiles(const char* name){
	Directory directory(name);

	std::vector<std::string> filenames;

	struct dirent *dir;

	while((dir = readdir(directory.dir)) != 0){
		if(strcmp(dir->d_name, ".") == 0) continue;
		if(strcmp(dir->d_name, "..") == 0) continue;
		filenames.push_back(dir->d_name);
	}

	return filenames;
}

static std::vector<std::string> discoverConfigFiles(const std::string& configDir){
	return getDirFiles(configDir.c_str());
}

static std::string discoverHomeDir(){
	char *env = getenv("HOME");

	if(!env){
		printf("No home environment variable set. Please set it\n");
		exit(2);
	}

	return env;
}

static std::string discoverXnp2Dir(const std::string& homeDir){
	return homeDir + "/.np2";
}

static std::string discoverConfigDir(const std::string& xnp2Dir){
	return xnp2Dir + "/config";
}

static std::string discoverArtifactDir(const std::string& xnp2Dir){
	return xnp2Dir + "/res";
}

static void createIfNoExist(const std::string& dirName){
	struct stat sb;

	if (stat(dirName.c_str(), &sb) < 0) {
		if (mkdir(dirName.c_str(), 0700) < 0) {
			perror(dirName.c_str());
			exit(1);
		}
	} else if (!S_ISDIR(sb.st_mode)) {
		printf("%s isn't directory.\n",
			dirName.c_str());
		exit(1);
	}
}

static void go(int argc, char *argv[]){
	BR::SignalFD sfd;

	bool listConfig = false;
	bool newConfig = false;
	bool useConfig = false;
	bool help = false;

	std::string progName;
	std::string configName;

	parseArguments(argc, argv, progName, configName, help, newConfig, listConfig, useConfig);

	if(help){
		usage(progName.c_str());
	}

	std::string homeDir = discoverHomeDir();
	std::string xnp2Dir = discoverXnp2Dir(homeDir);
	std::string configDir = discoverConfigDir(xnp2Dir);
	std::string artifactDir = discoverArtifactDir(xnp2Dir);

	printf("xnp2dir: %s\n", xnp2Dir.c_str());
	printf("configdir: %s\n", configDir.c_str());
	printf("artifactDir: %s\n", artifactDir.c_str());

	createIfNoExist(xnp2Dir);
	createIfNoExist(configDir);
	createIfNoExist(artifactDir);

	std::vector<std::string> configs = discoverConfigFiles(configDir);

	//TODO Statfile
	std::string statFile;

	if(listConfig){
		if(configs.empty()){
			printf("No available configs");
			usage(progName.c_str());
		}

		printf("Available configs:\n");

		for(const auto& config : configs){
			printf("-- %s\n", config.c_str());
		}

		return;
	}

	std::string configSetDir = configDir + "/" + configName;
	std::string configFile = configSetDir + "/config";

	if(newConfig){
		createIfNoExist(configSetDir);
		printf("Create new config %s\n", configFile.c_str());

		IniCfg iniCfg(np2oscfg, np2cfg);
		initload(configFile.c_str(), ini_title, iniCfg.config.data(), iniCfg.config.size());

		if(access(configFile.c_str(), F_OK) != 0){
			initsave(configFile.c_str(), ini_title, iniCfg.config.data(), iniCfg.config.size());
			printf("Config created, set font file in %s\n", configFile.c_str());
			return;
		}
		else
		{
			printf("Config already exists. Stop.\n");
			return;
		}
	}

	if(!useConfig){
		printf("No config selected\n");
		usage(progName.c_str());
		return;
	}

	//discoverSavFile(modulefile, statpath);
	//Config should have been selected

	printf("Use config %s\n", configFile.c_str());

	IniCfg iniCfg(np2oscfg, np2cfg);
	initload(configFile.c_str(), ini_title, iniCfg.config.data(), iniCfg.config.size());

	if(np2cfg.fontfile[0] == '\0'){
		printf("Set font file in %s", configFile.c_str());
		return;
	}

	file_setcd(artifactDir.c_str());

	rand_setseed((SINT32)time(NULL));
	keystat_initialize();
	soundmng_initialize();
	commng_initialize();

	pccore_init();
	S98_init();
	pccore_reset();

	for (int i = 0; i < 4; i++) {
		if(strlen(np2cfg.fdd[i])){
			printf("Ready Disk %s\n", np2cfg.fdd[i]);
			diskdrv_readyfdd(i, np2cfg.fdd[i], 0);
		}
	}

	BR::loop(sfd, np2cfg, np2oscfg);

	printf("Normal exit\n");

	pccore_cfgupdate();
	S98_trash();

	pccore_term();
	soundmng_deinitialize();

	initsave(configFile.c_str(), ini_title, iniCfg.config.data(), iniCfg.config.size());
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
	catch(const std::string& ex){
		printf("np2 exception thrown [%s]\n", ex.c_str());
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
