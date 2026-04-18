#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "sqlite3.h"

/* =========================
   VARIAVEIS GLOBAIS
   ========================= */
sqlite3 *db;

struct Usuario {
    char nome[100];
    char cpf[12];
    char senha[20];
    char nascimento[12];
};

struct Ocorrencia {
    int id;
    char descricao[500];
    char bairro[50];
    char dataHora[20];
    float lat, lon;
};

struct Usuario usuarioLogado;
int idUsuarioLogado = 0;

/* =========================
   PROTOTIPOS
   ========================= */
void menu();
void menuAdm();

/* =========================
   FUNCOES AUXILIARES
   ========================= */
void limpar() {
    system("cls");
}

void pausar() {
    system("pause");
}

void titulo(const char *txt) {
    printf("\n========================\n");
    printf("   %s\n", txt);
    printf("========================\n");
}

void ler(char *txt, int t) {
    int i = 0;
    int c;

    while (i < t - 1) {
        c = getch();

        if (c == 0 || c == 0xE0) {
            getch();
            continue;
        }

        if (c == '\r' || c == '\n') break;

        if (c == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (c >= 32 && c <= 126) {
            txt[i++] = (char)c;
            putchar(c);
        }
    }

    txt[i] = '\0';
    printf("\n");
}

void lerSenha(char *senha, int t) {
    int i = 0;
    int c;

    while (i < t - 1) {
        c = getch();

        if (c == 0 || c == 0xE0) {
            getch();
            continue;
        }

        if (c == '\r' || c == '\n') break;

        if (c == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (c >= 32 && c <= 126) {
            senha[i++] = (char)c;
            printf("*");
        }
    }

    senha[i] = '\0';
    printf("\n");
}

int stringVazia(const char *str) {
    int i;

    if (str == NULL || strlen(str) == 0) return 1;

    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ' && str[i] != '\t') {
            return 0;
        }
    }

    return 1;
}

int validarData(char *data) {
    int d, m, a;

    if (strlen(data) != 10) return 0;
    if (data[2] != '/' || data[5] != '/') return 0;
    if (sscanf(data, "%d/%d/%d", &d, &m, &a) != 3) return 0;
    if (m < 1 || m > 12) return 0;
    if (d < 1 || d > 31) return 0;
    if (a < 1900 || a > 2100) return 0;

    return 1;
}

int validarHora(char *hora) {
    int h, m;

    if (strlen(hora) != 5) return 0;
    if (hora[2] != ':') return 0;
    if (sscanf(hora, "%d:%d", &h, &m) != 2) return 0;
    if (h < 0 || h > 23) return 0;
    if (m < 0 || m > 59) return 0;

    return 1;
}

/* =========================
   BANCO DE DADOS
   ========================= */
int abrirBanco() {
    if (sqlite3_open("database/bancodedados.db", &db) != SQLITE_OK) {
        printf("Erro ao abrir banco: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);
    return 1;
}

void fecharBanco() {
    sqlite3_close(db);
}

/* =========================
   USUARIO
   ========================= */
int cpfExiste(const char *cpf) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT 1 FROM usuario WHERE cpf_usuario = ? LIMIT 1;";
    int existe = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return 0;
    }

    sqlite3_bind_text(stmt, 1, cpf, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        existe = 1;
    }

    sqlite3_finalize(stmt);
    return existe;
}

void cadastrar() {
    struct Usuario u;
    sqlite3_stmt *stmt;
    const char *sql =
        "INSERT INTO usuario (nome_usuario, cpf_usuario, senha, nascimento) "
        "VALUES (?, ?, ?, ?);";

    titulo("CADASTRO");

    do {
        printf("Nome: ");
        ler(u.nome, 100);
        if (stringVazia(u.nome)) {
            printf("Nome nao pode ser vazio.\n");
        }
    } while (stringVazia(u.nome));

    do {
        printf("CPF: ");
        ler(u.cpf, 12);

        if (stringVazia(u.cpf)) {
            printf("CPF nao pode ser vazio.\n");
        } else if (strlen(u.cpf) != 11) {
            printf("CPF deve ter exatamente 11 digitos.\n");
        } else if (cpfExiste(u.cpf)) {
            printf("CPF ja cadastrado.\n");
        }
    } while (stringVazia(u.cpf) || strlen(u.cpf) != 11 || cpfExiste(u.cpf));

    do {
        printf("Senha: ");
        lerSenha(u.senha, 20);
        if (stringVazia(u.senha)) {
            printf("Senha nao pode ser vazia.\n");
        }
    } while (stringVazia(u.senha));

    do {
        printf("Data de nascimento (DD/MM/AAAA): ");
        ler(u.nascimento, 12);
        if (!validarData(u.nascimento)) {
            printf("Data invalida.\n");
        }
    } while (!validarData(u.nascimento));

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar cadastro: %s\n", sqlite3_errmsg(db));
        pausar();
        return;
    }

    sqlite3_bind_text(stmt, 1, u.nome, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, u.cpf, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, u.senha, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, u.nascimento, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Cadastrado com sucesso!\n");
    } else {
        printf("Erro ao cadastrar usuario: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    pausar();
}

int login() {
    char cpf[12], senha[20];
    sqlite3_stmt *stmt;
    const char *sql =
        "SELECT id_usuario, nome_usuario, cpf_usuario, senha, nascimento "
        "FROM usuario WHERE cpf_usuario = ? AND senha = ?;";

    titulo("LOGIN");
    printf("CPF: ");
    ler(cpf, 12);
    printf("Senha: ");
    lerSenha(senha, 20);

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar login.\n");
        pausar();
        return 0;
    }

    sqlite3_bind_text(stmt, 1, cpf, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, senha, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        idUsuarioLogado = sqlite3_column_int(stmt, 0);

        strcpy(usuarioLogado.nome, (const char *)sqlite3_column_text(stmt, 1));
        strcpy(usuarioLogado.cpf, (const char *)sqlite3_column_text(stmt, 2));
        strcpy(usuarioLogado.senha, (const char *)sqlite3_column_text(stmt, 3));

        if (sqlite3_column_text(stmt, 4) != NULL) {
            strcpy(usuarioLogado.nascimento, (const char *)sqlite3_column_text(stmt, 4));
        } else {
            usuarioLogado.nascimento[0] = '\0';
        }

        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

/* =========================
   OCORRENCIA
   ========================= */
void novaOcorrencia() {
    struct Ocorrencia o;
    char dataTemp[15];
    char horaTemp[10];
    sqlite3_stmt *stmt;
    const char *sql =
        "INSERT INTO ocorrencia "
        "(descricao_ocorrencia, bairro_ocorrencia, latitude, longitude, data_hora, fk_usuario) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    titulo("NOVA OCORRENCIA");

    do {
        printf("Descricao: ");
        ler(o.descricao, 500);
        if (stringVazia(o.descricao)) {
            printf("Descricao nao pode ser vazia.\n");
        }
    } while (stringVazia(o.descricao));

    do {
        printf("Bairro: ");
        ler(o.bairro, 50);
        if (stringVazia(o.bairro)) {
            printf("Bairro nao pode ser vazio.\n");
        }
    } while (stringVazia(o.bairro));

    do {
        printf("Data da ocorrencia (DD/MM/AAAA): ");
        ler(dataTemp, 15);
        if (!validarData(dataTemp)) {
            printf("Data invalida.\n");
        }
    } while (!validarData(dataTemp));

    do {
        printf("Hora da ocorrencia (HH:MM): ");
        ler(horaTemp, 10);
        if (!validarHora(horaTemp)) {
            printf("Hora invalida.\n");
        }
    } while (!validarHora(horaTemp));

    sprintf(o.dataHora, "%s %s", dataTemp, horaTemp);

    printf("Latitude: ");
    scanf("%f", &o.lat);

    printf("Longitude: ");
    scanf("%f", &o.lon);
    getchar();

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar insercao: %s\n", sqlite3_errmsg(db));
        pausar();
        return;
    }

    sqlite3_bind_text(stmt, 1, o.descricao, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, o.bairro, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, o.lat);
    sqlite3_bind_double(stmt, 4, o.lon);
    sqlite3_bind_text(stmt, 5, o.dataHora, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, idUsuarioLogado);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("Ocorrencia cadastrada com sucesso!\n");
    } else {
        printf("Erro ao cadastrar ocorrencia: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    pausar();
}

int callbackListar(void *dados, int colunas, char **valores, char **nomesColunas) {
    int i;

    for (i = 0; i < colunas; i++) {
        printf("%s: %s\n", nomesColunas[i], valores[i] ? valores[i] : "NULL");
    }
    printf("------------------------\n");

    return 0;
}

void listar() {
    char *erro = NULL;
    const char *sql =
        "SELECT id_ocorrencia, descricao_ocorrencia, bairro_ocorrencia, data_hora, latitude, longitude "
        "FROM ocorrencia;";

    titulo("OCORRENCIAS");

    if (sqlite3_exec(db, sql, callbackListar, 0, &erro) != SQLITE_OK) {
        printf("Erro ao listar: %s\n", erro);
        sqlite3_free(erro);
    }

    pausar();
}

void buscar() {
    char bairro[50];
    sqlite3_stmt *stmt;
    const char *sql =
        "SELECT id_ocorrencia, data_hora, descricao_ocorrencia "
        "FROM ocorrencia WHERE bairro_ocorrencia = ?;";

    int encontrou = 0;

    titulo("BUSCAR POR BAIRRO");
    printf("Digite o bairro: ");
    ler(bairro, 50);

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar busca.\n");
        pausar();
        return;
    }

    sqlite3_bind_text(stmt, 1, bairro, -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("ID: %d | %s | %s\n",
               sqlite3_column_int(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_text(stmt, 2));
        encontrou = 1;
    }

    if (!encontrou) {
        printf("Nenhuma ocorrencia encontrada nesse bairro.\n");
    }

    sqlite3_finalize(stmt);
    pausar();
}

void deletar() {
    int id;
    sqlite3_stmt *stmt;
    const char *sql =
        "DELETE FROM ocorrencia WHERE id_ocorrencia = ? AND fk_usuario = ?;";

    titulo("DELETAR OCORRENCIA");
    printf("ID da ocorrencia: ");
    scanf("%d", &id);
    getchar();

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar exclusao.\n");
        pausar();
        return;
    }

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_int(stmt, 2, idUsuarioLogado);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        if (sqlite3_changes(db) > 0) {
            printf("Ocorrencia deletada com sucesso!\n");
        } else {
            printf("ID nao encontrado ou voce nao tem permissao.\n");
        }
    } else {
        printf("Erro ao deletar ocorrencia.\n");
    }

    sqlite3_finalize(stmt);
    pausar();
}

/* =========================
   SOS
   ========================= */
void sos() {
    int op;

    titulo("SOS - EMERGENCIA");
    printf("1 - Policia   (190)\n");
    printf("2 - SAMU      (192)\n");
    printf("3 - Bombeiros (193)\n");
    printf("0 - Voltar\n");
    printf("Opcao: ");
    scanf("%d", &op);
    getchar();

    if (op == 1) printf("Chamando Policia - 190\n");
    else if (op == 2) printf("Chamando SAMU - 192\n");
    else if (op == 3) printf("Chamando Bombeiros - 193\n");
    else if (op == 0) return;
    else printf("Opcao invalida!\n");

    pausar();
}

/* =========================
   MAPA
   ========================= */
#define LINHAS 3
#define COLUNAS 5

void exibirMapa() {
    char mapa[LINHAS][COLUNAS];
    int i, j, linha, coluna;
    char opcao;

    const char *nomesColunas[COLUNAS] = {
        "0- C.BoaVista",
        "1- Madalena",
        "2- Gracas",
        "3- Varzea",
        "4- Curado"
    };

    for (i = 0; i < LINHAS; i++) {
        for (j = 0; j < COLUNAS; j++) {
            mapa[i][j] = '.';
        }
    }

    do {
        printf("\nMapa atual:\n\n     ");
        for (j = 0; j < COLUNAS; j++) {
            printf("%-15s", nomesColunas[j]);
        }
        printf("\n");

        for (i = 0; i < LINHAS; i++) {
            printf("L%d   ", i);
            for (j = 0; j < COLUNAS; j++) {
                printf("%-15c", mapa[i][j]);
            }
            printf("\n");
        }

        printf("\nEscolha a linha (0 a 2): ");
        scanf("%d", &linha);
        printf("Escolha a coluna (0 a 4): ");
        scanf("%d", &coluna);

        if (linha >= 0 && linha < LINHAS && coluna >= 0 && coluna < COLUNAS) {
            mapa[linha][coluna] = 'X';
        } else {
            printf("Posicao invalida!\n");
        }

        printf("\nDeseja fazer outra marcacao? (s/n): ");
        scanf(" %c", &opcao);

    } while (opcao == 's' || opcao == 'S');

    printf("\nMapa FINAL:\n\n     ");
    for (j = 0; j < COLUNAS; j++) {
        printf("%-15s", nomesColunas[j]);
    }
    printf("\n");

    for (i = 0; i < LINHAS; i++) {
        printf("L%d   ", i);
        for (j = 0; j < COLUNAS; j++) {
            printf("%-15c", mapa[i][j]);
        }
        printf("\n");
    }

    printf("\nPressione ENTER para voltar...");
    getchar();
    getchar();
}

/* =========================
   ADMINISTRADOR
   ========================= */
void admListarUsuarios() {
    char *erro = NULL;
    const char *sql =
        "SELECT id_usuario, nome_usuario, cpf_usuario, nascimento FROM usuario;";

    titulo("USUARIOS CADASTRADOS");

    if (sqlite3_exec(db, sql, callbackListar, 0, &erro) != SQLITE_OK) {
        printf("Erro ao listar usuarios: %s\n", erro);
        sqlite3_free(erro);
    }

    pausar();
}

void admDeletarUsuario() {
    int id;
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM usuario WHERE id_usuario = ?;";

    titulo("DELETAR USUARIO");
    printf("ID do usuario a deletar: ");
    scanf("%d", &id);
    getchar();

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar exclusao.\n");
        pausar();
        return;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        if (sqlite3_changes(db) > 0) {
            printf("Usuario deletado com sucesso!\n");
        } else {
            printf("Usuario nao encontrado.\n");
        }
    } else {
        printf("Erro ao deletar usuario: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    pausar();
}

int loginAdm() {
    char email[90];
    char senha[20];
    sqlite3_stmt *stmt;
    const char *sql =
        "SELECT id_admin, nome_admin FROM administrador "
        "WHERE email_admin = ? AND senha_admin = ?;";

    titulo("LOGIN ADM");
    printf("Email ADM: ");
    ler(email, 90);
    printf("Senha ADM: ");
    lerSenha(senha, 20);

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Erro ao preparar login ADM.\n");
        pausar();
        return 0;
    }

    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, senha, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

/* =========================
   MENUS
   ========================= */
void menu() {
    int op;

    while (1) {
        limpar();
        printf("|==========================================================|\n");
        printf("|                        MENU                              |\n");
        printf("|                  Usuario: %-30s |\n", usuarioLogado.nome);
        printf("|==========================================================|\n");
        printf("|                  1 - Nova Ocorrencia                     |\n");
        printf("|                  2 - SOS                                 |\n");
        printf("|                  3 - Ver Ocorrencias                     |\n");
        printf("|                  4 - Buscar por Bairro                   |\n");
        printf("|                  5 - Deletar Ocorrencia                  |\n");
        printf("|                  6 - Mapa de Ocorrencia                  |\n");
        printf("|                  0 - Sair                                |\n");
        printf("|==========================================================|\n");
        printf("Opcao: ");
        scanf("%d", &op);
        getchar();

        switch (op) {
            case 1:
                novaOcorrencia();
                break;
            case 2:
                sos();
                break;
            case 3:
                listar();
                break;
            case 4:
                buscar();
                break;
            case 5:
                deletar();
                break;
            case 6:
                exibirMapa();
                break;
            case 0:
                return;
            default:
                printf("Opcao invalida.\n");
                pausar();
        }
    }
}

void menuAdm() {
    int op;

    while (1) {
        limpar();
        printf("|==========================================================|\n");
        printf("|                  MENU ADMINISTRADOR                      |\n");
        printf("|==========================================================|\n");
        printf("|                  1 - Listar Usuarios                     |\n");
        printf("|                  2 - Deletar Usuario                     |\n");
        printf("|                  3 - Ver Ocorrencias                     |\n");
        printf("|                  0 - Sair                                |\n");
        printf("|==========================================================|\n");
        printf("Opcao: ");
        scanf("%d", &op);
        getchar();

        switch (op) {
            case 1:
                admListarUsuarios();
                break;
            case 2:
                admDeletarUsuario();
                break;
            case 3:
                listar();
                break;
            case 0:
                return;
            default:
                printf("Opcao invalida.\n");
                pausar();
        }
    }
}

/* =========================
   MAIN
   ========================= */
int main() {
    int op;

    if (!abrirBanco()) {
        return 1;
    }

    while (1) {
        limpar();
        printf("|==========================================================|\n");
        printf("|               SEJA BEM VINDO AO FAROL                    |\n");
        printf("|==========================================================|\n");
        printf("|                  1 - Login Usuario                       |\n");
        printf("|                  2 - Cadastro                            |\n");
        printf("|                  3 - Login ADM                           |\n");
        printf("|                  0 - Sair                                |\n");
        printf("|==========================================================|\n");
        printf("Opcao: ");
        scanf("%d", &op);
        getchar();

        switch (op) {
            case 1:
                if (login()) {
                    menu();
                } else {
                    printf("CPF ou senha incorretos!\n");
                    pausar();
                }
                break;

            case 2:
                cadastrar();
                break;

            case 3:
                if (loginAdm()) {
                    menuAdm();
                } else {
                    printf("Email ou senha ADM incorretos!\n");
                    pausar();
                }
                break;

            case 0:
                fecharBanco();
                printf("Saindo...\n");
                return 0;

            default:
                printf("Opcao invalida.\n");
                pausar();
        }
    }

    fecharBanco();
    return 0;
}