// File: Const.h
// 10/2011
// abenjamin@wustl.edu - Alex Benjamin
// linfeixb27@gmail.com - Yinfan Li
// mahesh.mahadevan@gmail.com - Mahesh M.

#ifndef	CONST_H
#define CONST_H

#include <ace/ACE.h>
#include <string>

#define	ERR_MISSING_F_FLAG		-10
#define	ERR_MISSING_N_FLAG		-11
#define ERROR_CMD_LN_SYNTAX		-12
#define ERR_INVALID_ARG_NUM		-13
#define ERR_INVALID_FILE		-14
#define	ERR_NO_VALID_DEFS		-15
#define	ERR_OPEN_CONNECTION		-16
#define	ERR_INET_ADDR_SET		-17
#define ERR_REACTOR_LOOP_FAILED	-18

const int DEFAULT_PORT = 2000;
const ACE_TCHAR DEFAULT_ADDR[] = ACE_TEXT("localhost");
const int MIN_PORT = 1024;
const unsigned int MIN_PLAYER = 3; // # of players needed to start a game
const unsigned int MAX_PLAYER = 10; // # of players that can be playing a game (deck size / hand size)
const unsigned int MAX_PLAYER_DRAW = 8; // # of players that can be playing a game type Draw
const int timeBetweenGames = 5; // how long to wait before starting a new game

const int DEFAULT_LOG_BUFFERSIZE = 1024;

//Lab2 - Constants to define game types allowed.
const std::string GAME_TYPE_DRAW = "Draw";
const std::string GAME_TYPE_STUD = "Stud";

#endif
