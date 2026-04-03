## трём лишние зависимости
- причесать следующие стыренные файлы и убрать из них ненужные вещи:
  - include/vector3.h (из ardupilot)
  - include/rotations.h (из ardupilot)
  - include/ftype.h (из ardupilot)
- библиотеки
  - dual-vnh5019-motor-shield
  - FlyskyIBUS

## качественные изменения

### общение с главным автопилотом
- как ap_periph вообще тянет данные по can (и какие это данные)

### управление моторами
- это чисто на будущее: как сделать настройку и управление моторами более универсальным, чтобы наш ap_periph мог не только в skid steering
  - нужна поддержка различных функций rc_out
  - https://ardupilot.org/rover/docs/rover-motor-and-servo-configuration.html
- добавить поддержку sabertooth
- переместить низкоуровневые взаимодействия с моторами в srv_channel
- доделать `ap_wheel_rate_control`
  - доделать PID-регулятор (`ac_pid`)
	- потом надо его настроить

### прочее
- добавить, как будет определяться состояние armed
  - когда с этим разберёмся, надо в `ap_periph::update()` поменять аргумент в строке с `_motors.update()`

## src/

### load_parameters.cpp
- доделать загрузку параметров
  - откуда берётся _var_info?

### srv_channel_skid
- добавить расчёт для учёта заносов (`slew_list`) в `_calc_pwm`
  - см. `SRV_Channels::calc_pwm()` в https://github.com/ArduPilot/ardupilot/blob/4f1e86bf41eda77f1946cd432396551683224170/libraries/SRV_Channel/SRV_Channels.cpp
  - надо ли добавлять loop lock в `_calc_pwm`
