from socket import timeout
from communication import UARTCommunication


def communicate(communication):
    while True:
        value = input()
        if value == "q":
            break
        print("Writing bytes")
        communication.send_data(value)
        print("Reading bytes")
        print(communication.receive_data(len(value)).decode("utf-8"))


def main():
    uart = UARTCommunication()
    uart.configure(port="COM6", baudrate=115200, timeout=10)
    uart.open_communication()
    communicate(uart)
    uart.close_communication()


if __name__ == "__main__":
    main()
