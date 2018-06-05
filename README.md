# kopter
Папки
* camera_controller - проект в IAR, код для маленькой белой платки. USB-VirtualCOMport, может выдавать PPM, может выдавать данные в UART. Serial Number 0000000000507 (usbd_desc.c)  
* intros_controller - проект в IAR, код для платки интроса. USB-VirtualCOMport. При включении дергает оптопару, которая жмет кнопку включения интроса. Может выдавать данные в UART, причем ожидает от интроса ответов и пробует снова при ошибке передачи. Serial Number 000000000502 (usbd_desc.c) 
* lidar_controller - проект в IAR, код для маленькой белой платки. USB-VirtualCOMport, может выдавать PPM - для сервы, может выдавать данные в UART (не требуется). Serial Number 00000000001A(usbd_desc.c)  
* pultESP8266 - проект в VisualStudio Code + PlatformIO (Arduino framework) для ESP8266 WiFi-модуля пульта, подключается к TCP-серверу планшета и работает как мост UART-TCP в обе стороны - от контроллера кнопок к программе на планшете
* python - код на Python 2.7. Как для пульта, так и для коптера
* copter_control_apps_csharp - программка для вывода видео с платы видеозахвата и управления через графические кнопки на экране коптером; программка для отправки команд на коптер; написано на SharpDevelop (C# .NET, легковесная альтернатива VisualStudio)
