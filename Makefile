all: STM8S208-CAN-Speed-Calculator
	@./STM8S208-CAN-Speed-Calculator

STM8S208-CAN-Speed-Calculator: main.cpp
	@g++ main.cpp -o STM8S208-CAN-Speed-Calculator
