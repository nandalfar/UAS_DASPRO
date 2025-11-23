#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    int x ;
    int y ;
} koordinat ;

typedef struct 
{
    char username[25] ;
    int bestScore ;
} player ;

void initSnake() ;
bool cek1(koordinat a, koordinat b) ;
void spawn_apel() ;
void input() ;
void update() ;
void draw() ;
void gameOver() ;
void endGame() ;
void menuUtama() ;
void delWin(WINDOW * tmp) ;

int MaxHeight = 25, MaxWidth = 80 ;
int MinHeight = 35, MinWidth = 80 ;
int heightT, widthT ;
int startX, startY = 0 ;
int score ;
int pBadan ;
int iPlayer = -1 ;
bool lanjut = true ;
bool login = false ;
koordinat kepala, apel, gerak, badan[1000] ;
player playerNow ;
WINDOW * snakeWin ;

int main() {
    setlocale(LC_ALL, "");
    initscr() ;
    cbreak() ;
    noecho() ;
    
    curs_set(0) ;

    menuUtama() ;
    // getch() ;
    return 0;
}

void initSnake() {
    lanjut = true ;
    pBadan = 0 ;
    score = 0 ;
    snakeWin = newwin(MaxHeight, MaxWidth, startY, startX) ;
    box(snakeWin, 0, 0) ;
    refresh() ;
    wrefresh(snakeWin) ;
    keypad(snakeWin, true) ;
    nodelay(snakeWin, true) ;
    srand(time(NULL)) ;
    
    kepala.x = startX + 1, kepala.y = startY + 1 ;
    gerak.x = 1 ; gerak.y = 0 ;
    spawn_apel() ;
    
    mvwaddch(snakeWin, kepala.y, kepala.x, '>') ;
    mvwaddch(snakeWin, apel.y, apel.x, '@') ;
    wrefresh(snakeWin) ;
}

void endGame() {
    curs_set(1) ;
    endwin();
}

bool cek1 (koordinat a, koordinat b) {
    if(a.x != b.x) return false ;
    if(a.y != b.y) return false ;
    
    return true ;
}

bool cek_mati() {
    for(int i=0; i<pBadan; i++) {
        if(kepala.x == badan[i].x && kepala.y == badan[i].y) {
            lanjut = false ;
            return true ;
        }
    }

    if(kepala.x == 0 || kepala.x == MaxWidth-1 || kepala.y == 0 || kepala.y == MaxHeight-1){
        lanjut = false ;
        return true ;
    }
    return false ;
}

void cariUSN (char *targetName) {
    FILE *file = fopen("dataPlayer.dat", "rb+");
    if (!file) return;

    player temp;
    int index = 0 ;
    // Loop setiap data
    while (fread(&temp, sizeof(player), 1, file) == 1) {

        if (strcmp(temp.username, targetName) == 0) {
            iPlayer = index ; 
            break;
        }
        index++ ;
    }

    fclose(file);
}

void saveNPlayer() {
    FILE *file = fopen("dataPlayer.dat", "ab");  // append binary
    if (!file) {
        perror("Gagal membuka file");
        return;
    }

    fwrite(&playerNow, sizeof(player), 1, file);
    fclose(file);
}

void saveNScore() {
    FILE *file = fopen("dataPlayer.dat", "rb+");  // read + write
    if (!file) {
        perror("Gagal membuka file");
        return;
    }

    // Pindah ke posisi index
    fseek(file, iPlayer * sizeof(player), SEEK_SET);

    // Tulis data baru di posisi itu
    fwrite(&playerNow, sizeof(pBadan), 1, file);

    fclose(file);
}

void spawn_apel() {
    apel.x = 1 + rand() % (MaxWidth - 2) ; 
    apel.y = 1 + rand() % (MaxHeight - 2) ;
    while(cek1(apel, kepala)) {
        apel.x = 1 + rand() % (MaxWidth - 2); 
        apel.y = 1 + rand() % (MaxHeight - 2) ;
    }
}

void input() {
    int x = wgetch(snakeWin) ;
    
    switch (x)
    {
        case KEY_UP:
        if(gerak.y == 1) return ;
        gerak.x = 0 ;
        gerak.y = -1 ;
        break;
        
        case KEY_DOWN:
        if(gerak.y == -1) return ;
        gerak.x = 0 ;
        gerak.y = 1 ;
        break;
        
        case KEY_RIGHT:
        if(gerak.x == -1) return ;
        gerak.x = 1 ;
        gerak.y = 0 ;
        break;
        
        case KEY_LEFT:
        if(gerak.x == 1) return ;
        gerak.x = -1 ;
        gerak.y = 0 ;
        break;
        
        case '\e':
        endGame() ;
        exit(0) ;
        lanjut = false ;
        break;
        
        default:
        break;
    }
}

void update() {
    if(cek1 (apel, kepala)) {
        spawn_apel() ;
        score++ ;
        if(playerNow.bestScore < score) playerNow.bestScore = score ;
    }
    
    if(cek_mati()){
        gameOver() ;
        return ;
    }
    
    mvwaddch(snakeWin, kepala.y, kepala.x, ' ') ;
    if(pBadan > 0) mvwaddch(snakeWin, badan[pBadan-1].y, badan[pBadan-1].x, ' ') ;
    wrefresh(snakeWin) ;
    
    if(pBadan > 0) {
        for (int i=pBadan-1; i>0; i--) {
            badan [i] = badan [i-1] ;
        }
        badan[0] = kepala ;
    }
    if(score/1 > pBadan) {
        if(pBadan == 0) {
            badan [pBadan].y = kepala.y - gerak.y ;
            badan [pBadan].x = kepala.x - gerak.x ;
        }else {
            badan [pBadan].y = badan[pBadan-1].y - gerak.y ;
            badan [pBadan].x = badan[pBadan-1].x - gerak.x ;
        }
        pBadan++ ;
    }
    kepala.x += gerak.x ;
    kepala.y += gerak.y ;
}

void loginMessage() {
    int mid = (widthT - 36 - 1 ) / 2  ;
    WINDOW * logM = newwin(3, 36, 14, mid) ;
    wattron(logM, A_REVERSE) ;
    box(logM, 0, 0) ;
    wattroff(logM, A_REVERSE) ;
    mvwprintw(logM, 1, 1, "ANDA HARUS LOGIN TERLEBIH DAHULU!!") ;
    refresh() ;
    wrefresh(logM) ;
    napms(3000) ;
    delWin(logM) ;  
}

void inputUSN(char *usn, WINDOW * logP) {
    int index = 0 ;
    
    while (1) {
        int ch = wgetch(logP);
        
        // ENTER ditekan → selesai
        if (ch == '\n' || ch == KEY_ENTER) {
            break;
        }
        
        // BACKSPACE ditekan
        if ((ch == KEY_BACKSPACE || ch == 127 || ch == 8) && index > 0) {
            index--;
            usn[index] = '\0';
            
            // hapus karakter terakhir secara visual
            mvwprintw(logP, 2, 2 + index, " ");
            wmove(logP, 2, 2 + index);
            
            wrefresh(logP);
            continue;
        }
        
        // batasi panjang input
        if (index >= 25)
        continue;
        
        // hanya izinkan huruf/angka/spasi (opsional)
        if (isprint(ch)) {
            usn[index] = ch;
            index++ ;
            mvwprintw(logP, 2, 2 + index - 1, "%c", ch);
            wrefresh(logP);
        }
    }
}

bool konfirmNP() {
    int mid = (widthT - 50 - 1 ) / 2 ;
    WINDOW * newP = newwin(4, 50, 15, mid) ;
    keypad(newP, true) ;
    box(newP, 0, 0) ;
    mvwprintw(newP, 0, 2, " USN anda tidak ada, ingin membuat USN baru ? ") ;
    refresh() ;

    char *yn[] = {
        "YA",
        "TIDAK"
    };

    int highlight = 0 ;
    bool enter = false;
    
    while(!enter) {
        int x = 13 ;
        for(int i=0; i<2; i++) {
            if(i==highlight) {
                wattron(newP, A_REVERSE) ;
                mvwprintw(newP, 2, x, yn[i]) ;
                wattroff(newP, A_REVERSE) ;
            }else{
                mvwprintw(newP, 2, x, yn[i]) ;
            }
            x = 30 ;
        }
        wrefresh(newP) ;
    
        int p = wgetch(newP) ;
        switch (p)
        {
        case KEY_LEFT:
            if(highlight==0) continue ;
            highlight-- ;
            break;
        
        case KEY_RIGHT:
        if(highlight == 1) continue ;
            highlight++ ;
            break;
        
        case '\n':
            enter = true ;
            break;
        
        default:
            break;
        }
    }
    
    napms(500) ;
    delWin(newP) ;
    if(highlight == 0) return true ;
    else return false ;
}

void loginProses() {
    int mid = (widthT - 30 - 1 ) / 2  ;
    WINDOW * logP = newwin(5, 30, 15, mid) ;
    box(logP, 0, 0) ;
    mvwprintw(logP, 0, 2, "Masukkan USN (MAX 25)") ;
    mvwprintw(logP, 2, 1, "> ");
    refresh() ;
    wrefresh(logP) ;
    
    char usn[30] ;
    memset(usn, 0, sizeof(usn)) ;
    inputUSN(usn, logP) ;
    cariUSN(usn) ;

    delWin(logP) ;
    if(iPlayer == -1) {

        if(!konfirmNP()){
            return ;   
        }
    }

    login = true ;
    strcpy(playerNow.username, usn);
    napms(700) ;
}

void delWin(WINDOW * tmp) {
    wclear(tmp) ;
    wrefresh(tmp) ;
    delwin(tmp) ;
    refresh() ;
}

void draw() {
    if(gerak.x == 1) {
        mvwaddch(snakeWin, kepala.y, kepala.x, '>') ;
    }else if (gerak.x == -1) {
        mvwaddch(snakeWin, kepala.y, kepala.x, '<') ;
    } else if (gerak.y == 1) {
        mvwaddch(snakeWin, kepala.y, kepala.x, 'v') ;
    } else {
        mvwaddch(snakeWin, kepala.y, kepala.x, '^') ;
    }
    
    for(int i=0; i<pBadan; i++) {
        mvwaddch(snakeWin, badan[i].y, badan[i].x, 'O') ;
    }
    mvwaddch(snakeWin, apel.y, apel.x, '@') ;
    box(snakeWin, 0, 0) ;
    mvwprintw(snakeWin, 0, (MaxWidth/2)-(5 + (score/10) ), " Score: %d ", score) ;
    wrefresh(snakeWin) ;
}

void gameOver() {
    // deklarasi string ASCII yang akan dicetak
    
    const char *text_gameOver[] = {
        "░██████╗░░█████╗░███╗░░░███╗███████╗  ░█████╗░██╗░░░██╗███████╗██████╗░",
        "██╔════╝░██╔══██╗████╗░████║██╔════╝  ██╔══██╗██║░░░██║██╔════╝██╔══██╗",
        "██║░░██╗░███████║██╔████╔██║█████╗░░  ██║░░██║╚██╗░██╔╝█████╗░░██████╔╝",
        "██║░░╚██╗██╔══██║██║╚██╔╝██║██╔══╝░░  ██║░░██║░╚████╔╝░██╔══╝░░██╔══██╗",
        "╚██████╔╝██║░░██║██║░╚═╝░██║███████╗  ╚█████╔╝░░╚██╔╝░░███████╗██║░░██║",
        "░╚═════╝░╚═╝░░╚═╝╚═╝░░░░░╚═╝╚══════╝  ░╚════╝░░░░╚═╝░░░╚══════╝╚═╝░░╚═╝"
    };
    
    WINDOW * temp ;
    
    temp = newwin(10, 73, startY+8, startX+3) ;
    
    for(int i=5; i>0; i--) {  // for loop hitung mundur
        box(temp, 0, 0) ;
        mvwprintw(temp, 9, (72-48)/2, " Anda akan kembali ke menu utama dalam %d detik ", i) ;
        
        for (int j=0; j<6; j++) {   // for loop cetak UI game over
            mvwprintw(temp, j+2, 1, "%s", text_gameOver[j]) ;
        }
        refresh() ;
        wrefresh(temp) ;
        
        // flickering
        napms(750) ;
        for (int j=1; j<=8; j++) {
            for(int k=1; k<=71; k++) {
                mvwaddch(temp, j, k, ' ') ;
            }
        }
        wrefresh(temp) ;
        napms(250) ;
    }
    delWin(temp) ;
}

void titleUI(int WithM, WINDOW * titleWin) {

    // buat title
    const char *title[] = {
        "▄▄▀█▄───▄───────▄",
        "▀▀▀██──███─────███",
        "░▄██▀░█████░░░█████░░",
        "███▀▄███░███░███░███░▄",
        "▀█████▀░░░▀███▀░░░▀██▀"
    };
    
    refresh() ;
    
    for(int i=0; i<5; i++) {
        mvwprintw(titleWin, i+1, 1, "%s", title[i]) ;
    }
    wrefresh(titleWin) ;
}

int pilihanMenu(int WithM, WINDOW * mainMenu) {
    // pilihan
    char *choices [] = {
        "New Game",
        "Leaderboard",
        "Quit",
        "Login",
        "switch accounts"
    };
    
    int highlight = 0 ;
    int enter = false ;
    int m = (WithM-2-17)/2 + startX ;
    WINDOW * pilihan = newwin(9, 17, 15, m) ;
    keypad(pilihan, true) ;
    refresh() ;
    
    if(login) mvwprintw(mainMenu, 0, 3, " %s ", playerNow.username) ;
    wrefresh(mainMenu) ;
    
    while(!enter) {
        for(int i=0; i<5; i++) {\
            if(!login && i==4)continue ;
            
            if(i==highlight) {
                wattron(pilihan, A_REVERSE) ;
                mvwprintw(pilihan, i+1+i, (15-strlen(choices[i])+1) / 2 + 1, "%s", choices[i]) ;
                wattroff(pilihan, A_REVERSE) ;
            } else {
                mvwprintw(pilihan, i+1+i, (15-strlen(choices[i])+1) / 2 + 1, "%s", choices[i]) ;
            }
        }
        wrefresh(pilihan) ;
        
        int g = wgetch(pilihan) ;
        
        switch (g)
        {
            case KEY_UP:
            highlight-- ;
            break;
            case KEY_DOWN:
            highlight++ ;
            break;
            case '\n':
            enter = true ;
            break ;
            default:
            break;
        }
        highlight%=4 ;
        if(highlight<0)highlight = 3 ;
    }
    
    napms(1000) ;
    return highlight ;
}

void menuUtama() {
startGame1:
    // cek ukuran terminal user
    getmaxyx(stdscr, heightT, widthT) ;
    
    int HeightM = 35, WithM = 80 ;
    if(heightT < MinHeight || widthT < MinWidth){
        endGame() ;
        fprintf(stderr,
            "Terminal anda terlalu kecil: %d (Horizontal) x %d (Vertikal).\n"
            "Perbesar terminal anda setidaknya %d (Horizontal) x %d (Vertikal) lalu coba lagi.\n",
            widthT, heightT, MinWidth, MinHeight);
            exit(0);
        }
        
        // menetapkan letak window untuk snake game dan menu utama
        startX = (widthT - MaxWidth - 1 ) / 2 ; 
        
        // buat window untuk main menu
        WINDOW * mainMenu = newwin(HeightM, WithM, startY, startX) ;
        box(mainMenu, 0, 0) ;
        refresh() ;
        wrefresh(mainMenu) ;
        
        int m = (WithM-2-24)/2 + startX ;
        WINDOW * titleWin = newwin(7, 24, 2, m) ;
        titleUI(WithM, titleWin) ;


startGame2:
    switch (pilihanMenu(WithM, mainMenu))
    {
        case 0:  // main ular 
        if(!login) {
            loginMessage() ;
            goto startGame2 ;
        } 
        delWin(mainMenu) ;
        delWin(titleWin) ;
        initSnake() ;

        while(lanjut) {
            napms(120) ;
            input() ;
            
            update() ;
            if(!lanjut) break ;
            draw() ;
            
            
            napms(50) ;
            input() ;
            update() ;
            draw() ;
        }

        if(iPlayer == -1) saveNPlayer() ;
        else saveNScore() ;
        wclear(snakeWin) ;
        wrefresh(snakeWin) ;
        goto startGame1 ;
        break;
        
        case 1:
            break ;
        case 2:  //quit game
            delWin(mainMenu) ;
            delWin(titleWin) ;
            endGame() ;
            break ;
        case 3:  // login
            loginProses() ;
            goto startGame2 ;
            break ;
        default:
        break;
    }
}