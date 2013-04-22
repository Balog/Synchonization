//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ActnList.hpp>
#include <ActnMan.hpp>
#include <Buttons.hpp>
#include <ImgList.hpp>
#include <StdActns.hpp>
#include <DBActns.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TPageControl *PageControl2;
        TTabSheet *TabSheet4;
        TTabSheet *TabSheet5;
        TTreeView *TreeView1;
        TTreeView *TreeView2;
        TButton *Button1;
        TButton *Button2;
        TTabSheet *TabSheet6;
        TPageControl *PageControl1;
        TTabSheet *TabSheet1;
        TMemo *Memo1;
        TTabSheet *TabSheet2;
        TTabSheet *TabSheet3;
        TButton *Button3;
        TButton *Button4;
        TPageControl *PageControl3;
        TTabSheet *TabSheet7;
        TMemo *Memo3;
        TMemo *Memo4;
        TButton *Button5;
        TTabSheet *TabSheet8;
        TTabSheet *TabSheet9;
        TButton *Button6;
        TMemo *Memo2;
        TStringGrid *SG;
        TTreeView *TreeView3;
        TButton *Button7;
        TButton *Button8;
        TButton *Button9;
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
