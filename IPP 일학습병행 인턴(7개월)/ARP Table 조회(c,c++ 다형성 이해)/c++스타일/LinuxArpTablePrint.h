#include "ArpTablePrint.h"

class LinuxArpTablePrint : public ArpTablePrint{
public:
  LinuxArpTablePrint();
  virtual ~LinuxArpTablePrint();
  virtual void print();
};