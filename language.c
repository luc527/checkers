#include <stdio.h>
#include "checkers.h"

char *msgs[NLANGS][NMESSAGES];

static void init_pt()
{
    msgs[PT][SOURCE_PROMPT] = "> Origem: ";
    msgs[PT][DESTINATION_PROMPT] = "> Destino: ";
    msgs[PT][NOT_AN_OPTION] = "Essa nao e uma opção. Tente novamente.\n";
    msgs[PT][MUST_CAPTURE_WITH] = "Voce deve capturar com uma das seguintes pecas: ";
    msgs[PT][MUST_PERFORM_SEQUENTIAL_CAPTURE] = "Voce deve realizar uma captura sequencial a partir de ";
    msgs[PT][WHITE_WINS_MSG] = "Branco vence!\n";
    msgs[PT][BLACK_WINS_MSG] = "Preto vence!\n";
    msgs[PT][TIE_MSG] = "Empate.\n";
    msgs[PT][CURRENT_PLAYER] = "Jogador atual: ";
    msgs[PT][WHITE_PLAYER] = "branco (o@)\n";
    msgs[PT][BLACK_PLAYER] = "preto (*X)\n";
	msgs[PT][MUST_SELECT_MOVEMENT] = "Voce deve selecionar um movimento.";
	msgs[PT][ALREADY_SELECTED_MOVEMENT] = "Voce ja selecionou um movimento."
                                          " Cancele ou confirme-o.";
}

static void init_en()
{
    msgs[EN][SOURCE_PROMPT] = "> Source: ";
    msgs[EN][DESTINATION_PROMPT] = "> Destination: ";
    msgs[EN][NOT_AN_OPTION] = "That is not an option. Try again.\n";
    msgs[EN][MUST_CAPTURE_WITH] = "You must capture with one of the following: ";
    msgs[EN][MUST_PERFORM_SEQUENTIAL_CAPTURE] = "You must perform a sequential capture from: ";
    msgs[EN][WHITE_WINS_MSG] = "White wins!\n";
    msgs[EN][BLACK_WINS_MSG] = "Black wins!\n";
    msgs[EN][TIE_MSG] = "It's a tie.\n";
    msgs[EN][CURRENT_PLAYER] = "Current player: ";
    msgs[EN][WHITE_PLAYER] = "white (o@)\n";
    msgs[EN][BLACK_PLAYER] = "black (*X)\n";
	msgs[EN][MUST_SELECT_MOVEMENT] = "You must select a movement.";
	msgs[EN][ALREADY_SELECTED_MOVEMENT] = "You already selected a movement."
	                                      " Either undo or confirm it.";
}

void init_messages_array()
{
    init_pt();
    init_en();
}


char *getmsg(Message msg, Language lang)
{
	static char *err = "ERROR: Attempted to print invalid message"
	                   " and/or message in invalid language.";
	if (msg >= 0 && msg < NMESSAGES && lang >= 0 && lang < NLANGS)
		return msgs[lang][msg];
	else return err;
}

