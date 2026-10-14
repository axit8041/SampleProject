#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEFAULT_PORT 8080
#define BOARD_SIZE 9
#define BACKLOG 10

static const char INDEX_HTML[] =
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "<meta charset=\"UTF-8\">\n"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "<title>Tic-Tac-Toe</title>\n"
    "<style>\n"
    "body { font-family: Arial, sans-serif; display: flex; flex-direction: column; align-items: center; margin-top: 40px; }\n"
    "h1 { margin-bottom: 10px; }\n"
    "#status { font-size: 1.2em; margin-bottom: 15px; min-height: 1.5em; }\n"
    "#board { display: grid; grid-template-columns: repeat(3, 100px); grid-gap: 5px; }\n"
    ".cell { width: 100px; height: 100px; font-size: 2.5em; display: flex; align-items: center; justify-content: center; background: #f0f0f0; border: 1px solid #ccc; cursor: pointer; }\n"
    ".cell:hover { background: #e0e0e0; }\n"
    "#reset { margin-top: 20px; padding: 10px 20px; font-size: 1em; cursor: pointer; }\n"
    "</style>\n"
    "</head>\n"
    "<body>\n"
    "<h1>Tic-Tac-Toe</h1>\n"
    "<div id=\"status\">Loading...</div>\n"
    "<div id=\"board\"></div>\n"
    "<button id=\"reset\">New Game</button>\n"
    "<script>\n"
    "const boardEl = document.getElementById('board');\n"
    "const statusEl = document.getElementById('status');\n"
    "const resetBtn = document.getElementById('reset');\n"
    "let player = null;\n"
    "function render(state) {\n"
    "  boardEl.innerHTML = '';\n"
    "  for (let i = 0; i < 9; i++) {\n"
    "    const cell = document.createElement('div');\n"
    "    cell.className = 'cell';\n"
    "    cell.textContent = state.board[i] === '-' ? '' : state.board[i];\n"
    "    cell.onclick = () => move(i);\n"
    "    boardEl.appendChild(cell);\n"
    "  }\n"
    "  if (!player) player = state.turn;\n"
    "  if (state.winner) {\n"
    "    statusEl.textContent = 'Winner: ' + state.winner;\n"
    "  } else if (state.draw) {\n"
    "    statusEl.textContent = 'Draw!';\n"
    "  } else {\n"
    "    statusEl.textContent = 'Turn: ' + state.turn + ' (you are ' + player + ')';\n"
    "  }\n"
    "}\n"
    "async function fetchState() {\n"
    "  const res = await fetch('/state');\n"
    "  const state = await res.json();\n"
    "  render(state);\n"
    "}\n"
    "async function move(cell) {\n"
    "  await fetch('/move?player=' + player + '&cell=' + cell, {method: 'POST'});\n"
    "  await fetchState();\n"
    "}\n"
    "async function resetGame() {\n"
    "  await fetch('/reset', {method: 'POST'});\n"
    "  player = null;\n"
    "  await fetchState();\n"
    "}\n"
    "resetBtn.onclick = resetGame;\n"
    "setInterval(fetchState, 500);\n"
    "fetchState();\n"
    "</script>\n"
    "</body>\n"
    "</html>\n";

static pthread_mutex_t game_lock = PTHREAD_MUTEX_INITIALIZER;
static char board[BOARD_SIZE];
static char turn;
static char winner;
static int draw;
static volatile int running = 1;

static void init_game(void) {
    pthread_mutex_lock(&game_lock);
    for (int i = 0; i < BOARD_SIZE; i++) {
        board[i] = '-';
    }
    turn = 'X';
    winner = '\0';
    draw = 0;
    pthread_mutex_unlock(&game_lock);
}

static int check_winner(void) {
    const int wins[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
        {0, 4, 8}, {2, 4, 6}
    };
    for (int i = 0; i < 8; i++) {
        char c = board[wins[i][0]];
        if (c != '-' && c == board[wins[i][1]] && c == board[wins[i][2]]) {
            return c;
        }
    }
    return 0;
}

static int is_draw(void) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i] == '-') {
            return 0;
        }
    }
    return 1;
}

static void send_response(int client_fd, int status, const char *status_text,
                          const char *content_type, const char *body, size_t body_len) {
    char header[512];
    int header_len = snprintf(header, sizeof(header),
                              "HTTP/1.1 %d %s\r\n"
                              "Content-Type: %s\r\n"
                              "Content-Length: %zu\r\n"
                              "Connection: close\r\n"
                              "\r\n",
                              status, status_text, content_type, body_len);
    if (header_len > 0 && (size_t)header_len < sizeof(header)) {
        (void)send(client_fd, header, (size_t)header_len, 0);
    }
    if (body_len > 0) {
        (void)send(client_fd, body, body_len, 0);
    }
}

static void send_json(int client_fd, const char *json) {
    send_response(client_fd, 200, "OK", "application/json", json, strlen(json));
}

static void send_state(int client_fd) {
    char json[256];
    pthread_mutex_lock(&game_lock);
    snprintf(json, sizeof(json),
             "{\"board\":[\"%c\",\"%c\",\"%c\",\"%c\",\"%c\",\"%c\",\"%c\",\"%c\",\"%c\"],"
             "\"turn\":\"%c\",\"winner\":\"%s\",\"draw\":%s}",
             board[0], board[1], board[2], board[3], board[4], board[5],
             board[6], board[7], board[8], turn,
             winner ? &winner : "",
             draw ? "true" : "false");
    pthread_mutex_unlock(&game_lock);
    send_json(client_fd, json);
}

static int parse_query(const char *query, char *player, int *cell) {
    char p[8] = {0};
    int c = -1;
    char qbuf[256];
    if (strlen(query) >= sizeof(qbuf)) {
        return 0;
    }
    strncpy(qbuf, query, sizeof(qbuf) - 1);
    qbuf[sizeof(qbuf) - 1] = '\0';

    char *token = strtok(qbuf, "&");
    while (token) {
        char *eq = strchr(token, '=');
        if (!eq) {
            token = strtok(NULL, "&");
            continue;
        }
        *eq = '\0';
        const char *key = token;
        const char *value = eq + 1;
        if (strcmp(key, "player") == 0) {
            strncpy(p, value, sizeof(p) - 1);
        } else if (strcmp(key, "cell") == 0) {
            c = atoi(value);
        }
        token = strtok(NULL, "&");
    }
    if ((p[0] != 'X' && p[0] != 'O') || p[1] != '\0' || c < 0 || c >= BOARD_SIZE) {
        return 0;
    }
    *player = p[0];
    *cell = c;
    return 1;
}

static void handle_move(int client_fd, const char *query) {
    char player;
    int cell;
    if (!parse_query(query, &player, &cell)) {
        const char *err = "{\"error\":\"invalid parameters\"}";
        send_response(client_fd, 400, "Bad Request", "application/json",
                      err, strlen(err));
        return;
    }

    pthread_mutex_lock(&game_lock);
    const char *result;
    if (winner) {
        result = "game over";
    } else if (board[cell] != '-') {
        result = "occupied";
    } else if (player != turn) {
        result = "not your turn";
    } else {
        board[cell] = player;
        int w = check_winner();
        if (w) {
            winner = (char)w;
            result = "win";
        } else if (is_draw()) {
            draw = 1;
            result = "draw";
        } else {
            turn = (turn == 'X') ? 'O' : 'X';
            result = "ok";
        }
    }
    pthread_mutex_unlock(&game_lock);

    char json[128];
    snprintf(json, sizeof(json), "{\"result\":\"%s\"}", result);
    send_json(client_fd, json);
}

static void handle_request(int client_fd, const char *request) {
    char method[16];
    char path[256];
    char query[256] = {0};
    if (sscanf(request, "%15s %255s", method, path) != 2) {
        const char *bad = "Bad Request";
        send_response(client_fd, 400, "Bad Request", "text/plain", bad, strlen(bad));
        return;
    }

    char *q = strchr(path, '?');
    if (q) {
        *q = '\0';
        strncpy(query, q + 1, sizeof(query) - 1);
        query[sizeof(query) - 1] = '\0';
    }

    if (strcmp(method, "GET") == 0 && strcmp(path, "/") == 0) {
        send_response(client_fd, 200, "OK", "text/html", INDEX_HTML, sizeof(INDEX_HTML) - 1);
    } else if (strcmp(method, "GET") == 0 && strcmp(path, "/state") == 0) {
        send_state(client_fd);
    } else if (strcmp(method, "POST") == 0 && strcmp(path, "/move") == 0) {
        handle_move(client_fd, query);
    } else if (strcmp(method, "POST") == 0 && strcmp(path, "/reset") == 0) {
        init_game();
        send_json(client_fd, "{\"result\":\"reset\"}");
    } else {
        const char *notfound = "Not Found";
        send_response(client_fd, 404, "Not Found", "text/plain", notfound, strlen(notfound));
    }
}

static void *client_thread(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    char buffer[4096];
    ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        handle_request(client_fd, buffer);
    }
    close(client_fd);
    return NULL;
}

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    if (argc > 1) {
        int p = atoi(argv[1]);
        if (p > 0 && p <= 65535) {
            port = p;
        }
    }

    init_game();
    signal(SIGPIPE, SIG_IGN);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((uint16_t)port);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return EXIT_FAILURE;
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("Tic-Tac-Toe server listening on http://0.0.0.0:%d\n", port);
    fflush(stdout);

    while (running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            break;
        }

        int *fd_arg = malloc(sizeof(int));
        if (!fd_arg) {
            close(client_fd);
            continue;
        }
        *fd_arg = client_fd;

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_thread, fd_arg) != 0) {
            free(fd_arg);
            close(client_fd);
            continue;
        }
        pthread_detach(tid);
    }

    close(server_fd);
    return EXIT_SUCCESS;
}
