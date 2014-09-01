/**
  ******************************************************************************
  * @file    main.c
  * @author  Khusainov Timur
  * @version 0.0.0.3
  * @date    14.02.2012
  * @brief    STM8S208 CAN Speed Calculator
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2012 timypik@gmail.com </center></h2>
  ******************************************************************************
  */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#include <vector>
#include <string>
//------------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------------
typedef struct
{
	struct
	{
	   float Q_time;
	   float BS1_time;
	   float BS2_time;
	   float NB_time;
	};

	struct
	{
		uint16_t BRP;
		uint8_t  BS1;
		uint8_t  BS2;
		uint8_t  SJW;
	};

	float BaudRate;
	float BR_Error;
	float SamplePoint;
	float SP_Error;
}tCANSpeedSetup;

typedef struct
{
	uint32_t Baud;
	vector<tCANSpeedSetup> Setup;
} tCANBaud;

typedef vector<tCANBaud> tCANBauds;
//------------------------------------------------------------------------------
void CANCalc(tCANBauds *out, float f_osc, float SamplePiont, float SamplePiontErr)
{
	for (uint16_t m_brp = 1; m_brp <= 32; ++m_brp)
	{
		for (uint8_t m_bs1 = 1; m_bs1 <= 16; ++m_bs1)
		{
			for (uint8_t m_bs2 = 1; m_bs2 <= 8; ++m_bs2)
			{
				float
				_tq  = m_brp/((float)f_osc*1e6),
				_ts1 = _tq * (float)m_bs1,
				_ts2 = _tq * (float)m_bs2,
				_nbt = _tq + _ts1 + _ts2,
				_nbr = 1/_nbt,
				_sp  = ((_tq + _ts1)/_nbt)*100,
				_spe = fabs(_sp - SamplePiont);

				/* need calc osc tolerance */

				if (_spe < SamplePiontErr)
				{
					for (size_t i = 0; i < out->size(); ++i)
					{
						uint32_t _baud = out->at(i).Baud*1e3;
						float _baud_err = fabs(_baud - _nbr);
						
						if (_baud_err < (_baud * 1e-6))
						{
							tCANSpeedSetup _css;

							_css.Q_time   = _tq;
							_css.BS1_time = _ts1;
							_css.BS2_time = _ts2;
							_css.NB_time  = _nbt;
							_css.BRP      = m_brp;
							_css.BS1      = m_bs1;
							_css.BS2      = m_bs2;
							_css.SJW      = min(3, (m_bs2 - 1));
							_css.BaudRate = _nbr;
							_css.BR_Error = (_baud_err/_baud) * 100;
							_css.SamplePoint = _sp;
							_css.SP_Error = (_spe/SamplePiont) * 100;

							out->at(i).Setup.push_back(_css);
						}
					}
				}
			}
		}
	}
}
//------------------------------------------------------------------------------
void main(int argc, char* argv[])
{
	//----------------------------------------------------------------------------
	//uint32_t f_mhz = 11.059200*3;
	//uint32_t f_mhz = 12*3;
	//uint32_t f_mhz = 12;
	float f_mhz = 12;
	//float f_mhz = 14.7456;
	//float f_mhz = 11.0592;
	float sp_proc = 87.5;
	float sp_err = 5;

	printf("Settings: F_CPU - %fMHz, SP - %.2f, SRE - %.2f\n\n", f_mhz, sp_proc, sp_err);
	//----------------------------------------------------------------------------
	// Feel need speed
	//----------------------------------------------------------------------------
	vector<uint32_t> can_baud;
	
	can_baud.push_back(1000);
	can_baud.push_back(800);
	can_baud.push_back(500);
	can_baud.push_back(250);
	can_baud.push_back(125);
	can_baud.push_back(50);
	can_baud.push_back(20);
	can_baud.push_back(10);
	
	tCANBauds CANBauds;

	for (size_t i = 0; i < can_baud.size(); ++i)
	{
		tCANBaud _cb;
		_cb.Baud = can_baud[i];
		CANBauds.push_back(_cb);
	}
	//----------------------------------------------------------------------------
	// Start calculation
	//----------------------------------------------------------------------------
	CANCalc(&CANBauds, f_mhz, sp_proc, sp_err);
	//----------------------------------------------------------------------------
	// Print resul table
	//----------------------------------------------------------------------------
	printf("BR\tBR_Error\tSP\tSP_Error\tBRP BS1 BS2 SJW\n");
	
	for (size_t i = 0; i < CANBauds.size(); ++i)
	{
		vector<tCANSpeedSetup> *_csb = &CANBauds.at(i).Setup;

		for (size_t j = 0; j < _csb->size(); ++j)
		{
			tCANSpeedSetup *_css = &_csb->at(j);

			printf("%.f\t(%e)\t%.2f\t(%.2e)\t" "%3d %2d %1d %1d\n",
			(_css->BaudRate/1000),
			_css->BR_Error,
			_css->SamplePoint,
			_css->SP_Error,
			_css->BRP,
			_css->BS1,
			_css->BS2,
			_css->SJW);
		}
	}
	//----------------------------------------------------------------------------
	printf("\nPress any key..");
	getchar();
	//----------------------------------------------------------------------------
	return 0;
}
//------------------------------------------------------------------------------