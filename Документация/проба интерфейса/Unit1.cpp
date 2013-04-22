//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
SG->Cells[0][0]="Логин";
SG->Cells[1][0]="ФИО";
SG->Cells[2][0]="Запись";

SG->Cells[0][1]="Ivanov";
SG->Cells[1][1]="Иванов Р.П.";

SG->Cells[0][2]="Petrov";
SG->Cells[1][2]="Петров А.Т.";

SG->Cells[0][3]="Sidorov";
SG->Cells[1][3]="Сидоров М.К.";

SG->Cells[0][4]="Zalevsky";
SG->Cells[1][4]="Залевский Е.Н.";

}
//---------------------------------------------------------------------------




