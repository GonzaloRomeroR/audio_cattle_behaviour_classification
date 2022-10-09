from importlib.metadata import files
from scipy.io import wavfile
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
import math
from collections import namedtuple
import pandas as pd
import scipy
import os

class AudioDataset():
    
    def __init__(self, data_folder, annotation_file):

        self.audio_data = namedtuple("AudioData", ["data", "sample_freq", "label"])
        self.data_folder = data_folder 
        
        ds = pd.read_csv(f"{annotation_file}")
        self.filenames = list(ds['filename'])
        
        if 'label' in ds.columns:
            self.labels = ds['label'].values
        else:
            self.labels = -np.ones(len(self.filenames))
            
        self.cache = {}
    
    def __len__(self):
        return len(self.labels)
    
    def __getitem__(self, index):
        if index in self.cache:
            data, fs, label = self.cache[index]
        else:
            fname = f"{self.filenames[index] :04d}.wav"
            fpath = os.path.join(self.data_folder, fname)
            
            fs, data = scipy.io.wavfile.read(fpath)
            data = data / np.iinfo(data.dtype).max 
            label = self.labels[index]
            self.cache[index] = (data, fs, label)
            
        return self.audio_data(data, fs, label)


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
        return subelements, sub_init, sub_end


class FeatureExtractor:
    def __init__(self, data, sample_rate):
        self.data = data
        self.sample_rate = sample_rate

    def variance(self, data, ddof=0):
        n = len(data)
        if n == 0:
            return 0
        mean = sum(data) / n
        return sum((x - mean) ** 2 for x in data) / (n - ddof)

    def stdev(self, data):
        var = self.variance(data)
        std_dev = math.sqrt(var)
        return std_dev

    def get_duration(self):
        return len(self.data) / self.sample_rate

    def get_max_amplitude(self):
        return max(self.data) if len(self.data) > 0 else 0

    def get_zero_crosses(self):
        data_sign = np.sign(np.diff(self.data, append=0))
        return np.count_nonzero(np.abs(np.diff(data_sign)))

    def get_simetry(self):
        if len(self.data) == 0:
            return 0
        max_pos = np.argmax(self.data)
        return np.trapz(self.data[:max_pos], axis=0) / np.trapz(self.data, axis=0)

    def get_desviation(self):
        return self.stdev(self.data)

    def extract_features(self):
        features = []
        features.append(self.get_duration())
        features.append(self.get_max_amplitude())
        features.append(self.get_zero_crosses())
        features.append(self.get_simetry())
        features.append(self.get_desviation())
        return features


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
