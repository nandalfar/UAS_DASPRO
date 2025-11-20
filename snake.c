#include <ncurses.h>
#include <stdlib.h>
#include <time.h>


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

const int MaxHeight = 25, MaxWidth = 70 ;
int score = 0 ;
koordinat kepala, apel, gerak ;
bool lanjut = true ;
WINDOW * win ;

int main() {
    initscr() ;
    cbreak() ;
    noecho() ;
    
    curs_set(0) ;

    init() ;
    while(lanjut) {
        napms(120) ;
        input() ;
        update() ;
        draw() ;
        napms(50) ;
        input() ;
        update() ;
        draw() ;
    }

    curs_set(1) ;
    getch() ;
    endwin();
    return 0;
}

void init() {
    // cek ukuran terminal user
    int heightT, widthT ;
    getmaxyx(stdscr, heightT, widthT) ;
    
    // buat window untuk game snake
    if(widthT <= MaxWidth && heightT <= MaxHeight) {
        win = newwin(heightT, widthT, 0, 0) ;
    } else if(widthT <= MaxWidth) {
        win = newwin(MaxHeight, widthT, 0, 0) ;
    } else {
        win = newwin(MaxHeight, MaxWidth, 0, 0) ;
    }
    
    box(win, 0, 0) ;
    refresh() ;
    wrefresh(win) ;
    keypad(win, true) ;
    nodelay(win, true) ;
    srand(time(NULL)) ;

    kepala.x = kepala.y = 1 ;
    gerak.x = 1 ; gerak.y = 0 ;
    spawn_apel() ;

    mvwaddch(win, kepala.y, kepala.x, 'O') ;
    mvwaddch(win, apel.y, apel.x, '@') ;
    wrefresh(win) ;
}

bool cek1 (koordinat a, koordinat b) {
    if(a.x != b.x) return false ;
    if(a.y != b.y) return false ;

    return true ;
}

void spawn_apel() {
    apel.x = 1 + rand() % (MaxWidth-2) ; 
    apel.y = 1 + rand() % (MaxHeight-2) ;
    while(cek1(apel, kepala)) {
        apel.x = 1 + rand() % (MaxWidth-2); 
        apel.y = 1 + rand() % (MaxHeight-2) ;
    }
}

void input() {
    int x = wgetch(win) ;

    switch (x)
    {
    case KEY_UP:
        if(gerak.y == 1 || kepala.x >= MaxWidth || kepala.x <= 0) return ;
        gerak.x = 0 ;
        gerak.y = -1 ;
        break;
        
    case KEY_DOWN:
        if(gerak.y == -1 || kepala.x >= MaxWidth || kepala.x <= 0) return ;
        gerak.x = 0 ;
        gerak.y = 1 ;
        break;
        
    case KEY_RIGHT:
        if(gerak.x == -1 || kepala.y >= MaxHeight || kepala.y <= 0) return ;
        gerak.x = 1 ;
        gerak.y = 0 ;
        break;
        
    case KEY_LEFT:
        if(gerak.x == 1 || kepala.y >= MaxHeight || kepala.y <= 0) return ;
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

    mvwaddch(win, kepala.y, kepala.x, ' ') ;
    wrefresh(win) ;
    kepala.x += gerak.x ; if(kepala.x == 0) kepala.x = MaxWidth - 2 ; if(kepala.x == MaxWidth-1) kepala.x = 1 ;
    kepala.y += gerak.y ; if(kepala.y == 0) kepala.y = MaxHeight - 2 ; if(kepala.y == MaxHeight-1) kepala.y = 1 ;
}

void draw() {
    mvwaddch(win, kepala.y, kepala.x, 'O') ;
    mvwaddch(win, apel.y, apel.x, '@') ;
    box(win, 0, 0) ;
    mvwprintw(win, 0, 25, " Score: %d ", score) ;
    wrefresh(win) ;
}