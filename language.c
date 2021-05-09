#include <stdio.h>
#include "checkers.h"

static char *msgs[NLANGS][NMESSAGES] = {
    [PT] = {
        [SOURCE_PROMPT] = "> Origem: ",
        [DESTINATION_PROMPT] = "> Destino: ",
        [NOT_AN_OPTION] = "Essa nao e uma opção. Tente novamente.\n",
        [MUST_CAPTURE] = "Voce deve realizar uma captura.",
        [MUST_PERFORM_SEQUENTIAL_CAPTURE] = "Voce deve realizar uma captura sequencial.",
        [WHITE_WINS_MSG] = "Branco vence!\n",
        [BLACK_WINS_MSG] = "Preto vence!\n",
        [TIE_MSG] = "Empate.\n",
        [CURRENT_PLAYER] = "Jogador atual: ",
        [WHITE_PLAYER] = "branco (o@)",
        [BLACK_PLAYER] = "preto (*X)",
        [MUST_SELECT_MOVEMENT] = "Voce deve selecionar um movimento.",
        [ALREADY_SELECTED_MOVEMENT] = "Voce ja selecionou um movimento. Cancele ou confirme-o.",
        [INSTRUCTIONS] = ""
            "== MOVIMENTO NO TABULEIRO == \n"
            "a e d para se movimentar pelas opcoes de movimento\n"
            "m para marcar origem do movimento\n"
            "m marcar para selecionar destino do movimento\n"
            "u para desfazer o movimento\n"
            "espaço para confirmar o movimento\n",
    },
    [EN] = {
        [SOURCE_PROMPT] = "> Source: ",
        [DESTINATION_PROMPT] = "> Destination: ",
        [NOT_AN_OPTION] = "That is not an option. Try again.\n",
        [MUST_CAPTURE] = "You must perform a capture.",
        [MUST_PERFORM_SEQUENTIAL_CAPTURE] = "You must perform a sequential capture.",
        [WHITE_WINS_MSG] = "White wins!\n",
        [BLACK_WINS_MSG] = "Black wins!\n",
        [TIE_MSG] = "It's a tie.\n",
        [CURRENT_PLAYER] = "Current player: ",
        [WHITE_PLAYER] = "white (o@)",
        [BLACK_PLAYER] = "black (*X)",
        [MUST_SELECT_MOVEMENT] = "You must select a movement.",
        [ALREADY_SELECTED_MOVEMENT] = "You already selected a movement. Either undo or confirm it.",
        [INSTRUCTIONS] = ""
            "== BOARD MOVEMENT == \n"
            "a and d to cycle through the movement options\n"
            "m to mark movement source\n"
            "m again to mark movement destination\n"
            "u to undo movement\n"
            "SPACE to confirm movement\n",
    },
};

char *getmsg(Message msg, Language lang)
{
    static char *invalid_err = "ERROR: Attempted to print invalid message"
                               " and/or message in invalid language.";

    if (msg < 0 || msg >= NMESSAGES || lang < 0 || lang >= NLANGS)
        return invalid_err;
    else
        return msgs[lang][msg];
}

