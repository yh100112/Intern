#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#pragma once

class ArpTablePrint{
public:
  ArpTablePrint();
  virtual ~ArpTablePrint();
  virtual void print() = 0;
};