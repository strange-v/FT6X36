#include "FT6X36.h"

FT6X36 * FT6X36::_instance = nullptr;

FT6X36::FT6X36(TwoWire * wire, int8_t intPin)
{
	_instance = this;
	_wire = wire;
	_intPin = intPin;
}

FT6X36::~FT6X36()
{
	detachInterrupt(digitalPinToInterrupt(_intPin));
}

void FT6X36::isr()
{
	if (_instance)
		_instance->onInterrupt();
}

bool FT6X36::begin(uint8_t threshold)
{
	if (readRegister8(FT6X36_REG_PANEL_ID) != FT6X36_VENDID)
		return false;

	uint8_t id = readRegister8(FT6X36_REG_CHIPID);
	if (id != FT6206_CHIPID && id != FT6236_CHIPID && id != FT6336_CHIPID)
		return false;

	attachInterrupt(digitalPinToInterrupt(_intPin), FT6X36::isr, FALLING);

	writeRegister8(FT6X36_REG_DEVICE_MODE, 0x00);
	writeRegister8(FT6X36_REG_THRESHHOLD, threshold);
	writeRegister8(FT6X36_REG_TOUCHRATE_ACTIVE, 0x0E);
	
	writeRegister8(FT6X36_REG_OFFSET_LEFT_RIGHT, (uint8_t)10);
	writeRegister8(FT6X36_REG_OFFSET_UP_DOWN, (uint8_t)10);
	writeRegister8(FT6X36_REG_DISTANCE_LEFT_RIGHT, (uint8_t)50);
	writeRegister8(FT6X36_REG_DISTANCE_UP_DOWN, (uint8_t)50);
	writeRegister8(FT6X36_REG_DISTANCE_ZOOM, (uint8_t)50);

	return true;
}

void FT6X36::registerTouchHandler(void(*fn)())
{
	_handler = fn;
}

uint8_t FT6X36::touched(void)
{
	uint8_t n = readRegister8(FT6X36_REG_NUM_TOUCHES);
	if (n > 2)
	{
		n = 0;
	}
	return n;
}

TPoint FT6X36::getPoint(uint8_t n)
{
	readData();

	TPoint p {0, 0, 0};

	p.x = _touchX[n];
	p.y = _touchY[n];
	p.event = _touchEvent[n];

	return p;
}

uint8_t FT6X36::getState()
{
	Serial.print("TH_DIFF: ");
	Serial.println(readRegister8(FT6X36_REG_FILTER_COEF));
	Serial.print("CTRL: ");
	Serial.println(readRegister8(FT6X36_REG_CTRL));
	Serial.print("TIMEENTERMONITOR: ");
	Serial.println(readRegister8(FT6X36_REG_TIME_ENTER_MONITOR));
	Serial.print("PERIODACTIVE: ");
	Serial.println(readRegister8(FT6X36_REG_TOUCHRATE_ACTIVE));
	Serial.print("PERIODMONITOR: ");
	Serial.println(readRegister8(FT6X36_REG_TOUCHRATE_MONITOR));
	Serial.print("RADIAN_VALUE: ");
	Serial.println(readRegister8(FT6X36_REG_RADIAN_VALUE));
	Serial.print("OFFSET_LEFT_RIGHT: ");
	Serial.println(readRegister8(FT6X36_REG_OFFSET_LEFT_RIGHT));
	Serial.print("OFFSET_UP_DOWN: ");
	Serial.println(readRegister8(FT6X36_REG_OFFSET_UP_DOWN));
	Serial.print("DISTANCE_LEFT_RIGHT: ");
	Serial.println(readRegister8(FT6X36_REG_DISTANCE_LEFT_RIGHT));
	Serial.print("DISTANCE_UP_DOWN: ");
	Serial.println(readRegister8(FT6X36_REG_DISTANCE_UP_DOWN));
	Serial.print("DISTANCE_ZOOM: ");
	Serial.println(readRegister8(FT6X36_REG_DISTANCE_ZOOM));
	Serial.print("CIPHER: ");
	Serial.println(readRegister8(FT6X36_REG_CHIPID));
	Serial.print("G_MODE: ");
	Serial.println(readRegister8(FT6X36_REG_INTERRUPT_MODE));
	Serial.print("PWR_MODE: ");
	Serial.println(readRegister8(FT6X36_REG_POWER_MODE));
	Serial.print("FIRMID: ");
	Serial.println(readRegister8(FT6X36_REG_FIRMWARE_VERSION));
	Serial.print("FOCALTECH_ID: ");
	Serial.println(readRegister8(FT6X36_REG_PANEL_ID));
	Serial.print("STATE: ");
	Serial.println(readRegister8(FT6X36_REG_STATE));

	return 0;
}

void FT6X36::onInterrupt()
{
	if (_handler)
	{
		_handler();
	}
}

void FT6X36::readData(void)
{
	const uint8_t size = 16;
	uint8_t data[size];
	_wire->beginTransmission(FT6X36_ADDR);
	_wire->write(0);
	_wire->endTransmission();

	_wire->requestFrom((uint8_t)FT6X36_ADDR, size);
	for (uint8_t i = 0; i < size; i++)
		data[i] = Wire.read();

#ifdef FT6X36_DEBUG
	Serial.println("REGISTERS:");
	for (int16_t i = 0; i < size; i++) {
		Serial.print("0x");
		Serial.print(i, HEX);
		Serial.print(" = 0x");
		Serial.println(data[i], HEX);
	}

	Serial.println();
	Serial.print("TOUCHES: ");
	Serial.println(data[FT6X36_REG_NUM_TOUCHES]);
	Serial.print("GESTURE: ");
	Serial.println(data[FT6X36_REG_GESTURE_ID]);
#endif
	_touches = data[FT6X36_REG_NUM_TOUCHES];
	//if (_touches > 2)
	//	_touches = 0;

	const uint8_t addrShift = 6;
	for (uint8_t i = 0; i < 2; i++)
	{
		_touchX[i] = data[FT6X36_REG_P1_XH + i * addrShift] & 0x0F;
		_touchX[i] <<= 8;
		_touchX[i] |= data[FT6X36_REG_P1_XL + i * addrShift];
		_touchY[i] = data[FT6X36_REG_P1_YH + i * addrShift] & 0x0F;
		_touchY[i] <<= 8;
		_touchY[i] |= data[FT6X36_REG_P1_YL + i * addrShift];
		_touchEvent[i] = data[FT6X36_REG_P1_XH + i * addrShift] >> 6;
	}
}

void FT6X36::writeRegister8(uint8_t reg, uint8_t value)
{
	_wire->beginTransmission(FT6X36_ADDR);
	_wire->write(reg);
	_wire->write(value);
	_wire->endTransmission();
}

uint8_t FT6X36::readRegister8(uint8_t reg)
{
	_wire->beginTransmission(FT6X36_ADDR);
	_wire->write(reg);
	_wire->endTransmission();

	_wire->requestFrom((uint8_t)FT6X36_ADDR, (uint8_t)1);
	uint8_t value = _wire->read();

#ifdef I2C_DEBUG
	Serial.print("REG 0x");
	Serial.print(reg, HEX);
	Serial.print(": 0x");
	Serial.println(value, HEX);
#endif

	return value;
}