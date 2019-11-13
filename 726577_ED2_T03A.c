/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com reespalhamento linear
 *
 * RA: 726577
 * Aluno: Pablo Ferreira Laranjo
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


#define POS_OCUPADA 	"[%d] Ocupado: %s\n"
#define POS_LIVRE 		"[%d] Livre\n"
#define POS_REMOVIDA 	"[%d] Removido\n"

/* Estado das posições da tabela hash */
#define LIVRE       0
#define OCUPADO     1
#define REMOVIDO    2

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
#define INICIO_EXCLUSAO 			 			"**********************EXCLUIR*********************\n"

#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define ERRO_TABELA_CHEIA 		"ERRO: Tabela Hash esta cheia!\n\n"
#define REGISTRO_INSERIDO 		"Registro %s inserido com sucesso. Numero de colisoes: %d.\n\n"


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
 * Contém o estado da posição, a chave primária e o RRN do respectivo registro */
typedef struct {
	int estado;
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct {
    int tam;
    Chave *v;
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


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main() 
{    
	/* Arquivo */
	int carregarArquivo = 0, opcao = 0, tam;
	Hashtable tabela;
	scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo) 
		carregar_arquivo();

	/* Tabela Hash */
	scanf("%d", &tam);
	tam = prox_primo(tam);

	criar_tabela(&tabela, tam);
	if (carregarArquivo) 
	 carregar_tabela(&tabela);

	/* Execução do programa */
	while(opcao != 6) {
			scanf("%d%*c", &opcao);
			switch(opcao) 
			{
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
					if(	remover(&tabela))
						printf(SUCESSO);
					else
						printf(FALHA);
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

/* recupera registro do arquivo de dados */
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
	tabela->v = (Chave*)malloc(tam*sizeof(Chave));
	tabela->tam = tam;
}

void carregar_tabela(Hashtable* tabela){
	int i;
	nregistros = strlen(ARQUIVO)/TAM_REGISTRO;
	for(i = 0; i < nregistros; i++){
		Produto j = recuperar_registro(i);
		int pos = hash(j.pk, tabela->tam);
		while(tabela->v[pos].estado == OCUPADO){
			if(pos < tabela->tam-1)
				pos++;
			else pos = 0;
		}
		tabela->v[pos].rrn = i;
		strcpy(tabela->v[pos].pk, j.pk);
		tabela->v[pos].estado = OCUPADO;
	}
}

short hash(const char* chave, int tam){
	return (f(chave[0])+2*f(chave[1])+3*f(chave[2])+4*f(chave[3])+5*f(chave[4])+6*f(chave[5])+7*f(chave[6])+8*f(chave[7])) % tam;
}

void cadastrar(Hashtable* tabela){
	Produto p;
	int pos, ncolisoes, i, achouPosLivre;

	scanf("\n%[^\n]s", p.nome);
	scanf("\n%[^\n]s", p.marca);
	scanf("\n%[^\n]s", p.data);
	scanf("\n%[^\n]s", p.ano);
	scanf("\n%[^\n]s", p.preco);
	scanf("\n%[^\n]s", p.desconto);
	scanf("\n%[^\n]s", p.categoria);
	gerarChave(&p);

	pos = hash(p.pk, tabela->tam);
	ncolisoes = 0;
	achouPosLivre = 0;

	i = 0;
	while(i < tabela->tam){
		if(tabela->v[pos].estado != OCUPADO)
			achouPosLivre = 1;
		if(!achouPosLivre && tabela->v[pos].estado == OCUPADO && ncolisoes < tabela->tam){
			ncolisoes++;
			if(pos < tabela->tam-1)
				pos++;
			else pos = 0;
		}
		if(strcmp(tabela->v[i].pk, p.pk) == 0 && tabela->v[i].estado == OCUPADO){
			printf(ERRO_PK_REPETIDA, p.pk);
			return;
		}
		i++;
	}
	if(ncolisoes == tabela->tam){
		printf(ERRO_TABELA_CHEIA);
		return;
	}
	else{
		tabela->v[pos].rrn = nregistros;
		inserir_Registro_arquivoDados(p);
		strcpy(tabela->v[pos].pk, p.pk);
		tabela->v[pos].estado = OCUPADO;
		printf(REGISTRO_INSERIDO, p.pk, ncolisoes);
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
	char pk[TAM_PRIMARY_KEY], desconto[TAM_DESCONTO], c, temp[193], *p;
	int descontoInt, cont, i;
	int pos, nvisitados;
	
	scanf("%[^\n]s", pk);
	pos = hash(pk, tabela.tam);
	nvisitados = 0;
	while(nvisitados < tabela.tam){
		if(tabela.v[pos].estado == OCUPADO && strcmp(tabela.v[pos].pk, pk) == 0)
			break;
		if(pos < tabela.tam-1)
			pos++;
		else pos = 0;
		nvisitados++;
	}

	if(nvisitados == tabela.tam){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
	getchar();

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
	strncpy(temp, ARQUIVO + (tabela.v[pos].rrn)*192, 192);
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
	strncpy(ARQUIVO + (tabela.v[pos].rrn)*192 + cont, desconto, 3);
	return 1;
}

void buscar(Hashtable tabela){
	int pos, nvisitados;
	char pk[TAM_PRIMARY_KEY];
	scanf("\n%[^\n]s", pk);
	pos = hash(pk, tabela.tam);
	nvisitados = 0;
	while(nvisitados < tabela.tam){
		if(tabela.v[pos].estado == OCUPADO && strcmp(tabela.v[pos].pk, pk) == 0)
			break;
		if(pos < tabela.tam-1)
			pos++;
		else pos = 0;
		nvisitados++;
	}

	if(nvisitados == tabela.tam){
		printf(REGISTRO_N_ENCONTRADO);
		return;
	}

	exibir_registro(tabela.v[pos].rrn);
}

int remover(Hashtable* tabela){
	int pos, nvisitados;
	char pk[TAM_PRIMARY_KEY];
	scanf("\n%[^\n]s", pk);
	pos = hash(pk, tabela->tam);
	nvisitados = 0;
	while(nvisitados < tabela->tam){
		if(tabela->v[pos].estado == OCUPADO && strcmp(tabela->v[pos].pk, pk) == 0)
			break;
		if(pos < tabela->tam-1)
			pos++;
		else pos = 0;
		nvisitados++;
	}

	if(nvisitados == tabela->tam){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	tabela->v[pos].estado = REMOVIDO;
	/* marca o comeco do registro excluido */
	strncpy(ARQUIVO + (tabela->v[pos].rrn)*192, "*|", 2);
	return 1;
}

void liberar_tabela(Hashtable* tabela){
	free(tabela->v);
}

void imprimir_tabela(Hashtable tabela){
	int i;
	for(i = 0; i < tabela.tam; i++){
		if(tabela.v[i].estado == OCUPADO)
			printf(POS_OCUPADA, i, tabela.v[i].pk);
		else if(tabela.v[i].estado == LIVRE)
			printf(POS_LIVRE, i);
		else printf(POS_REMOVIDA, i);
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

/* Exibe o produto */
int exibir_registro(int rrn)
{
	float preco;
	int desconto;
	Produto j = recuperar_registro(rrn);
    char *cat, categorias[TAM_CATEGORIA];
	if(rrn<0)
		return 0;
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