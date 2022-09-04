from abc import abstractmethod
import serial


class Communication:
    @abstractmethod
    def configure(self):
        pass

    @abstractmethod
    def open_communication(self):
        pass

    @abstractmethod
    def close_communication(self):
        pass

    @abstractmethod
    def send_data(self, data):
        pass

    @abstractmethod
    def receive_data(self, size):
        pass

    @abstractmethod
    def close_communication(self):
        pass


class UARTCommunication:
    def __init__(self) -> None:
        self.ser = serial.Serial()

    def configure(self, **kwargs):
        self.ser.baudrate = kwargs["baudrate"]
        self.ser.port = kwargs["port"]
        self.ser.timeout = kwargs["timeout"]

    def open_communication(self):
        print("Opening serial communication")
        self.ser.open()

    def close_communication(self):
        print("Close serial communication")
        self.ser.close()

    def send_data(self, value):
        self.ser.write(bytes(value, "utf-8"))

    def receive_data(self, size):
        return self.ser.read(size)
