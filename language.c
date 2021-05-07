#include <stdio.h>
#include "checkers.h"

static char *msgs[NLANGS][NMESSAGES];

static bool msgs_initialized = false;

static void init_pt()
{
    msgs[PT][SOURCE_PROMPT] = "> Origem: ";
    msgs[PT][DESTINATION_PROMPT] = "> Destino: ";
    msgs[PT][NOT_AN_OPTION] = "Essa nao e uma opção. Tente novamente.\n";
    msgs[PT][MUST_CAPTURE] = "Voce deve realizar uma captura.";
    msgs[PT][MUST_PERFORM_SEQUENTIAL_CAPTURE] = "Voce deve realizar uma captura sequencial.";
    msgs[PT][WHITE_WINS_MSG] = "Branco vence!\n";
    msgs[PT][BLACK_WINS_MSG] = "Preto vence!\n";
    msgs[PT][TIE_MSG] = "Empate.\n";
    msgs[PT][CURRENT_PLAYER] = "Jogador atual: ";
    msgs[PT][WHITE_PLAYER] = "branco (o@)";
    msgs[PT][BLACK_PLAYER] = "preto (*X)";
    msgs[PT][MUST_SELECT_MOVEMENT] = "Voce deve selecionar um movimento.";
    msgs[PT][ALREADY_SELECTED_MOVEMENT] = "Voce ja selecionou um movimento."
                                          " Cancele ou confirme-o.";
    msgs[PT][INSTRUCTIONS] = ""
        "== MOVIMENTO NO TABULEIRO == \n"
        "WASD para se movimentar pelo tabuleiro\n"
        "m para marcar origem do movimento\n"
        "m marcar para selecionar destino do movimento\n"
        "u para desfazer o movimento\n"
        "SPACE para confirmar o movimento\n";
}

static void init_en()
{
    msgs[EN][SOURCE_PROMPT] = "> Source: ";
    msgs[EN][DESTINATION_PROMPT] = "> Destination: ";
    msgs[EN][NOT_AN_OPTION] = "That is not an option. Try again.\n";
    msgs[EN][MUST_CAPTURE] = "You must perform a capture.";
    msgs[EN][MUST_PERFORM_SEQUENTIAL_CAPTURE] = "You must perform a sequential capture.";
    msgs[EN][WHITE_WINS_MSG] = "White wins!\n";
    msgs[EN][BLACK_WINS_MSG] = "Black wins!\n";
    msgs[EN][TIE_MSG] = "It's a tie.\n";
    msgs[EN][CURRENT_PLAYER] = "Current player: ";
    msgs[EN][WHITE_PLAYER] = "white (o@)";
    msgs[EN][BLACK_PLAYER] = "black (*X)";
    msgs[EN][MUST_SELECT_MOVEMENT] = "You must select a movement.";
    msgs[EN][ALREADY_SELECTED_MOVEMENT] = "You already selected a movement."
                                          " Either undo or confirm it.";
    msgs[EN][INSTRUCTIONS] = ""
        "== BOARD MOVEMENT == \n"
        "WASD to move around the board\n"
        "m to mark movement source\n"
        "m again to mark movement destination\n"
        "u to undo movement\n"
        "SPACE to confirm movement\n";
}

static void init_messages()
{
    msgs_initialized = true;
    init_pt();
    init_en();
}


char *getmsg(Message msg, Language lang)
{
    static char *invalid_err = "ERROR: Attempted to print invalid message"
                               " and/or message in invalid language.";

    if (!msgs_initialized)  init_messages();

    if (msg < 0 || msg >= NMESSAGES || lang < 0 || lang >= NLANGS)
        return invalid_err;
    else
        return msgs[lang][msg];
}

