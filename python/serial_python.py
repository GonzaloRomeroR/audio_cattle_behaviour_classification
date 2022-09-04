import serial

ser = serial.Serial()


def configure():
    ser.baudrate = 115200
    ser.port = "COM6"
    ser.timeout = 10


def open_communication():
    print("Opening serial communication")
    ser.open()


def communicate():
    while True:
        value = input()
        if value == "q":
            break
        print("Writing bytes")
        ser.write(bytes(value, "utf-8"))
        print("Reading bytes")
        print(ser.read(13))


def close_communication():
    print("Close serial communication")
    ser.close()


def main():
    configure()
    open_communication()
    communicate()
    close_communication()


if __name__ == "__main__":
    main()
