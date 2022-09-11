from scipy.io import wavfile
import numpy as np
import matplotlib.pyplot as plt


def upload_audio_file(path: str):
    return wavfile.read(path)


def plot_audio(data, samplerate):
    plt.title("Audio")
    plt.xlabel("time [s]")
    plt.ylabel("intensity")
    plt.plot([i / samplerate for i in range(len(data))], data)
    plt.show()


class NoiseGate:
    def __init__(
        self, open_threshold: float, close_threshold: float, hold: float
    ) -> None:
        self.open_threshold = open_threshold
        self.close_threshold = close_threshold
        self.hold = hold

    def filter(self, data, sample_rate):
        filter_data = np.zeros(len(data))
        opened = False
        hold_time = 0
        for i, value in enumerate(np.abs(data)):
            if value > self.open_threshold or (
                value > self.close_threshold and opened == True
            ):
                opened = True
                hold_time = 0
            elif value < self.close_threshold and hold_time > self.hold:
                opened = False
                hold_time = 0
            else:
                hold_time += 1 / sample_rate
            filter_data[i] = data[i] if opened else 0
        return filter_data


# sample_rate, data = upload_audio_file("../data/audios_recording_01.wav")
# plot_audio(data, sample_rate)
# noise_gate = NoiseGate(open_threshold=30000, close_threshold=30000, hold=0.1)
# filtered_data = noise_gate.filter(data, sample_rate)
# plot_audio(filtered_data, sample_rate)
