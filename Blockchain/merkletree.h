#ifndef MERKLETREE_H
#define MERKLETREE_H

#include "sha256.h"
#include "stdlib.h"
#include "string.h"

typedef struct no {
  unsigned char hash[SHA256_SIZE_BYTES];
  struct no *left;
  struct no *right;
  struct no *pai;
} No;

typedef struct merkletree {
  No **folhas;
  No *raiz;
  int num_folhas;
} Merkletree;

No *criar_No_Folha(const char *transacao);
int verificar_transacao(No *raiz, const char *transacao);
No *criar_No_Interno(No *esquerdo, No *direito);
No *recriar_Merkle_Tree(No **folhas, int num_folhas);
Merkletree *criar_Merkle_Tree();
void adicionar_folha(Merkletree *arvore, const char *transacao);
No *ultimo_no(Merkletree *arvore);
void atualizar_hash_no(No *no);
void atualizar_caminho(No *folha_modificada);
int verificar_transacao(No *raiz, const char *transacao);
#endif