#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_CARACTERES 256
#define TAMANHO_ALFABETO 26

typedef struct NoTrie {
	struct NoTrie *filhos[TAMANHO_ALFABETO];
	bool fim_palavra;
} NoTrie;

/* ==================== OPERAÇÕES BÁSICAS ==================== */

NoTrie *criar_no(void) {
	NoTrie *no = malloc(sizeof(NoTrie));

	if (no == NULL)
		exit(1);

	for (int i = 0; i < 26; i++)
		no->filhos[i] = NULL;

	no->fim_palavra = false;
	return no;
}

bool existe_palavra(NoTrie *raiz, const char *palavra) {
	NoTrie *no_atual = raiz;
	size_t tamanho_palavra = strlen(palavra);

	for (size_t i = 0; i < tamanho_palavra; i++) {
		// Calcula o indice da letra, número entre 0 - 25.
		int indice_palavra = palavra[i] - 'a';

		// Dado o nó atual, caso o indice dessa letra
		// não seja encontrado nos filhos, então não é possível encontrá-la.
		if (no_atual->filhos[indice_palavra] == NULL)
			return false;

		// Se a letra realmente exista (existe um nó nesse índice)
		// então o nó atual vai passar a ser esse nó
		no_atual = no_atual->filhos[indice_palavra];
	}

	// Percorridos os nós para todas as letras, ao chegar ao fim,
	// verifica-se se foi encontrada uma palavra inteira.
	return no_atual->fim_palavra;
}

void inserir_palavra(NoTrie *raiz, const char *palavra) {
	NoTrie *no_atual = raiz;
	size_t tamanho_palavra = strlen(palavra);

	if (tamanho_palavra > MAX_CARACTERES) {
		printf("Tamanho máximo para inserir novas palavras é de %d caracteres.\n", MAX_CARACTERES);
		return;
	}

	// Percorre para cada caractere da palavra
	for (size_t i = 0; i < tamanho_palavra; i++) {
		// Descobre o 'indice' que representa a letra atual da palavra
		int indice_palavra = palavra[i] - 'a';

		// Caso esse indice nos filhos do nó atual esteja vazio, criamos ele
		if (no_atual->filhos[indice_palavra] == NULL) {
			// Nó para representar cada letra da palavra
			no_atual->filhos[indice_palavra] = criar_no();
		}

		// O nó atual passa a ser esse índice recém-criado
		no_atual = no_atual->filhos[indice_palavra];
	}

	// Marca o nó atual como contendo uma palavra inteira,
	// do caminho da raíz até ele
	no_atual->fim_palavra = true;
}

NoTrie *criar_trie(void) {
	NoTrie *raiz = criar_no();

	const char *palavras[] = {
		"abacaxi", "barata", "caderno", "dente", "elefante", "foco", "gato", "hoquei", "ilha", "jogo",
		"kiwi", "livro", "mapa", "nuvem", "ovo", "pedra", "queijo", "rato", "sapo", "telefone",
		"uva", "vela", "xaxim", "zebra", "amigo", "bola", "cao", "dado", "escada", "faca",
		"gelo", "horta", "jujuba", "kilo", "lua", "manteiga", "nado", "ovelha", "pato", "rosto",
		"sol", "teto", "vaca", "zipe", "amendoim", "biscoito", "cabelo", "escova", "foca", "lapis",
		"mar", "olho", "pipa", "quadro", "sopa", "tomate", "urso", "vento", "abajur", "coracao",
		"dolar", "estrela", "fruta", "jardim", "leao", "mochila", "nave", "quente", "rua", "tartaruga",
		"yeti", "bolo", "iate"
	};

	int qtd_palavras = sizeof(palavras) / sizeof(palavras[0]);

	for (int i = 0; i < qtd_palavras; i++)
		inserir_palavra(raiz, palavras[i]);

	return raiz;
}

bool possui_filhos(NoTrie *raiz) {
	for (int i = 0; i < TAMANHO_ALFABETO; i++) {
		if (raiz->filhos[i] != NULL)
			return true;
	}
	return false;
}

NoTrie *remover_palavra_interno(NoTrie *raiz, const char *chave, size_t nivel) {
	if (raiz == NULL)
		return NULL;

	if (nivel == strlen(chave)) {
		raiz->fim_palavra = false;
	} else {
		int indice = chave[nivel] - 'a';
		raiz->filhos[indice] = remover_palavra_interno(raiz->filhos[indice], chave, nivel + 1); 
	}

	if (raiz->fim_palavra || possui_filhos(raiz))
		return raiz;
	
	free(raiz);
	raiz = NULL;
	return raiz;
}

void remover_palavra(NoTrie **raiz, const char *chave) {
	*raiz = remover_palavra_interno(*raiz, chave, 0);
}

void destruir_trie(NoTrie *raiz) {
	if (raiz == NULL)
		return;

	for (int i = 0; i < TAMANHO_ALFABETO; i++) {
		if (raiz->filhos[i] == NULL)
			continue;
		destruir_trie(raiz->filhos[i]);
	}

	free(raiz);
	raiz = NULL;
}

/* ==================== OPERAÇÕES BÁSICAS ==================== */

/* ==================== APLICAÇÕES REAIS ==================== */

void coletar_palavras(NoTrie *no_atual, char *prefixo_temporario, size_t nivel) {
	// Caso tenha chegado em um nó marcado como fim de palavra,
	// exibe a palavra que foi coletada até ele
	if (no_atual->fim_palavra) {
		prefixo_temporario[nivel] = '\0';
		printf("%s\n", prefixo_temporario);
	}

	for (int i = 0; i < TAMANHO_ALFABETO; i++) {
		// Caso não exista um nó nessa posição, ignora a posição
		if (no_atual->filhos[i] == NULL)
			continue;

		// Transforma a posição de volta na letra original,
		// para ser possível exibi-la
		prefixo_temporario[nivel] = i + 'a';

		// Chama a função novamente, passando esse nó filho
		coletar_palavras(no_atual->filhos[i], prefixo_temporario, nivel + 1);
	}
}

NoTrie *encontrar_no(NoTrie *raiz, const char *prefixo) {
	NoTrie *no_atual = raiz;
	size_t tamanho_prefixo = strlen(prefixo);

	for (size_t i = 0; i < tamanho_prefixo; i++) {
		int indice_letra = prefixo[i] - 'a';

		if (no_atual->filhos[indice_letra] == NULL)
			return NULL;

		no_atual = no_atual->filhos[indice_letra];
	}

	return no_atual;
}

void exibir_palavras_com_prefixo(NoTrie *raiz, const char *prefixo) {
	NoTrie *no_prefixo = encontrar_no(raiz, prefixo);

	if (no_prefixo == NULL) {
		printf("nao foram encontradas palavras.\n");
		return;
	}

	// Armazena o valor do prefixo em uma variável temporária
	char prefixo_temporario[MAX_CARACTERES];
	strcpy(prefixo_temporario, prefixo);

	// Guarda valor do tamanho do prefixo original,
	// para que o temporário seja povoado a partir desse tamanho,
	// com as possibilidades que encontrar
	size_t tamanho_prefixo = strlen(prefixo);

	coletar_palavras(no_prefixo, prefixo_temporario, tamanho_prefixo);
}

void exibir_todas_palavras(NoTrie *raiz) {	
	char prefixo_temporario[MAX_CARACTERES];
	coletar_palavras(raiz, prefixo_temporario, 0);
}

/* ==================== APLICAÇÕES REAIS ==================== */

/* ==================== UTILITÁRIOS ==================== */

bool entrada_valida(const char *str) {
	while (*str) {
		if (!islower((unsigned char)*str))
			return false;
		str++;
	}
	return true;
}

void limpar_buffer(void) {
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}

void aguardar_usuario(void) {
	limpar_buffer();
	printf("Pressione enter para continuar...\n");
	getchar();
}

/* ==================== UTILITÁRIOS ==================== */

/* ==================== INTERFACES ==================== */
void exibir_menu(void) {
	printf("======================================================================\n");
	printf("Opcoes:\n\n");

	printf("- 1: Exibir todas as palavras\n");
	printf("- 2: Exibir todas as palavras com um certo prefixo\n");
	printf("- 3: Verificar se uma palavra existe no sistema\n");
	printf("- 4: Inserir uma nova palavra no sistema\n");
	printf("- 5: Remover uma palavra do sistema\n");
	printf("- 0: Encerrar o sistema ou encerrar uma operacao\n\n");
	
	printf("Sao permitidas apenas palavras:\n");
	printf("- Minusculas.\n");
	printf("- Sem acentos.\n");
	printf("- Sem espacos.\n");
	printf("======================================================================\n");
}

void exibir_todas_palavras_interface(NoTrie *raiz) {
	printf("\nExibindo todas as palavras cadastradas:\n\n");
	exibir_todas_palavras(raiz);
	aguardar_usuario();
}

void exibir_palavras_com_prefixo_interface(NoTrie *raiz) {
	char entrada[MAX_CARACTERES];
	
	printf("Digite um prefixo, para descobrir as suas palavras possiveis.\n\n");	
	
	while (true) {
		printf("> ");
		scanf("%s", entrada);

		if (!entrada_valida(entrada)) {
			if (entrada[0] == '0') {
				printf("Retornando ao menu...\n");
				break;
			}

			printf("Tente novamente...\nDigite uma palavra minuscula sem acentos ou espacos.\n");
			continue;
		}

		exibir_palavras_com_prefixo(raiz, entrada);
		printf("\n");
	}
}

void existe_palavra_interface(NoTrie *raiz) {
	printf("Insira uma palavra para verificar se existe no dicionario:\n\n");
	
	char entrada[MAX_CARACTERES];

	printf("> ");
	scanf("%s", entrada);

	while (!entrada_valida(entrada)) {
		if (entrada[0] == '0') {
			printf("Retornando ao menu...\n");
			return;
		}

		printf("Tente novamente...\nDigite uma palavra minuscula sem acentos ou espacos.\n");
	}

	bool existe = existe_palavra(raiz, entrada);
	
	printf("A palavra %s dicionario\n", existe ? "esta" : "nao existe");
	aguardar_usuario();
}

void inserir_palavra_interface(NoTrie *raiz) {
	char entrada[MAX_CARACTERES];
	
	printf("Insira uma palavra que voce deseja no dicionario:\n\n");

	printf("> ");
	scanf("%s", entrada);

	while (!entrada_valida(entrada)) {
		if (entrada[0] == '0') {
			printf("Retornando ao menu...\n");
			return;
		}

		printf("Tente novamente...\nDigite uma palavra minuscula sem acentos ou espacos.\n");
	}

	inserir_palavra(raiz, entrada);
	
	printf("Palavra cadastrada com sucesso.\n");
	aguardar_usuario();
}

void remover_palavra_interface(NoTrie **raiz) {
	char entrada[MAX_CARACTERES];
	
	printf("Insira uma palavra para remove-la do dicionario, caso exista:\n\n");

	printf("> ");
	scanf("%s", entrada);

	while (!entrada_valida(entrada)) {
		if (entrada[0] == '0') {
			printf("Retornando ao menu...\n");
			return;
		}
	
		printf("Tente novamente...\nDigite uma palavra minuscula sem acentos ou espacos.\n");
	}

	bool existe = existe_palavra(*raiz, entrada);

	if (!existe) {
		printf("Palavra nao esta no dicionario, logo nao e possivel remove-la.\n");
		return;
	}

	remover_palavra(raiz, entrada);

	printf("Palavra removida com sucesso.\n");
	aguardar_usuario();
}

/* ==================== INTERFACES ==================== */

int main(void)
{
	NoTrie *raiz = criar_trie();
	int opcao = -1;

	while (true) {
		exibir_menu();
		
		printf("> ");
		scanf("%d", &opcao);

		if (opcao == 0) {
			printf("Encerrando programa...\n");
			break;
		}

		switch (opcao) {
			case 1:
				exibir_todas_palavras_interface(raiz);
				break;
			case 2:
				exibir_palavras_com_prefixo_interface(raiz);
				break;
			case 3:
				existe_palavra_interface(raiz);
				break;
			case 4:
				inserir_palavra_interface(raiz);
				break;
			case 5:
				remover_palavra_interface(&raiz);
				break;
			default:
				printf("Digite uma opcao valida...\n");
				opcao = -1;
				break;
		}
	}

	destruir_trie(raiz);
}
