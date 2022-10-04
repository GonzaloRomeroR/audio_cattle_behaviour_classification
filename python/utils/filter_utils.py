import queue
from .sound_utils import *


class MovingAverage:
    def __init__(self, window_size) -> None:
        self.window_size = window_size
        self.queue_ma = queue.Queue(maxsize=window_size)
        self.average = 0

    def calculate_ma(self, value: float):
        if self.queue_ma.qsize() < self.window_size:
            self.queue_ma.put(value)
            if self.queue_ma.qsize() == self.window_size:
                self.average = sum(self.queue_ma.queue) / self.window_size
                return self.average
            return 0
        else:
            last = self.queue_ma.get()
            self.queue_ma.put(value)
            self.average += 1 / self.window_size * (value - last)
            return self.average


def main():
    file_name = "audios_recording_01"
    sample_rate, data = upload_audio_file(f"../data/{file_name}.wav")
    filter_ma = MovingAverage(window_size=3)
    filtered_data = []

    for value in data:
        absolute = abs(value) if abs(value) >= 0 else np.iinfo(value.dtype).max
        filtered_data.append(filter_ma.calculate_ma(absolute))
    plot_audio(filtered_data, sample_rate)


if __name__ == "__main__":
    main()
