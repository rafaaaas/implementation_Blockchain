#include "merkletree.h"

No *criar_No_Folha(const char *transacao) {
  No *novoNo = (No *)malloc(sizeof(No));
  novoNo->left = novoNo->right = NULL;
  sha256(transacao, strlen(transacao), novoNo->hash);
  return novoNo;
}

int verificar_transacao(No *raiz, const char *transacao) {
  unsigned char hash_transacao[SHA256_SIZE_BYTES];
  sha256((unsigned char *)transacao, strlen(transacao), hash_transacao);

  // Percorre a árvore para verificar se existe uma folha com esse hash
  if (raiz == NULL) {
    return 0;
  }

  // Se for uma folha, compara os hashes
  if (raiz->left == NULL && raiz->right == NULL) {
    return memcmp(raiz->hash, hash_transacao, SHA256_SIZE_BYTES) == 0;
  }

  // Caso contrário, busca nos filhos esquerdo e direito
  return verificar_transacao(raiz->left, transacao) ||
         verificar_transacao(raiz->right, transacao);
}

// Cria os nó que vão guardar os hash dos blocos abaixo
No *criar_No_Interno(No *esquerdo, No *direito) {
  No *novoNo = (No *)malloc(sizeof(No));
  novoNo->left = esquerdo;
  novoNo->right = direito;
  novoNo->left->pai = novoNo;
  novoNo->right->pai = novoNo;

  unsigned char Hashconcatenado[2 * SHA256_SIZE_BYTES];
  memcpy(Hashconcatenado, esquerdo->hash, SHA256_SIZE_BYTES);
  memcpy(Hashconcatenado + SHA256_SIZE_BYTES, direito->hash, SHA256_SIZE_BYTES);
  sha256(Hashconcatenado, sizeof(Hashconcatenado), novoNo->hash);
  return novoNo;
}

No *recriar_Merkle_Tree(No **folhas, int num_folhas) {
  if (num_folhas == 1)
    return folhas[0];

  int novo_num_folhas = (num_folhas + 1) / 2;
  No **pais = (No **)malloc(novo_num_folhas * sizeof(No *));

  for (int i = 0; i < novo_num_folhas; i++) {
    No *left = folhas[2 * i];
    No *right = (2 * i + 1 < num_folhas) ? folhas[2 * i + 1] : left;
    pais[i] = criar_No_Interno(left, right);
  }

  No *raiz = recriar_Merkle_Tree(pais, novo_num_folhas);
  free(pais);
  return raiz;
}

Merkletree *criar_Merkle_Tree() {
  Merkletree *arvore = (Merkletree *)malloc(sizeof(Merkletree));
  arvore->folhas = NULL;
  arvore->num_folhas = 0;
  arvore->raiz = NULL;

  return arvore;
}

void adicionar_folha(Merkletree *arvore, const char *transacao) {
  arvore->num_folhas++;
  arvore->folhas = realloc(arvore->folhas, arvore->num_folhas * sizeof(No *));
  arvore->folhas[arvore->num_folhas - 1] = criar_No_Folha(transacao);
  arvore->raiz = recriar_Merkle_Tree(arvore->folhas, arvore->num_folhas);
  if (arvore->raiz->pai)
    arvore->raiz->pai = NULL;
}

No *ultimo_no(Merkletree *arvore) {
  No *root = arvore->raiz;
  while (root->right != NULL) {
    root = root->right;
  }
  return root;
}

void atualizar_hash_no(No *no) {
  if (no == NULL)
    return;
  // Se for nó folha, não há necessidade de atualizar (ou você pode tratar
  // separadamente)
  if (no->left == NULL || no->right == NULL)
    return;

  unsigned char buffer[2 * SHA256_SIZE_BYTES];
  // Concatena os hashes dos filhos
  memcpy(buffer, no->left->hash, SHA256_SIZE_BYTES);
  memcpy(buffer + SHA256_SIZE_BYTES, no->right->hash, SHA256_SIZE_BYTES);

  // Calcula o hash do nó com base no buffer concatenado
  sha256(buffer, sizeof(buffer), no->hash);
}

void atualizar_caminho(No *folha_modificada) {
  No *atual = folha_modificada->pai;
  while (atual != NULL) {
    atualizar_hash_no(atual);
    atual = atual->pai;
  }
}
