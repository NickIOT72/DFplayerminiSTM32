# DF Player Mini for STM32

This DF Player Mini library is based on this [repository](https://github.com/DFRobot/DFRobotDFPlayerMini) with the objective to adapt it for CMAKE project with STM32 VS Code extension

On this library is set up a Serial module used to adapt the DF Player Mini library. Also, it's added an example based on this [one](https://github.com/DFRobot/DFRobotDFPlayerMini/blob/master/examples/FullFunction/FullFunction.ino)

## Set up project 

* If you are using NUCLEO-F446RE, just open the .ioc with [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)

* If you are using any other STM32 board:
     - Initiate a new project on STM32CubeMX selecting the board you want to implement
     - Configure any UART/USART peripheral as asynchronous with baudrate of 9600
     - Configure timer TIM2 or TIM3 in order to get a period of time of 1 ms (Review .ioc file for details on the implementation)


## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.
