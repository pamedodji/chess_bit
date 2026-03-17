#include "../header.h"

move parse_move(board *b, const char *str){
    int src_col = str[0] - 'a';
    int src_row = str[1] - '1';
    int dst_col = str[2] - 'a';
    int dst_row = str[3] - '1';
    u32 src_idx = src_row * 8 + src_col;
    u32 dst_idx = dst_row * 8 + dst_col;

    list_move l;
    legal_moves(b, &l);

    char prom = str[4];
    for (int i = 0; i < l.size; i++){
        u32 m_src = get_m_int(l.m[i], 0);
        u32 m_dst = get_m_int(l.m[i], 1);
        u32 m_flag = get_m_int(l.m[i], 3);
        if (m_src != src_idx || m_dst != dst_idx)
            continue;
        if (prom == '\0' || prom == ' ' || prom == '\n'){
            if (m_flag >= QUEEN && m_flag <= KNIGHT)
                continue;
            return l.m[i];
        }
        u32 wanted;
        if (prom == 'q')
            wanted = QUEEN;
        else if (prom == 'r')
            wanted = ROOK;
        else if (prom == 'b')
            wanted = BISHOP;
        else
            wanted = KNIGHT;
        if (m_flag == wanted)
            return l.m[i];
    }
    return 0;
}

void move_to_str(move m, char *out){
    u32 src = get_m_int(m, 0);
    u32 dst = get_m_int(m, 1);
    u32 flag = get_m_int(m, 3);
    out[0] = 'a' + (src % 8);
    out[1] = '1' + (src / 8);
    out[2] = 'a' + (dst % 8);
    out[3] = '1' + (dst / 8);
    out[4] = '\0';
    if (flag == QUEEN){
        out[4] = 'q'; 
        out[5] = '\0'; 
    }
    else if (flag == ROOK){
        out[4] = 'r'; out[5] = '\0';
    }
    else if (flag == BISHOP){
        out[4] = 'b'; out[5] = '\0';
    }
    else if (flag == KNIGHT){
        out[4] = 'n'; out[5] = '\0';
    }
}

void handle_position(board *b, rep_struct *rep, const char *line){
    const char *p = line + 9;
    if (strncmp(p, "startpos", 8) == 0){
        init_board(b, rep);
        p += 8;
    }
    else if (strncmp(p, "fen", 3) == 0){
        p += 4;
        char fen[128];
        const char *moves_ptr = strstr(p, " moves");
        if (moves_ptr){
            int len = moves_ptr - p;
            strncpy(fen, p, len);
            fen[len] = '\0';
            p = moves_ptr;
        }
        else{
            strncpy(fen, p, 127);
            fen[127] = '\0';
            p = p + strlen(p);
        }
        fen_to_board(b, rep, fen);
    }
    p = strstr(p, "moves");
    if (!p)
        return;
    p += 5;
    while (*p == ' ')
        p++;
    while (*p != '\0'){
        move m = parse_move(b, p);
        if (m != 0){
            unmake_info info;
            make_move(b, m, 1, &info);
        }
        while (*p != ' ' && *p != '\0')
            p++;
        while (*p == ' ')
            p++;
    }
}

void handle_go(board *b, int nb_threads, const char *line) {
    long wtime    = -1, btime = -1;
    long winc     =  0, binc  = 0;
    int  movestogo = 0;
    long movetime  = -1;
    int  depth     = -1;

    char buf[512];
    strncpy(buf, line, sizeof(buf)-1);
    char *tok = strtok(buf, " ");
    while (tok) {
        if      (!strcmp(tok, "wtime"))     { tok = strtok(NULL," "); if(tok) wtime     = atol(tok); }
        else if (!strcmp(tok, "btime"))     { tok = strtok(NULL," "); if(tok) btime     = atol(tok); }
        else if (!strcmp(tok, "winc"))      { tok = strtok(NULL," "); if(tok) winc      = atol(tok); }
        else if (!strcmp(tok, "binc"))      { tok = strtok(NULL," "); if(tok) binc      = atol(tok); }
        else if (!strcmp(tok, "movestogo")){ tok = strtok(NULL," "); if(tok) movestogo = atoi(tok); }
        else if (!strcmp(tok, "movetime")) { tok = strtok(NULL," "); if(tok) movetime  = atol(tok); }
        else if (!strcmp(tok, "depth"))    { tok = strtok(NULL," "); if(tok) depth     = atoi(tok); }
        tok = strtok(NULL, " ");
    }

    time_control tc;
    tc.wtime     = wtime;
    tc.btime     = btime;
    tc.winc      = winc;
    tc.binc      = binc;
    tc.movestogo = movestogo;
    tc.movetime  = movetime;
    tc.depth     = depth;

    move m = 0; /* Replace with yout best_move function*/

    if (m == 0) {
        printf("bestmove 0000\n");
        fflush(stdout);
        return;
    }
    char str[6];
    move_to_str(m, str);
    printf("bestmove %s\n", str);
    fflush(stdout);
}
void play_game(int nb_threads){
    board b;
    rep_struct rep;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)){
        if (strncmp(line, "uci", 3) == 0){
            printf("id name test\n");
            printf("id author X\n");
            printf("uciok\n");
            fflush(stdout);
        }
        else if (strncmp(line, "isready", 7) == 0){
            printf("readyok\n");
            fflush(stdout);
        }
        else if (strncmp(line, "ucinewgame", 10) == 0){
            init_board(&b, &rep);
        }
        else if (strncmp(line, "position", 8) == 0){
            handle_position(&b, &rep, line);
        }
        else if (strncmp(line, "go", 2) == 0){
            handle_go(&b, nb_threads, line);
        }
        else if (strncmp(line, "quit", 4) == 0){
            break;
        }
    }
}