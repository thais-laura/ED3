#include <stdio.h>
#include "funcionalidades.h"
#include "mensagens.h"

int main() {
    int func;
    scanf("%d", &func);

    switch (func) {
        case 1:
            funcionalidade1();
            break;

        case 2:
            funcionalidade2();
            break;

        case 3:
            funcionalidade3();
            break;

        case 4:
            funcionalidade4();
            break;

        default:
            msg_funcionalidade_inexistente();
    }
}
