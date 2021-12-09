#include "DeviceListener.h"

#include <iostream>

int main() {
  int device_event_flag = 0;

  DeviceListener *pListener;  // construtor sem parâmetros

  while (1) {
    switch (device_event_flag) {
      case 0:
        break;
      case 1:
        std::cout << "Dispositivo conectado." << std::endl;
        break;
      case 2:
        std::cout << "Dispositivo desconectado." << std::endl;
        break;
      default:
        std::cout << "Evento desconhecido detectado." << std::endl;
        break;
    }
  }
  return 0;
}