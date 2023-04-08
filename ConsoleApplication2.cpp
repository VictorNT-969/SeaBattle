/* Игра "морской бой"
   Корабли:
Размер Кол-во
    5   1
    4   2
    3   3
    2   4
Можно ставить бортом или торцом к берегу
Углами, бортами, торцами не должны соприкасаться
Регистрируется попадания без уточнения "ранил" или "убил" 
При попадании повторный (бонусный) ход, при промахе ход переходит сопернику 

. - промах, либо ячейка, в которой по расчетам нет корабля
O - целая часть корабля
X - попадание
Space - пустое поле */

#define __CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <io.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <sstream>

using namespace std;

string Repeat = "1"; // 0 - выход, 1 - сыграть еще раз
string mode = "0"; // 1 - человек-ПК, 2 - ПК1 (алгоритм \"A(4->?)->2\") -ПК2(\"A(5->?)->RND)\")
string inp = "0"; // 1 - ручная расстановка за человека, 2 - человек расставляет
string mode_pc_fire = "0"; // 1 - rnd; 2 - intell
string cont = "0"; // 2 - продолжить сохраненку
string HOR; // горизонтальная координата в строковом виде для ручного ввода
int HOR_INT=12; // горизонтальная координата в числовом виде (от 1 до 10)
int VERT=0; // вертикальная координата верхнего левого угла корабля (от 1 до 10 )
string VERT_str; // вериткальная координата для ввода имеет строковый вид (реализована защита от невалидных значений)
string VH = "q"; // вертикально или горизонтально расположен корабль
int VH_int; // рандом для расстановки для ПК
int t = 0; // попытка ввода координаты при ручной расстановке: 0 - неусп, 1 - усп.
// int tra = 0; // флаг успеха для рандомного поиска ячейки для удара компа
int persp; // флаг наличия хотя бы одной перспективной ячейки
int USP; // =1 - попал, 0 - промах
int n=0; // порядковый номер ячейки для заполнения матрицей ИИ "54321"
char max_arr; // поиск максимального в матрице огня ПК для алгоритма типа интеллект
string Intel_1 = "4 2 4 2 4  2 4 2 4 22 4 2 4 2  4 2 4 2 44 2 4 2 4  2 4 2 4 22 4 2 4 2  4 2 4 2 44 2 4 2 4  2 4 2 4 2"; // алгоритмическая сетка ПК1
string Intel_2 = "    5    5   5    5   5    5   5    5   5    5        5    5   5    5   5    5   5    5   5    5    "; // ПК2
//   string Intel_  = "4321543215321543215121543215121543215123543215123443215123453215123451215123451215123451235123451234";

void Show(char** ARR, char** ARRF, string A, string B) // функция отображения состояния полей
{
    // system("cls");
    cout << "\n    " << A << "          " << B << "\n    A B C D E F G H I J      A B C D E F G H I J\n";
    for (int y= 1; y < 11; y++) {
        if (y < 10) cout << " ";
        cout << y << "| ";
        for (int x = 1; x < 11; x++) {
            cout << ARR[x][y] << " ";
        }
        cout << "|";
        if (y < 10) cout << " ";
        cout << y << "| ";
        for (int x = 1; x < 11; x++) {
            cout << ARRF[x][y] << " ";
        }
        // if (y < 10) cout << " ";
        cout << "|"<< y ;
        cout << endl;
    }
    cout << "    A B C D E F G H I J      A B C D E F G H I J\n";
}
int Check(char** arr, int size, int x, int y, char vh, int PC) // функция проверки возможности установки корабля в данную позицию
{
    if ((vh == 'V') || (vh == 'v')) { // если вертикально
        for (int i1 = 0; i1 < size; i1++) {
            if (y+i1>10||arr[x][y+i1]!=' '||arr[x+1][y+i1]!=' '||arr[x-1][y+i1]!=' '||arr[x][y+i1-1]!=' '||arr[x][y+i1+1]!=' '||arr[x-1][y+i1-1]!=' '||arr[x+1][y+i1+1]!=' '||arr[x-1][y+i1+1]!=' '||arr[x+1][y+i1-1]!=' ')
            {
                if (PC==0) cout << "Невозможно установить корабль в указанную позицию\n"; // сообщение выводится при ручном вводе. При заполнении для ПК это не выводится
                return 0;
            }
        }
        for (int i1 = 0; i1 < size; i1++) {
            arr[x][y + i1] = 'O';
        }
    }
    else // если горизонтально
    {
        for (int i1 = 0; i1 < size; i1++) {
            if (x+i1 > 10||arr[x+i1][y]!=' '||arr[x+i1-1][y]!=' '||arr[x+i1+1][y]!=' '||arr[x+i1][y-1]!=' '||arr[x+i1][y+1]!=' '||arr[x+i1-1][y-1]!=' '||arr[x+i1+1][y+1]!=' '||arr[x+i1+1][y-1]!=' '||arr[x+i1-1][y+1]!=' ')
            {
                if (PC==0) cout << "Невозможно установить корабль в указанную позицию\n";
                return 0;
            }
        }
        for (int i1 = 0; i1 < size; i1++) {
            arr[x+i1][y] = 'O';
        }
    }
    return 1;
}
int CheckOst(char** arr) // подсчет живых остатков. 30 - макс, 0 - конец игры
{
    int Ost = 0;
    for (int i = 1; i < 11; i++)
    {
        for (int j = 1; j < 11; j++)
            if (arr[i][j] == 'O') Ost = Ost + 1;
    }
    return Ost;
}
void InputRnd(char** ARR) // расстановка кораблей случайным образом
{
    for (int i = 5; i >= 2; i--) { // размер_5 - 1шт, размер_4 - 2шт, размер_3 - 3шт, размер_2 - 4шт
        for (int ii = 1; ii <= 6 - i; ii++)
        {
            for (int t = 0; t < 100; t++) {
                HOR_INT = rand() % 11;
                if (HOR_INT == 0) HOR_INT = 1; // увеличение шансов расстановки по берегу для успешной расстановки всех кораблей
                if (HOR_INT == 11) HOR_INT = 10;
                VERT = rand() % 11;
                if (VERT == 0) VERT = 1; // увеличение шансов расстановки по берегу для успешной расстановки всех кораблей
                if (VERT == 11) VERT = 10;
                VH_int = rand() % 2;
                if (VH_int == 1)
                    VH = "v";
                else VH = "h";
                if (Check(ARR, i, HOR_INT, VERT, VH[0], 1) == 1) break;// при успешном вводе переход на ввод следующего корабля
                     //        t = 100; , иначе вводим корректные данные для текущего корабля
            }
        }
    }
    // Show(PC1_resurs, PC1_Fire); // подглядеть за ПК что он там расставил
}
void RndFire(char** ARR) // случайный огонь в небитые ячейки
{
    int tra = 1;
    while (tra == 1)
    {
        HOR_INT = rand() % 11;
        if (HOR_INT == 0) HOR_INT = 1;
        if (HOR_INT == 11) HOR_INT = 10;
        VERT = rand() % 11;
        if (VERT == 0) VERT = 1;
        if (VERT == 11) VERT = 10;
        // исключаем тухлые варианты
        if (ARR[HOR_INT + 1][VERT + 1] != 'X' && ARR[HOR_INT - 1][VERT - 1] != 'X' && ARR[HOR_INT + 1][VERT - 1] != 'X' && ARR[HOR_INT - 1][VERT + 1] != 'X' && ARR[HOR_INT + 1][VERT] != 'X' && ARR[HOR_INT - 1][VERT] != 'X' && ARR[HOR_INT][VERT - 1] != 'X' && ARR[HOR_INT][VERT + 1] != 'X' && ARR[HOR_INT][VERT] != 'X' && ARR[HOR_INT][VERT] != '.') tra = 0;
    }
}
void SearchPersp(char** ARR)
{// поиск перебором перспективных ячеек (отмечены знаком ?)
    HOR_INT = 0;
    VERT = 0;
    for (int y = 1; y < 11; y++)
    {
        for (int x = 1; x < 11; x++)
        {
            if (ARR[x][y] == '?')
            {
                HOR_INT = x;
                VERT = y;
            }
        }
    }
}
void FireTrue(char** ARR_Fire, char** ARR_Opp_Resurs, string Fighter)
{ // при успешном попадании корретируем карту огня (чтобы не лупить в молоко)
    cout << endl << endl << (char)(HOR_INT + 64) << " " << VERT << " - " << Fighter << " попал !!!";
    ARR_Fire[HOR_INT][VERT] = 'X';
    ARR_Opp_Resurs[HOR_INT][VERT] = 'X';
    USP = 1;

    ARR_Fire[HOR_INT - 1][VERT - 1] = '.';
    ARR_Fire[HOR_INT + 1][VERT + 1] = '.';
    ARR_Fire[HOR_INT - 1][VERT + 1] = '.';
    ARR_Fire[HOR_INT + 1][VERT - 1] = '.';

    if (ARR_Fire[HOR_INT + 1][VERT] != 'X' && ARR_Fire[HOR_INT + 1][VERT] != '.') ARR_Fire[HOR_INT + 1][VERT] = '?';
    if (ARR_Fire[HOR_INT - 1][VERT] != 'X' && ARR_Fire[HOR_INT - 1][VERT] != '.') ARR_Fire[HOR_INT - 1][VERT] = '?';
    if (ARR_Fire[HOR_INT][VERT + 1] != 'X' && ARR_Fire[HOR_INT][VERT + 1] != '.') ARR_Fire[HOR_INT][VERT + 1] = '?';
    if (ARR_Fire[HOR_INT][VERT - 1] != 'X' && ARR_Fire[HOR_INT][VERT - 1] != '.') ARR_Fire[HOR_INT][VERT - 1] = '?';

    if (ARR_Fire[HOR_INT][VERT - 1] == 'X' || ARR_Fire[HOR_INT][VERT + 1] == 'X')
    {
        ARR_Fire[HOR_INT - 1][VERT - 1] = '.';
        ARR_Fire[HOR_INT - 1][VERT] = '.';
        ARR_Fire[HOR_INT - 1][VERT + 1] = '.';
        ARR_Fire[HOR_INT + 1][VERT - 1] = '.';
        ARR_Fire[HOR_INT + 1][VERT] = '.';
        ARR_Fire[HOR_INT + 1][VERT + 1] = '.';
    }
    if (ARR_Fire[HOR_INT - 1][VERT] == 'X' || ARR_Fire[HOR_INT + 1][VERT] == 'X')
    {
        ARR_Fire[HOR_INT - 1][VERT - 1] = '.';
        ARR_Fire[HOR_INT][VERT - 1] = '.';
        ARR_Fire[HOR_INT + 1][VERT - 1] = '.';
        ARR_Fire[HOR_INT - 1][VERT + 1] = '.';
        ARR_Fire[HOR_INT][VERT + 1] = '.';
        ARR_Fire[HOR_INT + 1][VERT + 1] = '.';
    }
}


int main()
{
    setlocale(LC_ALL, "Russian");
    srand(time(NULL));
    while (Repeat == "1") {
        // матрица 12х12 (0..11 х 0..11). Крайние координаты игрового поля: 1х1 и 10х10. 
        // Матрица 12х12 выбрана из соображений упрощения алгоритма расстановки кораблей (1+10+1 = 12)
        char** Player_Resurs = new char* [12];
        char** Player_Fire = new char* [12];
        char** PC1_Resurs = new char* [12];
        char** PC1_Fire = new char* [12];
        char** PC2_Resurs = new char* [12];
        char** PC2_Fire = new char* [12];
        for (int i = 0; i < 12; i++) {
            Player_Resurs[i] = new char[12];
            Player_Fire[i] = new char[12];
            PC1_Resurs[i] = new char[12];
            PC1_Fire[i] = new char[12];
            PC2_Resurs[i] = new char[12];
            PC2_Fire[i] = new char[12];
            for (int j = 0; j < 12; j++) {
                Player_Resurs[i][j] = ' ';
                Player_Fire[i][j] = ' ';
                PC1_Resurs[i][j] = ' ';
                PC1_Fire[i][j] = ' ';
                PC2_Resurs[i][j] = ' ';
                PC2_Fire[i][j] = ' ';
            }
        }

        cout << "*** Игра \"Морской бой\" ***\n\n1. Размеры и количество кораблей: OOOOO - 1; OOOO - 2; OOO - 3; 00 - 4.\n";
        cout << "2. Корабли можно ставить бортом или торцом к берегу.\n3. Корабли не должны соприкасаться между собой углами, бортами или торцами.\n";
        cout << "4. Регистрируются попадания без уточнения статуса \"ранил\" или \"убил\".\n";
        cout << "5. При попадании повторный (бонусный) ход. При промахе ход переходит сопернику.\n";
        cout << "6. Обозначения полей: . - промах; O - целая часть корабля; X - попадание.\n\n\n*** УДАЧИ!!! Попробуй меня обыграть... ***\n\n\n";

        struct _finddata_t fileinfo;
        const char* Game = "Chel-PC1.txt";
        intptr_t done = _findfirst(Game, &fileinfo);
        if (done != -1) {
            while (cont != "1" && cont != "2" && cont != "3")
            {
                cout << "Обнаружена незавершенная игра \"Игрок-Комп1\". (1) - удалить, (2) - продолжить сохраненную игру, (3) - игнорировать\n";
                cin >> cont;
            }

            if (cont == "1") remove(Game);
            if (cont == "2") {
                mode = "1";
                mode_pc_fire = "2";
                FILE* out;
                if (fopen_s(&out, Game, "r") != NULL) cout << "Невозможно прочитать файл!\n";
                else cout << "Файл успешно открыт для чтения!\n";
                for (int j = 1; j < 11; j++)
                {
                    for (int i = 1; i < 11; i++)
                    {
                        fscanf_s(out, "%1c", &Player_Resurs[i][j], 2);
                        fscanf_s(out, "%1c", &Player_Fire[i][j], 2);
                        fscanf_s(out, "%1c", &PC1_Resurs[i][j], 2);
                        fscanf_s(out, "%1c", &PC1_Fire[i][j], 2);
                    }
                }
                fclose(out);
            }

        }

        while (mode != "1" && mode != "2") {
            cout << "Играет Игрок-Комп1 (1) или Комп1-Комп2 (2) ?\n";
            cin >> mode;
        }

        if (mode == "1" && cont != "2") {
            while (inp != "1" && inp != "2") {
                cout << "Комп случайно расставляет за Игрока (1) или Ручная расстановка (2) ?\n";
                cin >> inp;
            }
        }

        while (mode_pc_fire != "1" && mode_pc_fire != "2") {
            cout << "Комп1 стреляет случайно (1) или по алгоритму (2) ?\n";
            cin >> mode_pc_fire;
        }

        for (int i = 1; i < 11; i++) {
            for (int j = 1; j < 11; j++) {
                if (mode_pc_fire == "2" && cont != "2")
                {
                    PC1_Fire[i][j] = Intel_1[n];
                    PC2_Fire[i][j] = Intel_2[n];
                    n++;
                }
            }
        }
        n = 0;

        if (mode == "1") { // Человек-ПК
            if (inp == "2" && cont != "2") { // ручная расстановка
                Show(Player_Resurs, Player_Fire, "Игрок", "\t\tКомп1");
                for (int i = 5; i >= 2; i--) { // размер_5 - 1шт, размер_4 - 2шт, размер_3 - 3шт, размер_2 - 4шт
                    for (int ii = 1; ii <= 6 - i; ii++)
                    {
                        t = 0;
                        while (t == 0) {
                            while (HOR_INT < 1 || HOR_INT>10)
                            {
                                cout << "Указать верхнюю левую точку " << ii << "-го Корабля размера \"" << i << "\"" << endl;
                                cout << "A-J,a-j : ";
                                cin >> HOR;
                                HOR_INT = toupper(HOR[0]) - 64;
                            }

                            while (VERT < 1 || VERT > 10)
                            {
                                cout << "1-10 : ";
                                cin >> VERT;
                            }

                            while (VH[0] != 'V' && VH[0] != 'v' && VH[0] != 'H' && VH[0] != 'h')
                            {
                                cout << "V,v - вертикально,\nH,h - горизонтально :";
                                cin >> VH;
                            }

                            if (Check(Player_Resurs, i, HOR_INT, VERT, VH[0], 0) == 1)
                                t = 1; // при успешном вводе переход на ввод следующего корабля, иначе вводим корректные данные для текущего корабля

                            // независимо от результата ввода устанавливаем невалидные значения перед очередным вводом
                            HOR_INT = 12; // горизонтальная координата (валидна от 1 до 10)
                            VERT = 0; // вертикальная координата верхнего левого угла корабля (валидна от 1 до 10). 
                            VH = "q"; // v,V,h,H - список валидных значений первого символа
                            Show(Player_Resurs, Player_Fire, "Игрок", "\t\tКомп1");
                        }
                    }
                }
                cout << "Расстановка за Игрока завершена успешно!" << endl;
            }

            if (inp == "1" && cont != "2") while (CheckOst(Player_Resurs) != 30) InputRnd(Player_Resurs); // случайная расстановка компом за Человека
            Show(Player_Resurs, Player_Fire, "Игрок", "\t\tКомп1");

            while (CheckOst(PC1_Resurs) != 30 && cont != "2") InputRnd(PC1_Resurs); // расстановка случайным образом для ПК

            // Show(PC1_Resurs, PC1_Fire, "Корабли Компа", "Прилеты от Компа"); // подглядеть за ПК что он там расставил

            while (CheckOst(Player_Resurs) * CheckOst(PC1_Resurs) > 0)
            {
                USP = 1; // ход Человека
                while (USP == 1) {
                    HOR_INT = 0;
                    while (HOR_INT < 1 || HOR_INT>10)
                    {
                        cout << "Save play (S,s),\nFire (A-J,a-j) : ";
                        cin >> HOR;
                        HOR_INT = toupper(HOR[0]) - 64;
                        if (HOR_INT == 19)
                        {
                            cout << "Save play as " << Game << endl;
                            FILE* out;
                            if (fopen_s(&out, Game, "w") != NULL) cout << "Не могу создать файл!\n";
                            else cout << "Файл " << Game << "успешно создан!\n";
                            for (int j = 1; j < 11; j++)
                            {
                                for (int i = 1; i < 11; i++)
                                {
                                    fprintf(out, "%1c", Player_Resurs[i][j]);
                                    fprintf(out, "%1c", Player_Fire[i][j]);
                                    fprintf(out, "%1c", PC1_Resurs[i][j]);
                                    fprintf(out, "%1c", PC1_Fire[i][j]);
                                }
                            }
                            fclose(out);
                        }
                    }
                    VERT = 0;
                    while (VERT < 1 || VERT>10)
                    {
                        cout << "Fire (1-10) : ";
                        cin >> VERT_str;
                        std::istringstream(VERT_str) >> VERT;
                    }
                    if (PC1_Resurs[HOR_INT][VERT] == '.' || PC1_Resurs[HOR_INT][VERT] == 'X')
                    {
                        USP = 0;
                        cout << "Лошара! Координата уже была бита...\n";
                    }
                    if (PC1_Resurs[HOR_INT][VERT] == ' ')
                    {
                        PC1_Resurs[HOR_INT][VERT] = '.';
                        USP = 0;
                        Player_Fire[HOR_INT][VERT] = '.';
                        cout << "Мазепа...\n";
                    }
                    if (PC1_Resurs[HOR_INT][VERT] == 'O')
                    {
                        PC1_Resurs[HOR_INT][VERT] = 'X';
                        USP = 1;
                        Player_Fire[HOR_INT][VERT] = 'X';
                        cout << "Попал!!!\n";
                        if (CheckOst(PC1_Resurs) == 0) break;
                    }
                    Show(Player_Resurs, Player_Fire, "Игрок", "\t\tКомп1");
                    // Show(PC1_Resurs, PC1_Fire, "PC1_Res", "\t\tPC1_Fire");
                }

                USP = 1; // ход PC1
                while (USP == 1)
                {
                    SearchPersp(PC1_Fire);
                    if (HOR_INT * VERT == 0 && mode_pc_fire == "2") // если нет перспективных ячеек и ПК использует алгоритм, то ищем от начала в конец макс по матрице
                    {
                        max_arr = '0';
                        for (int y = 1; y < 11; y++)
                        {
                            for (int x = 1; x < 11; x++)
                            {
                                if (PC1_Fire[x][y] > '0' && PC1_Fire[x][y] < '6' && PC1_Fire[x][y] >= max_arr)
                                {
                                    max_arr = PC1_Fire[x][y];
                                    HOR_INT = x;
                                    VERT = y;
                                }
                            }
                        }
                    }
                    if (HOR_INT * VERT == 0) RndFire(PC1_Fire); // если ничего не нашлось, то генерим случайно 
                    if (Player_Resurs[HOR_INT][VERT] == 'O')  FireTrue(PC1_Fire, Player_Resurs, "Комп");
                    if (CheckOst(Player_Resurs) == 0) break;
                    if (Player_Resurs[HOR_INT][VERT] == ' ') // если комп попал в пустое
                    {
                        cout << (char)(HOR_INT + 64) << " " << VERT << " - Комп-1 промазал...\n";
                        PC1_Fire[HOR_INT][VERT] = '.';
                        USP = 0;
                        Player_Resurs[HOR_INT][VERT] = '.';
                    }
                    Show(Player_Resurs, Player_Fire, "Игрок", "Комп1");
                }
            }
            if (CheckOst(Player_Resurs) == 0 && CheckOst(PC1_Resurs) > 0) cout << "Победил Комп";
            if (CheckOst(PC1_Resurs) == 0 && CheckOst(Player_Resurs) > 0) cout << "Победил Игрок";
            if (CheckOst(PC1_Resurs) == 0 && CheckOst(Player_Resurs) == 0) cout << "Ничья. Типа Комп успел выстрелить перед кончиной";
        }


        if (mode == "2") // PC1-PC2
        {
            while (CheckOst(PC1_Resurs) != 30) InputRnd(PC1_Resurs); // случайная расстановка за PC1
            while (CheckOst(PC2_Resurs) != 30) InputRnd(PC2_Resurs); // случайная расстановка за PC2
            Show(PC1_Resurs, PC2_Resurs, "Комп1", "\t\tКомп2");
            while (CheckOst(PC2_Resurs) * CheckOst(PC1_Resurs) > 0)
            {
                USP = 1; // Ход PC1
                while (USP == 1)
                {
                    SearchPersp(PC1_Fire);
                    if (HOR_INT * VERT == 0 && mode_pc_fire == "2") // если нет перспективных ячеек и ПК использует алгоритм, то ищем от начала в конец макс по матрице
                    {
                        max_arr = '0';
                        for (int y = 1; y < 11; y++)
                        {
                            for (int x = 1; x < 11; x++)
                            {
                                if (PC1_Fire[x][y] > '0' && PC1_Fire[x][y] < '6' && PC1_Fire[x][y] >= max_arr)
                                {
                                    max_arr = PC1_Fire[x][y];
                                    HOR_INT = x;
                                    VERT = y;
                                }
                            }
                        }
                    }
                    if (HOR_INT * VERT == 0) RndFire(PC1_Fire); // если ничего не нашлось, то генерим случайно 
                    if (PC2_Resurs[HOR_INT][VERT] == 'O') FireTrue(PC1_Fire, PC2_Resurs, "Комп1");
                    if (CheckOst(PC2_Resurs) == 0) break;
                    if (PC2_Resurs[HOR_INT][VERT] == ' ') // если попал в пустое
                    {
                        cout << endl << (char)(HOR_INT + 64) << " " << VERT << " - Комп1 промазал...";
                        PC1_Fire[HOR_INT][VERT] = '.';
                        USP = 0;
                        PC2_Resurs[HOR_INT][VERT] = '.';
                    }
                    Show(PC1_Resurs, PC2_Resurs, "Комп1", "\t\tКомп2");
                }

                USP = 1; // Ход PC2
                while (USP == 1)
                {
                    SearchPersp(PC2_Fire);
                    if (HOR_INT * VERT == 0 && mode_pc_fire == "2") // если нет перспективных ячеек и ПК использует алгоритм, то ищем от начала в конец макс по матрице
                    {
                        max_arr = '0';
                        for (int y = 1; y < 11; y++)
                        {
                            for (int x = 1; x < 11; x++)
                            {
                                if (PC2_Fire[x][y] > '0' && PC2_Fire[x][y] < '6' && PC2_Fire[x][y] >= max_arr)
                                {
                                    max_arr = PC2_Fire[x][y];
                                    HOR_INT = x;
                                    VERT = y;
                                }
                            }
                        }
                    }
                    if (HOR_INT * VERT == 0) RndFire(PC2_Fire); // если ничего не нашлось, то генерим случайно 
                    if (PC1_Resurs[HOR_INT][VERT] == 'O') FireTrue(PC2_Fire, PC1_Resurs, "Комп2");
                    if (CheckOst(PC1_Resurs) == 0) break;
                    if (PC1_Resurs[HOR_INT][VERT] == ' ') // если комп-2 попал в пустое
                    {
                        cout << endl << (char)(HOR_INT + 64) << " " << VERT << " - Комп2 промазал...";
                        PC2_Fire[HOR_INT][VERT] = '.';
                        USP = 0;
                        PC1_Resurs[HOR_INT][VERT] = '.';
                    }
                    Show(PC1_Resurs, PC2_Resurs, "Комп1", "\t\tКомп2");
                }
            }

            if (CheckOst(PC2_Resurs) == 0 && CheckOst(PC1_Resurs) > 0) cout << "Победил Комп1";
            if (CheckOst(PC1_Resurs) == 0 && CheckOst(PC2_Resurs) > 0) cout << "Победил Комп2";
            if (CheckOst(PC1_Resurs) == 0 && CheckOst(PC2_Resurs) == 0) cout << "Ничья. Типа оппонент успел выстрелить перед кончиной";
            // Show(PC1_Resurs, PC2_Resurs, "Комп1", "\t\tКомп2");

        } // PC1-PC2

        for (int i = 0; i < 12; i++) {
            delete[]Player_Resurs[i];
            delete[]Player_Fire[i];
            delete[]PC1_Resurs[i];
            delete[]PC1_Fire[i];
            delete[]PC2_Resurs[i];
            delete[]PC2_Fire[i];
        }
        delete[]Player_Resurs;
        delete[]Player_Fire;
        delete[]PC1_Resurs;
        delete[]PC1_Fire;
        delete[]PC2_Resurs;
        delete[]PC2_Fire;

        cout << "\nСыграем еще? (1 - да, 0 - нет)\n";
        cin >> Repeat;
        if (Repeat == "0") return 0;
        else Repeat = "1";
    }
}