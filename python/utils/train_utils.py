import numpy as np
import pandas as pd
import torch
import os
from torch.utils.data import Dataset
from sound_utils import upload_audio_file
from sound_utils import FeatureExtractor, NoiseGate


def preprocess():
    pass


class AudioDataset(Dataset):
    def __init__(self, X, Y):
        super().__init__()
        self.X = torch.tensor(X.values).float()
        self.Y = torch.tensor(Y).float()

    def __len__(self):
        return len(self.X)

    def __getitem__(self, idx):
        x = self.X[idx]
        y = self.Y[idx]
        return x, y


def upload_dataset(path):
    dataset = []
    for file in os.listdir(path):
        file = upload_audio_file(f"{path}/{file}")
        dataset.append((file[0], file[1] / np.iinfo(file[1].dtype).max))
    return dataset
