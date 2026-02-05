// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicalData.h"
#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include <tchar.h>

HANDLE serialPortH;

bool UPhysicalData::DeviceConnection(FString port)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Device Data"));

	FString portCombo = "COM" + port;

	std::string portS = TCHAR_TO_UTF8(*portCombo);
	std::wstring portWS;
	portWS.assign(portS.begin(), portS.end());

	serialPortH = CreateFile(portWS.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (serialPortH == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Device Not Found"));
			UE_LOG(LogTemp, Error, TEXT("IFMagic Device: Device Not Found on Com Port %s"), *port);
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error"));
		UE_LOG(LogTemp, Error, TEXT("IFMagic Device: Error!"));
		return false;
	}

	DCB dcbSP = { 0 };

	dcbSP.DCBlength = sizeof(dcbSP);

	if (!GetCommState(serialPortH, &dcbSP)) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("State Error"));
		UE_LOG(LogTemp, Error, TEXT("IFMagic Device: State Error!"));
		return false;
	}

	dcbSP.BaudRate = CBR_115200;
	dcbSP.ByteSize = 8;
	dcbSP.StopBits = ONESTOPBIT;
	dcbSP.Parity = NOPARITY;

	if (!SetCommState(serialPortH, &dcbSP)) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Serial Port Set Error"));
		UE_LOG(LogTemp, Error, TEXT("IFMagic Device: Serial Port Set Error!"));
		return false;
	}

	COMMTIMEOUTS timeouts = { 0 };

	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(serialPortH, &timeouts)) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Timeout Error"));
		UE_LOG(LogTemp, Error, TEXT("IFMagic Device: Timeout Error!"));
		return false;
	}
	return true;
}

void UPhysicalData::DataObject(FString &data) {
	char szBuff[256] = { 0 };
	DWORD dwBytesRead = 0;

	std::string s = "";
	std::string fData = "";

	if (!ReadFile(serialPortH, szBuff, 256, &dwBytesRead, NULL)) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Port Closed"));
		UE_LOG(LogTemp, Warning, TEXT("IFMagic Device: Port Closed!"));
	}
	else {
		
		for (int i = 0; i < sizeof(szBuff); i++) {
			s = s + szBuff[i];
		}

		std::istringstream iss(s);
		getline(iss, fData);

		data = UTF8_TO_TCHAR(fData.c_str());
	}
 
}

void UPhysicalData::DeviceConclude()
{
	CloseHandle(serialPortH);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Port Closed"));
	UE_LOG(LogTemp, Log, TEXT("IFMagic Device: Port is now closed."));
}

void UPhysicalData::DeviceOutput(FString value) {
	char szBuff[256] = { 0 };
	DWORD dwBytesRead = 0;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, (TEXT("%s"), value));


	std::string val = std::string(TCHAR_TO_UTF8(*value));

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, (TEXT("%s"), UTF8_TO_TCHAR(val.c_str())));


	for (int i = 0; i < 40; i++) {
		szBuff[i] = val[i];
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, (TEXT("Data %s"), UTF8_TO_TCHAR(szBuff)));

	if (WriteFile(serialPortH, szBuff, 256, &dwBytesRead, NULL)) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, (TEXT("Data %s"), UTF8_TO_TCHAR(szBuff)));
	}
}

void UPhysicalData::RawData(const int port, const FString value, FString &portData) {
	std::string val = std::string(TCHAR_TO_UTF8(*value));

	for (int i = 0; i < port; i++) {
		val = val.erase(0, (val.find(";") +1));
	}
	portData = UTF8_TO_TCHAR(val.substr(0, val.find(";")).c_str());
}

void UPhysicalData::Orientation(const FString data, float& xAxis, float& yAxis, float& zAxis) {
	std::string _data = std::string(TCHAR_TO_UTF8(*data));

	if (_data.length() > 9) {
		std::string orientation = _data.substr(0, _data.find(";"));
		std::string _xAxis = orientation.substr(0, orientation.find(":"));
		if (_xAxis.length() > 0) {
			xAxis = std::stof(_xAxis);
		}

		orientation = orientation.erase(0, (orientation.find(":") + 1));
		std::string _yAxis = orientation.substr(0, orientation.find(":"));
		if (_yAxis.length() > 0) {
			yAxis = std::stof(_yAxis);
		}

		orientation = orientation.erase(0, (orientation.find(":") + 1));
		std::string _zAxis = orientation.substr(0, orientation.find(":"));
		if (_zAxis.length() > 0) {
			zAxis = std::stof(_zAxis);
		}
	}
}

bool UPhysicalData::Button(const int port, const FString data, int& state) {
	std::string val = std::string(TCHAR_TO_UTF8(*data));

	for (int i = 0; i < port; i++) {
		val = val.erase(0, (val.find(";") + 1));
	}
	std::string type = val.substr(0, val.find(":"));

	if (type == "1") {
		val = val.erase(0, (val.find(":") + 1));
		val = val.erase(0, (val.find(":") + 1));
		std::string _state = val.substr(0, val.find(":"));
		if (_state.length() > 0) {
			state = std::stoi(_state);
		}
		
		return true;
	}
	else {
		return false;
	}
}

bool UPhysicalData::Joystick(const int port, const FString data, int& xAxis, int& yAxis) {
	std::string val = std::string(TCHAR_TO_UTF8(*data));

	for (int i = 0; i < port; i++) {
		val = val.erase(0, (val.find(";") + 1));
	}
	std::string type = val.substr(0, val.find(":"));
	val = val.erase(0, (val.find(":") + 1));
	std::string sensor = val.substr(0, val.find(":"));

	if (type == "3" && sensor == "1") {
		val = val.erase(0, (val.find(":") + 1));
		std::string _xAxis = val.substr(0, val.find(","));
		if (_xAxis.length() > 0) {
			xAxis = std::stoi(_xAxis);
		}
		val = val.erase(0, (val.find(",") + 1));
		std::string _yAxis = val.substr(0, val.find(":"));
		if (_yAxis.length() > 0) {
			yAxis = std::stoi(_yAxis);
		}

		return true;
	}
	else {
		return false;
	}
}

bool UPhysicalData::Light(const int port, const FString data, int& brightness) {
	std::string val = std::string(TCHAR_TO_UTF8(*data));

	for (int i = 0; i < port; i++) {
		val = val.erase(0, (val.find(";") + 1));
	}

	std::string sensor = val.substr(0, val.find(":"));

	if (sensor == "2") {
		val = val.erase(0, (val.find(":") + 1));
		std::string _brightness = val.substr(0, val.find(":"));
		if (_brightness.length() > 0) {
			brightness = std::stoi(_brightness);
		}

		return true;
	}
	else {
		return false;
	}
}

bool UPhysicalData::Distance(const int port, const FString data, float& distance) {
	std::string val = std::string(TCHAR_TO_UTF8(*data));

	for (int i = 0; i < port; i++) {
		val = val.erase(0, (val.find(";") + 1));
	}
	std::string sensor = val.substr(0, val.find(":"));

	if (sensor == "8") {
		val = val.erase(0, (val.find(":") + 1));
		std::string _distance = val.substr(0, val.find(":"));
		if (_distance.length() > 0) {
			distance = std::stof(_distance);
		}

		return true;
	}
	else {
		return false;
	}
}

bool UPhysicalData::Proximity(const int port, const FString data, int& proximity) {
	std::string val = std::string(TCHAR_TO_UTF8(*data));

	for (int i = 0; i < port; i++) {
		val = val.erase(0, (val.find(";") + 1));
	}
	std::string type = val.substr(0, val.find(":"));
	val = val.erase(0, (val.find(":") + 1));
	std::string sensor = val.substr(0, val.find(":"));

	if (type == "6" && sensor == "2") {
		val = val.erase(0, (val.find(":") + 1));
		std::string _proximity = val.substr(0, val.find(":"));
		if (_proximity.length() > 0) {
			proximity = std::stoi(_proximity);
		}

		return true;
	}
	else {
		return false;
	}
}

bool UPhysicalData::Gesture(const int port, const FString data, FString& direction) {
	std::string val = std::string(TCHAR_TO_UTF8(*data));

	for (int i = 0; i < port; i++) {
		val = val.erase(0, (val.find(";") + 1));
	}
	std::string type = val.substr(0, val.find(":"));
	val = val.erase(0, (val.find(":") + 1));
	std::string sensor = val.substr(0, val.find(":"));

	if (type == "6" && sensor == "1") {
		val = val.erase(0, (val.find(":") + 1));
		std::string _direction = val.substr(0, val.find(";"));

		direction = UTF8_TO_TCHAR(_direction.c_str());

		return true;
	}
	else {
		return false;
	}
}


FString UPhysicalData::Tone(int port, int pitch) {
	std::string _port = std::to_string(port);
	std::string _vol = "FF";
	std::string _pitch = std::to_string(pitch);

	std::string val = _port + "vt" + _vol + _pitch;

	return  UTF8_TO_TCHAR(val.c_str());
}

FString UPhysicalData::Servo(int port, int degree) {
	std::string _port = std::to_string(port);
	std::string _degree = std::to_string(degree);

	std::string val = _port + "vs" + _degree;

	return  UTF8_TO_TCHAR(val.c_str());
}

FString UPhysicalData::LED(int port, ELEDColor color) {

	std::string _port = std::to_string(port);
	std::string _color;

	if (color == ELEDColor::White) {
		_color = _port + "vl" + "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
	}
	else if (color == ELEDColor::Red) {
		_color = _port + "vl" + "FF0000FF0000FF0000FF0000FF0000FF0000";
	}
	else if (color == ELEDColor::Green) {
		_color = _port + "vl" + "00FF0000FF0000FF0000FF0000FF0000FF00";
	}
	else if (color == ELEDColor::Blue) {
		_color = _port + "vl" + "0000FF0000FF0000FF0000FF0000FF0000FF";
	}
	else if (color == ELEDColor::Purple) {
		_color = _port + "vl" + "FF00FFFF00FFFF00FFFF00FFFF00FFFF00FF";
	}
	else if (color == ELEDColor::Orange) {
		_color = _port + "vl" + "FF2F00FF2F00FF2F00FF2F00FF2F00FF2F00";
	}
	else if (color == ELEDColor::Yellow) {
		_color = _port + "vl" + "FFA000FFA000FFA000FFA000FFA000FFA000";
	}
	else if (color == ELEDColor::Aqua) {
		_color = _port + "vl" + "00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF";
	}
	else if (color == ELEDColor::Pink) {
		_color = _port + "vl" + "FF4FFFFF4FFFFF4FFFFF4FFFFF4FFFFF4FFF";
	}
	else if (color == ELEDColor::Off) {
		_color = _port + "vl" + "000000000000000000000000000000000000";
	}
	 
	return  UTF8_TO_TCHAR(_color.c_str());
}


