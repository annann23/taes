#ifndef _TLEMSGDEF_H_
#define _TLEMSGDEF_H_

#include "cfe.h"

#pragma pack(1)

typedef struct
{
	uint8 TlmHeader[CFE_SB_TLM_HDR_SIZE];


	uint8 commandCounter;
	uint8 cmdErrCounter;

	uint16 boardTemp[2];
	uint16 BCROutputCurrent;
	uint16 BCROutputVoltage;

	uint16 OutputCurrent12V;
	uint16 OutputCurrentVBat;
	uint16 OutputCurrent3_3V;
	uint16 OutputCurrent5V;

	uint16 BCRCurrent[6];
	uint16 BCRVoltage[6];
	uint16 BCRTemp[6];

	uint16 BatteryVoltage;
	uint16 BatteryCurrent;
	uint16 BatteryCurrentDir;
	uint16 BatteryBoardTemp;

	uint16 PDMState;
	uint8 solarPanelDepolyFlag;



}EPS_HKTelemetry_t;

#pragma pack()

#endif
