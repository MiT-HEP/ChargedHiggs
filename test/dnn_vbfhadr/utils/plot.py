import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages


def plot(history,output="training.pdf"):
    pp = PdfPages(output)
    #history = model.fit(x, y, validation_split=0.25, epochs=50, batch_size=16, verbose=1)

    # Plot training & validation accuracy values
    for key in history.history:
        print "valid keys are",key
    plt.plot(history.history['acc'],label="Train")
    #plt.plot(history.history['val_accuracy'])
    plt.title('Model accuracy')
    plt.ylabel('Accuracy')
    plt.xlabel('Epoch')
    #plt.legend(['Train', 'Test'], loc='upper left')
    plt.legend(loc='upper left')
    plt.savefig(pp, format='pdf')
    plt.show(block=False)

    # Plot training & validation loss values
    plt.plot(history.history['loss'],label="Train")
    #plt.plot(history.history['val_loss'])
    plt.title('Model loss')
    plt.ylabel('Loss')
    plt.xlabel('Epoch')
    plt.legend(loc='upper left')
    plt.savefig(pp, format='pdf')
    plt.show(block=False)
    pp .close()
