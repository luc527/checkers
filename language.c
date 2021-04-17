#include <stdio.h>
#include "checkers.h"

char *messages[NLANGS][NMESSAGES];

static void init_pt()
{
    messages[PT][SOURCE_PROMPT] = "> Origem: ";
    messages[PT][DESTINATION_PROMPT] = "> Destino: ";
    messages[PT][NOT_AN_OPTION] = "Essa nao e uma opção. Tente novamente.\n";
    messages[PT][MUST_CAPTURE_WITH] = "Voce deve capturar com uma das seguintes pecas: ";
    messages[PT][MUST_PERFORM_SEQUENTIAL_CAPTURE] = "Voce deve realizar uma captura sequencial a partir de ";
    messages[PT][WHITE_WINS_MSG] = "Branco vence!\n";
    messages[PT][BLACK_WINS_MSG] = "Preto vence!\n";
    messages[PT][TIE_MSG] = "Empate.\n";
    messages[PT][CURRENT_PLAYER] = "Jogador atual: ";
    messages[PT][WHITE_PLAYER] = "branco (o@)\n";
    messages[PT][TIE_MSG] = "preto (*X)\n";
}

static void init_en()
{
    messages[EN][SOURCE_PROMPT] = "> Source: ";
    messages[EN][DESTINATION_PROMPT] = "> Destination: ";
    messages[EN][NOT_AN_OPTION] = "That is not an option. Try again.\n";
    messages[EN][MUST_CAPTURE_WITH] = "You must capture with one of the following: ";
    messages[EN][MUST_PERFORM_SEQUENTIAL_CAPTURE] = "You must perform a sequential capture from: ";
    messages[EN][WHITE_WINS_MSG] = "White wins!\n";
    messages[EN][BLACK_WINS] = "Black wins!\n";
    messages[EN][TIE_MSG] = "It's a tie.\n";
    messages[EN][CURRENT_PLAYER] = "Current player: ";
    messages[EN][WHITE_PLAYER] = "white (o@)\n";
    messages[EN][TIE_MSG] = "black (*X)\n";
}

void init_messages_array()
{
    init_pt();
    init_en();
}


void printmsg(Message msg, Language lang)
{
    if (msg >= 0 && msg < NMESSAGES && lang >= 0 && lang < NLANGS) {
        printf("%s", messages[lang][msg]);
    } else {
        printf("Attempted to print invalid message (%d) and/or message in invalid language (%d).\n",
                msg, lang);
    }
}

