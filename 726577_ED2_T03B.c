/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com encadeamento
 *
 * RA: 
 * Aluno: 
 * ========================================================================== */

/* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51

#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA 							 "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM						  "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 						"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO					    "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO  "**********************EXCLUIR*********************\n"
#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define REGISTRO_INSERIDO "Registro %s inserido com sucesso.\n\n"



/* Registro do jogo */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;

/* Registro da Tabela Hash
 * Contém a chave primária, o RRN do registro atual e o ponteiro para o próximo
 * registro. */
typedef struct chave {
	char pk[TAM_PRIMARY_KEY];
	int rrn;
	struct chave *prox;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct {
  int tam;
  Chave **v;
} Hashtable;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();


/* Exibe o jogo */
int exibir_registro(int rrn);

/*Função de Hash*/
short hash(const char* chave, int tam);


/*Auxiliar para a função de hash*/
short f(char x);

/*Retorna o primeiro número primo >= a*/
int  prox_primo(int a);

/*Funções do Menu*/
void carregar_tabela(Hashtable* tabela);
void cadastrar(Hashtable* tabela);
int  alterar(Hashtable tabela);
void buscar(Hashtable tabela);
int  remover(Hashtable* tabela);
void liberar_tabela(Hashtable* tabela);

/* <<< DECLARE AQUI OS PROTOTIPOS >>> */

Produto recuperar_registro(int rrn);
void criar_tabela(Hashtable* tabela, int tam);
void imprimir_tabela(Hashtable tabela);
void gerarChave(Produto* p);
void inserir_Registro_arquivoDados(Produto p);
int inserir_tabela(Hashtable* tabela, int pos, Chave chave);



/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */

int main() 
{
	
	/* Arquivo */
	int carregarArquivo = 0;
	scanf("%d%*c", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo) 
		carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d%*c", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo) 
		carregar_tabela(&tabela);
	


	/* Execução do programa */
	int opcao = 0;
	while(opcao != 6) {
		scanf("%d%*c", &opcao);
		switch(opcao) {

		case 1:
			cadastrar(&tabela);
			break;
		case 2:
			printf(INICIO_ALTERACAO);
			if(alterar(tabela))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3:
			printf(INICIO_BUSCA);
			buscar(tabela);
			break;
		case 4:
			printf(INICIO_EXCLUSAO);
			printf("%s", (remover(&tabela)) ? SUCESSO : FALHA );
			break;
		case 5:
			printf(INICIO_LISTAGEM);
			imprimir_tabela(tabela);
			break;
		case 6:
			liberar_tabela(&tabela);
			break;

		case 10:
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;

		default:
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}


/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */

Produto recuperar_registro(int rrn){
	Produto j;
	char temp[193], *p;
	strncpy(temp, ARQUIVO + ((rrn)*192), 192);
	temp[192] = '\0';
	p = strtok(temp,"@");
	strcpy(j.pk,p);
	p = strtok(NULL,"@");
	strcpy(j.nome,p);
	p = strtok(NULL,"@");
	strcpy(j.marca,p);
	p = strtok(NULL,"@");
	strcpy(j.data,p);
	p = strtok(NULL,"@");
	strcpy(j.ano,p);
	p = strtok(NULL,"@");
	strcpy(j.preco,p);
	p = strtok(NULL,"@");
	strcpy(j.desconto,p);
	p = strtok(NULL,"@");
	strcpy(j.categoria,p);
	return j;
}

int prox_primo(int a){
	int i, j, nprimos, primo_atual;

	int diferenca = 10;
	int max_divisor = sqrt(a+diferenca);

	int* primos = NULL;
	int* p = NULL;
	p = (int*)realloc(primos, max_divisor*sizeof(int));
	primos = p;

	nprimos = 0;
	primo_atual = 2;

	while(1){
		for(i = primo_atual; i <= max_divisor; i++)
			primos[i] = 0;

		for(i = primo_atual; i <= max_divisor; i++){
			if(primos[i] != -1){
				primos[nprimos] = i;
				for(j = i*2; j <= max_divisor; j+=i)
					primos[j] = -1;
				nprimos++;
				primo_atual = i;
			}
		}

		for(i = 0; i < nprimos && a < (a+diferenca); i++){
			if(a % primos[i] == 0){
				a++;
				i = -1;
			}
		}
		if(i == nprimos)
			return a;

		a+=diferenca;
		diferenca*=2;
		max_divisor = sqrt(a+diferenca);
		p = (int*)realloc(primos, max_divisor*sizeof(int));
		primos = p;
		primo_atual++;
	}
}

void criar_tabela(Hashtable* tabela, int tam){
	int i;
	tabela->tam = tam;
	tabela->v = calloc(tam, sizeof(Chave*));
	for(i = 0; i < tam; i++)
		tabela->v[i] = NULL;
}

void carregar_tabela(Hashtable* tabela){
	nregistros = strlen(ARQUIVO)/TAM_REGISTRO;

	Chave chave;
	for(int i = 0; i < nregistros; i++){
		Produto p = recuperar_registro(i);
		int pos = hash(p.pk, tabela->tam);
		strcpy(chave.pk, p.pk);
		chave.rrn = i;

		inserir_tabela(tabela, pos, chave);
	}
}

short hash(const char* chave, int tam){
	return (f(chave[0])+2*f(chave[1])+3*f(chave[2])+4*f(chave[3])+5*f(chave[4])+6*f(chave[5])+7*f(chave[6])+8*f(chave[7])) % tam;
}

void cadastrar(Hashtable* tabela){
	int pos;
	Chave chave;
	Produto p;

	scanf("%[^\n]s", p.nome);
	scanf("\n%[^\n]s", p.marca);
	scanf("\n%[^\n]s", p.data);
	scanf("\n%[^\n]s", p.ano);
	scanf("\n%[^\n]s", p.preco);
	scanf("\n%[^\n]s", p.desconto);
	scanf("\n%[^\n]s", p.categoria);
	gerarChave(&p);

	pos = hash(p.pk, tabela->tam);
	strcpy(chave.pk, p.pk);
	chave.rrn = nregistros;

	if(inserir_tabela(tabela, pos, chave)){
		printf(REGISTRO_INSERIDO, p.pk);
		inserir_Registro_arquivoDados(p);
	} else{
		printf(ERRO_PK_REPETIDA, p.pk);
	}
}

int inserir_tabela(Hashtable* tabela, int pos, Chave chave){
	Chave* novo = calloc(1, sizeof(Chave));
	strcpy(novo->pk, chave.pk);
	novo->rrn = chave.rrn;

	/* lista vazia */
	if (tabela->v[pos] == NULL) {
		tabela->v[pos] = novo;
		novo->prox = NULL;
		return 1;
	} else {
		Chave* aux = tabela->v[pos];
		/* insere no comeco */
		if(strcmp(chave.pk, aux->pk) < 0){
			novo->prox = aux;
			tabela->v[pos] = novo;
			return 1;
		} 
		else if(strcmp(chave.pk, aux->pk) == 0){
			return 0;
		}
		else{
			/* insere no meio */
			while(aux->prox != NULL) {
				if(strcmp(chave.pk, aux->prox->pk) < 0) {
					novo->prox = aux->prox;
					aux->prox = novo;
					return 1;
				}
				else if(strcmp(chave.pk, aux->prox->pk) == 0){
					return 0;
				}
				aux = aux->prox;
			}
			/* insere no fim */
			if(aux->prox == NULL){
				aux->prox = novo;
				novo->prox = NULL;
				return 1;
			}
		}
	}
}

/* insere as informacoes de um produto no arquivo de dados no formato de registro */
void inserir_Registro_arquivoDados(Produto p){
	int pos = nregistros*192;

	strncpy(ARQUIVO + pos, p.pk, strlen(p.pk));
	pos += strlen(p.pk);
	strcpy(ARQUIVO + pos, "@");
	pos++;

	strncpy(ARQUIVO + pos, p.nome, strlen(p.nome));
	pos += strlen(p.nome);
	strcpy(ARQUIVO + pos, "@");
	pos++;

	strncpy(ARQUIVO + pos, p.marca, strlen(p.marca));
	pos += strlen(p.marca);
	strcpy(ARQUIVO + pos, "@");
	pos++;

	strncpy(ARQUIVO + pos, p.data, strlen(p.data));
	pos += strlen(p.data);
	strcpy(ARQUIVO + pos, "@");
	pos++;

	strncpy(ARQUIVO + pos, p.ano, strlen(p.ano));
	pos += strlen(p.ano);
	strcpy(ARQUIVO + pos, "@");
	pos++;

	strncpy(ARQUIVO + pos, p.preco, strlen(p.preco));
	pos += strlen(p.preco);
	strcpy(ARQUIVO + pos, "@");
	pos++;

	strncpy(ARQUIVO + pos, p.desconto, strlen(p.desconto));
	pos += strlen(p.desconto);
	strcpy(ARQUIVO + pos, "@");
	pos++;

	strncpy(ARQUIVO + pos, p.categoria, strlen(p.categoria));
	pos += strlen(p.categoria);
	strcpy(ARQUIVO + pos, "@");
	pos++;

	while(pos < (nregistros+1)*192) {
		ARQUIVO[pos] = '#';
		pos++;
	}
	ARQUIVO[pos] = '\0';

	nregistros++;
}

int alterar(Hashtable tabela){
	char pk[TAM_PRIMARY_KEY], desconto[20], c, temp[193], *p;
	int descontoInt, cont, i, pos;
	Chave* aux;
	
	scanf("%[^\n]s", pk);
	getchar();
	pos = hash(pk, tabela.tam);

	aux = tabela.v[pos];
	if(tabela.v[pos] == NULL){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
	/* procura chave */
	while(aux != NULL && strcmp(aux->pk, pk) != 0)
		aux = aux->prox;

	if(aux == NULL){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	while(1) {
		cont = 0;
		while(cont < 4 && (c = getchar()) != '\n') {
			desconto[cont] = c;
			cont++;
		}
		if(cont == 4) {
			printf(CAMPO_INVALIDO);
			while((c = getchar()) != '\n') {}
		}
		else if(cont < 3){
			printf(CAMPO_INVALIDO);
		}
		else {
			desconto[3] = '\0';
			sscanf(desconto,"%d", &descontoInt);
			if(descontoInt < 0 || descontoInt > 100) {
				printf(CAMPO_INVALIDO);
			}
			else break;
		}
	}

	/* a partir do rrn da chave encontrada, busca o registro no arquivo */
	strncpy(temp, ARQUIVO + (aux->rrn)*192, 192);
	temp[192] = '\0';

	/* percorre o registro ate o campo de desconto*/
	cont = 0;
	p = strtok(temp,"@");
	cont += strlen(p) + 1;

	for(i = 0; i < 4; i++) {
		p = strtok(NULL,"@");
		cont += strlen(p) + 1;
	}
	p = strtok(NULL,"@");
	cont += strlen(p) + 1;

	/* altera o campo de desconto no arquivo */
	strncpy(ARQUIVO + (aux->rrn)*192 + cont, desconto, 3);
	return 1;
}

void buscar(Hashtable tabela){
	int pos;
	Chave* aux;

	char pk[TAM_PRIMARY_KEY];
	scanf("\n%[^\n]s", pk);
	pos = hash(pk, tabela.tam);

	aux = tabela.v[pos];
	if(tabela.v[pos] == NULL){
		printf(REGISTRO_N_ENCONTRADO);
		return;
	}
	/* procura chave */
	while(aux != NULL && strcmp(aux->pk, pk) != 0)
		aux = aux->prox;

	if(aux == NULL){
		printf(REGISTRO_N_ENCONTRADO);
		return;
	}

	exibir_registro(aux->rrn);
}

int remover(Hashtable* tabela){
	int pos;
	char pk[TAM_PRIMARY_KEY];
	Chave* aux;

	scanf("%[^\n]s", pk);
	pos = hash(pk, tabela->tam);
	
	if(tabela->v[pos] == NULL){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
	aux = tabela->v[pos];
	/* remove do comeco */
	if(strcmp(aux->pk, pk) == 0) {
		if(aux->prox != NULL)
			tabela->v[pos] = aux->prox;
		else tabela->v[pos] = NULL;
		free(aux);
		return 1;
	}
	/* procura chave */
	while(aux->prox != NULL && strcmp(aux->prox->pk, pk) != 0)
		aux = aux->prox;
	/* retorna 0 se nao achou */
	if(aux->prox == NULL){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	/* remove do fim */
	if(aux->prox->prox == NULL) {
		free(aux->prox);
		aux->prox = NULL;
		return 1;
	}
	/* remove do meio */
	Chave* p = aux->prox;
	aux->prox = aux->prox->prox;
	free(p);
	return 1;
}

void liberar_tabela(Hashtable* tabela){
	free(tabela->v);
}

void imprimir_tabela(Hashtable tabela){
	int i;
	Chave* aux;

	for(i = 0; i < tabela.tam; i++){
		printf("[%d]", i);
		aux = tabela.v[i];
		while(aux != NULL){
			printf(" %s", aux->pk);
			aux = aux->prox;
		}
		printf("\n");
	}
}

/* gera chave de um produto */
void gerarChave(Produto* p){
	strncpy(p->pk, p->nome, 2);
	strncpy(p->pk+2, p->marca, 2);
	strncpy(p->pk+4, p->data, 2);
	strncpy(p->pk+6, p->data+3, 2);
	strncpy(p->pk+8, p->ano, 2);
	p->pk[10] = '\0';
}


/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
}

/*Auxiliar para a função de hash*/
short f(char x)
{
	return (x < 59) ? x - 48 : x - 54; 
}

/* Exibe o jogo */
int exibir_registro(int rrn)
{
	if(rrn<0)
		return 0;
	float preco;
	int desconto;
	Produto j = recuperar_registro(rrn);
  char *cat, categorias[TAM_CATEGORIA];
	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.marca);
	printf("%s\n", j.data);
	printf("%s\n", j.ano);
	sscanf(j.desconto,"%d",&desconto);
	sscanf(j.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);
	strcpy(categorias, j.categoria);
  for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    printf("%s ", cat);
	printf("\n");
	return 1;
}