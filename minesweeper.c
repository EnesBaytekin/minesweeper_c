#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>

char* create_map(char width,char height,char n) {
    char* map = (char*)calloc(1,width*height);
    char r,c;
    while (n > 0) {
        r = rand()%height;
        c = rand()%width;
        if (map[r*width+c] == 0) {
            map[r*width+c] = 9;
            n--;
        }
    }
    for (r=0;r<height;r++) {
        for (c=0;c<width;c++) {
            if (map[r*width+c] == 9) continue;
            for (char dr=-1;dr<=1;dr++) {
                for (char dc=-1;dc<=1;dc++) {
                    if (dr == 0 && dc == 0) continue;
                    if (0 <= r+dr && r+dr < height && 0 <= c+dc && c+dc < width) {
                        if (map[(r+dr)*width+(c+dc)] == 9) {
                            map[r*width+c]++;
                        }
                    }
                }
            }
        }
    }
    return map;
}

void print_map(char* map,char* faces,char width,char height,int selection,int mine_count,int flag_count,char color) {
    for (char r=0;r<height;r++) {
        for (char c=0;c<width;c++) {
            char ch;
            if      (faces[r*width+c] == 0) ch = ' ';
            else if (faces[r*width+c] == 1) {
                if      (map[r*width+c]==0) ch = ' ';
                else if (map[r*width+c]==9) ch = '*';
                else ch = map[r*width+c]+48;
            }
            else if (faces[r*width+c] == 2) ch = '^';
            else if (faces[r*width+c] == 3) ch = '?';
            _Bool selected = (r == selection/width && c == selection%width);
            if (selected) printf("\033[46;90m");
            else if (faces[r*width+c] != 1) {
                if      (color == 0) printf("\033[42;90m");
                else if (color == 1) printf("\033[43;90m");
                else if (color == 2) printf("\033[44;90m");
                else if (color == 3) printf("\033[45;90m");
            }
            else if (map[r*width+c] == 9) printf("\033[41;90m");
            if (faces[r*width+c] != 1) printf("\033[90m[\033[97m%c\033[90m]\033[0m",ch);
            else printf("\033[90m[\033[97m%c\033[90m]\033[0m",ch);
        }
        printf("\n");
    }
    printf("mines: %d/%d\n",flag_count,mine_count);
}

void boom(char* map,char* faces,char width,char height) {
    for (char r=0;r<height;r++) {
        for (char c=0;c<width;c++) {
            if (map[r*width+c] == 9) {
                faces[r*width+c] = 1;
            }
        }
    }
}

void open_cell(char* map,char* faces,char width,char height,int selection) {
    if (faces[selection] == 0 || faces[selection] == 3) {
        faces[selection] = 1;
        switch (map[selection]) {
            case 0:
                for (char dr=-1;dr<=1;dr++) {
                    for (char dc=-1;dc<=1;dc++) {
                        if (dr == 0 && dc == 0) continue;
                        if (0 <= selection/width+dr && selection/width+dr < height && 0 <= selection%width+dc && selection%width+dc < width) {
                            open_cell(map,faces,width,height,selection+dr*width+dc);
                        }
                    }
                }
                break;
            case 9:
                boom(map,faces,width,height);
                break;
        }
    }
    else if (faces[selection] == 1) {
        if (0 < map[selection] && map[selection] < 9) {
            char n = 0;
            for (char dr=-1;dr<=1;dr++) {
                for (char dc=-1;dc<=1;dc++) {
                    if (dr == 0 && dc == 0) continue;
                    if (0 <= selection/width+dr && selection/width+dr < height && 0 <= selection%width+dc && selection%width+dc < width) {
                        if (faces[selection+dr*width+dc] == 2) n++;
                    }
                }
            }
            if (n == map[selection]) {
                for (char dr=-1;dr<=1;dr++) {
                    for (char dc=-1;dc<=1;dc++) {
                        if (dr == 0 && dc == 0) continue;
                        if (0 <= selection/width+dr && selection/width+dr < height && 0 <= selection%width+dc && selection%width+dc < width) {
                            if (faces[selection+dr*width+dc] == 0 || faces[selection+dr*width+dc] == 3) {
                                open_cell(map,faces,width,height,selection+dr*width+dc);
                            }
                        }
                    }
                }
            }
        }
    }
}

void change_face(char* faces,int selection,int* flag_count) {
    if (faces[selection] != 1) {
        if (faces[selection] == 2) (*flag_count)--;
        faces[selection]++;
        if (faces[selection] == 1) faces[selection]++;
        if (faces[selection] == 4) faces[selection] = 0;
        if (faces[selection] == 2) (*flag_count)++;
    }
}

void reset(char* map,char* faces,char width,char height,char mine_count,int* flag_count) {
    free(map);
    map = create_map(width,height,mine_count);
    for (int i=0;i<width*height;i++) {
        faces[i] = 0;
    }
    *flag_count = 0;
}

int main() {
    srand(time(NULL));
    char width = 25;
    char height = 25;
    char mine_count = 100;
    int flag_count = 0;
    char* map = create_map(width,height,mine_count);
    char* faces = (char*)calloc(1,width*height); // 0: close, 1: open, 2: flag, 3: question_mark
    int selection = 0;
    char ch;
    char color = 0;
    while (1) {
        system("cls");
        print_map(map,faces,width,height,selection,mine_count,flag_count,color);
        ch = getch();
        switch (ch) {
            case 'q':
                exit(0);
            case 'w':
                if (selection/width > 0) selection -= width;
                break;
            case 's':
                if (selection/width < height-1) selection += width;
                break;
            case 'a':
                if (selection%width > 0) selection--;
                break;
            case 'd':
                if (selection%width < width-1) selection++;
                break;
            case ' ':
            case 'k':
                open_cell(map,faces,width,height,selection);
                break;
            case 'l':
            case 'f':
            case 'e':
                change_face(faces,selection,&flag_count);
                break;
            case 'r':
                reset(map,faces,width,height,mine_count,&flag_count);
                break;
            case 'c':
                color = (color+1)%4;
        }
    }
    return 0;
}