#pragma once

#define PORT_PM_CMD_STATUS 0  // Write Command, Read Status
#define PORT_PM_CMD_DATAL  1  // Command Data L
#define PORT_PM_CMD_DATAH  2  // Command Data H
#define PORT_TEST          3  // Test port
// Test Port : Can be used to test the board ports
// - Each time a value is read, answer +1
// - Each time a value is written, increment a counter in register, if not = Previous +1, except 0 to initialize

__force_inline uint8_t GetPortType(uint32_t port)
{ if (port<0x3FF) return PORT_Table[port>>3];
     else return 0xFF;
}

// Set "Size" consecutive block to "Type" in the Port type table 
__force_inline void SetPortType(uint32_t port,uint8_t Type, uint8_t Size)
{
  for (int i=0;i<Size;i++) 
  { PORT_Table[(port>>3)+i] = Type; }
}

// Delete a Device Type from the Port table
__force_inline void DelPortType(uint8_t Type)
{
  for (int i=0;i<128;i++)
  { if (PORT_Table[i]==Type) PORT_Table[i]=DEV_NULL; }
}

extern volatile bool PM_DoIOR_B;
extern volatile bool PM_DoIOR_W;
extern volatile bool PM_DoIOW_B;
extern volatile bool PM_DoIOW_W;
extern uint8_t *PM_IORW_Base;   // Pointer to the table used by the IO Read / Write command
extern volatile uint16_t PM_IORW_Size;      // Max size of the IO Read / Write command

extern uint8_t dev_pmio_install();
extern void dev_pmio_remove(uint16_t BasePort);
extern void dev_pmio_update();
extern bool dev_pmio_ior(uint32_t CTRL_AL8,uint8_t *Data );
extern void dev_pmio_iow(uint32_t CTRL_AL8,uint8_t ISAIOW_Data);