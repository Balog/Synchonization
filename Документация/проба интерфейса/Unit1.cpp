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
SG->Cells[0][0]="�����";
SG->Cells[1][0]="���";
SG->Cells[2][0]="������";

SG->Cells[0][1]="Ivanov";
SG->Cells[1][1]="������ �.�.";

SG->Cells[0][2]="Petrov";
SG->Cells[1][2]="������ �.�.";

SG->Cells[0][3]="Sidorov";
SG->Cells[1][3]="������� �.�.";

SG->Cells[0][4]="Zalevsky";
SG->Cells[1][4]="��������� �.�.";

}
//---------------------------------------------------------------------------




