#include "ArpTablePrint.h"

class WinArpTablePrint : public ArpTablePrint{
public:
  WinArpTablePrint(){};
  virtual ~WinArpTablePrint();
  virtual void print();
};