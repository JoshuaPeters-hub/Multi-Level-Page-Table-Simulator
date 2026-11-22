#include <bitset>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define MemSize (65536)

class PhyMem {
 public:
  bitset<32> readdata;
  PhyMem() {
    DMem.resize(MemSize);
    ifstream dmem;
    string line;
    int i = 0;
    dmem.open("pt_initialize.txt");
    if (dmem.is_open()) {
      while (getline(dmem, line)) {
        DMem[i] = bitset<8>(line);
        i++;
      }
    } else
      cout << "Unable to open page table init file";
    dmem.close();
  }
  bitset<32> outputMemValue(bitset<12> Address) {
    bitset<32> readdata;
    /**TODO: implement!
     * Returns the value stored in the physical address
     */
    unsigned long address = Address.to_ulong();
    unsigned long value = 0;
    for (int i = 0; i < 4; i++) {
      value = (value << 8) | DMem[address + i].to_ulong();
    }
    readdata = bitset<32>(value);

    return readdata;
  }

 private:
  vector<bitset<8> > DMem;
};

int main(int argc, char* argv[]) {
  PhyMem myPhyMem;

  ifstream traces;
  ifstream PTB_file;
  ofstream tracesout;

  string outname;
  outname = "pt_results.txt";

  traces.open(argv[1]);
  PTB_file.open(argv[2]);
  tracesout.open(outname.c_str());

  // Initialize the PTBR
  bitset<12> PTBR;
  PTB_file >> PTBR;

  string line;
  bitset<14> virtualAddr;

  /*********************************** ↓↓↓ Todo: Implement by you ↓↓↓
   * ******************************************/

  // Read a virtual address from the PageTable and convert it to the physical
  // address - CSA23
  if (traces.is_open() && tracesout.is_open()) {
    while (getline(traces, line)) {
      // TODO: Implement!
      //  Access the outer page table
      virtualAddr = bitset<14>(line);
      unsigned long outerIdx = (virtualAddr.to_ulong() >> 10) & 0xF;
      bitset<12> outerPTEAddr(PTBR.to_ulong() + (outerIdx * 4));
      bitset<32> outerPTE = myPhyMem.outputMemValue(outerPTEAddr);
      int outerValid = outerPTE[0];
      int innerValid = 0;
      int physicalAddress = 0;
      unsigned long memValue = 0;

      // If outer page table valid bit is 1, access the inner page table
      if (outerValid == 1) {
        unsigned long innerBase = (outerPTE.to_ulong() >> 20) & 0xFFF;
        unsigned long innerIdx = (virtualAddr.to_ulong() >> 6) & 0xF;
        unsigned long innerPTEAddr = innerBase + (innerIdx * 4);
        bitset<32> innerPTE = myPhyMem.outputMemValue(bitset<12>(innerPTEAddr));
        innerValid = innerPTE[0];

        if (innerValid == 1) {
          unsigned long frame = (innerPTE.to_ulong() >> 26) & 0x3F;
          unsigned long offset = virtualAddr.to_ulong() & 0x3F;
          physicalAddress = (frame << 6) | offset;
          memValue = myPhyMem.outputMemValue(bitset<12>(physicalAddress)).to_ulong();
        }
      }

      tracesout << outerValid << ", " << innerValid << ", 0x"
                << hex << setfill('0') << setw(3) << physicalAddress 
                << ", 0x" << setw(8) << memValue
                << dec << endl;

      

      // Return valid bit in outer and inner page table, physical address, and
      // value stored in the physical memory.
      //  Each line in the output file for example should be: 1, 0, 0x000,
      //  0x00000000
    }
    traces.close();
    tracesout.close();
  }

  else
    cout << "Unable to open trace or traceout file ";

  return 0;
}