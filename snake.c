#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define min(a,b) ((a) < (b) ? (a) : (b))

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
void initNcurses() ;
void initColor() ;
void charWcolor(WINDOW * curWin, int y, int x, char s, int color) ;

int gameHeight = 25, gameWidth = 80 ;
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
    initNcurses() ;
    initColor() ;
    menuUtama() ;
    return 0;
}

void initColor() {
    start_color() ;
    use_default_colors() ;
    init_pair(1, COLOR_RED, -1);
    init_pair(2, COLOR_GREEN, -1);
    init_pair(3, COLOR_YELLOW, -1);
    init_pair(4, COLOR_BLACK, COLOR_YELLOW);
}

void initNcurses() {
    setlocale(LC_ALL, "");
    initscr() ;
    cbreak() ;
    noecho() ;
    curs_set(0) ;
}

void initSnake() {
    lanjut = true ;
    pBadan = 0 ;
    score = 0 ;
    snakeWin = newwin(gameHeight, gameWidth, startY, startX) ;
    box(snakeWin, 0, 0) ;
    refresh() ;
    wrefresh(snakeWin) ;
    keypad(snakeWin, true) ;
    nodelay(snakeWin, true) ;
    srand(time(NULL)) ;
    
    kepala.x = startX + 1, kepala.y = startY + 1 ;
    gerak.x = 1 ; gerak.y = 0 ;
    spawn_apel() ;
    
    charWcolor(snakeWin, kepala.y, kepala.x, '>', 2) ;
    charWcolor(snakeWin, apel.y, apel.x, '@', 1) ;
    wrefresh(snakeWin) ;
}

void endGame() {
    curs_set(1) ;
    endwin();
}

void printWcolor(WINDOW * curWin, int y, int x, char *s, int color) {
    wattron(curWin, COLOR_PAIR(color)) ;
    mvwprintw(curWin, y, x, "%s", s) ;
    wattroff(curWin, COLOR_PAIR(color)) ;
}

void charWcolor(WINDOW * curWin, int y, int x, char s, int color) {
    wattron(curWin, COLOR_PAIR(color)) ;
    mvwaddch(curWin, y, x, s) ;
    wattroff(curWin, COLOR_PAIR(color)) ;
}

void colorBox(WINDOW * curWin, int color) {
    wattron(curWin, COLOR_PAIR(color)) ;
    box(curWin, 0, 0) ;
    wattroff(curWin, COLOR_PAIR(color)) ;
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

    if(kepala.x == 0 || kepala.x == gameWidth-1 || kepala.y == 0 || kepala.y == gameHeight-1){
        lanjut = false ;
        return true ;
    }
    return false ;
}

void merge(player arr[], int l, int m, int r) {
    int n1 = m - l + 1 ;
    int n2 = r - m ;

    player L[n1], R[n2] ;

    for(int i=0; i<n1; i++) L[i] = arr[l + i] ;
    for(int i=0; i<n2; i++) R[i] = arr[m + i + 1] ;
    
    int i = 0, j = 0, k = l;

    while(i < n1 && j < n2) {
        if(L[i].bestScore >= R[j].bestScore) {
            arr[k] = L[i] ;
            i++ ;
        }else {
            arr[k] = R[j] ;
            j++ ;
        }
        k++ ;
    }

    while(i < n1) {
        arr[k] = L[i] ;
        i++ ;
        k++ ;
    }

    while(j < n2){
        arr[k] = R[j] ;
        j++ ;
        k++ ;
    }
}

void mergeSort(player arr[], int l, int r) {
    if(l < r) {
        int m = l + (r - l) /2 ;

        mergeSort(arr, l, m) ; 
        mergeSort(arr, m+1, r) ;

        merge(arr, l, m, r) ;
    }
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
            playerNow.bestScore = temp.bestScore ;
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
    fwrite(&playerNow, sizeof(player), 1, file);

    fclose(file);
}

void spawn_apel() {
    apel.x = 1 + rand() % (gameWidth - 2) ; 
    apel.y = 1 + rand() % (gameHeight - 2) ;
    while(cek1(apel, kepala)) {
        apel.x = 1 + rand() % (gameWidth - 2); 
        apel.y = 1 + rand() % (gameHeight - 2) ;
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
    printWcolor(logM, 1, 1, "ANDA HARUS LOGIN TERLEBIH DAHULU!!", 3) ;
    refresh() ;
    wrefresh(logM) ;

    for(int i=0; i<5; i++) {
        colorBox(logM, 4) ;
        wrefresh(logM) ;
        napms(600) ;

        colorBox(logM, 3) ;
        wrefresh(logM) ;
        napms(400) ;
    }
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
        if (index >= 25) continue;
        
        // hanya izinkan huruf/angka/spasi (opsional)
        if (isprint(ch)) {
            usn[index] = ch;
            index++ ;
            mvwprintw(logP, 2, 2 + index - 1, "%c", ch);
            wrefresh(logP);
        }
    }

    usn[index] = '\0' ;
}

bool konfirmNP() {
    int mid = (widthT - 50 - 1 ) / 2 ;
    WINDOW * newP = newwin(4, 50, 15, mid) ;
    keypad(newP, true) ;
    colorBox(newP, 3) ;
    printWcolor(newP, 0, 2, " USN anda tidak ada, ingin membuat USN baru ? ", 3) ;
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
                printWcolor(newP, 2, x, yn[i], 4) ;
            }else{
                printWcolor(newP, 2, x, yn[i], 3) ;
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
    
    delWin(newP) ;
    if(highlight == 0) return true ;
    else return false ;
}

void loginProses() {
    int mid = (widthT - 30 - 1 ) / 2  ;
    WINDOW * logP = newwin(5, 30, 15, mid) ;
    colorBox(logP, 3) ;
    printWcolor(logP, 0, 2, " Masukkan USN (MAX 25) ", 3) ;
    charWcolor(logP, 2, 1, '>', 3) ;
    refresh() ;
    wrefresh(logP) ;
    
    char usn[30] ;
    memset(usn, 0, sizeof(usn)) ;
    inputUSN(usn, logP) ;
    cariUSN(usn) ;

    if(iPlayer == -1) {
        if(!konfirmNP()){
            delWin(logP) ;
            return ;   
        }else{
            strcpy(playerNow.username, usn);
            playerNow.bestScore = 0 ;
            saveNPlayer() ;
            cariUSN(playerNow.username) ;
        }
    }
    
    login = true ;
    strcpy(playerNow.username, usn);
    playerNow.bestScore = 
    napms(700) ;
    delWin(logP) ;
}

void delWin(WINDOW * tmp) {
    wclear(tmp) ;
    wrefresh(tmp) ;
    delwin(tmp) ;
    refresh() ;
}

void draw() {
    if(gerak.x == 1) {
        charWcolor(snakeWin, kepala.y, kepala.x, '>', 2) ;
    }else if (gerak.x == -1) {
        charWcolor(snakeWin, kepala.y, kepala.x, '<', 2) ;
    } else if (gerak.y == 1) {
        charWcolor(snakeWin, kepala.y, kepala.x, 'v', 2) ;
    } else {
        charWcolor(snakeWin, kepala.y, kepala.x, '^', 2) ;
    }
    
    for(int i=0; i<pBadan; i++) {
        printWcolor(snakeWin, badan[i].y, badan[i].x, "●", 2) ;
    }
    charWcolor(snakeWin, apel.y, apel.x, '@', 1) ;
    colorBox(snakeWin, 3) ;
    mvwprintw(snakeWin, 0, (gameWidth/2)-(5 + (score/10) ), " Score: %d ", score) ;
    wrefresh(snakeWin) ;
}

void leaderboard(int WidthM) {
    FILE *file = fopen("dataPlayer.dat", "rb");
    if (!file) return;

    player allPlayer [100] ;
    player tmp ;
    int index = 0 ;
    while (fread(&tmp, sizeof(player), 1, file) == 1) {
        allPlayer[index] = tmp ;
        index++ ;
    }

    fclose(file) ;

    if(index == 0)return ;

    mergeSort(allPlayer, 0, index-1) ;
    
    char *leaderText[] = {
        "█░░ █▀▀ █▀▀█ █▀▀▄ █▀▀ █▀▀█ █▀▀▄ █▀▀█ █▀▀█ █▀▀█ █▀▀▄",
        "█░░ █▀▀ █▄▄█ █░░█ █▀▀ █▄▄▀ █▀▀▄ █░░█ █▄▄█ █▄▄▀ █░░█",
        "▀▀▀ ▀▀▀ ▀░░▀ ▀▀▀░ ▀▀▀ ▀░▀▀ ▀▀▀░ ▀▀▀▀ ▀░░▀ ▀░▀▀ ▀▀▀░"
    };

    int m = (WidthM-2-53)/2 + startX ;
    WINDOW * leaderTitle = newwin(5, 53, 4, m) ;
    for(int i=0; i<3; i++){
        printWcolor(leaderTitle, i+1, 1, leaderText[i], 3) ;
    }
    
    refresh() ;
    wrefresh(leaderTitle) ;

    m = (WidthM-2-35)/2 + startX ;
    WINDOW * leaderP = newwin(21, 35, 11, m) ;
    colorBox(leaderP, 3) ;
    mvwprintw(leaderP, 20, 2, " Press any key to return " );
    refresh() ;

    for(int i=0; i<min(index, 10); i++) {
        mvwprintw(leaderP, i+1+i, 1, "%s", allPlayer[i].username) ;

        int j ;
        for(j=strlen(allPlayer[i].username)+1; j<=30; j++) {
            mvwprintw(leaderP, i+1+i, j, " ") ;
        }
        mvwprintw(leaderP, i+1+i, j, "%d", allPlayer[i].bestScore) ;
    }
    wrefresh(leaderP) ;

    wgetch(leaderP) ;
    delWin(leaderTitle) ;
    delWin(leaderP) ;
}

void gameOver() {
    // deklarasi string ASCII yang akan dicetak
    
    char *text_gameOver[] = {
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
        colorBox(temp, 3) ;
        mvwprintw(temp, 9, (72-48)/2, " Anda akan kembali ke menu utama dalam %d detik ", i) ;
        
        for (int j=0; j<6; j++) {   // for loop cetak UI game over
            printWcolor(temp, j+2, 1, text_gameOver[j], 3) ;
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

void titleUI(int WidthM, WINDOW * titleWin) {

    // buat title
    char *title[] = {
        "▄▄▀█▄   ▄       ▄",
        "▀▀▀██──███ ─── ███",
        "░▄██▀░█████░░░█████░░",
        "███▀▄███░███░███░███░▄",
        "▀█████▀░░░▀███▀░░░▀██▀"
    };
    
    for(int i=0; i<5; i++) {
        printWcolor(titleWin, i+1, 1, title[i], 3) ;
    }
    wrefresh(titleWin) ;
}

int pilihanMenu(int WidthM, WINDOW * mainMenu) {
    // pilihan
    char *choices [] = {
        "New Game",
        "Leaderboard",
        "Quit",
        "Login"
    };
    
    if(login) choices[3] = "Logout" ;

    int highlight = 0 ;
    int enter = false ;
    int m = (WidthM-2-17)/2 + startX ;
    WINDOW * pilihan = newwin(11, 17, 15, m) ;
    keypad(pilihan, true) ;
    refresh() ;
    
    if(login) mvwprintw(mainMenu, 0, 3, " %s ", playerNow.username) ;
    wrefresh(mainMenu) ;
    
    while(!enter) {
        for(int i=0; i<4; i++) {
            if(i==highlight) {
                printWcolor(pilihan, i+1+i, (15-strlen(choices[i])-1) / 2 + 1, choices[i], 4) ;
            } else {
                printWcolor(pilihan, i+1+i, (15-strlen(choices[i])-1) / 2 + 1, choices[i], 3) ;
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
    delWin(pilihan) ;
    return highlight ;
}

void menuUtama() {
startGame1:
    // cek ukuran terminal user
    getmaxyx(stdscr, heightT, widthT) ;
    
    int HeightM = 35, WidthM = 80 ;
    if(heightT < MinHeight || widthT < MinWidth){
        endGame() ;
        fprintf(stderr,
            "Terminal anda terlalu kecil: %d (Horizontal) x %d (Vertikal).\n"
            "Perbesar terminal anda setidaknya %d (Horizontal) x %d (Vertikal) lalu coba lagi.\n",
            widthT, heightT, MinWidth, MinHeight);
            exit(0);
        }
        
    // menetapkan letak window untuk snake game dan menu utama
    startX = (widthT - gameWidth - 1 ) / 2 ; 
    
    // buat window untuk main menu
    WINDOW * mainMenu = newwin(HeightM, WidthM, startY, startX) ;
    refresh() ;

startGame2:
    int m = (WidthM-2-24)/2 + startX ;
    WINDOW * titleWin = newwin(7, 24, 4, m) ;
    refresh() ;
    
startGame3:
    colorBox(mainMenu, 3) ;
    wrefresh(mainMenu) ;
    titleUI(WidthM, titleWin) ;
    switch (pilihanMenu(WidthM, mainMenu))
    {
        case 0:  // main ular 
        if(!login) {
            loginMessage() ;
            goto startGame3 ;
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

        saveNScore() ;
        wclear(snakeWin) ;
        wrefresh(snakeWin) ;
        goto startGame1 ;
        break;
        
        case 1:
            delWin(titleWin) ;
            leaderboard(WidthM) ;
            goto startGame2 ;
            break ;
        case 2:  //quit game
            delWin(mainMenu) ;
            delWin(titleWin) ;
            endGame() ;
            break ;
        case 3:  // login & logout
            if(login) {
                login = false ;
                iPlayer = -1 ;
            }else{
                loginProses() ;
            }
            goto startGame3 ;
            break ;
        default:
        break;
    }
}