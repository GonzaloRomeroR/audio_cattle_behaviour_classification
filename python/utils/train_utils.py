import numpy as np
import pandas as pd
import torch
import os
from torch.utils.data import Dataset
from .sound_utils import upload_audio_file

from sklearn.metrics import plot_confusion_matrix
from sklearn.metrics import roc_auc_score
from sklearn.metrics import classification_report
from functools import wraps
from time import time
from timeit import default_timer as timer
import matplotlib.pyplot as plt
from .plot_utils import plot_roc_curve
import torch
from urllib.parse import urlparse
from sklearn.metrics import confusion_matrix, ConfusionMatrixDisplay
from sklearn.model_selection import GridSearchCV


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


def get_best_model(
    train_model,
    X_train,
    y_train,
    X_test,
    y_test,
    params,
    models,
    model_name,
):

    model = Model(train_model, {})
    perform_grid_search(model, X_train, y_train, params)

    params = model.best_params

    model = Model(train_model, params)
    evaluate_model(model, X_train, y_train, X_test, y_test)
    models[model_name] = model

    return model


def evaluate_model(model, X_train, y_train, X_test, y_test):
    model.train(X_train, y_train)
    model.classification_report(X_test, y_test)
    print("Training time: {} s".format(model.train_time))
    # model.plot_curve(X_test, y_test)


def perform_grid_search(model, X_train, y_train, params):
    model.grid_search(X_train, y_train, params)
    print("Grid search time: {} s".format(model.grid_time))


class Model:
    def __init__(self, model_class, params):
        self.model = model_class(**params)
        self.train_time = None

    def train(self, X, y):
        start = timer()
        self.model.fit(X, y)
        end = timer()
        self.train_time = end - start

    def grid_search(self, X, y, params):
        start = timer()
        grid = GridSearchCV(self.model, params)
        grid.fit(X, y)
        end = timer()
        self.best_params = grid.best_params_
        print(self.best_params)
        self.grid_time = end - start

    def predict(self, X):
        return self.model.predict(X)

    def classification_report(self, X, y):
        y_pred = self.predict(X)

        # roc_auc = roc_auc_score(np.array(y), y_pred, multi_class="ovo", average="macro")
        # self.roc_auc = roc_auc
        # print("ROC_AUC = {}\n".format(roc_auc))
        print(classification_report(y, y_pred, digits=5))
        self.report = classification_report(y, y_pred, digits=5, output_dict=True)
        return self.report

    def confusion_matrix(self, X, y):
        y_pred = self.predict(X)
        cm = confusion_matrix(y, y_pred)
        disp = ConfusionMatrixDisplay(confusion_matrix=cm)
        disp.plot()
        plt.grid(None)

    def plot_curve(self, X, y):
        y_pred = self.predict(X)
        plot_roc_curve(y, y_pred)


def run_model(model, X_train, y_train, X_test, y_test):
    model.fit(X_train, y_train)
    y_pred = model.predict(X_test)
    roc_auc = roc_auc_score(y_test, y_pred)
    print("ROC_AUC = {}\n".format(roc_auc))
    print(classification_report(y_test, y_pred, digits=5))
    plot_confusion_matrix(model, X_test, y_test, cmap=plt.cm.Blues, normalize="all")
    plt.grid(None)
    return model, roc_auc
