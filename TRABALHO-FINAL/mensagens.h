#ifndef MENSAGENSERRO_H_INCLUDED
#define MENSAGENSERRO_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>

// Funções para impressão de mensagens de acordo com o erro
void msg_reg_removido();
void msg_rrn_invalido();
void msg_registro_inexistente();
void msg_arquivo_inexistente();
void msg_dados_ja_cadastrados();
void msg_funcionalidade_inexistente();
void msg_erro_padrao();
void msg_alocacao_mem();

#endif // MENSAGENSERRO_H_INCLUDED