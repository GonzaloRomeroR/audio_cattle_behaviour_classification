import matplotlib.pyplot as plt
from sklearn.metrics import roc_curve
from sklearn.metrics import auc


def plot_roc_curve(y_val, y_pred):
    fpr, tpr, _ = roc_curve(y_val, y_pred)
    auc_read = auc(fpr, tpr)
    plt.figure()
    plt.plot(fpr, tpr, label=f"ROC curve (area={auc_read}')")
    plt.plot([0, 1], [0, 1], "k--")
    plt.xlim([0.0, 1.0])
    plt.ylim([0.0, 1.05])
    plt.xlabel("False Positive Rate")
    plt.ylabel("True Positive Rate")
    plt.title("ROC curve")
    plt.legend(loc="lower right")
    plt.show()
