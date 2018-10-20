/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01
 *
 * RA: 726577
 * Aluno: Pablo Ferreira Laranjo
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY	11
#define TAM_NOME 		51
#define TAM_MARCA 		51
#define TAM_DATA 		11
#define TAM_ANO 		3
#define TAM_PRECO 		8
#define TAM_DESCONTO 	4
#define TAM_CATEGORIA 	51


#define TAM_REGISTRO 	192
#define MAX_REGISTROS 	1000
#define MAX_CATEGORIAS 	30
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas para o usuario */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!"
#define INICIO_BUSCA 		 		"**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM  			"**********************LISTAR**********************\n"
#define INICIO_ALTERACAO 			"**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO  			"**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO  			"**********************ARQUIVO*********************\n"
#define INICIO_ARQUIVO_SECUNDARIO	"*****************ARQUIVO SECUNDARIO****************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"



/* Registro do Produto */
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


/*----- Registros dos Índices -----*/

/* Struct para índice Primário */
typedef struct primary_index{
  char pk[TAM_PRIMARY_KEY];
  int rrn;
} Ip;

/* Struct para índice secundário */
typedef struct secundary_index{
  char pk[TAM_PRIMARY_KEY];
  char string[TAM_NOME];
} Is;

/* Struct para índice secundário de preços */
typedef struct secundary_index_of_final_price{
  float price;
  char pk[TAM_PRIMARY_KEY];
} Isf;

/* Lista ligada para o Índice abaixo*/
typedef struct linked_list{
  char pk[TAM_PRIMARY_KEY];
  struct linked_list *prox;
} ll;

/* Struct para lista invertida */
typedef struct reverse_index{
  char cat[TAM_CATEGORIA];
  ll* lista;
} Ir;

/*----- GLOBAL -----*/
char ARQUIVO[TAM_ARQUIVO];

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto);

/* Recupera do arquivo o registro com o rrn informado
 *  e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn);

/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int* nregistros);

/* Realiza os scanfs na struct Produto */
void ler_entrada(char* registro, Produto *novo);

/* Rotina para impressao de indice secundario */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat);

/* cadastra um novo produto no arquivo de dados e nos indices */
int cadastrar(Produto p, char* registro, Ip *indice_primario, Is* iproduct, Is* ibrand, Isf *iprice, Ir* icategory, int* ncat, int* nregistros);

/* altera o campo desconto de um registro */
int alterar(Ip* indice_primario, Isf* iprice, int nregistros);

/* marca um registro como excluido no arquivo de dados */
int remover(Ip* indice_primario, int nregistros);

/* busca por um registro */
void busca(Ip* indice_primario, Is* iproduct, Is* ibrand, Ir* icategory, int nregistros, int ncat);

/* Lista produtos */
void listagem(Ip* indice_primario, Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat);

/* libera espaco referente a produtos excluidos */
void liberaEspaco(Ip* indice_primario, Is* iproduct, Is* ibrand, Isf* iprice, Ir* icategory, int* ncat, int* nregistros);

/* desaloca listas e vetores, antes do termino do programa */
void liberaMemoria(Ip* indice_primario, Is* iproduct, Is* ibrand, Isf* iprice, Ir* icategory, int ncat, int nregistros);

/* busca binaria para retornar mais de uma chave referente ao mesmo nome de produto */
int bsearch_nomesProd_repetidos(Is* iproduct, char* nomeProduto, int low, int high, char repetidos_chaves[TAM_PRIMARY_KEY][MAX_REGISTROS], int* n);

/* busca binaria para retornar mais de uma chave referente a mesma marca */
int bsearch_marcasRepetidas(Is* ibrand, char* nomeMarca, int low, int high, char repetidos_chaves[TAM_PRIMARY_KEY][MAX_REGISTROS], int* n);

/* cria os indices secundarios, para o nome, marca, preco e categorias */
void criar_isecondary(Is* iproduct, Is* ibrand, Isf* iprice, Ir* icategory, int* ncat, int* nregistros);

/* cria uma posicao no vetor de indices primarios, baseado na sua posicao no Arquivo */
void criar_iprimary_unico(Ip *indice_primario, int rrn);

/* cria uma posicao no vetor de cada indice secundario, baseado nas suas posicoes no Arquivo */
void criar_isecondary_unico(Is* iproduct, Is* ibrand, Isf* iprice, Ir* icategory, int* ncat, int rrn);

/* gera a chave de um produto */
void gerarChave(Produto* p);

/* insere chave na lista de uma categoria */
void inserir_lista(ll** lista, char* pk);

/* remove chave da lista de uma categoria */
int remover_lista(ll** lista, char* pk);

/* verifica existencia de chave na lista de uma categoria */
int busca_lista(ll** lista, char* pk);

void apaga_lista(ll** lista);

/* funcoes para ordenacao de array de strings */
void ordena_arrayOfStrings(char str[TAM_PRIMARY_KEY][MAX_REGISTROS], int n);
void swap(char* str, char* str2);

/* funcoes de comparacao, usadas em bsearch e qsort */
int compara_Ip_pk (const void* a, const void* b) {
	Ip* aux1 = (Ip*)a;
	Ip* aux2 = (Ip*)b;
	return strcmp(aux1->pk, aux2->pk);
};

int compara_Is_pk (const void* a, const void* b) {
	Is* aux1 = (Is*)a;
	Is* aux2 = (Is*)b;
	return strcmp(aux1->pk, aux2->pk);
};

int compara_Is_string (const void* a, const void* b) {
	Is* aux1 = (Is*)a;
	Is* aux2 = (Is*)b;
	if(strcmp(aux1->string, aux2->string) == 0)
		return strcmp(aux1->pk, aux2->pk);
	return strcmp(aux1->string, aux2->string);
};

int compara_Isf_pk (const void* a, const void* b) {
	Isf* aux1 = (Isf*)a;
	Isf* aux2 = (Isf*)b;
	return strcmp(aux1->pk, aux2->pk);
};

int compara_Isf_price (const void* a, const void* b) {
	Isf* aux1 = (Isf*)a;
	Isf* aux2 = (Isf*)b;
	if(aux1->price > aux2->price) return 1;
	if(aux1->price == aux2->price) return strcmp(aux1->pk, aux2->pk);
	if(aux1->price < aux2->price) return -1;
};

int compara_Ir_cat (const void* a, const void* b) {
	Ir* aux1 = (Ir*)a;
	Ir* aux2 = (Ir*)b;
	return strcmp(aux1->cat, aux2->cat);
};


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main(){
  /* Arquivo */
	int carregarArquivo = 0, nregistros = 0, ncat = 0;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	/* Índice primário */
	Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
  	if (!iprimary) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_iprimary(iprimary, &nregistros);

	/*Alocar e criar índices secundários*/
	Is* iproduct = (Is*)malloc(MAX_REGISTROS * sizeof(Is));
	Is* ibrand = (Is*)malloc(MAX_REGISTROS * sizeof(Is));
	Isf* iprice = (Isf*)malloc(MAX_REGISTROS * sizeof(Isf));
	Ir* icategory = (Ir*)malloc(MAX_CATEGORIAS * sizeof(Ir));
	if (!iproduct || !ibrand || !iprice || !icategory) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	for (int i = 0; i < MAX_CATEGORIAS; i++)
		icategory[i].lista = NULL;
	criar_isecondary(iproduct, ibrand, iprice, icategory, &ncat, &nregistros);

	/* variaveis */
	char registro[TAM_REGISTRO];
	Produto novo;

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao)
		{
			case 1:
				/*cadastro*/
				ler_entrada(registro, &novo);
				if (!cadastrar(novo, registro, iprimary, iproduct, ibrand, iprice, icategory, &ncat, &nregistros))
					printf(ERRO_PK_REPETIDA, novo.pk);

			break;
			case 2:
				/*alterar desconto*/
				printf(INICIO_ALTERACAO);
				if(alterar(iprimary, iprice, nregistros))
					printf(SUCESSO);
				else
					printf(FALHA);
				
			break;
			case 3:
				/*excluir produto*/
				printf(INICIO_EXCLUSAO);
				if(remover(iprimary, nregistros))
					printf(SUCESSO);
				else
					printf(FALHA);

			break;
			case 4:
				/*busca*/
				printf(INICIO_BUSCA );
				busca(iprimary, iproduct, ibrand, icategory, nregistros, ncat);
			break;
			case 5:
				/*listagens*/
				printf(INICIO_LISTAGEM);
				listagem(iprimary, iproduct, ibrand, icategory, iprice, nregistros, ncat);
			break;
			case 6:
				/*libera espaço*/
				liberaEspaco(iprimary, iproduct, ibrand, iprice, icategory, &ncat, &nregistros);
			break;
			case 7:
				/*imprime o arquivo de dados*/
				printf(INICIO_ARQUIVO);
				if(strlen(ARQUIVO) == 0)
					printf(ARQUIVO_VAZIO"\n");
				else
					printf("%s\n", ARQUIVO);
			break;
			case 8:
				/*imprime os índices secundários*/
				imprimirSecundario(iproduct, ibrand, icategory, iprice, nregistros, ncat);
				if(nregistros == 0)
					printf("\n");
			break;
			case 9:
	      		/*Liberar memória e finalizar o programa */
				liberaMemoria(iprimary, iproduct, ibrand, iprice, icategory, ncat, nregistros);
				return 0;
			break;
			default:
				printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}


/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto)
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
	if(!com_desconto)
	{
		printf("%s\n", j.preco);
		printf("%s\n", j.desconto);
	}
	else
	{
		sscanf(j.desconto,"%d",&desconto);
		sscanf(j.preco,"%f",&preco);
		preco = preco *  (100-desconto);
		preco = ((int) preco)/ (float) 100 ;
		printf("%07.2f\n",  preco);

	}
	strcpy(categorias, j.categoria);

	cat = strtok (categorias, "|");

	while(cat != NULL){
		printf("%s", cat);
		cat = strtok (NULL, "|");
		if(cat != NULL){
			printf(", ");
		}
	}

	printf("\n");

	return 1;
}


int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}


/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn)
{
	char temp[193], *p;
	strncpy(temp, ARQUIVO + ((rrn)*192), 192);
	temp[192] = '\0';
	Produto j;
	p = strtok(temp,"@");
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
	gerarChave(&j);
	return j;
}


/* Imprimir indices secundarios */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat){
	int opPrint = 0;
	ll *aux;
	printf(INICIO_ARQUIVO_SECUNDARIO);
	scanf("%d", &opPrint);
	if(!nregistros)
		printf(ARQUIVO_VAZIO);
	switch (opPrint) {
		case 1:		
			for(int i = 0; i < nregistros; i++){
				printf("%s %s\n",iproduct[i].pk, iproduct[i].string);
			}
		break;
		case 2:
			for(int i = 0; i < nregistros; i++){
				printf("%s %s\n",ibrand[i].pk, ibrand[i].string);
			}
		break;
		case 3:
			for(int i = 0; i < ncat; i++){
				printf("%s", icategory[i].cat);
				aux =  icategory[i].lista;
				while(aux != NULL){
					printf(" %s", aux->pk);
					aux = aux->prox;
				}
				printf("\n");
			}
		break;

		case 4:
		for(int i = 0; i < nregistros; i++){
			printf("%s %.2f\n",iprice[i].pk, iprice[i].price);
		}
		break;
	}
}


/* ================================================================================================================
												MINHA IMPLEMENTACAO
 ================================================================================================================== */


/* cadastra um novo produto no arquivo de dados e nos indices */
int cadastrar(Produto p, char* registro, Ip *indice_primario, Is* iproduct, Is* ibrand, Isf *iprice, Ir* icategory, int* ncat, int* nregistros) {
	Ip* busca = NULL;	
	/* verifica se a chave ja existe */
	if(*nregistros > 0) {
		Ip aux; strcpy(aux.pk, p.pk);
		busca = (Ip*)bsearch(&aux, indice_primario, *nregistros, sizeof(Ip), compara_Ip_pk);

		/* se achou e nao esta marcado como excluido */
		if(busca && busca->rrn != -1)
			return 0;
	}

	int n = strlen(ARQUIVO)/TAM_REGISTRO;
	/* se o produto ainda nao existe, o cadastra no arquivo */
	strcpy(ARQUIVO + n*192, registro);

	/* insere nos indices
	se o produto nao existe, o cadastra pela primeira vez */
	if(!busca || *nregistros == 0) {
		criar_iprimary_unico(indice_primario, *nregistros);
		criar_isecondary_unico(iproduct, ibrand, iprice, icategory, ncat, *nregistros);
		(*nregistros)++;

		/* ordena o indice primario e os indices secundarios para o nome, marca e preco */
		qsort(indice_primario, *nregistros, sizeof(Ip), compara_Ip_pk);
		qsort(iproduct, *nregistros, sizeof(Is), compara_Is_string);
		qsort(ibrand, *nregistros, sizeof(Is), compara_Is_string);
		qsort(iprice, *nregistros, sizeof(Isf), compara_Isf_price);
	}
	/* se o produto ja existe, significa que foi excluido, entao sobrepoe aquele indice */
	else
		busca->rrn = n;

	return 1;
}

/* altera o campo desconto de um registro */
int alterar(Ip* indice_primario, Isf* iprice, int nregistros) {
	char chave[TAM_PRIMARY_KEY];
	scanf("%s", chave);

	if(nregistros == 0) {
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	/* verifica se a chave existe */
	Ip ip_aux; strcpy(ip_aux.pk, chave);
	Ip* busca = (Ip*)bsearch(&ip_aux, indice_primario, nregistros, sizeof(Ip), compara_Ip_pk);
	if(!busca || busca->rrn == -1) {
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	getchar();

	/* se existir, recebe o novo desconto, verificando se a entrada esta no padrao correto */
	char desconto[TAM_DESCONTO], c;
	int descontoInt, cont;

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
	char temp[193], *p, *precoOriginal;
	strncpy(temp, ARQUIVO + (busca->rrn)*192, 192);
	temp[192] = '\0';

	/* percorre o registro ate o campo de desconto*/
	cont = 0;
	p = strtok(temp,"@");
	cont += strlen(p) + 1;

	for(int i = 0; i < 3; i++) {
		p = strtok(NULL,"@");
		cont += strlen(p) + 1;
	}
	p = strtok(NULL,"@");
	cont += strlen(p) + 1;
	precoOriginal = p;

	/* altera o campo de desconto no arquivo */
	strncpy(ARQUIVO + (busca->rrn)*192 + cont, desconto, 3);

	/* busca no indice de precos, produto com o preco a ser alterado, baseado no novo desconto */
	qsort(iprice, nregistros, sizeof(Isf), compara_Isf_pk);
	Isf isf_aux; strcpy(isf_aux.pk, chave);
	Isf* buscaP = (Isf*)bsearch(&isf_aux, iprice, nregistros, sizeof(Isf), compara_Isf_pk);

	/* recebe preco original com novo desconto aplicado */
	float novoPreco;
	sscanf(precoOriginal, "%f", &novoPreco);
	novoPreco = (novoPreco * (100-descontoInt))/100.0;
	novoPreco = novoPreco * 100;
	novoPreco = ((int)novoPreco/(float)100);

	buscaP->price = novoPreco;
	qsort(iprice, nregistros, sizeof(Isf), compara_Isf_price);

	return 1;
}

int remover(Ip* indice_primario, int nregistros) {
	char chave[TAM_PRIMARY_KEY];
	scanf("%s", chave);

	if(nregistros == 0) {
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	/* verifica se a chave existe */
	Ip ip_aux; strcpy(ip_aux.pk, chave);
	Ip* buscaIp = (Ip*)bsearch(&ip_aux, indice_primario, nregistros, sizeof(Ip), compara_Ip_pk);

	if(!buscaIp || buscaIp->rrn == -1) {
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	/* marca o comeco do registro excluido */
	strncpy(ARQUIVO + (buscaIp->rrn)*192, "*|", 2);
	buscaIp->rrn = -1;

	return 1;
}

void busca(Ip* indice_primario, Is* iproduct, Is* ibrand, Ir* icategory, int nregistros, int ncat) {
	int opcao = 0;
	scanf("%d", &opcao);

	char str[TAM_NOME];
	scanf("\n%[^\n]s", str);

	Ip ip_aux, *buscaIp;
	Ir* buscaCat;
	char repetidos_chaves[TAM_PRIMARY_KEY][MAX_REGISTROS], categoria[TAM_CATEGORIA];
	int n = 0;

	switch(opcao){
		case 1:
			strcpy(ip_aux.pk, str);
			buscaIp = (Ip*)bsearch(&ip_aux, indice_primario, nregistros, sizeof(Ip), compara_Ip_pk);
			if(!buscaIp)
				printf(REGISTRO_N_ENCONTRADO);
			else if(!exibir_registro(buscaIp->rrn, '\0')) 
				printf(REGISTRO_N_ENCONTRADO);
		break;

		case 2:
			if(bsearch_nomesProd_repetidos(iproduct, str, 0, nregistros-1, repetidos_chaves, &n) && n > 1) {
				int flag = 0; /* determina se ja foi feita alguma impressao */
				ordena_arrayOfStrings(repetidos_chaves, n);
				int i;
				for(i = 0; i < n; i++) {
					strcpy(ip_aux.pk, repetidos_chaves[i]);
					buscaIp = (Ip*)bsearch(&ip_aux, indice_primario, nregistros, sizeof(Ip), compara_Ip_pk);
					if(!flag && exibir_registro(buscaIp->rrn, '\0')) {
						flag = 1;
					} else if(buscaIp->rrn != -1){
						printf("\n");
						exibir_registro(buscaIp->rrn, '\0');
					}
				}
				if(!flag)
					printf(REGISTRO_N_ENCONTRADO);
			}
			else if(n == 1) {
				strcpy(ip_aux.pk, repetidos_chaves[0]);
				buscaIp = (Ip*)bsearch(&ip_aux, indice_primario, nregistros, sizeof(Ip), compara_Ip_pk);
				if(!exibir_registro(buscaIp->rrn, '\0'))
					printf(REGISTRO_N_ENCONTRADO);
			}
			else
				printf(REGISTRO_N_ENCONTRADO);
		break;

		case 3:
			scanf("\n%[^\n]s", categoria);
			buscaCat = (Ir*)bsearch(categoria, icategory, ncat, sizeof(Ir), compara_Ir_cat);			
			if(buscaCat && bsearch_marcasRepetidas(ibrand, str, 0, nregistros-1, repetidos_chaves, &n) && n > 1) {
				int flag = 0; /* determina se ja foi feita alguma impressao */
				ordena_arrayOfStrings(repetidos_chaves, n);
				for(int i = 0; i < n; i++) {
					if(busca_lista(&buscaCat->lista, repetidos_chaves[i])) {
						strcpy(ip_aux.pk, repetidos_chaves[i]);
						buscaIp = (Ip*)bsearch(&ip_aux, indice_primario, nregistros, sizeof(Ip), compara_Ip_pk);
						if(!flag && exibir_registro(buscaIp->rrn, '\0')) {
							flag = 1;
						}else if(buscaIp->rrn != -1){
							printf("\n");
							exibir_registro(buscaIp->rrn, '\0');
						}
					}
				}
				if(!flag)
					printf(REGISTRO_N_ENCONTRADO);
			}
			else if(n == 1) {
				strcpy(ip_aux.pk, repetidos_chaves[0]);
				buscaIp = (Ip*)bsearch(&ip_aux, indice_primario, nregistros, sizeof(Ip), compara_Ip_pk);
				if(!exibir_registro(buscaIp->rrn, '\0'))
					printf(REGISTRO_N_ENCONTRADO);
			}
			else
				printf(REGISTRO_N_ENCONTRADO);

		break;
	}
}

/* Lista produtos */
void listagem(Ip* indice_primario, Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat){
	int opcao = 0;
	scanf("%d", &opcao);

	int flag = 0;
	char categoria[TAM_CATEGORIA];
	Ip ip_aux, *buscaIp;
	Ir* buscaCat;
	ll *aux;

	switch (opcao) {
		case 1:		
			for(int i = 0; i < nregistros; i++){
				if(!flag && exibir_registro(indice_primario[i].rrn, '\0')) {
					flag = 1;
				}else if(indice_primario[i].rrn != -1){
					printf("\n");
					exibir_registro(indice_primario[i].rrn, '\0');
				}
			}
			if(!flag)
				printf(REGISTRO_N_ENCONTRADO);
		break;
		case 2:
			scanf("\n%[^\n]s", categoria);
			buscaCat = (Ir*)bsearch(categoria, icategory, ncat, sizeof(Ir), compara_Ir_cat);
			if(buscaCat){
				aux = buscaCat->lista;
				while(aux != NULL){
					strcpy(ip_aux.pk, aux->pk);
					buscaIp = (Ip*)bsearch(&ip_aux, indice_primario, nregistros, sizeof(Ip), compara_Ip_pk);
					if(!flag && exibir_registro(buscaIp->rrn, '\0')) {
						flag = 1;
					}else if(buscaIp->rrn != -1){
						printf("\n");
						exibir_registro(buscaIp->rrn, '\0');
					}
					aux = aux->prox;
				}
			}
			if(!flag)
				printf(REGISTRO_N_ENCONTRADO);
		break;
		case 3:
			for(int i = 0; i < nregistros; i++){
				strcpy(ip_aux.pk, ibrand[i].pk);
				buscaIp = (Ip*)bsearch(&ip_aux, indice_primario, nregistros, sizeof(Ip), compara_Ip_pk);
				if(!flag && exibir_registro(buscaIp->rrn, '\0')) {
					flag = 1;
				}else if(buscaIp->rrn != -1){
					printf("\n");
					exibir_registro(buscaIp->rrn, '\0');
				}
			}
			if(!flag)
				printf(REGISTRO_N_ENCONTRADO);
		break;

		case 4:
			for(int i = 0; i < nregistros; i++){
				strcpy(ip_aux.pk, iprice[i].pk);
				buscaIp = (Ip*)bsearch(&ip_aux, indice_primario, nregistros, sizeof(Ip), compara_Ip_pk);
				if(!flag && exibir_registro(buscaIp->rrn, '1')) {
					flag = 1;
				}else if(buscaIp->rrn != -1){
					printf("\n");
					exibir_registro(buscaIp->rrn, '1');
				}
			}
			if(!flag)
				printf(REGISTRO_N_ENCONTRADO);
		break;
	}
}

/* libera espaco referente a produtos excluidos */
void liberaEspaco(Ip* indice_primario, Is* iproduct, Is* ibrand, Isf* iprice, Ir* icategory, int* ncat, int* nregistros) {
	int i;
	char* p;

	/* remove produtos marcados como excluido do arquivo de dados */
	i = 0;
	while(i < strlen(ARQUIVO)) {
		p = ARQUIVO + i;
		if(p[0] == '*') {
			p += 192;
			strcpy(ARQUIVO + i, p);
		} else i+=192;
	}

	/* atualiza indice primario */
	*nregistros = strlen(ARQUIVO)/TAM_REGISTRO;
	criar_iprimary(indice_primario, nregistros);

	/* atualiza indices secundarios */
	for (int i = 0; i < *ncat; i++)
		apaga_lista(&icategory[i].lista);

	*ncat = 0;
	criar_isecondary(iproduct, ibrand, iprice, icategory, ncat, nregistros);
}

/* libera memoria alocada por indices e listas */
void liberaMemoria(Ip* indice_primario, Is* iproduct, Is* ibrand, Isf* iprice, Ir* icategory, int ncat, int nregistros) {
	for(int i = 0; i < ncat; i++)
		apaga_lista(&icategory[i].lista);

	free(icategory);
	free(indice_primario);
	free(iproduct);
	free(ibrand);
	free(iprice);
}

void ordena_arrayOfStrings(char str[TAM_PRIMARY_KEY][MAX_REGISTROS], int n){
	int menor, i, j;
	for(i = 0; i < n; i++) {
		menor = i;
		for(j = i; j < n; j++) {
			if(strcmp(str[j], str[menor]) < 0)
				menor = j;
		}
		swap(str[i], str[menor]);
	}
}

void swap(char* str, char* str2) {
	char aux[TAM_PRIMARY_KEY];
	strcpy(aux, str);
	strcpy(str, str2);
	strcpy(str2, aux);
}

/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int* nregistros) {
	if (*nregistros == 0)
		return;

	int i;
	for(i = 0; i < *nregistros; i++) {
		criar_iprimary_unico(indice_primario, i);
	}
	qsort(indice_primario, *nregistros, sizeof(Ip), compara_Ip_pk);
}

/* cria uma posicao no vetor de indices primarios, baseado na sua posicao no Arquivo */
void criar_iprimary_unico(Ip *indice_primario, int rrn) {
	Produto j;
	indice_primario[rrn].rrn = rrn;
	j = recuperar_registro(rrn);
	strcpy(indice_primario[rrn].pk, j.pk);
}

/* cria os indices secundarios, para o nome, marca, preco e categorias */
void criar_isecondary(Is* iproduct, Is* ibrand, Isf* iprice, Ir* icategory, int* ncat, int* nregistros) {
	if (*nregistros == 0)
		return;

	int i;
	for(i = 0; i < *nregistros; i++) {
		criar_isecondary_unico(iproduct, ibrand, iprice, icategory, ncat, i);
	}
	
	/* ordena os indices secundarios para o nome, marca e preco */
	qsort(iproduct, *nregistros, sizeof(Is), compara_Is_string);
	qsort(ibrand, *nregistros, sizeof(Is), compara_Is_string);
	qsort(iprice, *nregistros, sizeof(Isf), compara_Isf_price);
}

/* cria uma posicao no vetor de cada indice secundario, baseado nas suas posicoes no Arquivo */
void criar_isecondary_unico(Is* iproduct, Is* ibrand, Isf* iprice, Ir* icategory, int* ncat, int rrn) {

	Ir* busca;
	float preco;
	int desconto;
	char* cat, categorias[TAM_CATEGORIA];
	Produto j;

	j = recuperar_registro(rrn);
	strcpy(iproduct[rrn].pk, j.pk);
	strcpy(iproduct[rrn].string, j.nome);
	strcpy(ibrand[rrn].pk, j.pk);
	strcpy(ibrand[rrn].string, j.marca);

	strcpy(iprice[rrn].pk, j.pk);
	sscanf(j.preco,"%f", &preco);
	sscanf(j.desconto,"%d", &desconto);
	preco = (preco * (100-desconto))/100.0;
	preco = preco * 100;
	preco = ((int)preco/(float)100);
	iprice[rrn].price = preco;

	strcpy(categorias, j.categoria);

	/* procura cada categoria do produto em icategory */
	cat = strtok (categorias, "|");
	while(cat != NULL){
		busca = (Ir*)bsearch(cat, icategory, *ncat, sizeof(Ir), compara_Ir_cat);

		/* se existir, insere chave na lista dessa categoria */
		if(busca != NULL) {
			inserir_lista(&busca->lista, j.pk);
		}

		/* se nao, cria categoria em icategory, insere na sua lista e ordena icategory */
		else {
			strcpy(icategory[*ncat].cat, cat);
			inserir_lista(&icategory[*ncat].lista, j.pk);
			(*ncat)++;
			qsort(icategory, *ncat, sizeof(Ir), compara_Ir_cat);
		}

		cat = strtok (NULL, "|");
	}
}

/* busca binaria para retornar mais de uma chave referente ao mesmo nome de produto */
int bsearch_nomesProd_repetidos(Is* iproduct, char* nomeProduto, int low, int high, char repetidos_chaves[TAM_PRIMARY_KEY][MAX_REGISTROS], int* n) {
	if(low > high || high < low)
		return 0;

	int mid = (low+high)/2;
	if(strcmp(nomeProduto, iproduct[mid].string) == 0) {
		int i = mid, j = 0;
		while(strcmp(nomeProduto, iproduct[i].string) == 0 && i >= 0) {
			if(i > 0 && strcmp(iproduct[i].pk, iproduct[i-1].pk) != 0) {
				strcpy(repetidos_chaves[j], iproduct[i].pk);
				j++;
			} else if(i == 0){
				strcpy(repetidos_chaves[j], iproduct[i].pk);
				j++;
			}
			i--;
		}
		i = mid+1;
		while(i <= high && strcmp(nomeProduto, iproduct[i].string) == 0 && strcmp(iproduct[i].pk, iproduct[i-1].pk) != 0) {
			strcpy(repetidos_chaves[j], iproduct[i].pk);
			i++; j++;
		}
		*n = j;
		return 1;
	}
	if(strcmp(nomeProduto, iproduct[mid].string) < 0)
		return bsearch_nomesProd_repetidos(iproduct, nomeProduto, low, mid-1, repetidos_chaves, n);
	if(strcmp(nomeProduto, iproduct[mid].string) > 0)
		return bsearch_nomesProd_repetidos(iproduct, nomeProduto, mid+1, high, repetidos_chaves, n);
}

/* busca binaria para retornar mais de uma chave referente a mesma marca */
int bsearch_marcasRepetidas(Is* ibrand, char* nomeMarca, int low, int high, char repetidos_chaves[TAM_PRIMARY_KEY][MAX_REGISTROS], int* n) {
	if(low > high || high < low)
		return 0;

	int mid = (low+high)/2;
	if(strcmp(nomeMarca, ibrand[mid].string) == 0) {
		int i = mid, j = 0;
		while(strcmp(nomeMarca, ibrand[i].string) == 0 && i >= 0) {
			if(i > 0 && strcmp(ibrand[i].pk, ibrand[i-1].pk) != 0) {
				strcpy(repetidos_chaves[j], ibrand[i].pk);
				j++;
			} else if(i == 0){
				strcpy(repetidos_chaves[j], ibrand[i].pk);
				j++;
			}
			i--;
		}
		i = mid+1;
		while(i <= high && strcmp(nomeMarca, ibrand[i].string) == 0 && strcmp(ibrand[i].pk, ibrand[i-1].pk) != 0) {
			strcpy(repetidos_chaves[j], ibrand[i].pk);
			i++; j++;
		}
		*n = j;
		return 1;
	}
	if(strcmp(nomeMarca, ibrand[mid].string) < 0)
		return bsearch_marcasRepetidas(ibrand, nomeMarca, low, mid-1, repetidos_chaves, n);
	if(strcmp(nomeMarca, ibrand[mid].string) > 0)
		return bsearch_marcasRepetidas(ibrand, nomeMarca, mid+1, high, repetidos_chaves, n);
}

/* insere chave na lista de uma categoria */
void inserir_lista(ll** lista, char* pk) {
	ll* novo = (ll*)malloc(sizeof(ll));
	strcpy(novo->pk, pk);

	/* lista vazia */
	if (!(*lista)) {
		*lista = novo;
		novo->prox = NULL;
	} else {
		ll* aux = *lista;
		/* insere no comeco */
		if(strcmp(pk, aux->pk) < 0) {
			novo->prox = aux;
			*lista = novo;
			return;
		}
		/* insere no meio */
		while(aux->prox != NULL) {
			if(strcmp(pk, aux->prox->pk) < 0) {
				novo->prox = aux->prox;
				aux->prox = novo;
				return;
			}
			aux = aux->prox;
		}
		/* insere no fim */
		if(!aux->prox) {
			aux->prox = novo;
			novo->prox = NULL;
		}
	}
}

/* remove chave da lista de uma categoria */
int remover_lista(ll** lista, char* pk) {
	ll* aux = *lista;
	/* remove do comeco */
	if(strcmp(aux->pk, pk) == 0) {
		if(aux->prox)
			*lista = aux->prox;
		else *lista = NULL;
		free(aux);
		return 1;
	}
	/* procura chave */
	while(aux->prox && strcmp(aux->prox->pk, pk) != 0)
		aux = aux->prox;
	/* retorna 0 se nao achou */
	if(!aux->prox)
		return 0;

	/* remove do fim */
	if(aux->prox->prox == NULL) {
		free(aux->prox);
		aux->prox = NULL;
		return 1;
	}
	/* remove do meio */
	ll* p = aux->prox;
	aux->prox = aux->prox->prox;
	free(p);
	return 1;
}

/* verifica existencia de chave na lista de uma categoria */
int busca_lista(ll** lista, char* pk) {
	ll* aux = *lista;

	/* procura chave */
	while(aux != NULL && strcmp(aux->pk, pk) != 0)
		aux = aux->prox;

	/* retorna 0 se nao achar, ou 1 se achar */
	if(!aux)
		return 0;
	return 1;
}

void apaga_lista(ll** lista) {
	ll* aux = *lista;

	while(aux){
		ll* p = aux->prox;
		free(aux);
		aux = p;
	}

	*lista = NULL;
}

/* Realiza os scanfs na struct Produto */
void ler_entrada(char* registro, Produto *novo) {

	/* recebe os campos de um produto e os convertem para os padroes de um registro */
	int tam;

	scanf("%[^\n]s", novo->nome);
	strcpy(registro, novo->nome);
	tam = strlen(novo->nome);
	strcpy(registro+tam, "@");
	tam++;

	scanf("\n%[^\n]s", novo->marca);
	strcpy(registro+tam, novo->marca);
	tam += strlen(novo->marca);
	strcpy(registro+tam, "@");
	tam++;

	scanf("\n%[^\n]s", novo->data);
	strcpy(registro+tam, novo->data);
	tam += strlen(novo->data);
	strcpy(registro+tam, "@");
	tam++;

	scanf("\n%[^\n]s", novo->ano);
	strcpy(registro+tam, novo->ano);
	tam += strlen(novo->ano);
	strcpy(registro+tam, "@");
	tam++;

	scanf("\n%[^\n]s", novo->preco);
	strcpy(registro+tam, novo->preco);
	tam += strlen(novo->preco);
	strcpy(registro+tam, "@");
	tam++;

	scanf("\n%[^\n]s", novo->desconto);
	strcpy(registro+tam, novo->desconto);
	tam += strlen(novo->desconto);
	strcpy(registro+tam, "@");
	tam++;

	scanf("\n%[^\n]s", novo->categoria);
	strcpy(registro+tam, novo->categoria);
	tam += strlen(novo->categoria);
	strcpy(registro+tam, "@");
	tam++;

	while(tam < 192) {
		registro[tam] = '#';
		tam++;
	}
	registro[tam] = '\0';

	gerarChave(novo);
}

/* gera a chave primaria de um produto */
void gerarChave(Produto* p) {
	strncpy(p->pk, p->nome, 2);
	strncpy(p->pk+2, p->marca, 2);
	strncpy(p->pk+4, p->data, 2);
	strncpy(p->pk+6, p->data+3, 2);
	strncpy(p->pk+8, p->ano, 2);
	p->pk[10] = '\0';
}