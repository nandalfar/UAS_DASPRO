#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <stdio.h>

typedef struct {
    int x ;
    int y ;
} koordinat ;

void init() ;
bool cek1(koordinat a, koordinat b) ;
void spawn_apel() ;
void input() ;
void update() ;
void draw() ;
void gameOver_B(bool Big) ;
void endGame() ;

int MaxHeight = 25, MaxWidth = 80 ;
int MinHeight = 10, MinWidth = 20 ;
int startX, startY = 0 ;
int score = 0 ;
int pBadan = 0 ;
koordinat kepala, apel, gerak, badan[1000] ;
bool lanjut = true ;
WINDOW * win ;

int main() {
    setlocale(LC_ALL, "");
    initscr() ;
    cbreak() ;
    noecho() ;
    
    curs_set(0) ;

    init() ;
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
    return 0;
}

void init() {
    // cek ukuran terminal user
    int heightT, widthT ;
    getmaxyx(stdscr, heightT, widthT) ;
    
    if(heightT < MinHeight || widthT < MinWidth){
        endGame() ;
        fprintf(stderr,
            "Terminal anda terlalu kecil: %d (Horizontal) x %d (Vertikal).\n"
            "Perbesar terminal anda setidaknya %d (Horizontal) x %d (Vertikal) lalu coba lagi.\n",
            widthT, heightT, MinWidth, MinHeight);
        exit(0);
    }
    // menetapkan ukuran window untuk snake game
    if(widthT <= MaxWidth && heightT <= MaxHeight) {
        startX = 0 ;
        MaxHeight = heightT - 2 ;
        MaxWidth = widthT - 2 ; 
    } else if (widthT <= MaxWidth) {
        startX = 0 ;
        MaxWidth = widthT - 2 ; 
    } else if (heightT <= MaxHeight) {
        startX = (widthT - MaxWidth - 1 ) / 2 ; 
        MaxHeight = heightT - 2 ;
    } else {
        startX = (widthT - MaxWidth - 1 ) / 2 ; 
    }
    
    win = newwin(MaxHeight, MaxWidth, startY, startX) ;
    
    box(win, 0, 0) ;
    refresh() ;
    wrefresh(win) ;
    keypad(win, true) ;
    nodelay(win, true) ;
    srand(time(NULL)) ;
    
    kepala.x = startX + 1, kepala.y = startY + 1 ;
    gerak.x = 1 ; gerak.y = 0 ;
    spawn_apel() ;
    
    mvwaddch(win, kepala.y, kepala.x, '>') ;
    mvwaddch(win, apel.y, apel.x, '@') ;
    wrefresh(win) ;
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
    int x = wgetch(win) ;

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
        gameOver_B( (MaxHeight >= 18 && MaxWidth >= 75) ) ;
        napms(4000) ;
        return ;
    }

    mvwaddch(win, kepala.y, kepala.x, ' ') ;
    if(pBadan > 0) mvwaddch(win, badan[pBadan-1].y, badan[pBadan-1].x, ' ') ;
    wrefresh(win) ;
    
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

void draw() {
    if(gerak.x == 1) {
        mvwaddch(win, kepala.y, kepala.x, '>') ;
    }else if (gerak.x == -1) {
        mvwaddch(win, kepala.y, kepala.x, '<') ;
    } else if (gerak.y == 1) {
        mvwaddch(win, kepala.y, kepala.x, 'v') ;
    } else {
        mvwaddch(win, kepala.y, kepala.x, '^') ;
    }
    
    for(int i=0; i<pBadan; i++) {
        mvwaddch(win, badan[i].y, badan[i].x, 'O') ;
    }
    mvwaddch(win, apel.y, apel.x, '@') ;
    box(win, 0, 0) ;
    mvwprintw(win, 0, (MaxWidth/2)-(5 + (score/10) ), " Score: %d ", score) ;
    wrefresh(win) ;
}

void gameOver_B(bool Big) {
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

    if(Big) {
        int midW = (MaxWidth-75)/2 ;
        int midH = (MaxHeight-18)/2 ;
        temp = newwin(MaxHeight-10, MaxWidth - 2, 4, startX) ;
        for (int i=0; i<6; i++) {
            mvwprintw(temp, i+midH, midW, "%s", text_gameOver[i]) ;
        }
    }else{
        temp = newwin(3, MaxWidth, (MaxHeight-3-2) / 2, startX) ;
        mvwprintw(temp, 1, 1, "GAME OVER") ;
    }
    box(temp, 0, 0) ;
    
    refresh() ;
    wrefresh(temp) ;

}