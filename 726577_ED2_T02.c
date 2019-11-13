/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
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
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51
#define TAM_STRING_INDICE (TAM_MARCA + TAM_NOME)


#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define MAX_ORDEM 150
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!"
#define INICIO_BUSCA 				"********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM				"********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 			"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO				"********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO      "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO    "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP 			"Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS 			"Busca por %s.\nNos percorridos:\n"


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


/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_ip *chave;		/* vetor das chaves e rrns [m-1]*/
	int *desc;	/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_is *chave;		/* vetor das chaves e rrns [m-1]*/
	int *desc;	/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_is;

typedef struct {
	int raiz;
} Indice;

/* Variáveis globais */
char ARQUIVO[MAX_REGISTROS * TAM_REGISTRO + 1];
char ARQUIVO_IP[2000*sizeof(Chave_ip)];
char ARQUIVO_IS[2000*sizeof(Chave_is)];
/* Ordem das arvores */
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
/*Quantidade de bytes que ocupa cada nó da árvore nos arquivos de índice:*/
int tamanho_registro_ip;
int tamanho_registro_is;
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

Produto recuperar_registro(int rrn);
void gerarChave(Produto* p);

/* retorna string no formato de registro para o arquivo de dados a partir das informacoes de um produto */
void inserir_Registro_arquivoDados(Produto p);

void insere_btree_ip(Indice *iprimary, Chave_ip nova_chave);
void insere_btree_ip_AUX(int rrn, Chave_ip nova_chave, int* rrn_filho_direito, Chave_ip* chave_promovida);
void divide_NO_ip(node_Btree_ip* node, Chave_ip chave, int* rrn_filho_direito, Chave_ip* chave_promovida);

void insere_btree_is(Indice *ibrand, Chave_is nova_chave);
void insere_btree_is_AUX(int rrn, Chave_is nova_chave, int* rrn_filho_direito, Chave_is* chave_promovida);
void divide_NO_is(node_Btree_is* node, Chave_is chave, int* rrn_filho_direito, Chave_is* chave_promovida);

node_Btree_ip* busca_btree(int rrn, char* pk);
int busca_codigo(int rrn, char* pk, Chave_ip* chave);
int busca_marca_nome(int rrn, char* string, Chave_is* chave);

void pre_ordem(int rrn, int nivel);
void em_ordem(int rrn);


/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
 
 /********************FUNÇÕES DO MENU*********************/
 void cadastrar(Indice* iprimary, Indice* ibrand);
 
 int alterar(Indice iprimary);
 
 void buscar(Indice iprimary,Indice ibrand);
 
 void listar(Indice iprimary,Indice ibrand);
 
 /*******************************************************/
 
 /* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
 int carregar_arquivo();
 
/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);
 
/* (Re)faz o índice de marcas e nomes  */
void criar_ibrand(Indice *ibrand) ;
 
/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip);
 
/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip);
 
/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
void *criar_no(char ip);
 
/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no(void *node, char ip);
 
/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto p);
 
/* Exibe o Produto */
int exibir_registro(int rrn);

int main()
{
	char *p; /* # */
	Indice iprimary;
	Indice ibrand;
	int opcao;
  /* Arquivo */
	int carregarArquivo = 0; nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = ordem_ip*3 + 4 + (-1 + ordem_ip)*14;
	tamanho_registro_is = ordem_is*3 + 4 + (-1 + ordem_is)* (	TAM_STRING_INDICE +9);

	/* Índice primário */
	criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Produtos */
	criar_ibrand(&ibrand);

	/* Execução do programa */
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao) {
		case 1: /* Cadastrar um novo Produto */
			cadastrar(&iprimary, &ibrand);
			break;
		case 2: /* Alterar o desconto de um Produto */
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar um Produto */
			printf(INICIO_BUSCA);
			buscar(iprimary, ibrand);
			break;
		case 4: /* Listar todos os Produtos */
			printf(INICIO_LISTAGEM);
			listar(iprimary, ibrand);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			if(!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IP; *p!='\0'; p+=tamanho_registro_ip)
				{
					fwrite( p , 1 ,tamanho_registro_ip,stdout);
					puts("");
				}
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			if(!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IS; *p!='\0'; p+=tamanho_registro_is)
				{
					fwrite( p , 1 ,tamanho_registro_is,stdout);
					puts("");

				}
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
			return 0;
		default: /* exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}


/* Exibe o Produto */
int exibir_registro(int rrn)
{
	float preco;
	int desconto;
	Produto j = recuperar_registro(rrn);
    char *cat, categorias[TAM_CATEGORIA];

	if(rrn < 0)
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

/* gera chave de um produto */
void gerarChave(Produto* p){
	strncpy(p->pk, p->nome, 2);
	strncpy(p->pk+2, p->marca, 2);
	strncpy(p->pk+4, p->data, 2);
	strncpy(p->pk+6, p->data+3, 2);
	strncpy(p->pk+8, p->ano, 2);
	p->pk[10] = '\0';
}

/* aloca NO de arvore para ser utilizado em memoria principal */
void *criar_no(char ip){
	/* aloca NO da arvore de indices primarios */
	if(ip == 'p'){
		node_Btree_ip* NOVO = (node_Btree_ip*)malloc(sizeof(node_Btree_ip));
		NOVO->num_chaves = 0;
		NOVO->chave = (Chave_ip*)malloc((ordem_ip-1) * sizeof(Chave_ip));
		NOVO->desc = (int*)malloc(ordem_ip * sizeof(int));
		return NOVO;
	}
	/* aloca NO da arvore de indices secundarios */
	else{
		node_Btree_is* NOVO = (node_Btree_is*)malloc(sizeof(node_Btree_is));
		NOVO->num_chaves = 0;
		NOVO->chave = (Chave_is*)malloc((ordem_is-1) * sizeof(Chave_is));
		NOVO->desc = (int*)malloc(ordem_is * sizeof(int));
		return NOVO;
	}
}

/* desaloca campos dinamicos de NO, inclusive ele mesmo */
void libera_no(void *node, char ip){
	/* desaloca NO da arvore de indices primarios */
	if(ip == 'p'){
		node_Btree_ip* p = node;
		free(p->desc);
		free(p->chave);
		free(p);
	}
	/* desaloca NO da arvore de indices secundarios */
	else{
		node_Btree_is* p = node;
		free(p->desc);
		free(p->chave);
		free(p);
	}
}

/* (Re)faz o indice primario */
void criar_iprimary(Indice *iprimary){
	Produto p;
	Chave_ip chave;
	int i;

	iprimary->raiz = -1;
	
	if(strlen(ARQUIVO) == 0)
		return;

	/* insere demais chaves e/ou novos NOS */
	for(i = 0; i < nregistros; i++){
		p = recuperar_registro(i);
		chave.rrn = i;
		strcpy(chave.pk, p.pk);
		insere_btree_ip(iprimary, chave);
	}
}

void insere_btree_ip(Indice *iprimary, Chave_ip nova_chave){
	/* arvore vazia, insere o primeiro NO */
	if(iprimary->raiz == -1){
		node_Btree_ip* node = criar_no('p');
		iprimary->raiz = 0;
		node->chave[node->num_chaves].rrn = nova_chave.rrn;
		strcpy(node->chave[node->num_chaves].pk, nova_chave.pk);
		node->folha = 'F';
		node->num_chaves++;
		write_btree(node, 0, 'p');
		libera_no(node, 'p');
		nregistrosip++;
	}
	/* arvore nao vazia */
	else{
		Chave_ip chave_promovida;
		int rrn_filho_direito = -1;
		insere_btree_ip_AUX(iprimary->raiz, nova_chave, &rrn_filho_direito, &chave_promovida);

		/* se ocorreu split na raiz, altura da arvore mudou, atualiza rrn da raiz do iprimary */
		if(rrn_filho_direito != -1){
			node_Btree_ip* nova_raiz = criar_no('p');
			nova_raiz->folha = 'N';
			nova_raiz->num_chaves = 1;
			nova_raiz->chave[0].rrn = chave_promovida.rrn;
			strcpy(nova_raiz->chave[0].pk, chave_promovida.pk);
			nova_raiz->desc[0] = iprimary->raiz;
			nova_raiz->desc[1] = rrn_filho_direito;

			write_btree(nova_raiz, nregistrosip, 'p');
			libera_no(nova_raiz, 'p');
			iprimary->raiz = nregistrosip;
			nregistrosip++;
		}
	}
}

void insere_btree_ip_AUX(int rrn, Chave_ip nova_chave, int* rrn_filho_direito, Chave_ip* chave_promovida){
	node_Btree_ip* node = read_btree(rrn, 'p');
	if(node->folha == 'F'){
		/* se houver espaco no NO, insere chave na posicao correta arrastando posicoes seguintes para frente */
		if(node->num_chaves < ordem_ip-1){
			int i = node->num_chaves-1;
			while(i >= 0 && strcmp(nova_chave.pk, node->chave[i].pk) < 0){
				node->chave[i+1].rrn = node->chave[i].rrn;
				strcpy(node->chave[i+1].pk, node->chave[i].pk);
				i--;
			}
			node->chave[i+1].rrn = nova_chave.rrn;
			strcpy(node->chave[i+1].pk, nova_chave.pk);
			node->num_chaves++;

			write_btree(node, rrn, 'p');
			libera_no(node, 'p');
			*rrn_filho_direito = -1;
			return;
		}
		/* NO cheio, divide o NO */
		divide_NO_ip(node, nova_chave, rrn_filho_direito, chave_promovida);
		write_btree(node, rrn, 'p');
		libera_no(node, 'p');
		return;
	}
	/* procura NO folha para insercao */
	else{
		int i = node->num_chaves-1;
		while(i >= 0 && strcmp(nova_chave.pk, node->chave[i].pk) < 0)
			i--;
		insere_btree_ip_AUX(node->desc[i+1], nova_chave, rrn_filho_direito, chave_promovida);

		/* se alguma chave foi promovida */
		if(*rrn_filho_direito != -1){
			nova_chave.rrn = chave_promovida->rrn;
			strcpy(nova_chave.pk, chave_promovida->pk);
			/* se tem vaga o NO atual, insere chave nesse NO  */
			if(node->num_chaves < ordem_ip-1){
				/* acha a posicao de insercao */
				int i = node->num_chaves-1;
				while(i >= 0 && strcmp(nova_chave.pk, node->chave[i].pk) < 0){
					node->chave[i+1].rrn = node->chave[i].rrn;
					strcpy(node->chave[i+1].pk, node->chave[i].pk);
					node->desc[i+2] = node->desc[i+1];
					i--;
				}
				/* insere a nova chave */
				node->chave[i+1].rrn = nova_chave.rrn;
				strcpy(node->chave[i+1].pk, nova_chave.pk);
				node->desc[i+2] = *rrn_filho_direito;
				node->num_chaves++;
				/* salva no arquivo */
				write_btree(node, rrn, 'p');
				libera_no(node, 'p');
				*rrn_filho_direito = -1;
				return;
			}
			/* se o NO atual estiver cheio, o divide */
			else{
				divide_NO_ip(node, nova_chave, rrn_filho_direito, chave_promovida);
				write_btree(node, rrn, 'p');
				libera_no(node, 'p');
				return;
			}
		}
	}
}

void divide_NO_ip(node_Btree_ip* node, Chave_ip chave, int* rrn_filho_direito, Chave_ip* chave_promovida){
	int chave_alocada = 0, i = node->num_chaves-1, j;
	node_Btree_ip* new_node = criar_no('p');

	new_node->folha = node->folha;
	new_node->num_chaves = (ordem_ip-1)/2;

	for(j = new_node->num_chaves-1; j >= 0; j--){
		if(!chave_alocada && strcmp(chave.pk, node->chave[i].pk) > 0){
			new_node->chave[j].rrn = chave.rrn;
			strcpy(new_node->chave[j].pk, chave.pk);
			new_node->desc[j+1] = *rrn_filho_direito;
			chave_alocada = 1;
		}
		else{
			new_node->chave[j].rrn = node->chave[i].rrn;
			strcpy(new_node->chave[j].pk, node->chave[i].pk);
			new_node->desc[j+1] = node->desc[i+1];
			i--;
		}
	}

	if(!chave_alocada){
		while(i >= 0 && strcmp(chave.pk, node->chave[i].pk) < 0){
			node->chave[i+1].rrn = node->chave[i].rrn;
			strcpy(node->chave[i+1].pk, node->chave[i].pk);
			node->desc[i+2] = node->desc[i+1];
			i--;
		}
		node->chave[i+1].rrn = chave.rrn;
		strcpy(node->chave[i+1].pk, chave.pk);
		node->desc[i+2] = *rrn_filho_direito;
	}

	chave_promovida->rrn = node->chave[ordem_ip/2].rrn;
	strcpy(chave_promovida->pk, node->chave[ordem_ip/2].pk);
	new_node->desc[0] = node->desc[(ordem_ip/2)+1];
	node->num_chaves = ordem_ip/2;

	write_btree(new_node, nregistrosip, 'p');
	libera_no(new_node, 'p');
	*rrn_filho_direito = nregistrosip;
	nregistrosip++;
}

/* (Re)faz o índice de marcas e nomes */
void criar_ibrand(Indice *ibrand){
	int i;
	Produto p;
	Chave_is chave;

	ibrand->raiz = -1;
	if(strlen(ARQUIVO) == 0)
		return;

	/* insere demais chaves e/ou novos NOS */
	for(i = 0; i < nregistros; i++){
		p = recuperar_registro(i);
		strcpy(chave.string, p.marca);
		strcpy(chave.string+strlen(p.marca), "$");
		strcpy(chave.string+strlen(p.marca)+1, p.nome);
		strcpy(chave.pk, p.pk);
		insere_btree_is(ibrand, chave);
	}
}

void insere_btree_is(Indice *ibrand, Chave_is nova_chave){
	/* arvore vazia, insere o primeiro NO */
	if(ibrand->raiz == -1){
		node_Btree_is* node = criar_no('s');
		ibrand->raiz = 0;
		strcpy(node->chave[node->num_chaves].string, nova_chave.string);
		strcpy(node->chave[node->num_chaves].pk, nova_chave.pk);
		node->folha = 'F';
		node->num_chaves++;
		write_btree(node, 0, 's');
		libera_no(node, 's');
		nregistrosis++;
	}
	/* arvore nao vazia */
	else{
		Chave_is chave_promovida;
		int rrn_filho_direito = -1;
		insere_btree_is_AUX(ibrand->raiz, nova_chave, &rrn_filho_direito, &chave_promovida);

		/* se ocorreu split na raiz, altura da arvore mudou, atualiza rrn da raiz do iprimary */
		if(rrn_filho_direito != -1){
			node_Btree_is* nova_raiz = criar_no('s');
			nova_raiz->folha = 'N';
			nova_raiz->num_chaves = 1;
			strcpy(nova_raiz->chave[0].string, chave_promovida.string);
			strcpy(nova_raiz->chave[0].pk, chave_promovida.pk);
			nova_raiz->desc[0] = ibrand->raiz;
			nova_raiz->desc[1] = rrn_filho_direito;

			write_btree(nova_raiz, nregistrosis, 's');
			libera_no(nova_raiz, 's');
			ibrand->raiz = nregistrosis;
			nregistrosis++;
		}
	}
}

void insere_btree_is_AUX(int rrn, Chave_is nova_chave, int* rrn_filho_direito, Chave_is* chave_promovida){
	node_Btree_is* node = read_btree(rrn, 's');
	if(node->folha == 'F'){
		/* se houver espaco no NO, insere chave na posicao correta arrastando posicoes seguintes para frente */
		if(node->num_chaves < ordem_is-1){
			int i = node->num_chaves-1;
			while(i >= 0 && strcmp(nova_chave.string, node->chave[i].string) < 0){
				strcpy(node->chave[i+1].string, node->chave[i].string);
				strcpy(node->chave[i+1].pk, node->chave[i].pk);
				i--;
			}
			strcpy(node->chave[i+1].string, nova_chave.string);
			strcpy(node->chave[i+1].pk, nova_chave.pk);
			node->num_chaves++;

			write_btree(node, rrn, 's');
			libera_no(node, 's');
			*rrn_filho_direito = -1;
			return;
		}
		/* NO cheio, divide o NO */
		divide_NO_is(node, nova_chave, rrn_filho_direito, chave_promovida);
		write_btree(node, rrn, 's');
		libera_no(node, 's');
		return;
	}
	/* procura NO folha para insercao */
	else{
		int i = node->num_chaves-1;
		while(i >= 0 && strcmp(nova_chave.string, node->chave[i].string) < 0)
			i--;
		insere_btree_is_AUX(node->desc[i+1], nova_chave, rrn_filho_direito, chave_promovida);

		/* se alguma chave foi promovida */
		if(*rrn_filho_direito != -1){
			strcpy(nova_chave.string, chave_promovida->string);
			strcpy(nova_chave.pk, chave_promovida->pk);
			/* se tem vaga o NO atual, insere chave nesse NO  */
			if(node->num_chaves < ordem_is-1){
				/* acha a posicao de insercao */
				int i = node->num_chaves-1;
				while(i >= 0 && strcmp(nova_chave.string, node->chave[i].string) < 0){
					strcpy(node->chave[i+1].string, node->chave[i].string);
					strcpy(node->chave[i+1].pk, node->chave[i].pk);
					node->desc[i+2] = node->desc[i+1];
					i--;
				}
				/* insere a nova chave */
				strcpy(node->chave[i+1].string, nova_chave.string);
				strcpy(node->chave[i+1].pk, nova_chave.pk);
				node->desc[i+2] = *rrn_filho_direito;
				node->num_chaves++;
				/* salva no arquivo */
				write_btree(node, rrn, 's');
				libera_no(node, 's');
				*rrn_filho_direito = -1;
				return;
			}
			/* se o NO atual estiver cheio, o divide */
			else{
				divide_NO_is(node, nova_chave, rrn_filho_direito, chave_promovida);
				write_btree(node, rrn, 's');
				libera_no(node, 's');
				return;
			}
		}
	}
}

void divide_NO_is(node_Btree_is* node, Chave_is chave, int* rrn_filho_direito, Chave_is* chave_promovida){
	int chave_alocada = 0, i = node->num_chaves-1, j;
	node_Btree_is* new_node = criar_no('s');

	new_node->folha = node->folha;
	new_node->num_chaves = (ordem_is-1)/2;

	for(j = new_node->num_chaves-1; j >= 0; j--){
		if(!chave_alocada && strcmp(chave.string, node->chave[i].string) > 0){
			strcpy(new_node->chave[j].string, chave.string);
			strcpy(new_node->chave[j].pk, chave.pk);
			new_node->desc[j+1] = *rrn_filho_direito;
			chave_alocada = 1;
		}
		else{
			strcpy(new_node->chave[j].string, node->chave[i].string);
			strcpy(new_node->chave[j].pk, node->chave[i].pk);
			new_node->desc[j+1] = node->desc[i+1];
			i--;
		}
	}

	if(!chave_alocada){
		while(i >= 0 && strcmp(chave.string, node->chave[i].string) < 0){
			strcpy(node->chave[i+1].string, node->chave[i].string);
			strcpy(node->chave[i+1].pk, node->chave[i].pk);
			node->desc[i+2] = node->desc[i+1];
			i--;
		}
		strcpy(node->chave[i+1].string, chave.string);
		strcpy(node->chave[i+1].pk, chave.pk);
		node->desc[i+2] = *rrn_filho_direito;
	}

	strcpy(chave_promovida->string, node->chave[ordem_is/2].string);
	strcpy(chave_promovida->pk, node->chave[ordem_is/2].pk);
	new_node->desc[0] = node->desc[(ordem_is/2)+1];
	node->num_chaves = ordem_is/2;

	write_btree(new_node, nregistrosis, 's');
	libera_no(new_node, 's');
	*rrn_filho_direito = nregistrosis;
	nregistrosis++;
}

/* Escreve um NO da arvore no arquivo de indice correspondente */
void write_btree(void *salvar, int rrn, char ip){
	int i, j;
    /* salva NO no arquivo de indices primarios */
    if(ip == 'p'){
        int pos = rrn * tamanho_registro_ip; /* recebe posicao inicial de escrita */
        char p[TAM_REGISTRO];
 
        node_Btree_ip* node = (node_Btree_ip*)salvar;
        sprintf(p, "%03d", node->num_chaves);
        strncpy(ARQUIVO_IP + pos, p, 3);
        pos+=3; /* move posicao para inicio da primeira chave */
 
        /* copia cada chave e seu rrn para o arquivo */
        for(i = 0; i < node->num_chaves; i++){
            strcpy(p, node->chave[i].pk);
            strncpy(ARQUIVO_IP + pos, p, 10);
            pos+=10;
            sprintf(p, "%04d", node->chave[i].rrn);
            strncpy(ARQUIVO_IP + pos, p, 4);
            pos+=4; /* move posicao para inicio da proxima chave */
        }
        for(i = node->num_chaves; i < ordem_ip-1; i++){
            for(j = 0; j < 14; j++)
                ARQUIVO_IP[pos++] = '#';
        }
        ARQUIVO_IP[pos++] = node->folha;
 
        if(node->folha == 'N'){
            for(i = 0; i <= node->num_chaves; i++){
                sprintf(p, "%03d", node->desc[i]);
                strncpy(ARQUIVO_IP + pos, p, 3);
                pos+=3;
            }
        }
 
        while(pos < tamanho_registro_ip * (rrn+1))
            ARQUIVO_IP[pos++] = '*';
 
    }
    /* salva NO no arquivo de indices secundarios */
    else{
        int pos = rrn * tamanho_registro_is; /* recebe posicao inicial de escrita */
        char p[TAM_STRING_INDICE];
 
        node_Btree_is* node = (node_Btree_is*)salvar;
        sprintf(p, "%03d", node->num_chaves);
        strncpy(ARQUIVO_IS + pos, p, 3);
        pos+=3; /* move posicao para inicio da primeira chave */
 
        /* copia cada chave e string para o arquivo */
        for(i = 0; i < node->num_chaves; i++){
        	/* copia a chave de um NO */
            strcpy(p, node->chave[i].pk);
            strncpy(ARQUIVO_IS + pos, p, 10);
            pos+=10;
 			
 			/* copia a string de um NO */
            sprintf(p, "%s", node->chave[i].string);
            strncpy(ARQUIVO_IS + pos, p, strlen(p));
            pos+=strlen(p);
 
 			/* le ate o inicio da proxima chave, escrevendo as '#' */
            while(pos < ((rrn*tamanho_registro_is)+(i+1)*111) + 3)
                ARQUIVO_IS[pos++] = '#';
        }
        /* le ate o final do registro, escrevendo as '#' */
        for(i = node->num_chaves; i < ordem_is-1; i++){
            for(j = 0; j < 111; j++)
                ARQUIVO_IS[pos++] = '#';
        }
        ARQUIVO_IS[pos++] = node->folha;
 		/* se o NO nao for folha, escreve seus descendentes */
        if(node->folha == 'N'){
            for(i = 0; i <= node->num_chaves; i++){
                sprintf(p, "%03d", node->desc[i]);
                strncpy(ARQUIVO_IS + pos, p, 3);
                pos+=3;
            }
        }
 		/* escreve '*' para descendentes vazios */
        while(pos < tamanho_registro_is * (rrn+1))
            ARQUIVO_IS[pos++] = '*';
    }
}

/*Lê um nó do arquivo de índice e retorna na estrutura */
void *read_btree(int rrn, char ip){
	int i, j;
	/* le NO do arquivo de indices primarios */
	if(ip == 'p'){
		char p[11]; /* auxiliar para conversao de strings em inteiros */
		int pos = tamanho_registro_ip * rrn;
		node_Btree_ip* node = criar_no('p');

		/* NO recebe numero de chaves */
		strncpy(p, ARQUIVO_IP + pos, 3); /* p recebe numero de chaves como string */
		p[3] = '\0';
		sscanf(p, "%d", &node->num_chaves);
		pos+=3;

		/* NO recebe chaves e rrn das chaves */
		for(i = 0; i < node->num_chaves; i++){
			strncpy(p, ARQUIVO_IP + pos, 10); /* recebe a chave */
			p[10] = '\0';
			strcpy(node->chave[i].pk, p);
			pos+=10;

			strncpy(p, ARQUIVO_IP + pos, 4); /* p recebe o rrn da chave no arq. de dados como string */
			p[4] = '\0';
			sscanf(p, "%d", &node->chave[i].rrn);
			pos+=4;
		}

		/* pula chaves vazias */
		while(ARQUIVO_IP[pos] == '#')
			pos++;

		node->folha = ARQUIVO_IP[pos++]; /* NO recebe flag de folha */

		/* se nao for uma folha, NO recebe os rrns dos respectivos descendentes */
		if(node->folha == 'N'){
			for(i = 0; i <= node->num_chaves; i++){
				strncpy(p, ARQUIVO_IP + pos, 3); /* p recebe o rrn de cada descendente como string */
				p[3] = '\0';
				sscanf(p, "%d", &node->desc[i]);
				pos+=3;
			}
		}

		return node;
	}
	/* le NO do arquivo de indices secundarios */
	else{
		char p[4]; /* auxiliar para conversao de strings em inteiros */
		int pos = tamanho_registro_is * rrn;
		node_Btree_is* node = criar_no('s');

		/* NO recebe numero de chaves */
		strncpy(p, ARQUIVO_IS + pos, 3); /* p recebe numero de chaves como string */
		p[3] = '\0';
		sscanf(p, "%d", &node->num_chaves);
		pos+=3;

		/* NO recebe chaves e strings das chaves */
		for(i = 0; i < node->num_chaves; i++){
			strncpy(node->chave[i].pk, ARQUIVO_IS + pos, 10); /* p recebe a chave */
			node->chave[i].pk[10] = '\0';
			pos+=10;

			/* recebe as strings */
			j = 0;
			while(ARQUIVO_IS[pos] != '#'){
				node->chave[i].string[j] = ARQUIVO_IS[pos];
				j++; pos++;
			}
			node->chave[i].string[j] = '\0';
			
			/* pula chaves vazias */
			while(ARQUIVO_IS[pos] == '#')
				pos++;
		}

		node->folha = ARQUIVO_IS[pos++]; /* NO recebe flag de folha */

		/* se nao for uma folha, NO recebe os rrns dos respectivos descendentes */
		if(node->folha == 'N'){
			for(i = 0; i <= node->num_chaves; i++){
				strncpy(p, ARQUIVO_IS + pos, 3); /* p recebe o rrn de cada descendente como string */
				p[3] = '\0';
				sscanf(p, "%d", &node->desc[i]);
				pos+=3;
			}
		}

		return node;
	}
}

node_Btree_ip* busca_btree(int rrn, char* pk){
	node_Btree_ip* node = read_btree(rrn, 'p');
	/* busca posicao no NO onde a chave deveria estar */
	int i = 0, rrn_aux;
	while(i < node->num_chaves && strcmp(pk, node->chave[i].pk) > 0)
		i++;

	/* se a chave estiver na posicao, retorna NO */
	if(i < node->num_chaves && strcmp(pk, node->chave[i].pk) == 0)
		return node;

	/* se for uma folha, retorna NULL, pois nao achou a chave */
	if(node->folha == 'F'){
		libera_no(node, 'p');
		return NULL;
	}

	/* se nao for folha, procura no descendente daquela posicao */
	rrn_aux = node->desc[i];
	libera_no(node, 'p');
	return busca_btree(rrn_aux, pk);
}

void cadastrar(Indice* iprimary, Indice* ibrand){
	Produto p;

	/* recebe os campos de um produto */
	scanf("\n%[^\n]s", p.nome);
	scanf("\n%[^\n]s", p.marca);
	scanf("\n%[^\n]s", p.data);
	scanf("\n%[^\n]s", p.ano);
	scanf("\n%[^\n]s", p.preco);
	scanf("\n%[^\n]s", p.desconto);
	scanf("\n%[^\n]s", p.categoria);
	gerarChave(&p);

	/* se a arvore estiver vazia */
	if(iprimary->raiz == -1){
		inserir_registro_indices(iprimary, ibrand, p);
		inserir_Registro_arquivoDados(p);
		return;
	}
	/* verifica se a chave ja existe na arvore */
	else{
		node_Btree_ip* busca = busca_btree(iprimary->raiz, p.pk);
		if(busca == NULL){
			inserir_registro_indices(iprimary, ibrand, p);
			inserir_Registro_arquivoDados(p);
		}
		else{
			printf(ERRO_PK_REPETIDA, p.pk);
			libera_no(busca, 'p');
		}
	}
}

/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto p){
	Chave_ip chaveip;
	Chave_is chaveis;

	strcpy(chaveip.pk, p.pk);
	chaveip.rrn = nregistros;

	strcpy(chaveis.string, p.marca);
	strcpy(chaveis.string+strlen(p.marca), "$");
	strcpy(chaveis.string+strlen(p.marca)+1, p.nome);
	strcpy(chaveis.pk, p.pk);

	insere_btree_ip(iprimary, chaveip);
	insere_btree_is(ibrand, chaveis);
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

int alterar(Indice iprimary){
	char pk[TAM_PRIMARY_KEY], desconto[TAM_DESCONTO], c, temp[193], *p;
	int descontoInt, cont, i, j;
	node_Btree_ip* busca = criar_no('p');
	
	scanf("%[^\n]s", pk);

	if(iprimary.raiz != -1){
		busca = busca_btree(iprimary.raiz, pk);
		/* se o NO com a chave existir, recebe o novo desconto, verificando se a entrada esta no padrao correto */
		if(busca != NULL){
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
		}
		else{		
			printf(REGISTRO_N_ENCONTRADO);
			return 0;
		}
	}
	else{		
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	/* percorre o NO ate a chave a ser alterada */
	i = 0;
	while(strcmp(busca->chave[i].pk, pk) != 0)
		i++;

	/* a partir do rrn da chave encontrada, busca o registro no arquivo */
	strncpy(temp, ARQUIVO + (busca->chave[i].rrn)*192, 192);
	temp[192] = '\0';

	/* percorre o registro ate o campo de desconto*/
	cont = 0;
	p = strtok(temp,"@");
	cont += strlen(p) + 1;

	for(j = 0; j < 4; j++) {
		p = strtok(NULL,"@");
		cont += strlen(p) + 1;
	}
	p = strtok(NULL,"@");
	cont += strlen(p) + 1;

	/* altera o campo de desconto no arquivo */
	strncpy(ARQUIVO + (busca->chave[i].rrn)*192 + cont, desconto, 3);
	libera_no(busca, 'p');
	return 1;
}

/* opcoes de buscar por codigo ou por nome e marca */
void buscar(Indice iprimary, Indice ibrand){
	char pk[TAM_PRIMARY_KEY], marca[TAM_MARCA], nome[TAM_NOME], string[TAM_STRING_INDICE];
	int opcao;
	scanf("%d\n", &opcao);

	switch(opcao){
		case 1:
			scanf("%[^\n]s", pk);
			printf(NOS_PERCORRIDOS_IP, pk);
			
			if(iprimary.raiz != -1){
				Chave_ip chaveip;
				if(busca_codigo(iprimary.raiz, pk, &chaveip)){
					printf("\n");
					exibir_registro(chaveip.rrn);
				}
				else printf("\n"REGISTRO_N_ENCONTRADO);
			}
			else printf("\n"REGISTRO_N_ENCONTRADO);
		break;

		case 2:
			/* recebe nome e marca, e as transforma em uma string separada por '$' */
			scanf("%[^\n]s", marca);
			scanf("\n%[^\n]s", nome);
			strcpy(string, marca);
			strcpy(string+strlen(marca), "$");
			strcpy(string+strlen(marca)+1, nome);

			printf(NOS_PERCORRIDOS_IS, string);

			/* chama a funcao de busca caso a arvore nao esteja vazia */
			if(ibrand.raiz != -1){
				Chave_is chaveis;
				/* retorna a chave procurada em chaveis, se foi encontrada */
				if(busca_marca_nome(ibrand.raiz, string, &chaveis)){
					/* busca pela chave na arvore de indices primarios, para obter o rrn da chave para impressao */
					node_Btree_ip* busca = busca_btree(iprimary.raiz, chaveis.pk);

					int i = 0;
					while(strcmp(busca->chave[i].pk, chaveis.pk) != 0)
						i++;

					printf("\n");
					exibir_registro(busca->chave[i].rrn);
					libera_no(busca, 's');
				}
				else printf("\n"REGISTRO_N_ENCONTRADO);
			}
			else printf("\n"REGISTRO_N_ENCONTRADO);
		break;
	}
}

int busca_codigo(int rrn, char* pk, Chave_ip* chave){
	node_Btree_ip* node = read_btree(rrn, 'p');
	int i = 0, pos = 0, flag = 0, filho;

	while(i < node->num_chaves){
		if(strcmp(pk, node->chave[i].pk) > 0)
			pos++;
	
		if(!flag){
			printf("%s", node->chave[i].pk);
			flag = 1;
		}
		else printf(", %s", node->chave[i].pk);
	
		i++;
	}
	printf("\n");

	if(node->folha == 'F' && strcmp(pk, node->chave[pos].pk) != 0)
		return 0;

	if(strcmp(pk, node->chave[pos].pk) == 0){

		chave->rrn = node->chave[pos].rrn;
		strcpy(chave->pk, node->chave[pos].pk);
		libera_no(node, 'p');
		return 1;
	}

	filho = node->desc[pos];
	libera_no(node, 'p');

	return busca_codigo(filho, pk, chave);
}

int busca_marca_nome(int rrn, char* string, Chave_is* chave){
	node_Btree_is* node = read_btree(rrn, 's');

	/* procura pela posicao da chave, ou pela posicao do descendente onde a chave devera ser procurada */
	int pos, i, filho;
	for(i = 0, pos = 0; i < node->num_chaves; i++){
		if(strcmp(string, node->chave[i].string) > 0)
			pos++;
	}

	/* imprime o NO */
	printf("%s", node->chave[0].string);
	for(i = 1; i < node->num_chaves; i++){
		printf(", %s", node->chave[i].string);
	}
	printf("\n");

	/* se for uma folha, e a posicao da chave nao corresponde com a string procurada, a chave nao existe na arvore */
	if(node->folha == 'F' && strcmp(string, node->chave[pos].string) != 0)
		return 0;

	filho = node->desc[pos];
	if(pos == node->num_chaves)
		pos--;

	/* se a string da chave na posicao encontrada eh igual a string procurada, a chave foi encontrada */
	if(strcmp(string, node->chave[pos].string) == 0){
		strcpy(chave->string, node->chave[pos].string);
		strcpy(chave->pk, node->chave[pos].pk);
		libera_no(node, 's');
		return 1;
	}

	/* se nao achou a chave, procura no descendente correspondente */
	libera_no(node, 's');
	return busca_marca_nome(filho, string, chave);
}

void listar(Indice iprimary,Indice ibrand){
	int opcao = 0;
	scanf("%d", &opcao);

	switch(opcao){
		case 1:
			if(iprimary.raiz != -1)
				pre_ordem(iprimary.raiz, 1);
			else printf(REGISTRO_N_ENCONTRADO);
		break;

		case 2:
			if(ibrand.raiz != -1)
				em_ordem(ibrand.raiz);
			else printf(REGISTRO_N_ENCONTRADO);
		break;

		default:
		break;
	}
}

void pre_ordem(int rrn, int nivel){
	node_Btree_ip* node = read_btree(rrn, 'p');
	int i;

	printf("%d - %s", nivel, node->chave[0].pk);

	i = 1;
	while(i < node->num_chaves){
		printf(", %s", node->chave[i].pk);
		i++;
	}
	printf("\n");

	if(node->folha == 'N'){
		for(i = 0; i <= node->num_chaves; i++)
			pre_ordem(node->desc[i], nivel+1);
	}
	libera_no(node, 'p');
}

void em_ordem(int rrn){
	node_Btree_is* node = read_btree(rrn, 's');
	int i, pos;

	if(node->folha == 'N')
		em_ordem(node->desc[0]);

	for(i = 0; i < node->num_chaves; i++){
		char* p = strtok(node->chave[i].string, "$");
		printf("%s", p);
		pos = strlen(p);
		while(pos < 50){
			printf("-");
			pos++;
		}
		printf(" ");

		p = strtok(NULL, "$");
		printf("%s", p);
		pos = strlen(p);
		while(pos < 50){
			printf("-");
			pos++;
		}
		printf("\n");

		if(node->folha == 'N')
			em_ordem(node->desc[i+1]);
	}
	libera_no(node, 'p');
}