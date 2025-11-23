#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int x ;
    int y ;
} koordinat ;

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
int score = 0 ;
int pBadan = 0 ;
bool lanjut = true ;
bool login = false ;
koordinat kepala, apel, gerak, badan[1000] ;
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
    return false ;
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
        if(gerak.y == 1 || kepala.x >= (MaxWidth-1) || kepala.x <= 0) return ;
        gerak.x = 0 ;
        gerak.y = -1 ;
        break;
        
        case KEY_DOWN:
        if(gerak.y == -1 || kepala.x >= (MaxWidth-1) || kepala.x <= 0) return ;
        gerak.x = 0 ;
        gerak.y = 1 ;
        break;
        
        case KEY_RIGHT:
        if(gerak.x == -1 || kepala.y >= (MaxHeight-1) || kepala.y <= 0) return ;
        gerak.x = 1 ;
        gerak.y = 0 ;
        break;
        
        case KEY_LEFT:
        if(gerak.x == 1 || kepala.y >= (MaxHeight-1) || kepala.y <= 0) return ;
        gerak.x = -1 ;
        gerak.y = 0 ;
        break;
        
        case '\e':
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
    kepala.x += gerak.x ; if(kepala.x == 0) kepala.x = (MaxWidth - 2) ; if(kepala.x == MaxWidth - 1) kepala.x = 1 ;
    kepala.y += gerak.y ; if(kepala.y == 0) kepala.y = (MaxHeight - 2) ; if(kepala.y == MaxHeight - 1) kepala.y = 1 ;
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
        mvwprintw(temp, 9, (72-42)/2, " Anda akan keluar dari game dalam %d detik ", i) ;
        
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
    
}

void menuUtama() {
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
        
        // menetapkan letak window untuk snake game
        startX = (widthT - MaxWidth - 1 ) / 2 ; 
        
        // buat window untuk main menu
        WINDOW * mainMenu = newwin(HeightM, WithM, startY, startX) ;
        refresh() ;
        box(mainMenu, 0, 0) ;
        wrefresh(mainMenu) ;
        getch() ;
        
        // buat title
        const char *title[] = {
            "▄▄▀█▄───▄───────▄",
            "▀▀▀██──███─────███",
            "░▄██▀░█████░░░█████░░",
            "███▀▄███░███░███░███░▄",
            "▀█████▀░░░▀███▀░░░▀██▀"
        };
        
        int m = (WithM-2-24)/2 + startX ;
        WINDOW * titleWin = newwin(7, 24, 2, m) ;
        refresh() ;
        
        for(int i=0; i<5; i++) {
            mvwprintw(titleWin, i+1, 1, "%s", title[i]) ;
        }
        wrefresh(titleWin) ;
        
        // pilihan
        char *choices [] = {
            "New Game",
            "Leaderboard",
            "Login",
            "switch accounts"
        };
        
        int highlight = 0 ;
        int enter = false ;
        m = (WithM-2-17)/2 + startX ;
        WINDOW * pilihan = newwin(7, 17, 15, m) ;
        keypad(pilihan, true) ;
        refresh() ;
        
        while(!enter) {
            for(int i=0; i<4; i++) {\
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
            highlight%=3 ;
        }

        delWin(mainMenu) ;
        delWin(titleWin) ;
        napms(1000) ;
        
        switch (highlight)
        {
            case 0:
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
            
            endGame() ;
            break;
            
            default:
            break;
        }
}