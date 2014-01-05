#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>
 
/*Milisecond based timing macros*/
static double _startMS;
double getMilis();
#define SLEEP(d) usleep(d*1000)
#define WAIT_START() {_startMS=getMilis();}
#define WAIT_FOR(dur) {double _r = _startMS+dur-getMilis();if(_r>0)SLEEP(_r);}

/*Macros to get main window dimension*/
int _MAX_Y, _MAX_X;
#define GET_MAX_WIN_SZ(w) getmaxyx(w,_MAX_Y,_MAX_X)

/* String format constants */
#define FMT_GAME "%s vs %s"

/*Cross-thread variables needed between the UI and the data fetching*/
volatile int _start_line=0; /*For commentary text scrolling*/
volatile int _cur_game=0;
volatile int _kill=0;
volatile int _resize=0;
volatile int _getting_data=0;

/*Commentary text buffer*/
#define MAX_BUFF_LEN 1024
#define BUFF_WIDTH 81
char **_com_lines;
int _num_lines=0;

/* Thread timing constants (in ms) */
#define SLEEP_UI 50
#define SLEEP_DATA 10000

/* Game struct and list*/
#define MAX_TEAM_LEN 140
typedef struct{
  char team_home[MAX_TEAM_LEN];
  char team_away[MAX_TEAM_LEN];
  char game_id[MAX_TEAM_LEN];
}t_game;
t_game *_games;
int _num_games;

/**
 * Description:
 * Gets the current system time in miliseconds
 */
double getMilis()
{
  struct timeval  tv;
  gettimeofday(&tv, NULL);

  return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
}

/**
 * Description:
 * Helper function to trim leading whitespace from a char*
 * Credit where credit is due...
 * See Philip's answer @ http://stackoverflow.com/questions/656542/trim-a-string-in-c  
 */
char *trim(char *s) {
  size_t len;
  char *cur;

  if(s && *s) {
          len = strlen(s);
          cur = s;

          while(*cur && isspace(*cur))
                  ++cur, --len;

          if(s != cur)
                  memmove(s, cur, len + 1);

  }

  return s;
}

/**
 * Description:
 * Prints out the given game's live commentary to the standard window.
 * This is mainly just a wrapper around the getEvents bash script.
 */
void getEvents()
{
  FILE *fp;
  char path[1035];
  int line_count=0;

  /* Open the command for reading. */
  sprintf(path,"php getEvents.php %s",_games[_cur_game].game_id);
  fp = popen(path, "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    return;
  }

  /* Clear commentary buffer */
  for(;_num_lines>0;_num_lines--)
    free(_com_lines[_num_lines-1]);

  /* Read the output a line at a time and store it in the buffer */
  path[0]='\0';
  while (fgets(path, sizeof(path)-1, fp) != NULL && line_count<MAX_BUFF_LEN){
    _com_lines[line_count] = calloc(BUFF_WIDTH,sizeof(char));
    strcpy(_com_lines[line_count++],path);
  }
  
  _num_lines=line_count;

  /* close */
  pclose(fp);
}

/**
 * Description:
 * Calls the getEvents function periodically.
 * Should exist in it's own thread
 */ 
void *getEvents_thrd(void *arg){
  while(!_kill){
    WAIT_START();
    _getting_data=1;
    getEvents();
    _getting_data=0;
    WAIT_FOR(SLEEP_DATA);
  }
  return NULL;
}

/**
 * Description:
 * Print the visible section of the commentary buffer
 * to the window
 */
void printCommentary(WINDOW *win_com)
{
  int line_no=0,x=1,y=1,text_line=0;
  char *buf;

  for(;line_no<_num_lines;line_no++){

    buf =  trim(_com_lines[line_no]);
    if(0==strcmp(buf,"EVENT_START\n")){
      wattroff(win_com,COLOR_PAIR(1));
      wattron(win_com,COLOR_PAIR(2));
    }
    else if(0==strcmp( buf,"EVENT_END\n")){
      wattron(win_com,COLOR_PAIR(1));
      wattroff(win_com,COLOR_PAIR(2));
    }

    else{
      if(text_line>=_start_line)
        mvwprintw(win_com,y++,x,"%s",_com_lines[line_no]);
      text_line++;
    }
  }
}

/**
 * Description:
 * Prints out the select list of games to the given
 * ncurses window
 */
void printGamesList(WINDOW *win_games)
{
  int i,x=1,y=1,hglt=_cur_game;/*hglt is a snapshot of _cur_game
                                 Really don't want the highlighted game
                                 changing mid-function call*/

  for(i=0;i<_num_games;i++){
    
    if(i==hglt){
      wattroff(win_games,COLOR_PAIR(1));
      wattron(win_games,COLOR_PAIR(2));
    }

    mvwprintw(win_games,y,x,FMT_GAME,
        _games[i].team_home,
        _games[i].team_away);
    y+=3;

    if(i==hglt){
      wattron(win_games,COLOR_PAIR(1));
      wattroff(win_games,COLOR_PAIR(2));
    }
  }
}

/**
 * Description:
 * Populates a list of game structs from the given competition
 */
void loadGames()
{

  FILE *fp;
  char path[1035];
  int g_cnt=0;

  /* Open the command for reading. */
  fp = popen("php getGames.php", "r");
  if (fp == NULL) {
    printf("Failed to run getGames command\n" );
    return;
  }

  /* Read the number of game structs (first line of output) */
  if(NULL == fgets(path, sizeof(path)-1, fp))
    puts("Empty games stream");

  sscanf(path,"%d",&_num_games);
  _games=calloc(_num_games,sizeof(t_game));
  printf("callocd %d",_num_games);
  /* Read the output a line at a time and store it in the buffer */
  while (fgets(path, sizeof(path)-1, fp) != NULL){
    strcpy(_games[g_cnt].team_home, path);
    fgets(path, sizeof(path)-1, fp);
    strcpy(_games[g_cnt].team_away, path);
    fgets(path, sizeof(path)-1, fp);
    strcpy(_games[g_cnt].game_id, path);
    puts("gamedone");
    g_cnt++;
  }
  /* close */
  pclose(fp);

}

/**
 * Description:
 * Calls all functions requried to update and handle the ncurses interface
 */
void handleUI(WINDOW *win_com, WINDOW *win_games){
  int ch;

  /* Resize the windows if needed */
  if(_resize){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    wresize(win_com,w.ws_row,(int)(w.ws_col*2.0/3));
    wresize(win_games,w.ws_row, w.ws_col/3.0);
    mvwin(win_com, 0, (int)(w.ws_col/3.0));
    mvwin(win_games, 0, 0);

    endwin();
    _resize=0;
  }

  if(!_getting_data){
    /* Clear windows */
    wclear(win_games);
    wclear(win_com);

    /* Update commentary text */
    printCommentary(win_com);

    /* Update game list*/
    printGamesList(win_games);

    /* Refresh windows*/
    box(win_com,0,0);
    wnoutrefresh(win_com);
    box(win_games,0,0);
    wnoutrefresh(win_games);
    doupdate();
  }

  /* Handle up/down keys */
  
  while( ERR != ( ch=getch() ) ){/*while there is a keypress to handle...*/
    switch(ch){

    case KEY_UP:
      _start_line=_start_line==0?0:_start_line-1;
      break;

    case KEY_DOWN:
      _start_line=_start_line+1;
      break;
    
    case KEY_PPAGE:
      if(_num_games)
        _cur_game=_cur_game==0?_num_games-1:_cur_game-1;
      getEvents();
      _start_line=0;
      break;
      
    case KEY_NPAGE:
      _cur_game++;
      _cur_game=_cur_game==_num_games?0:_cur_game;
      getEvents();
      _start_line=0;
      break;

    case 'q':
      _kill=1;
      printw("Stopping all threads... Could take up to %d seconds"
          ,SLEEP_DATA/1000);
      break;
    }

  }
}

/** 
 * Description:
 * Calls the handleUI function periodically
 * For use in its own thread
 */
void *handleUI_thrd(void *com){
  WINDOW *win_com=((WINDOW**)com)[0];
  WINDOW *win_games=((WINDOW**)com)[1];

  while(!_kill){
    WAIT_START();
    handleUI(win_com,win_games);
    WAIT_FOR(SLEEP_UI);
  }
  return NULL;
}

/**
 * Description:
 * Sets a 'terminal resized' flag.
 * Called using <signal.h>
 */
void resizeHandler(int sig)
{
   _resize=1;
}

int main(int argc, char** argv)
{
  pthread_t thrd_data,thrd_ui;
  WINDOW *win_com, *win_games, *wins[2];

  _com_lines = calloc(MAX_BUFF_LEN, sizeof(char*) );
  loadGames();

  /*Ncurses init and config*/
  initscr();
  cbreak();             /*Don't echo user input with getch*/
  keypad(stdscr, TRUE); /*Lets us use the func keys and arrows*/
  nodelay(stdscr, TRUE);/*Makes getch non-blocking*/
  curs_set(0);          /*Don't show cursor*/
  signal(SIGWINCH,resizeHandler);

  /* Window inits */
  GET_MAX_WIN_SZ(stdscr);
  win_com = newwin(_MAX_Y, _MAX_X*2/3, 0, _MAX_X/3);
  win_games = newwin(_MAX_Y, _MAX_X/3, 0, 0);
  wins[0]=win_com;wins[1]=win_games;

  /* Setup colours */
  start_color();
  init_pair(1,COLOR_BLUE,COLOR_BLACK);
  init_pair(2,COLOR_GREEN,COLOR_BLACK);
  wattron(win_com,COLOR_PAIR(1));
  wattron(win_games,COLOR_PAIR(1));
 
  /*Start UI and data threads*/
  pthread_create(&thrd_data, NULL, getEvents_thrd, NULL);
  pthread_create(&thrd_ui, NULL, handleUI_thrd, (void*)wins);
  
  /* Join threads */
  pthread_join(thrd_ui,NULL);
  pthread_join(thrd_data,NULL);

  /* Clean up */
  endwin();
  return 0;
}
