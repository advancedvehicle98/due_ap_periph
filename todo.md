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
- это чисто на будущее: как сделать настройку и управление моторами более универсальным, чтобы наш ap_periph мог не только в steering/throttle
  - нужна поддержка различных функций rc_out (т.е. у нас пока только работает groundsteering/26 и throttle/70)
  - https://ardupilot.org/rover/docs/rover-motor-and-servo-configuration.html
- добавить поддержку sabertooth

### прочее
- добавить, как будет определяться состояние armed
  - когда с этим разберёмся, надо в `srv_channel_skid::update()` поменять аргумент в строке с `_output`

## src/

### load_parameters.cpp
- доделать загрузку параметров
  - откуда берётся _var_info?

### srv_channel_skid
- добавить расчёт для учёта заносов (`slew_list`) в `_calc_pwm`
  - см. `SRV_Channels::calc_pwm()` в https://github.com/ArduPilot/ardupilot/blob/4f1e86bf41eda77f1946cd432396551683224170/libraries/SRV_Channel/SRV_Channels.cpp
  - надо ли добавлять loop lock в `_calc_pwm`
