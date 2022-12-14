from importlib.metadata import files
from scipy.io import wavfile
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path


def upload_audio_file(path: str):
    return wavfile.read(path)


def write_audio_file(data, sample_rate, path):
    return wavfile.write(path, sample_rate, data)


def plot_audio(data, samplerate):
    plt.title("Audio")
    plt.xlabel("time [s]")
    plt.ylabel("intensity")
    plt.plot([i / samplerate for i in range(len(data))], data)
    plt.show()


def write_audio_files(elements, sample_rate, path, file_names="audio"):
    Path(path).mkdir(parents=True, exist_ok=True)
    for i, element in enumerate(elements):
        write_audio_file(element, sample_rate, f"{path}/{file_names}_{i}.wav")


class NoiseGate:
    def __init__(
        self, open_threshold: float, close_threshold: float, hold: float
    ) -> None:
        self.open_threshold = open_threshold
        self.close_threshold = close_threshold
        self.hold = hold

        # Real time filtering
        self.opened = False
        self.hold_time = 0

    def real_time_filter(self, value, sample_rate):
        if not self.opened:
            if abs(value) > self.open_threshold:
                self.opened = True
                self.hold_time = 0
                return value
            return 0
        else:
            if self.hold_time > self.hold:
                self.opened = False
                self.hold_time = 0
                return 0
            elif abs(value) > self.close_threshold:
                self.hold_time = 0
            else:
                self.hold_time += 1 / sample_rate
            return value

    def filter(self, data, sample_rate):
        filter_data = np.zeros(len(data))
        opened = False
        hold_time = 0
        for i, value in enumerate(np.abs(data)):
            if not opened:
                if value > self.open_threshold:
                    opened = True
                    hold_time = 0
            else:
                if hold_time > self.hold:
                    opened = False
                    hold_time = 0
                elif value > self.close_threshold:
                    hold_time = 0
                else:
                    hold_time += 1 / sample_rate
            filter_data[i] = data[i] if opened else 0
        return filter_data

    def split(self, data, sample_rate):
        opened = False
        hold_time = 0
        subelements = []
        sub_init = []
        sub_end = []
        for i, value in enumerate(np.abs(data)):
            if not opened:
                if value > self.open_threshold:
                    opened = True
                    hold_time = 0
                    sub_init.append(i)
            else:
                if hold_time > self.hold:
                    opened = False
                    hold_time = 0
                    sub_end.append(i)
                elif value > self.close_threshold:
                    hold_time = 0
                else:
                    hold_time += 1 / sample_rate

        if len(sub_init) > len(sub_end):
            sub_end.append((len(data) - 1))

        for init, end in zip(sub_init, sub_end):
            subelements.append(data[init:end])
        return subelements


def main():
    file_name = "audios_recording_01"
    sample_rate, data = upload_audio_file(f"../data/{file_name}.wav")
    # plot_audio(data, sample_rate)
    noise_gate = NoiseGate(open_threshold=30000, close_threshold=20000, hold=1)
    # filtered_data = noise_gate.filter(data, sample_rate)
    splitted_data = noise_gate.split(data, sample_rate)

    # plot_audio(filtered_data, sample_rate)

    write_audio_files(
        splitted_data, sample_rate, path="./results", file_names=file_name
    )

    # for el in splitted_data:
    #    plot_audio(el, sample_rate)


if __name__ == "__main__":
    main()
